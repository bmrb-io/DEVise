// YError.java
// last updated on 07/30/99


public class YError extends Error
{
    private static String defaultMsg = "YError has been throwed!";

    public YError()
    {
        this(null, null);
    }

    public YError(String s)
    {
        this(s, null);
    }

    public YError(String s, String w)
    {
        super( ((s == null)?defaultMsg:s) + ((w == null)?"":(" in " + w)) );
    }
}

