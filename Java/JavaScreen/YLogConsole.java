// YLogConsole.java
// last updated on 03/27/99


public class YLogConsole
{
    private int loglevel;

    public YLogConsole(int level)
    {
        loglevel = level;
    }

    public void p(String msg, int level)
    {
        if (level > loglevel)
            return;
        else
            System.out.print(msg);
    }

    public void p(String msg)
    {
        p(msg, 1);
    }

    public void pn(String msg, int level)
    {
        if (level > loglevel)
            return;
        else
            System.out.println(msg);
    }

    public void pn(String msg)
    {
        pn(msg, 1);
    }
}
  