#include <stdio.h>
#include <time.h>
#include <stdlib.h>

/**
 * sample -> contains the values of the sample.
 * sampleSize -> the number of elements in the sample.
 */

int *sample, sampleSize, maxElement;
int *frequency;
/*
 * @param size the size of the sample.
 * @param maxElement the maximum element in the sample.
 * Generates the sample to apply the calculations on.
 */

void generateRandomSample(int size, int maxNum) {
    srand((unsigned)time(0));
    sampleSize = size;
    maxElement = maxNum;
    sample = malloc(sampleSize * sizeof(int));
    frequency = malloc((maxElement + 1) * sizeof(int));
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

double square(double x) {
    return x * x;
}

double sqrt(double x) {
    const double EPS = 1e-6;
    double lo = 0, hi = x, mid, ans = -1;
    while(lo - hi < EPS) {
        mid = (lo + hi) / 2;
        if(mid * mid - x <= EPS) {
            ans = mid;
            lo = mid + EPS;
        } else {
            hi = mid - EPS;
        }
    }
    return ans;
}

double squareDifference(double mean){
    double numerator = 0.0;
    for(int i = 0; i < sampleSize; i++)
        numerator += square(sample[i] - mean);
    return numerator;
}

double standardDeviation(){
    double average = mean();
    double numerator = squareDifference(average);
    double denominator = sampleSize - 1;
    return sqrt(numerator/denominator);
}

double variance(){
    return square(standardDeviation());
}

int mode(){
    for(int i = 0; i <= maxElement; i++)
        frequency[i] = 0;
    int maxFreq = 0;
    int modeNum = 0;
    for (int i = 0; i < sampleSize; i++) {
        if(++frequency[sample[i]] > maxFreq){
            maxFreq = frequency[sample[i]];
            modeNum = sample[i];
        }
    }
    return modeNum;
}

int main() {
    generateRandomSample(50, 100);
    printSample();
    printf("Mode is: %d\n", mode());
    printf("Mean is: %f\n",mean());
    printf("Standard Deviation is: %f, Variance is %f", standardDeviation(), variance());
    return 0;
}
