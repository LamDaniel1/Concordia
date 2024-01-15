//Daniel Lam
//ID: 40248073

#include <stdlib.h>
#include "singular.h"
#include <string.h>
#include <strings.h>
#include <math.h>
#include <stddef.h>

static optionalParam precision = {
    .value = 0,
    .isProvided = false
};

static bool _eq(double x, double y) {
    return x == y;
}

static bool _neq(double x, double y) {
    return x != y;
}

static bool _geq(double x, double y) {
    return x >= y;
}

static bool _leq(double x, double y) {
    return x <= y;
}

static bool _less(double x, double y) {
    return x < y;
}

static bool _greater(double x, double y) {
    return x > y;
}

static const char* filterTypes[] = {"EQ", "NEQ", "GEQ", "LEQ", "LESS", "GREATER"};

typedef bool (* singular_filter)(double, double);

static singular_filter filterPtrArray[] = {&_eq, &_neq, &_geq, &_leq, &_less, &_greater};

int getPrecision() {
    return precision.value;
}

bool getPrecisionIsProvided() {
    return precision.isProvided;
}

// Returns length of certain int e.g. int a = 567 => 3.
int getIntLength(int n) {
    int length = n < 0;
    do {
        length++;
    } while(n /= 10);
    return length;
}

void setPrecision(int prec) {
    if (prec < 0) {
        fprintf(stderr, "FATAL ERROR in %s:%d Setting Precision to negative value is not allowed!\n", __FILE__, __LINE__);
        exit(EXIT_FAILURE); 
    }
    precision.value = prec;
    precision.isProvided = true;
}

void print(double a[], size_t size) {
    char* doubleFormat;

    if (!precision.isProvided) {
        doubleFormat = "%lf";
    } else {
        int formatLength = getIntLength(getPrecision()) + 5;
        doubleFormat = (char*) malloc(formatLength * sizeof(char));
        strcpy(doubleFormat, "%.");
        sprintf(doubleFormat + 2, "%d", getPrecision());
        strcat(doubleFormat, "lf");
    }

    // Loop through array and print out each number while applying new precision format
    for (int i = 0; i < size; i++) {
        printf(doubleFormat, a[i]);

        // Print new line if last number of the array, if not print space
        if (i == size-1) {
            printf("\n");
        } else {
            printf(" ");
        }
    }

    if (precision.isProvided) {
        free(doubleFormat);
    }
}

void shift(double a[], size_t size, double by) {
    for (int i = 0; i < size; i++) {
        a[i] = a[i] + by;
    }
}

// shiftFilteredElements() is a helper function for filter() to shift elements in array after removing an incorrect element 
void shiftFilteredElements(double a[], int i, int count) {
    for (int j = i; j < count; j++) {
        a[j] = a[j+1];
    }
}

size_t filter(double a[], size_t count, enum filter_type t, double threshold) {
    size_t totalCorrectElements = 0;
    for (int i = 0; i < count; i++) {
        if (!(* filterPtrArray[t])(a[i], threshold)) {
            count--;
            shiftFilteredElements(a, i, count);
            i--;
        } else {
            totalCorrectElements++;
        }
    }
    return totalCorrectElements;
}

filter_type returnFilterTypeInt(char* filterParam) {
    filter_type result = -1;
    size_t filterTypesLength = sizeof(filterTypes)/sizeof(filterTypes[0]);
    for (int i = 0; i < filterTypesLength; i++) {
        if (strcasecmp(filterParam, filterTypes[i]) == 0) {
            result = i;
        }
    }
    
    if (result == -1) {
        fprintf(stderr, "FATAL ERROR in %s:%d - Unsupported filter type found!\n", __FILE__, __LINE__);
        exit(EXIT_FAILURE);
    }

    return result;
}

const char* returnFilterTypeString(filter_type filterTypeInt) {
    return filterTypes[filterTypeInt];
}

void printSingle(double num) {
    char* doubleFormat;

    if (!precision.isProvided) {
        doubleFormat = "%lf";
    } else {
        int formatLength = getIntLength(getPrecision()) + 5;
        doubleFormat = (char*) malloc(formatLength * sizeof(char));
        strcpy(doubleFormat, "%.");
        sprintf(doubleFormat + 2, "%d", getPrecision());
        strcat(doubleFormat, "lf"); 
    }
    printf(doubleFormat, num);
    if (precision.isProvided) {
        free(doubleFormat);
    }
}