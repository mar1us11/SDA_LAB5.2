#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct {
    int day;
    int month;
    int year;
} Date;

typedef struct {
    char authorName[70];
    char title[70];
    char publisher[70];
    Date publishingDate;
    Date purchasedDate;
    double priceUnit;
    int available;
} RegistryBib;

// reads one line from fp into buf, strips the newline
static int readline(FILE *fp, char *buf, int size) {
    if (!fgets(buf, size, fp)) return 0;
    buf[strcspn(buf, "\n")] = '\0';
    return 1;
}

// consumes leading whitespace/newlines so the next read lands on real data
static void skip_blank(FILE *fp) {
    int c;
    while ((c = fgetc(fp)) != EOF) {
        if (c != '\n' && c != '\r' && c != ' ') {
            ungetc(c, fp);
            return;
        }
    }
}

// returns how many years the book has been in use relative to the current year
int getYears(RegistryBib *r) {
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    int currentYear = tm_info->tm_year + 1900;
    return currentYear - r->purchasedDate.year;
}

// swaps two records in place using a temporary copy
void swap(RegistryBib *a, RegistryBib *b) {
    RegistryBib temp = *a;
    *a = *b;
    *b = temp;
}

// partitions around the rightmost element; returns the pivot's final index
int partition(RegistryBib *arr, int left, int right) {
    int pivot = getYears(arr + right);
    int i = left - 1;

    for (int j = left; j < right; j++) {
        if (getYears(arr + j) < pivot) {
            i++;
            swap(arr + i, arr + j);
        }
    }

    swap(arr + i + 1, arr + right);
    return i + 1;
}

// sorts arr[left..right] in ascending order of years of use
void quickSort(RegistryBib *arr, int left, int right) {
    if (left < right) {
        int pi = partition(arr, left, right);
        quickSort(arr, left, pi - 1);
        quickSort(arr, pi + 1, right);
    }
}

// prints all n records to stdout
void printElem(RegistryBib *arr, int n) {
    for (int i = 0; i < n; i++) {
        RegistryBib *r = arr + i;

        printf("Author: %s\n", r->authorName);
        printf("Title: %s\n", r->title);
        printf("Publisher: %s\n", r->publisher);

        printf("Publishing Date: %d/%d/%d\n",
               r->publishingDate.day,
               r->publishingDate.month,
               r->publishingDate.year);

        printf("Purchasing Date: %d/%d/%d\n",
               r->purchasedDate.day,
               r->purchasedDate.month,
               r->purchasedDate.year);

        printf("Years of use: %d\n", getYears(r));

        printf("Price: %.2lf, Available: %s\n",
               r->priceUnit,
               r->available ? "Yes" : "No");

        printf("\n");
    }
}

// reads n records from fp into arr; returns how many were successfully read
int scanElem(RegistryBib *arr, int n, FILE *fp) {
    for (int i = 0; i < n; i++) {
        skip_blank(fp);
        char buf[256];

        if (!readline(fp, (arr + i)->authorName, 70)) return i;
        if (!readline(fp, (arr + i)->title, 70)) return i;
        if (!readline(fp, (arr + i)->publisher, 70)) return i;

        if (!readline(fp, buf, sizeof(buf))) return i;
        sscanf(buf, "%d %d %d",
               &(arr + i)->publishingDate.day,
               &(arr + i)->publishingDate.month,
               &(arr + i)->publishingDate.year);

        if (!readline(fp, buf, sizeof(buf))) return i;
        sscanf(buf, "%d %d %d",
               &(arr + i)->purchasedDate.day,
               &(arr + i)->purchasedDate.month,
               &(arr + i)->purchasedDate.year);

        if (!readline(fp, buf, sizeof(buf))) return i;
        sscanf(buf, "%lf", &(arr + i)->priceUnit);

        if (!readline(fp, buf, sizeof(buf))) return i;
        sscanf(buf, "%d", &(arr + i)->available);
    }
    return n;
}

