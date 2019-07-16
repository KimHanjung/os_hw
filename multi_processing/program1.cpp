#include <iostream>
#include <math.h>
#include <cstdlib>
#include <sys/time.h>
#include <cstring>
#include <chrono>

using namespace std;
using namespace std::chrono;

long double function(long double *arr, int size, long double value);
long double sum(long double * arr, int size, long double square);

int main(int argc, char** argv)
{
    high_resolution_clock::time_point start = high_resolution_clock::now();
    int square = atoi(argv[1]);
    int degree = atoi(argv[2]);
	long double *arr = new long double[degree+1];

	for(int i=0; i<=degree;i++)
		arr[i] = atof(argv[i+3]);
    
	long double result;
	result = sum(arr, degree , square);
	cout << fixed;
	cout.precision(4);
    high_resolution_clock::time_point finish = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(finish-start).count();
	cout << "Riemann sum: " << result << endl;
	cout << "Total time: " << duration << endl;

	return 0;
}

long double function(long double*arr, int size, long double value) // get the value of f(x)
{
	long double result = 0;
	for (int i = 0; i <size; i++)
		result += arr[i] * pow(value,size-i);
	result += arr[size];
	return result;
}

long double sum(long double * arr, int size, long double square) // get the sum of Rieman from 0 to square-1
{
	long double delta = 1000 / square;
	long double result = 0;
	for (int i = 0; i < square; i++)
		result += function(arr, size, i*delta)*delta;
	return result;
}