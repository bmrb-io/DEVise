import java.io.*;
import java.util.*;

public class DEViseLogWriter implements Runnable
{
    jspop pop = null;

    Thread writeThread = null;

    // status < 0, write thread stopped
    // status = 0, write thread is running but idle
    // status = 1, write thread is writing to log file
    int status = -1;
    int timestep = 60 * 1000; // minimum is 1 minutes
    // action = 0, stop writer
    // action = 1, writer normal state
    int action = 1;

    public DEViseLogWriter(jspop j)
    {
        pop = j;

        if (pop.logThreadTimestep > timestep) {
            timestep = pop.logThreadTimestep;
        }
    }

    public synchronized void startWriter()
    {
        if (status < 0) {
            action = 1;
            status = 0;
            writeThread = new Thread(this);
            writeThread.start();
        }
    }

    public synchronized void stopWriter()
    {
        if (status < 0) {
            return;
        } else {
            action = 0;
            notifyAll();
        }
    }

    private synchronized void setStatus(int s)
    {
        status = s;
    }

    public synchronized int getStatus()
    {
        return status;
    }

    public synchronized int getAction()
    {
        return action;
    }

    private synchronized void waitForAction()
    {
        status = 0;
        try {
            wait(timestep);
        } catch (InterruptedException e) {
        }
    }

    public void run()
    {
        while (getAction() > 0) {
            waitForAction();
            if (getAction() > 0) {
                setStatus(1);
            }
        }

        setStatus(-1);
    }
}
