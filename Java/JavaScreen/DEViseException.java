// Define exception might be recoverable
public class DEViseException extends Exception
{
    public DEViseException()
    {
        super("DEViseException has been throwed!");
    }
    
    public DEViseException(String s)
    {
        super(s);
    }
}
