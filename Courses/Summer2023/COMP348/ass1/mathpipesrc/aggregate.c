//Daniel Lam
//ID: 40248073

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

static double _count(double* array, int size) {
    return size;
}

static double _min(double* array, int size) {
    double currentMin = array[0];
    for (int i = 0; i < size; i++) {
        if (array[i] < currentMin) {
            currentMin = array[i];
        }
    }
    return currentMin;
}

static double _max(double* array, int size) {
    double currentMax = array[0];
    for (int i = 0; i < size; i++) {
        if (array[i] > currentMax) {
            currentMax = array[i];
        }
    }
    return currentMax;
}

static double _sum(double* array, int size) {
    double currentSum = 0;
    for (int i = 0; i < size; i++) {
        currentSum = currentSum + array[i];
    }
    return currentSum;
}

static double _avg(double* array, int size) {
    double sum = _sum(array, size);
    return sum/size;
}

static double _pavg(double* array, int size) {
    double min = _min(array, size);
    double max = _max(array, size);
    return (min + max)/2;
}

static const char* funcNames[6] = {"COUNT", "MIN", "MAX", "SUM", "AVG", "PAVG"};

typedef double (* aggregate_func)(double*, int);

static aggregate_func funcPtrArray[] = {&_count, &_min, &_max, &_sum, &_avg, &_pavg};

double aggregate(const char* func, double* array, int size) {
    size_t funcNamesLength = sizeof(funcNames)/sizeof(funcNames[0]);
    for (int i = 0; i < funcNamesLength; i++) {
        if (strcasecmp(func, funcNames[i]) == 0) {
            double aggregateResult = (* funcPtrArray[i])(array, size);
            return aggregateResult;
        }
    }

    fprintf(stderr, "FATAL ERROR in %s:%d - Unsupported function found in aggregate.c!\n", __FILE__, __LINE__);
    exit(EXIT_FAILURE);
}

