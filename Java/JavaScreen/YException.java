public class YException extends Exception
{
    protected int id = 0;

    public YException()
    {
        super("YException has been throwed!");
    }

    public YException(String s)
    {
        super(s);
    }

    public YException(String s, int i)
    {
        super(s);
        id = i;
    }

    public int getID()
    {
        return id;
    }
}

