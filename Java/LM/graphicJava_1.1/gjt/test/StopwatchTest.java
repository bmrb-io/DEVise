package gjt.test;

import java.applet.Applet;
import java.awt.*;
import gjt.*;

/**
 * Unit test for the Stopwatch class.  This unit test creates 3 
 * LabelCanvases and 3 stopwatches.  Each LabelCanvas selects
 * and deselects at the rate of once per half second, once
 * per second and once every two seconds.  The selections and
 * deselections are controlled by 3 stopwatches.
 * 
 * @version 1.0, Fri Oct 25 1996
 * @author  David Geary
 * @see     gjt.test.UnitTest
 * @see     gjt.Stopwatch
 * @see		gjt.StopwatchClient
 */
public class StopwatchTest extends UnitTest {
    public String title() { 
        return "Stopwatch Test"; 
    }
    public Panel centerPanel() { 
        return new StopwatchTestPanel();
    }
}

class StopwatchTestPanel extends Panel 
                         implements StopwatchClient {
    private Stopwatch   oneStopwatch, twoStopwatch, 
	                    oneHalfStopwatch;

	private LabelCanvas twoCanvas, oneCanvas, 
	                    oneHalfCanvas;

    public StopwatchTestPanel() {
		twoStopwatch           = new Stopwatch(this, 2000);
		oneStopwatch           = new Stopwatch(this, 1000);
		oneHalfStopwatch       = new Stopwatch(this, 500);

		oneCanvas     = new LabelCanvas("One Second");
		oneHalfCanvas = new LabelCanvas("One Half Second");
		twoCanvas     = new LabelCanvas("Two Seconds");

		add(oneCanvas);	
		add(oneHalfCanvas);
		add(twoCanvas);

		oneStopwatch.start();
		oneHalfStopwatch.start();
		twoStopwatch.start();
	}
	public void tick(Stopwatch stopwatch) {
		LabelCanvas blinkMe = null;

		if(stopwatch == twoStopwatch)     blinkMe = twoCanvas;
		if(stopwatch == oneStopwatch)     blinkMe = oneCanvas;
		if(stopwatch == oneHalfStopwatch) 
			blinkMe = oneHalfCanvas;

		if(blinkMe.isSelected()) blinkMe.deselect();
		else                     blinkMe.select  ();
	}
}
