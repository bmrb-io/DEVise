import  java.io.*;
import  java.net.*;

// Socket used to send and receive Image data between DEViseServer and DEViseClient
public class DEViseImgSocket
{   
    private Socket socket = null;
    private DataInputStream is = null;
    private DataOutputStream os = null;
    //private BufferedOutputStream os = null;
        
    public DEViseImgSocket(Socket arg) throws IOException
    {
        socket = arg;        
        os = new DataOutputStream(new BufferedOutputStream(new DataOutputStream(socket.getOutputStream())));
        //os = new BufferedOutputStream(socket.getOutputStream(), 512 * 1024);
        is = new DataInputStream(socket.getInputStream());
    }
               
    public DEViseImgSocket(String hostname, int port) throws UnknownHostException, IOException
    {
        socket = new Socket(hostname, port); 
        os = new DataOutputStream(new BufferedOutputStream(new DataOutputStream(socket.getOutputStream())));
        //os = new BufferedOutputStream(socket.getOutputStream(), 512 * 1024);
        is = new DataInputStream(socket.getInputStream());
    }
          
    public void closeSocket() throws DEViseNetException
    {
        try  {                               
            os.close();
            is.close();
            socket.close();
        }  catch (IOException e)  {
            throw new DEViseNetException("DEVise Image Socket Error: Communication Error occured while closing socket connection!");
        }
    } 
    
    public void sendImg(byte[] imageData) throws DEViseNetException
    {   
        try {
            os.write(imageData, 0, imageData.length);
            os.flush();
        } catch (IOException e) {
            throw new DEViseNetException("DEVise Image Socket Error: Communication Error occured while sending image data!");
        }
    }
    
    public byte[] receiveImg(int imageSize) throws DEViseNetException
    {   
        byte[] imageData = new byte[imageSize];
        
        try {
            is.readFully(imageData);
        } catch (IOException e) {
            throw new DEViseNetException("DEVise Image Socket Error: Communication Error occured while receiving image data!");
        }
        
        return imageData;
    }  
}
 
