import java.io.*;

public final class YLogFile
{
    private static BufferedWriter logFile = null;
    private static String logFileName = null;

    YLogFile()
    {
        // No Log Information is written
    }

    public YLogFile(String arg) throws YException
    {
        if (arg == null)
            logFileName = new String("js-defaults.log");
        else
            logFileName = arg;

        try {
            logFile = new BufferedWriter(new FileWriter(logFileName));
        } catch (IOException e) {
            logFile = null;
            throw new YException("Can not open log file " + logFileName);
        }
    }

    public static void println(String msg)
    {
        if (logFile != null) {
            try {
                logFile.write(msg, 0, msg.length());
                logFile.newLine();
            } catch (IOException e) {
                // do nothing now
            }
        }
    }

    public static void print(String msg)
    {
        if (logFile != null) {
            try {
                logFile.write(msg, 0, msg.length());
            }  catch (IOException e) {
                // do nothing now
            }
        }
    }

    public static void close()
    {
        if (logFile != null) {
            try {
                logFile.close();
            }  catch (IOException e) {
                // do nothing now
            }

            logFile = null;
        }
    }

    protected void finalize()
    {
        close();
    }
}

