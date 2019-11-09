#include <mpi.h> 
#include <stdio.h>
#include <time.h>
#include <stdlib.h>


#define num 50

int array[] = {1, 2, 3, 4, 5};
int temp[num];
int *sample, *sampleTemp,sampleSize, maxElement, *frequency, *frequencyTemp;
int operation = 0;
int called = 0;

void generateRandomSample(int size, int maxNum) {
	called = 1;		
	srand((unsigned)time(0));
	sampleSize = size;
	maxElement = maxNum;
	sample = malloc(sampleSize * sizeof(int));
	sampleTemp = malloc(sampleSize * sizeof(int));
	frequency = malloc((maxElement + 1) * sizeof(int));
	frequencyTemp = malloc((maxElement + 1) * sizeof(int));
	for(int i = 0; i < sampleSize; i++)
	sample[i] = rand() % (maxElement + 1);
}

void printSample() {
    printf("{");
    for(int i = 0; i < sampleSize; i++)
        printf("%d%s", sample[i], i < sampleSize - 1 ? ", " : "}\n");
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

int main() {

	int process_id, number_of_processes, elements_per_process, elements_received;
	//int done = 0;
	MPI_Init(NULL, NULL);
	MPI_Comm_rank(MPI_COMM_WORLD, &process_id);
	MPI_Comm_size(MPI_COMM_WORLD, &number_of_processes);
	double finalAverage;
	for(operation = 0; operation<2;operation++){
		if(!called) {generateRandomSample(num, 2000); printSample();}
		if(process_id == 0){ //master
			if(operation == 0){
				elements_per_process = sampleSize / number_of_processes;
				if(number_of_processes > 1){
					for(int i = 1; i < number_of_processes - 1; i++){
						MPI_Send(&elements_per_process, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
						MPI_Send(&sample[i*elements_per_process], elements_per_process, MPI_INT, i, 0, MPI_COMM_WORLD);
					}
					int elements_for_nth = sampleSize - ((number_of_processes-1) * elements_per_process);
					MPI_Send(&elements_for_nth, 1, MPI_INT, number_of_processes - 1, 0, MPI_COMM_WORLD);
					MPI_Send(&sample[(number_of_processes-1) * elements_per_process], elements_for_nth, MPI_INT, number_of_processes - 1, 0, MPI_COMM_WORLD);
					
				}
				int sum = 0;
				for(int i=0; i<elements_per_process;i++) sum+=sample[i];
				int tmp;
				for(int i=1;i<number_of_processes;i++){
					MPI_Recv(&tmp, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
					sum+=tmp;		
				}
				printf("Sum of array is: %d\n", sum);
				double average = (sum*1.0)/(sampleSize*1.0);
				finalAverage = average;
				printf("Average is: %f\n", finalAverage);
			}
			else if (operation == 1){
				if(number_of_processes > 1){
					for(int i = 1; i < number_of_processes - 1; i++){
						MPI_Send(&elements_per_process, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
						MPI_Send(&sample[i*elements_per_process], elements_per_process, MPI_INT, i, 0, MPI_COMM_WORLD);
						MPI_Send(&finalAverage, 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD);
					}
					int elements_for_nth = sampleSize - ((number_of_processes-1) * elements_per_process);
					MPI_Send(&elements_for_nth, 1, MPI_INT, number_of_processes - 1, 0, MPI_COMM_WORLD);
					MPI_Send(&sample[(number_of_processes-1) * elements_per_process], elements_for_nth, MPI_INT, number_of_processes - 1, 0, MPI_COMM_WORLD);
					MPI_Send(&finalAverage, 1, MPI_DOUBLE, number_of_processes - 1, 0, MPI_COMM_WORLD);
					
				}
				double numerator = 0;
				double tmpNumerator;
				for(int i=0; i<elements_per_process;i++) numerator+=((sample[i]-finalAverage)*(sample[i]-finalAverage));
				for(int i=1;i<number_of_processes;i++){
					MPI_Recv(&tmpNumerator, 1, MPI_DOUBLE, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
					numerator+=tmpNumerator;		
				}
				double variance = (numerator/(sampleSize-1));
				printf("Standard Variation is: %f\n", sqrt(variance));
				printf("Variance is: %f\n", variance);
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
				printf("Mode is: %d\n", modeNum);
			}
		}
		else{ //slaves
			MPI_Recv(&elements_received, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			MPI_Recv(&temp, elements_received, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			if(operation==0){
				int sum = 0;
				for(int i=0;i<elements_received;i++) sum+=temp[i];
				MPI_Send(&sum, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
			}
			else if(operation == 1){
				double avg;
				MPI_Recv(&avg, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				double sum = 0;
				for(int i=0;i<elements_received;i++) sum+=((temp[i]-avg)*(temp[i]-avg));
				MPI_Send(&sum, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
			}
		}
	}
	MPI_Finalize();
}
