// YException.java
// last updated on 07/27/99


public class YException extends Exception
{
    // indicate which level this exception is
    private int id;
    // indicate where this exception happened
    private String where = null;
    // indicate the error message
    private String msg = null;

    private static String defaultMsg = "YException has been throwed!";

    public YException()
    {
        this(null, null, 0);
    }

    public YException(String s)
    {
        this(s, null, 0);
    }

    public YException(String s, String w)
    {
        this(s, w, 0);
    }

    public YException(String s, int i)
    {
        this(s, null, i);
    }

    public YException(String s, String w, int i)
    {
        super( ((s == null)?defaultMsg:s) + ((w == null)?"":(" in " + w)) );

        id = i;
        where = w;
        msg = s;
    }

    public int getID()
    {
        return id;
    }

    public String getWhere()
    {
        if (where == null)
            return new String("");
        else
            return where;
    }

    public String getMsg()
    {
        if (msg == null) {
            return new String("");
        } else {
            return msg;
        }
    }
}

