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

#include "stdafx.h"
#include <iostream>
#include <cstdlib>
#include <malloc.h>
#include <ctime>

void quicksort(  double* const values, const int begin, const int end );

int _tmain(int argc, _TCHAR* argv[])
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

	std::cout << "Sorting with serial quicksort: ";
	clock_t begin = clock();
	quicksort( a, 0, size - 1 );
    clock_t end = clock();
    std::cout << "Done" << std::endl;

    std::cout << "Serial quicksort took: " << ( ((double)(end-begin)) / CLOCKS_PER_SEC ) << "s" << std::endl;
	int x = 0;
	std::cin >> x;
	free( a );
	return 0;
}

void quicksort(  double* const values, const int begin, const int end ) {
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
	if( l - begin > 1 ) quicksort( values, begin, l- 1 );	
	if( end - l > 1 ) quicksort( values, l + 1, end );
}

