// YException.java
// last updated on 03/27/99


public class YException extends Exception
{
    // indicate which level this exception is
    private int id;
    // indicate where this exception happened
    private String where = null;

    public YException()
    {
        this("YException been throwed!", null, 0);
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
        super(s);

        where = w;
        id = i;
    }

    public int getID()
    {
        return id;
    }

    public String getWhere()
    {
        if (where == null)
            return new String("program");
        else
            return where;
    }

    public String getMsg()
    {
        if (where != null) {
            return getMessage() + " in " + where;
        } else {
            return getMessage();
        }
    }
}

