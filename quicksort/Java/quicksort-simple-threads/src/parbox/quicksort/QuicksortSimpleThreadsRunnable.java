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

import java.util.ArrayList;
import java.util.List;

/**
 *  A runnable that implements the quicksort algorithm. The constructor takes the typical parameter of a quicksort
 *  and a integer for the maximum amount of recursions. If the recursions parameter is 0, no addition threads are
 *  started, but the next parts of the arrays are processes within this thread.
 *
 *  @author Stephan Rehfeld
 */
public class QuicksortSimpleThreadsRunnable implements Runnable {

    /**
     * The array to sort.
     */
    private final double[] values;

    /**
     * The index where to begin.
     */
    private final int begin;

    /**
     * The index where to end.
     */
    private final int end;

    /**
     * Amount of recursions.
     */
    private final int recursions;


    /**
     * The recursive implementation of quicksort.
     *
     * @param values The array to sort.
     * @param begin The index where to begin.
     * @param end The index where to end.
     * @param recursions Amount of recursions.
     */
    public QuicksortSimpleThreadsRunnable( final double[] values, final int begin, final int end, final int recursions ) {
        this.values = values;
        this.begin = begin;
        this.end = end;
        this.recursions = recursions;
    }

    @Override
    public void run() {
        this.quicksort( values, begin, end, recursions );
    }

    /**
     * The recursive implementation of quicksort.
     *
     * @param values The array to sort.
     * @param begin The index where to begin.
     * @param end The index where to end.
     * @param recursions Amount of recursions.
     */
    private void quicksort( final double[] values, final int begin, final int end, final int recursions ) {
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

        if( recursions > 0 ) {
            final List< Thread > threads = new ArrayList< Thread >();
            if( l - begin > 1 ) {
                final Runnable runnable = new QuicksortSimpleThreadsRunnable( values, begin, l- 1, recursions - 1 );
                threads.add( new Thread( runnable ) );
            }
            if( end - l > 1 ) {
                final Runnable runnable = new QuicksortSimpleThreadsRunnable( values, l + 1, end, recursions - 1 );
                threads.add( new Thread( runnable ) );
            }
            for( final Thread thread : threads ) thread.start();
            try {
                for( final Thread thread : threads ) thread.join();
            } catch ( final InterruptedException e ) {
                e.printStackTrace();
            }
        } else {
            if( l - begin > 1 ) quicksort( values, begin, l- 1, recursions );
            if( end - l > 1 ) quicksort( values, l + 1, end, recursions );
        }

    }

}
