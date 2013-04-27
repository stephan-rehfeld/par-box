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

package parbox.quicksort;

import java.util.Random;


/**
 * A small benchmark application that benchmarks a threaded quicksort implementation. It sorts a data array that
 * contains values of type double and measures the time it takes to sort it. The size of the data array is passed as
 * command line argument.
 *
 * @author Stephan Rehfeld
 */
public class QuicksortSimpleThreads {

    /**
     * The main method of the application. It's expected that the size of the data array is passed as command line
     * argument.
     *
     * @param args The command line arguments.
     */
    public static void main( final String[] args ) throws InterruptedException {
        final Random random = new Random( 23 );
        final int size = Integer.parseInt( args[0] );
        final double[] a = new double[size];
        System.out.print( "Generating data array: " );
        for( int i = 0; i < a.length; ++i ) {
            a[i] = random.nextDouble();
        }
        System.out.println( "Done" );

        System.out.print( "Sorting with simple threaded quicksort: " );
        long begin = System.nanoTime();
        final Runnable runnable = new QuicksortSimpleThreadsRunnable( a, 0, a.length - 1, 9 );
        final Thread thread = new Thread( runnable );
        thread.start();

        thread.join();
        long end = System.nanoTime();
        System.out.println( "Done" );

        System.out.println( "Serial quicksort took: " + ( ((double)(end-begin)) / 1000000000.0 ) + "s" );
    }

}