// prompts the user for n books and writes the raw data to experiment.txt
void inputToFile(int *n) {
    FILE *fp = fopen("experiment.txt", "w");
    if (!fp) return;

    printf("Enter number of books: ");
    scanf("%d", n);
    while (getchar() != '\n');

    fprintf(fp, "%d\n", *n);

    RegistryBib temp;

    for (int i = 0; i < *n; i++) {
        printf("\nBook %d\n", i + 1);

        printf("Author: ");
        fgets(temp.authorName, 70, stdin);
        temp.authorName[strcspn(temp.authorName, "\n")] = 0;

        printf("Title: ");
        fgets(temp.title, 70, stdin);
        temp.title[strcspn(temp.title, "\n")] = 0;

        printf("Publisher: ");
        fgets(temp.publisher, 70, stdin);
        temp.publisher[strcspn(temp.publisher, "\n")] = 0;

        printf("Publishing date (d m y): ");
        scanf("%d %d %d",
              &temp.publishingDate.day,
              &temp.publishingDate.month,
              &temp.publishingDate.year);

        printf("Purchase date (d m y): ");
        scanf("%d %d %d",
              &temp.purchasedDate.day,
              &temp.purchasedDate.month,
              &temp.purchasedDate.year);

        printf("Price: ");
        scanf("%lf", &temp.priceUnit);

        printf("Available (1/0): ");
        scanf("%d", &temp.available);
        while (getchar() != '\n');

        fprintf(fp, "%s\n%s\n%s\n",
                temp.authorName,
                temp.title,
                temp.publisher);

        fprintf(fp, "%d %d %d\n",
                temp.publishingDate.day,
                temp.publishingDate.month,
                temp.publishingDate.year);

        fprintf(fp, "%d %d %d\n",
                temp.purchasedDate.day,
                temp.purchasedDate.month,
                temp.purchasedDate.year);

        fprintf(fp, "%.2lf\n%d\n",
                temp.priceUnit,
                temp.available);
    }

    fclose(fp);
}

// reads experiment.txt and prints its contents to stdout
void displayFromFile(void) {
    FILE *fp = fopen("experiment.txt", "r");
    if (!fp) return;

    int n;
    fscanf(fp, "%d", &n);
    while (fgetc(fp) != '\n');

    RegistryBib *arr = malloc(sizeof(RegistryBib) * n);
    if (!arr) {
        printf("Memory allocation failed\n");
        fclose(fp);
        return;
    }

    int read = scanElem(arr, n, fp);
    fclose(fp);

    printElem(arr, read);

    free(arr);
}


// writes the sorted records with all fields to output.txt
void writeOutput(RegistryBib *arr, int n) {
    FILE *fp = fopen("output.txt", "w");
    if (!fp) return;

    for (int i = 0; i < n; i++) {
        RegistryBib *r = arr + i;

        fprintf(fp, "Author: %s\n", r->authorName);
        fprintf(fp, "Title: %s\n", r->title);
        fprintf(fp, "Publisher: %s\n", r->publisher);

        fprintf(fp, "Publishing Date: %d/%d/%d\n",
                r->publishingDate.day,
                r->publishingDate.month,
                r->publishingDate.year);

        fprintf(fp, "Purchasing Date: %d/%d/%d\n",
                r->purchasedDate.day,
                r->purchasedDate.month,
                r->purchasedDate.year);

        fprintf(fp, "Years of use: %d\n", getYears(r));
        fprintf(fp, "Price: %.2lf\n", r->priceUnit);
        fprintf(fp, "Available: %s\n\n", r->available ? "Yes" : "No");
    }

    fclose(fp);
}

/* prepends output.txt to experiment.txt via a temporary file */
void copyToExperiment(void) {
    FILE *src = fopen("output.txt", "r");
    if (!src) return;

    FILE *old = fopen("experiment.txt", "r");
    if (!old) { fclose(src); return; }

    FILE *temp = fopen("temp.txt", "w");
    if (!temp) { fclose(src); fclose(old); return; }

    int ch;

    while ((ch = fgetc(src)) != EOF)
        fputc(ch, temp);

    while ((ch = fgetc(old)) != EOF)
        fputc(ch, temp);

    fclose(src);
    fclose(old);
    fclose(temp);

    // replace experiment.txt with the merged temp file
    remove("experiment.txt");
    rename("temp.txt", "experiment.txt");
}


// lets the user pick which output file to read, then prints it to stdout
void readChoice(void) {
    int choice;
    printf("Read file: 1-output.txt, 2-experiment.txt: ");
    scanf("%d", &choice);

    FILE *fp = fopen(choice == 1 ? "output.txt" : "experiment.txt", "r");
    if (!fp) return;

    int ch;
    while ((ch = fgetc(fp)) != EOF)
        putchar(ch);

    fclose(fp);
}

int main(void) {
    int n;

    inputToFile(&n);
    displayFromFile();

    //re-open to load the data into memory for sorting
    FILE *fp = fopen("experiment.txt", "r");
    if (!fp) return 1;

    fscanf(fp, "%d", &n);
    while (fgetc(fp) != '\n');

    RegistryBib *arr = malloc(sizeof(RegistryBib) * n);
    if (!arr) {
        printf("Memory allocation failed\n");
        fclose(fp);
        return 1;
    }

    scanElem(arr, n, fp);
    fclose(fp);

    quickSort(arr, 0, n - 1);

    writeOutput(arr, n);
    copyToExperiment();

    free(arr);

    int read;
    printf("Do you want to read a file? (1-yes / 0-no): ");
    scanf("%d", &read);

    if (read)
        readChoice();

    return 0;
}