#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int *sample, sampleSize, maxElement;

void printSample() {
    printf("{");
    for (int i = 0; i < sampleSize; i++)
        printf("%d%s", sample[i], i < sampleSize - 1 ? ", " : "}\n");
}

double sqrt(double x) {
    const double EPS = 1e-6;
    double lo = 0, hi = x, mid, ans = -1;
    while (lo - hi < EPS) {
        mid = (lo + hi) / 2;
        if (mid * mid - x <= EPS) {
            ans = mid;
            lo = mid + EPS;
        } else {
            hi = mid - EPS;
        }
    }
    return ans;
}

void readTestCase(int testCaseNumber) {
    const int OUTPUT_LINES = 6;
    freopen("/home/abdulrahman/CLionProjects/Microprocessors/Test-Cases", "r", stdin);
    int testCasesCount;
    scanf("%d", &testCasesCount);
    for (int testCase = 1; testCase <= testCasesCount; testCase++) {
        scanf("%d", &sampleSize);
        sample = malloc(sampleSize * sizeof(int));
        for (int i = 0; i < sampleSize; i++) {
            scanf("%d", &sample[i]);
            if (i == 0 || sample[i] > maxElement)
                maxElement = sample[i];
        }
        char temp[100];
        for (int i = 0; i < OUTPUT_LINES; i++)
            fgets(temp, 100, stdin);
        if (testCase == testCaseNumber) break;
    }
}

int main() {

    int process_id, number_of_processes;

    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &process_id);
    MPI_Comm_size(MPI_COMM_WORLD, &number_of_processes);

    readTestCase(1);
//    readTestCase(2);
//    readTestCase(3);

    int elementsPerProcess = sampleSize / number_of_processes;
    int masterElements = sampleSize - (elementsPerProcess * number_of_processes);
    int* slave_array;
    double avg;
    for (int operation = 0; operation < 3; operation++) {
        if (operation == 0) {
            slave_array = malloc(sizeof(int) * elementsPerProcess);
            MPI_Scatter(&sample[masterElements], elementsPerProcess, MPI_INT, slave_array, elementsPerProcess, MPI_INT, 0, MPI_COMM_WORLD);
            int processSum = 0;
            for (int i = 0; i < elementsPerProcess; i++)
                processSum += slave_array[i];
            int *partialSums = NULL;
            if (process_id == 0) {
                partialSums = malloc(sizeof(int) * number_of_processes);
            }
            MPI_Gather(&processSum, 1, MPI_INT, partialSums, 1, MPI_INT, 0, MPI_COMM_WORLD);
            if (process_id == 0) {
                int totalSum = 0;
                for (int i = 0; i < number_of_processes; i++)
                    totalSum += partialSums[i];
                for (int i = 0; i < masterElements; i++)
                    totalSum += sample[i];
                avg = 1.0 * totalSum / sampleSize;
                if (process_id == 0) {
                    printf("Sum is %d\n", totalSum);
                    printf("Average is %.2f\n", avg);
                }
            }
            MPI_Barrier(MPI_COMM_WORLD);
            double *average = malloc(sizeof(double) * number_of_processes);
            for(int i=0; i<number_of_processes; i++)
                average[i] = avg;
            MPI_Scatter(average,1,MPI_DOUBLE, &avg,1, MPI_DOUBLE,0,MPI_COMM_WORLD);
            MPI_Barrier(MPI_COMM_WORLD);
        } else if (operation == 1) {
            double processSum = 0;
            for (int i = 0; i < elementsPerProcess; i++)
                processSum += (slave_array[i] * 1.0 - avg) * (slave_array[i] * 1.0 - avg);
            double *partialSums = NULL;
            if (process_id == 0) {
                partialSums = malloc(sizeof(double) * number_of_processes);
            }
            MPI_Gather(&processSum, 1, MPI_DOUBLE, partialSums, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
            if (process_id == 0) {
                double totalSum = 0;
                for (int i = 0; i < number_of_processes; i++)
                    totalSum += partialSums[i];
                for (int i = 0; i < masterElements; i++)
                    totalSum += (sample[i] * 1.0 - avg) * (sample[i] * 1.0 - avg);
                printf("Standard Deviation %.2f\n", sqrt((totalSum)/(sampleSize*1.0-1)));
                printf("Variance is %.2f\n", (totalSum)/(sampleSize*1.0-1));
            }
        } else if(operation == 2) {
            int* freq = malloc((maxElement + 1) * sizeof(int));
            for(int i = 0; i <= maxElement; i++)
                freq[i] = 0;
            for(int i = 0; i < elementsPerProcess; i++)
                freq[slave_array[i]]++;
            int* finalFreq = malloc(number_of_processes * (maxElement + 1) * sizeof(int));
            MPI_Gather(freq, maxElement + 1, MPI_INT, finalFreq, maxElement + 1, MPI_INT, 0, MPI_COMM_WORLD);
            MPI_Barrier(MPI_COMM_WORLD);
            if(process_id == 0) {
                int freqAnswer[maxElement + 1];
                for(int i = 0; i <= maxElement; i++)
                    freqAnswer[i] = 0;
                for(int i = 0; i < number_of_processes; i++) {
                    for(int j = 0; j < maxElement + 1; j++) {
                        freqAnswer[j] += finalFreq[(maxElement + 1) * i + j];
                    }
                }
                for(int i = 0; i < masterElements; i++)
                    freqAnswer[sample[i]]++;
                int maxSoFar = freqAnswer[0];
                int mode = 0;
                for(int i = 1; i <= maxElement; i++)
                    if(freqAnswer[i] > maxSoFar) {
                        maxSoFar = freqAnswer[i];
                        mode = i;
                    }
                printf("Mode is %d\n", mode);
            }
        }
    }
    MPI_Finalize();
    return 0;

}
