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

package parbox;

import java.util.Random;

/**
 * A small benchmark application that benchmarks a serial quicksort implementation. It sorts a data array that contains
 * values of type double and measures the time it takes to sort it. The size of the data array is passed as command
 * line argument.
 *
 * @author Stephan Rehfeld
 */
public class Quicksort {

    /**
     * The main method of the application. It's expected that size of the data array is passed as command line argument.
     *
     * @param args The command line arguments.
     */
    public static void main( final String[] args ) {
        final Random random = new Random( 23 );
        final int size = Integer.parseInt( args[0] );
        final double[] a = new double[size];
        System.out.print( "Generating data array: " );
        for( int i = 0; i < a.length; ++i ) {
            a[i] = random.nextDouble();
        }
        System.out.println( "Done" );

        System.out.print( "Sorting with serial quicksort: " );
        long begin = System.nanoTime();
        quicksort( a, 0, a.length - 1 );
        long end = System.nanoTime();
        System.out.println( "Done" );

        System.out.println( "Serial quicksort took: " + ( ((double)(end-begin)) / 1000000000.0 ) + "s" );
    }

    /**
     * The recursive implementation of quicksort.
     *
     * @param values The array to sort.
     * @param begin The index where to begin.
     * @param end The index where to end.
     */
    public static void quicksort( final double[] values, final int begin, final int end ) {
        int pivot = end;
        int l = begin;
        int r = end;
        while( l != r ) {
            while( values[l] <= values[pivot] && l != r ) ++l;
            while( values[r] >= values[pivot] && l != r ) --r;
            if( l != r ) {
                final double temp = values[l];
                values[l] = values[r];
                values[r] = temp;
            }
        }
        if( l != pivot ) {
            final double temp = values[l];
            values[l] = values[pivot];
            values[pivot] = temp;
        }

        if( l - begin > 1 ) quicksort( values, begin, l- 1 );
        if( end - l > 1 ) quicksort( values, l + 1, end );

    }


}
