import java.awt.*;
import java.io.*;

public final class YDebug
{
    private static YGUIMsg guimsg = null;
    private static BufferedWriter logfile = null;

    public YDebug()
    {
        if (YGlobals.YISDEBUG && YGlobals.YISGUI) {
            guimsg = new YGUIMsg();
        }

        if (YGlobals.YISLOG) {
            try {
                logfile = new BufferedWriter(new FileWriter("debug.log"));
            } catch (IOException e) {
                logfile = null;
            }
        }
    }

    public synchronized static void println(String msg, int id)
    {
        if (id == 0) {
            if (YGlobals.YISDEBUG) {
                if (YGlobals.YISGUI && guimsg != null) {
                    guimsg.println(msg);
                } else {
                    System.out.println(msg);
                }
            }
        } else if (id == 1) {
            if (YGlobals.YISLOG) {
                if (logfile != null) {
                    try {
                        logfile.write(msg, 0, msg.length());
                        logfile.newLine();
                    } catch (IOException e) {
                        closeLogFile();
                    }
                }
            }
        } else if (id == 2) {
            if (YGlobals.YISDEBUG) {
                if (YGlobals.YISGUI && guimsg != null) {
                    guimsg.println(msg);
                } else {
                    System.out.println(msg);
                }
            }

            if (YGlobals.YISLOG) {
                if (logfile != null) {
                    try {
                        logfile.write(msg, 0, msg.length());
                        logfile.newLine();
                    } catch (IOException e) {
                        closeLogFile();
                    }
                }
            }
        } else {
        }
    }

    public synchronized static void println(String msg)
    {
        println(msg, 2);
    }

    public synchronized static void print(String msg, int id)
    {
        if (id == 0) {
            if (YGlobals.YISDEBUG) {
                if (YGlobals.YISGUI && guimsg != null) {
                    guimsg.print(msg);
                } else {
                    System.out.print(msg);
                }
            }
        } else if (id == 1) {
            if (YGlobals.YISLOG) {
                if (logfile != null) {
                    try {
                        logfile.write(msg, 0, msg.length());
                    } catch (IOException e) {
                        closeLogFile();
                    }
                }
            }
        } else if (id == 2) {
            if (YGlobals.YISDEBUG) {
                if (YGlobals.YISGUI && guimsg != null) {
                    guimsg.print(msg);
                } else {
                    System.out.print(msg);
                }
            }

            if (YGlobals.YISLOG) {
                if (logfile != null) {
                    try {
                        logfile.write(msg, 0, msg.length());
                    } catch (IOException e) {
                        closeLogFile();
                    }
                }
            }
        } else {
        }
    }

    public synchronized static void print(String msg)
    {
        print(msg, 2);
    }

    private static void closeLogFile()
    {
        if (logfile != null) {
            try {
                logfile.close();
            } catch (IOException e) {
            }

            logfile = null;
        }
    }

    private static void closeGUI()
    {
        if (guimsg != null) {
            guimsg.close();
            guimsg = null;
        }
    }

    public synchronized static void close()
    {
        closeLogFile();
        closeGUI();
    }
}

final class YGUIMsg extends Frame
{
    private static TextArea textArea = null;

    public YGUIMsg()
    {
        this(10, 50);
    }

    public YGUIMsg(int x, int y)
    {
        textArea = new TextArea(x, y);
        textArea.setFont(new Font("Monospaced", Font.PLAIN, 14));
        textArea.setEditable(false);
        add("Center", textArea);

        setTitle("Debug Information");
        pack();
        show();
    }

    public void print(String msg)
    {
        textArea.append(msg);
    }

    public void println(String msg)
    {
        textArea.append(msg + "\n");
    }

    public void clear()
    {
        textArea.setText("");
    }

    public void close()
    {
        dispose();
    }
}

