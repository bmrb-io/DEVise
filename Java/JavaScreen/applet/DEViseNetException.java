// Define error happened while communicate with DEVise Server, nonrecoverable
public class DEViseNetException extends Exception
{
    public DEViseNetException()
    {
        super("DEViseNetException has been throwed!");
    }
    
    public DEViseNetException(String s)
    {
        super(s);
    }
}
