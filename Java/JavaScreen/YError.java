// Define exception might be recoverable
public class YError extends Exception
{
    public YError()
    {
        super("YError has been throwed!");
    }
    
    public YError(String s)
    {
        super(s);
    }
}
