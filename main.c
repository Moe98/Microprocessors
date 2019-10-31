#include <stdio.h>
#include <time.h>
#include <stdlib.h>

/**
 * sample -> contains the values of the sample.
 * sampleSize -> the number of elements in the sample.
 */

int *sample, sampleSize;

/*
 * @param size the size of the sample.
 * @param maxElement the maximum element in the sample.
 * Generates the sample to apply the calculations on.
 */

void generateRandomSample(int size, int maxElement) {
    srand((unsigned)time(0));
    sampleSize = size;
    sample = malloc(sampleSize * sizeof(int));
    for(int i = 0; i < sampleSize; i++)
        sample[i] = rand() % (maxElement + 1);
}

void printSample() {
    printf("{");
    for(int i = 0; i < sampleSize; i++)
        printf("%d%s", sample[i], i < sampleSize - 1 ? ", " : "}\n");
}

int summation();

double mean();

double variance();

double standardDeviation();

int mode();

int main() {
    generateRandomSample(10, 100);
    printSample();
    return 0;
}
