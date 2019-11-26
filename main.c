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

    double avg;
    int sum;
    double squareSum;
    double variance;
    double standardDeviation;
    for(int operation=0; operation < 3; operation++){
        if(process_id == 0){
            if(operation == 0) {
                int *slave_array = malloc(sizeof(int) * elementsPerProcess);
                MPI_Scatter(&sample[masterElements], elementsPerProcess, MPI_INT, slave_array, elementsPerProcess,
                            MPI_INT, 0, MPI_COMM_WORLD);
                int processSum = 0;
                for (int i = 0; i < elementsPerProcess; i++) {
                    processSum += slave_array[i];
                    //printf("elements of process %d %d\n", process_id, slave_array[i]);
                }
                int *partialSums = NULL;
                if (process_id == 0) {
                    partialSums = malloc(sizeof(int) * number_of_processes);
                }
                MPI_Gather(&processSum, 1, MPI_INT, partialSums, 1, MPI_INT, 0, MPI_COMM_WORLD);
                if (process_id == 0) {
                    int totalSum = 0;
                    for (int i = 0; i < number_of_processes; i++) {
                        //printf("received from process %d is %d\n", i, partialSums[i]);
                        totalSum += partialSums[i];
                    }
                    for (int i = 0; i < masterElements; i++) {
                        totalSum += sample[i];
                    }
                    sum = totalSum;
                    avg = 1.0 * sum / sampleSize;
                    if (process_id == 0) {
                        printf("avg is %f\n", avg);
                        printf("sum is %d\n", sum);
                        //operation = 1;
                    }
                }
            }
            if(operation==1){
                printf("avg here %f\n", avg);
                int *slave_array = malloc(sizeof(int) * elementsPerProcess);
                MPI_Scatter(&sample[masterElements], elementsPerProcess, MPI_INT, slave_array, elementsPerProcess, MPI_INT, 0, MPI_COMM_WORLD);
                double processSquareSum = 0;
                for(int i=0;i<elementsPerProcess;i++){
                    processSquareSum += (slave_array[i] - avg) * (slave_array[i] - avg);
                    printf("elements of process %d %d\n", process_id, slave_array[i]);
                }
                double *partialSquareSums=NULL;
                if(process_id==0){
                    partialSquareSums = malloc(sizeof(double)*number_of_processes);
                }
                MPI_Gather(&processSquareSum,1,MPI_DOUBLE,partialSquareSums,1,MPI_DOUBLE,0,MPI_COMM_WORLD);
                if(process_id==0){
                    double totalSquareSum = 0;
                    for(int i=0;i<number_of_processes;i++){
                        //printf("received from process %d is %d\n", i, partialSums[i]);
                        totalSquareSum += partialSquareSums[i];
                    }
                    for(int i=0;i<masterElements;i++){
                        totalSquareSum += (1.0 * sample[i] - avg) * (1.0 * sample[i] - avg);
                    }
                    squareSum = totalSquareSum;
                    variance = squareSum / (sampleSize - 1);
                    standardDeviation = sqrt(variance);
                    if(process_id==0){
                        printf("total sum is %f\n", totalSquareSum);
                        printf("stdVar is %f\n", standardDeviation);
                        printf("variance is %f\n", variance);
                       // operation = 3;
                    }
                }

            }
        }

    }
    MPI_Finalize();
    return 0;

}
