import  java.awt.*;

public final class YDebug
{
    private static boolean isdebug = false;
    private static boolean isgui = false;
    private static YGUIMsg guimsg = null;

    public YDebug()
    {
        // no debug information is written
    }

    public YDebug(boolean arg)
    {
        isdebug = true;
        isgui = arg;

        if (isgui) {
            guimsg = new YGUIMsg();
        }
    }

    public static void println(String msg)
    {
        if (isdebug) {
            if (isgui) {
                guimsg.print(msg + "\n");
            } else {
                System.out.print(msg + "\n");
            }
        }
    }

    public static void print(String msg)
    {
        if (isdebug) {
            if (isgui) {
                guimsg.print(msg);
            } else {
                System.out.print(msg);
            }
        }
    }

    public static void close()
    {
        if (isgui) {
            guimsg.close();
        }
    }
    
    public static void setVisible(boolean flag)
    {
        if (isgui) {
            guimsg.setVisible(flag);
        }
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

    public void clear()
    {
        textArea.setText("");
    }

    public void close()
    {
        dispose();
    }

    protected void finalize()
    {
        close();
    }
}

