#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>

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

double summation(double difference){
    double summation = 0;
    for(int i = 0; i < sampleSize; i++)
        summation += (sample[i] - difference);
    return summation;
}

double mean(){
    return (summation(0)/(sampleSize*1.0));
}

double squareDifference(double mean){
    double numerator = 0.0;
    for(int i = 0; i < sampleSize; i++)
        numerator += pow((sample[i] - mean), 2);
    return numerator;
}

double standardDeviation(){
    double average = mean();
    double numerator = squareDifference(average);
    double denominator = sampleSize - 1;
    return sqrt(numerator/denominator);
}

double variance(){
    return pow(standardDeviation(), 2);
}

int mode();

int main() {
    generateRandomSample(10, 100);
    printSample();
    printf("Mean is: %f\n",mean());
    printf("Standard Deviation is: %f, Variance is %f", standardDeviation(), variance());
    return 0;
}
