#include <iostream>
#include <unistd.h>
#include <math.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <mutex>
#include <sys/ipc.h>
#include <chrono>

using namespace std;
using namespace std::chrono;


std::mutex mut;

long double function(long double *arr, int size, long double value);
long double sum(long double * arr, int size, long double square, int p_num, int gap);
int cntw=0;
long double * result_v;
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
    

    int *pid;
    pid = (int*)malloc(sizeof(int)*process); // make the array of child processes
	long double result = 0;
    long double tmp;
    const int key = 1234;
    const int size = 4096;
    int q, shm_fd;
    int *ptr;
    ptr = new int[process];
    shm_fd = shmget(key, 1024, 0644|IPC_CREAT);
    result_v = (long double *)shmat(shm_fd, NULL, 0);
    for(int i=0; i<250; i++)
        result_v[i] = 0;
    


    for(int i=0; i<process;i++)
    {
        pid[i] = fork();
        
        if((pid[i]==0)) // make child process & child's working 
        {   
            mut.lock(); // synchronize
            tmp = sum(arr, degree, square, i*gap, gap);
            result_v[i%250]+=tmp; // to control the synchronize
            mut.unlock();
            exit(1);  
        }
        else if(pid[i]<0)
        {
            perror("");
        }
        
        
    }
    for(int i=0; i<process; i++)
        wait((int*)0);
    for(int i=0; i<250; i++)
        result += result_v[i];
    
    cout << fixed;
    cout.precision(4);
    high_resolution_clock::time_point finish = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(finish-start).count();

	cout << "Riemann sum: " << result<< endl;
    cout << "Total time: " << duration <<endl;
    shmdt(result_v);
    shmctl(shm_fd, 0, 0);
	return 0;
}

long double function(long double *arr, int size, long double value) // get the value of f(x)  
{
	long double result = 0;
	for (int i = 0; i <size; i++)
		result += arr[i] * pow(value,size-i);
	result += arr[size];
	return result;
}

long double sum(long double* arr, int size, long double square, int p_num, int gap) //get the sum of Rieman from p_num to gap
{
	long double delta = 1000 / square;
	long double result = 0;
    int des = p_num+gap;

    if(des>square) // coordinate the gap of last section
        des = square;
    if(p_num>square)
        p_num = square;
	for (int i = p_num; i < des; i++)
		result += function(arr, size, i*delta)*delta;
	return result;
}