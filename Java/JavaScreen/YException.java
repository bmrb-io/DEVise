// Define exception might be recoverable
public class YException extends Exception
{
    public YException()
    {
        super("YException has been throwed!");
    }
    
    public YException(String s)
    {
        super(s);
    }
}
