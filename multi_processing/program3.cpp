#include <iostream>
#include <math.h>
#include <cstdlib>
#include <pthread.h>
#include <sys/time.h>
#include <chrono>
#include <mutex>

using namespace std;
using namespace std::chrono;

long double result = 0;
long double function(long double*arr, int size, long double value);
void *sum(void *thread_addr);

std::mutex mut;

struct thread_data { // have the info of thread
    long double * arr;
    int size;
    long double square;
    int p;
    int gap_num;
};

int main(int argc, char** argv)
{
    high_resolution_clock::time_point start = high_resolution_clock::now();
    int process = atoi(argv[1]);
    int square = atoi(argv[2]);
    int degree = atoi(argv[3]);
    int gap = (square-1) / process + 1;
    long double *arr = new long double[degree+1];

	for(int i=0; i<=degree;i++)
		arr[i] = atof(argv[i+4]);
    void * status;
    int rc;

    pthread_t threads[process]; // make the threads
    pthread_attr_t attr;
    struct thread_data *td;
    td = new struct thread_data[process];


   pthread_attr_init(&attr);
   pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    for(int i=0; i<process; i++)
    {   
        td[i].arr = arr;
        td[i].gap_num = gap;
        td[i].p = i*gap;
        td[i].size = degree;
        td[i].square = square;
        rc = pthread_create(&threads[i], NULL, sum, (void*)&td[i]); //create the thread that excutes the function
      if (rc) {
         cout << "Error:unable to create thread," << rc << endl;
         exit(-1);
      }
    }
     pthread_attr_destroy(&attr);
   for(int i = 0; i < process; i++ ) {
      rc = pthread_join(threads[i], &status);
      if (rc) {
         cout << "Error:unable to join," << rc << endl;
         exit(-1);
      }
   }

	cout << fixed;
	cout.precision(4);
    high_resolution_clock::time_point finish = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(finish-start).count();
	cout << "Riemann sum: " << result << endl;
    cout << "Total time: "<< duration<<endl;

	return 0;
}

long double function(long double*arr, int size, long double value) // get the value of f(x)
{
	long double result_2 = 0;
	for (int i = 0; i <size; i++)
		result_2 += arr[i] * pow(value,size-i);
	result_2 += arr[size];
	return result_2;
}

void *sum(void *thread_addr) //get the sum of Rieman from p_num to gap
{
    struct thread_data *from;
    from = (struct thread_data *)thread_addr; // get the info from the thread
    long double square = from->square;
    int p_num = from->p;
    int gap = from->gap_num;
    long double* arr = from->arr;
    int size = from->size;
    long double tmp=0;
	long double delta = 1000 / square;
    int des = p_num+gap;
    if(des>square) // coordinate the gap of last section
        des = square;
	for (int i = p_num; i < des; i++)
		tmp += function(arr, size, i*delta)*delta;
    mut.lock();
    result += tmp; // synchronize the shared memory by using mutex
    mut.unlock();

    pthread_exit(NULL);
}