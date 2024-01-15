//Daniel Lam
//ID: 40248073

// #include <stdio.h>
#include <stdlib.h>
#include "aggregate.h"
#include "singular.h"
#include <string.h>
#include <strings.h>
#include <ctype.h>

typedef enum operation {
    COUNT = 0, 
    MIN = 1,
    MAX = 2,
    SUM = 3,
    AVG = 4,
    PAVG = 5,
    PRINT = 6,
    FILTER = 7,
    SHIFT = 8
} operation;

static const char * const operations[] = {
    "COUNT",
    "MIN",
    "MAX",
    "SUM",
    "AVG",
    "PAVG",
    "PRINT",
    "FILTER",
    "SHIFT"
};

typedef struct {
    char* array;
    int length;
    int capacity;
} charList;

typedef struct {
    double* array;
    int length;
    int capacity;
} doubleList;

static optionalParam sizeParam = {
    .value = 0,
    .isProvided = false,
};

static optionalParam operationParam = {
    .value = 0,
    .isProvided = false,
};

static filter_type filterTypeParam;

static double thresholdParam;

int getSize() {
    return sizeParam.value;
}

void setSize(int newSize) {
    if (newSize < 0) {
        fprintf(stderr, "FATAL ERROR in %s:%d - Setting size to negative value is not allowed!\n", __FILE__, __LINE__);
        exit(EXIT_FAILURE); 
    }
    sizeParam.value = newSize;
    sizeParam.isProvided = true;
}

int getOperation() {
    return operationParam.value;
}

void setOperation(operation newOperation) {
    if (newOperation < 0) {
        fprintf(stderr, "FATAL ERROR in %s:%d - Setting size to negative value is not allowed!\n", __FILE__, __LINE__);
        exit(EXIT_FAILURE); 
    }
    operationParam.value = newOperation;
    operationParam.isProvided = true;
}

// toLowerCase is a helper method that takes a string and converts all to lowercase
void toLowerCase(char * s) {
    while(*s != '\0') {
        *s = tolower(*s);
        s++;
    }
}

// duplicateString() copies the string to result
char* duplicateString(const char * s) {
    char* res = malloc((strlen(s) + 1) * sizeof(char));
    if (res != NULL) {
        strcpy(res, s);
    }
    return (char *) res;
}

// customStrCaseStr is a helper method for strcasectr as it is not supposed in Ubuntu
char* customStrCaseStr(const char * haystack, const char * needle) {
    char* haystackTemp = duplicateString(haystack);
    char* needleTemp = duplicateString(needle);
    
    toLowerCase(haystackTemp);
    toLowerCase(needleTemp);
    char * result = strstr(haystackTemp, needleTemp);
    
    if (result != NULL)
        result = (char *) haystack + (result - haystackTemp);

    free(haystackTemp);
    free(needleTemp);
    return result;
}

bool isEndOfLine(char currentChar) {
    return currentChar == '\n' || currentChar == '\r' || currentChar == EOF;
}

bool isSeparator(char currentChar) {
    return currentChar == ' ' || isEndOfLine(currentChar);
}

bool getNextNumber(char * buffer) {
    char currentChar = getchar();
    int i;
    for (i = 0; !isSeparator(currentChar); i++) {
        buffer[i] = currentChar;
        currentChar = getchar();
    }
    buffer[i] = '\0';
    return isEndOfLine(currentChar);
}

void insertIntoList(doubleList * currentDoubleList, double num) {
    if (currentDoubleList->array == NULL) {

        currentDoubleList->array = realloc(currentDoubleList->array, 30 * sizeof(double));

        if (currentDoubleList->array == NULL) {
            fprintf(stderr, "FATAL ERROR in %s:%d - double array is NULL.\n", __FILE__, __LINE__);
            exit(EXIT_FAILURE);
        }
    }

    if (currentDoubleList->length >= currentDoubleList->capacity) {
        currentDoubleList->array = realloc(currentDoubleList->array, 30 * sizeof(double));
        if (currentDoubleList->array == NULL) {
            fprintf(stderr, "FATAL ERROR in %s at line %d: array is NULL.\n", __FILE__, __LINE__);
            exit(EXIT_FAILURE);
        }
    }

    currentDoubleList->array[currentDoubleList->length] = num;
    currentDoubleList->length++;
}

void computeRow(double* array, int size) {
    operation currentOperation = getOperation();

    // Aggregate Operations
    if (currentOperation <= PAVG && currentOperation >= COUNT) {
        if (array != NULL) {
            double result = aggregate(operations[currentOperation], array, size);
            
            printSingle(result);
        }
        if(array != NULL || feof(stdin))
            printf("%s", feof(stdin) ? "\n" : " ");
    }
    // Singular Operation
    else {
        if (currentOperation == SHIFT) {
            shift(array, size, thresholdParam);
        } else if (currentOperation == FILTER) {
            size = filter(array, size, filterTypeParam, thresholdParam);
        }
        print(array, size);
    }
}

