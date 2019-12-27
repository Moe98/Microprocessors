#include <mpi.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#define num 50

int temp[num];
int *sample, sampleSize, maxElement;
int operation = 0;

void generateRandomSample(int size, int maxNum) {
    srand((unsigned) time(0));
    sampleSize = size;
    maxElement = maxNum;
    sample = malloc(sampleSize * sizeof(int));
    for (int i = 0; i < sampleSize; i++)
        sample[i] = rand() % (maxElement + 1);
}

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

int min(int a, int b) { return a < b ? a : b; }
int max(int a, int b) { return a > b ? a : b; }

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

void readInput() {
    printf("Enter size of Sample:\n");
    scanf("%d", &sampleSize);
    printf("Enter the values of the sample (Positive Integers < 1e6) separated by white spaces:\n");
    sample = malloc(sampleSize * sizeof(int));
    for (int i = 0; i < sampleSize; i++) {
        scanf("%d", &sample[i]);
        if (i == 0 || sample[i] > maxElement)
            maxElement = sample[i];
    }
}

int main() {

    int process_id, number_of_processes, elements_per_process, elements_received;

    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &process_id);
    MPI_Comm_size(MPI_COMM_WORLD, &number_of_processes);

    if (process_id == 0) {
//        readTestCase(1);
//        readTestCase(2);
//        readTestCase(3);
//        generateRandomSample(rand() % (int)(1e4), rand() % (int)(1e5));
        readInput();
        printSample();
    }

    double finalAverage;
    for (operation = 0; operation <= 2; operation++) {
        if (process_id == 0) { //master
            if (operation == 0) {
                elements_per_process = sampleSize / number_of_processes;
                if (number_of_processes > 1) {
                    for (int i = 1; i < number_of_processes - 1; i++) {
                        MPI_Send(&elements_per_process, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
                        MPI_Send(&sample[i * elements_per_process], elements_per_process, MPI_INT, i, 0, MPI_COMM_WORLD);
                    }
                    int elements_for_nth = sampleSize - ((number_of_processes - 1) * elements_per_process);
                    MPI_Send(&elements_for_nth, 1, MPI_INT, number_of_processes - 1, 0, MPI_COMM_WORLD);
                    MPI_Send(&sample[(number_of_processes - 1) * elements_per_process], elements_for_nth, MPI_INT,number_of_processes - 1, 0, MPI_COMM_WORLD);
                }
                int sum = 0;
                for (int i = 0; i < elements_per_process; i++) sum += sample[i];
                int tmp;
                for (int i = 1; i < number_of_processes; i++) {
                    MPI_Recv(&tmp, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    sum += tmp;
                }
                printf("Sum of array is: %d\n", sum);
                double average = (sum * 1.0) / (sampleSize * 1.0);
                finalAverage = average;
                printf("Average is: %.2f\n", finalAverage);
            } else if (operation == 1) {
                if (number_of_processes > 1) {
                    for (int i = 1; i < number_of_processes - 1; i++) {
                        MPI_Send(&elements_per_process, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
                        MPI_Send(&sample[i * elements_per_process], elements_per_process, MPI_INT, i, 0,
                                 MPI_COMM_WORLD);
                        MPI_Send(&finalAverage, 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD);
                    }
                    int elements_for_nth = sampleSize - ((number_of_processes - 1) * elements_per_process);
                    MPI_Send(&elements_for_nth, 1, MPI_INT, number_of_processes - 1, 0, MPI_COMM_WORLD);
                    MPI_Send(&sample[(number_of_processes - 1) * elements_per_process], elements_for_nth, MPI_INT,
                             number_of_processes - 1, 0, MPI_COMM_WORLD);
                    MPI_Send(&finalAverage, 1, MPI_DOUBLE, number_of_processes - 1, 0, MPI_COMM_WORLD);

                }
                double numerator = 0;
                double tmpNumerator;
                for (int i = 0; i < elements_per_process; i++)
                    numerator += ((sample[i] - finalAverage) * (sample[i] - finalAverage));
                for (int i = 1; i < number_of_processes; i++) {
                    MPI_Recv(&tmpNumerator, 1, MPI_DOUBLE, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    numerator += tmpNumerator;
                }
                double variance = (numerator / (sampleSize - 1));
                printf("Standard Deviation is: %.2f\n", sqrt(variance));
                printf("Variance is: %.2f\n", variance);
            } else if(operation == 2) {
                int usedProcesses = min(sampleSize, number_of_processes);
                elements_per_process = sampleSize / usedProcesses;
                for(int i = 1; i < usedProcesses; i++) {
                    int numberOfSentElements = elements_per_process;
                    if(i == usedProcesses - 1)
                        numberOfSentElements += (sampleSize % usedProcesses);
                    MPI_Send(&numberOfSentElements, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
                    MPI_Send(&sample[elements_per_process * i], numberOfSentElements, MPI_INT, i, 0, MPI_COMM_WORLD);
                    MPI_Send(&maxElement, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
                }
                int totalFrequency[maxElement +  1], frequency[maxElement + 1];
                for(int i = 0; i <= maxElement; i++)
                    totalFrequency[i] = frequency[i] = 0;
                for(int i = 0; i < elements_per_process; i++)
                    totalFrequency[sample[i]]++;
                for(int i = 1; i < usedProcesses; i++) {
                    MPI_Recv(frequency, maxElement + 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    for(int i = 0; i <= maxElement; i++)
                        totalFrequency[i] += frequency[i];
                }
                int maxFreq = 0;
                int modeNum = 0;
                for (int i = 0; i < sampleSize; i++) {
                    if (totalFrequency[sample[i]] > maxFreq) {
                        maxFreq = totalFrequency[sample[i]];
                        modeNum = sample[i];
                    }
                }
                printf("Mode is: %d\n", modeNum);
            }
        } else { //slaves
            MPI_Recv(&elements_received, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(&temp, elements_received, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            if (operation == 0) {
                int sum = 0;
                for (int i = 0; i < elements_received; i++) sum += temp[i];
                MPI_Send(&sum, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
            } else if (operation == 1) {
                double avg;
                MPI_Recv(&avg, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                double sum = 0;
                for (int i = 0; i < elements_received; i++) sum += ((temp[i] - avg) * (temp[i] - avg));
                MPI_Send(&sum, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
            } else if(operation == 2) {
                MPI_Recv(&maxElement, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                int frequency [maxElement + 1];
                for(int i = 0; i <= maxElement; i++)
                    frequency[i] = 0;
                for (int i = 0; i < elements_received; i++)
                    frequency[temp[i]]++;
                MPI_Send(frequency, maxElement + 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
            }
        }
    }
    MPI_Finalize();
}
