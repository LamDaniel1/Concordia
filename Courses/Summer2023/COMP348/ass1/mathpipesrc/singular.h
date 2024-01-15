//Daniel Lam
//ID: 40248073

#ifndef SINGULAR_H
#define SINGULAR_H

#include <stdio.h>
#include <stdbool.h>

typedef enum filter_type { EQ = 0, NEQ = 1, GEQ = 2, LEQ = 3, LESS = 4, GREATER = 5 } filter_type;
void print(double a[], size_t size);
void shift(double a[], size_t size, double by);
size_t filter(double a[], size_t count, enum filter_type t, double threshold);

typedef struct {
    int value;
    bool isProvided;
} optionalParam;

int getPrecision();
void setPrecision(int prec);

filter_type returnFilterTypeInt(char* filterParam);

const char* returnFilterTypeString(filter_type filterTypeInt);

void printSingle(double num);

#endif