// executeRowStdin() continuously goes through line by line of stdin and reads every number from input
void executeRowStdin() {
    doubleList currentDoubleList = {
        .array = NULL,
        .length = 0,
        .capacity = 0
    };

    int maxBufferLimit = 50;
    char* bufferInput = malloc(maxBufferLimit * sizeof(char));
    int size = sizeParam.isProvided ? sizeParam.value : 256;

    bool isEndOfFile;

    do {
        // Check if end of file has been reached

        isEndOfFile = getNextNumber(bufferInput);

        // Convert number in stdin from string to double
        char* ptr;
        double num = strtod(bufferInput, &ptr);
        if (*ptr != '\0') {
            fprintf(stderr, "FATAL ERROR in %s:%d - Buffer taking invalid stdin!\n", __FILE__, __LINE__);
            exit(EXIT_FAILURE);
        }

        // Add element into double list
        if (strcmp("", bufferInput) != 0)
            insertIntoList(&currentDoubleList, num);

    } while( (currentDoubleList.length < size) && !(isEndOfFile));

    // If size was not provided, then set size to current length of list
    if (!sizeParam.isProvided) {
        sizeParam.value = currentDoubleList.length;
        sizeParam.isProvided = true;
    }
    
    // computeRow() computes every row of the doubleList
    computeRow(currentDoubleList.array, currentDoubleList.length);

    // Free up malloc so avoid mem leak
    free(currentDoubleList.array);
    free(bufferInput);

}

int main(int argc, char* argv[]) {

    // STEP 1: INITIALIZE AND PARSE ALL ARGUMENTS OF MATHPIPE
    // Ignore first elem of argv[] e.g "mathpipe -size=3 PAVG -prec=1" ==> {"mathpipe", "-size=3", "PAVG", "-prec=1"}
    for (int i = 1; i < argc; i++) {
        char* currentParam = argv[i];

        // If the param begins with '-size='
        if (customStrCaseStr(currentParam, "-size=") == currentParam) {

            // Get char ptr of where '=' is
            char* equalChar = strchr(currentParam, '=');

            // Code for converting param after '=' into int and set the size
            char *ptr; 
            int sizeParam = (int) strtol(equalChar + 1, &ptr, 10);
            if (*ptr != '\0') {
                fprintf(stderr, "FATAL ERROR in %s:%d - Size is an invalid parameter!\n", __FILE__, __LINE__);
                exit(EXIT_FAILURE);
            }   

            setSize(sizeParam);
        }
        // If the param begins with -prec= 
        else if (customStrCaseStr(currentParam, "-prec=") == currentParam) {

            // Code for converting param after '=' into int and set the precision
            char* equalChar = strchr(currentParam, '=');
            char *ptr; 
            int precisionParam = (int) strtol(equalChar + 1, &ptr, 10);
            if (*ptr != '\0') {
                fprintf(stderr, "FATAL ERROR in %s:%d - Precision is an invalid parameter!\n", __FILE__, __LINE__);
                exit(EXIT_FAILURE);
            }   

            setPrecision(precisionParam);
        }
        // If param is not prec or size, it is an operation
        else {

            // Find corresponding operation in integer
            int operationInt;
            size_t operationsLength = sizeof(operations)/sizeof(operations[0]);
            for (int operationIndex = 0; operationIndex < operationsLength; operationIndex++) {
                if (strcasecmp(currentParam, operations[operationIndex]) == 0) {
                    operationInt = operationIndex;
                }
            }

            // Set correspond operation
            setOperation(operationInt);

            // if operation is FILTER, target the keywords EQ, NEQ, LESS, GREATER, etc
            if (operationInt == FILTER) {
                i++;
                if (i >= argc) {
                    fprintf(stderr, "FATAL ERROR in %s:%d: Missing argument for operation! (filter only).\n", __FILE__, __LINE__);
                    exit(EXIT_FAILURE);
                }

                // Set param for filter type using method inside singular.c
                currentParam = argv[i];
                filterTypeParam = returnFilterTypeInt(currentParam);
            }

            // if operation is FILTER or SHIFT, target the threshold (5, 10, 15, etc)
            if (operationInt == FILTER || operationInt == SHIFT) {
                i++;
                if (i >= argc) {
                    fprintf(stderr, "FATAL ERROR in %s:%d: Missing argument for operation! (filter & shift).\n", __FILE__, __LINE__);
                    exit(EXIT_FAILURE);
                }
                
                // strtod() documentation
                // https://www.tutorialspoint.com/c_standard_library/c_function_strtod.htm
                char *ptr;
                currentParam = argv[i];
                thresholdParam = strtod(currentParam, &ptr);
                
            }
        }
    }

    // STEP 2: RECEIVE STDIN FROM SAMPLE.TXT AND PRINT OUT OUTPUT TO TERMINAL

    // Start reading from stdin input after executing "cat sample.txt | mathpipe ..."
    while (!feof(stdin)) {
        executeRowStdin();
    }
    
    return 0;
}