/*
 * Copyright 2013 Stephan Rehfeld
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <algorithm>

void quicksort(  double* const values, const int begin, const int end, const int recursions  );

int main()
{
	srand( 23 );
	const int size = 10000000; 
	std::cout << size << std::endl;
	double* const a = (double*)malloc( sizeof( double ) * size );
	std::cout << "Generating data array: ";
	for( int i = 0; i < size; ++i ) {
		a[i] = rand();
	}
	std::cout << "Done" << std::endl;

	std::cout << "Sorting with parallel quicksort: ";

	std::chrono::time_point<std::chrono::system_clock> start, end;
	start = std::chrono::system_clock::now();
	
	quicksort( a, 0, size-1, 9 );

    end = std::chrono::system_clock::now();
    std::cout << "Done" << std::endl;
	
	__int64 elapsed_milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();

    std::cout << "Parallel quicksort took: " << ( ((double)(elapsed_milliseconds)) / 1000 ) << "s" << std::endl;
	int x = 0;
	std::cin >> x;
	free( a );
	return 0;
}

void quicksort(  double* const values, const int begin, const int end, const int recursions ) {
	const int pivot = end;
	int l = begin;
	int r = end;
	while( l != r ) {
		while( values[l] <= values[pivot] && l != r ) ++l;
		while( values[r] >= values[pivot] && l != r ) --r;
		if( l != r ) {
			const double temp = values[l];
			values[l] = values[r];
			values[r] = temp;
		}
	}
	if( l != pivot ) {
		const double temp = values[l];
		values[l] = values[pivot];
		values[pivot] = temp;
	}
	if( recursions > 0 ) {
		std::vector<std::thread> threads;

		if( l - begin > 1 ) {
			std::thread t(quicksort, values, begin, l- 1, recursions -1 );
			threads.push_back( std::move( t ) );
		}
		if( end - l > 1 ) {
			std::thread t(quicksort, values, l + 1, end, recursions - 1 );
			threads.push_back( std::move( t ) );
		}

		std::for_each( threads.begin(), threads.end(), [](std::thread &t ) {
			t.join();
		});

	} else {
		if( l - begin > 1 ) quicksort( values, begin, l- 1, recursions -1 );	
		if( end - l > 1 ) quicksort( values, l + 1, end, recursions - 1 );
	}
}
