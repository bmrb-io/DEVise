package gjt;

/**
 * Client of a Stopwatch.  Stopwatches that have non-null 
 * clients, call their clients' tick() method every 50 
 * milliseconds.<p>
 *
 * @version 1.0, Apr 21 1996
 * @version 1.1, Nov 8  1996
 *
 *  INTERFACE CHANGE:  Modified tick() so that it takes a reference
 *                     to the associated Stopwatch.
 *
 * @author  David Geary
 * @see     Stopwatch
 */
public interface StopwatchClient {
    public void tick(Stopwatch stopwatch);
}
