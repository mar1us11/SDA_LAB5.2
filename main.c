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