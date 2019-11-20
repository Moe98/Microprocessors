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
    int x = 0;

    readTestCase(1);
//    readTestCase(2);
//    readTestCase(3);

    int sum = 0;
    int start = process_id * (sampleSize / number_of_processes);
    int end = sampleSize / number_of_processes + start;

    if (sampleSize % number_of_processes != 0 && process_id == number_of_processes - 1) {
        end = sampleSize;
    }
    int flag = 1;
    if (sampleSize / number_of_processes == 0) {
        flag = 0;
    }
    if (flag == 1) {
        for (int i = start; i < end; ++i) {
            sum += sample[i];
        }
        int totalSum;
        MPI_Reduce(&sum, &totalSum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
        double Average;
        if (process_id == 0) {
            Average = (totalSum * 1.0) / sampleSize;
        }
        MPI_Bcast(&Average, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        if (process_id == 0) {
            printf("Sum of array is: %d\n", totalSum);
            printf("Average is: %.2f\n", Average);
        }
        long double values = 0;
        long double totalValues;
        if (sampleSize % number_of_processes != 0 && process_id == number_of_processes - 1) {
            end = sampleSize;
        }
        for (int i = start; i < end; i++)
            values += ((sample[i] - Average) * (sample[i] - Average));
        MPI_Reduce(&values, &totalValues, 1, MPI_LONG_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
        if (process_id == 0) {
            long double variance = (totalValues * 1.0) / (sampleSize - 1);
            printf("Standard Deviation is: %f\n", sqrt(variance));
            printf("Variance is: %Lf\n", variance);
        }
        int MaxElement = 30;
        int *freq = malloc((MaxElement + 1) * sizeof(int));
        if (sampleSize % number_of_processes != 0 && process_id == number_of_processes - 1) {
            end = sampleSize;
        }
        for (int i = start; i < end; i++) {
            freq[sample[i]] += 1;
        }
        int *freqFinal = malloc((MaxElement + 1) * sizeof(int));
        if (sampleSize % number_of_processes != 0 && process_id == number_of_processes - 1) {
            end = sampleSize;
        }
        for (int i = 0; i < MaxElement + 1; i++) {
            MPI_Reduce(&freq[i], &freqFinal[i], 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
        }
        int Max = 0;
        int index = -1;
        for (int i = 0; i < MaxElement + 1; i++) {
            if (freqFinal[i] > Max) {
                Max = freqFinal[i];
                index = i;
            }
        }
        if (process_id == 0) {
            printf("Mode is: %d\n", index);
            printf("Number of Repitions: %d\n", Max);
        }
        MPI_Finalize();
    } else {
        if (process_id == 0) {
            printf("The array size is less than the number of processes only one process will be working\n");
            int MaxElement = 30;
            for (int i = 0; i < sampleSize; ++i) {
                sum += sample[i];
            }
            printf("Sum of array is: %d\n", sum);
            double Average = (sum * 1.0) / sampleSize;
            printf("Average is: %.2f\n", Average);
            double values;
            for (int i = 0; i < sampleSize; i++)
                values += ((sample[i] - Average) * (sample[i] - Average));
            long double variance = values / sampleSize;
            printf("Standard Deviation is: %f\n", sqrt(variance));
            printf("Variance is: %Lf\n", variance);
            int *freq = malloc((MaxElement + 1) * sizeof(int));
            for (int i = 0; i < sampleSize; i++) {
                freq[sample[i]] += 1;
            }
            int Max = -1;
            int index = 0;
            for (int i = 0; i < MaxElement + 1; i++) {
                if (freq[i] > Max) {
                    Max = freq[i];
                    index = i;
                }
            }
            printf("Mode is: %d\n", index);
            printf("Number of Repetitions: %d\n", Max);

            MPI_Finalize();
            return 0;
        } else {
            MPI_Finalize();
            return 0;
        }
    }

}
