import  java.net.*; 
import  java.io.*;   
import  java.util.*;

public class DEViseImgServerSocket extends ServerSocket implements Runnable
{
    private boolean isListen = true;
    private DEViseCmdServerSocket cmdServer = null;
   
    public DEViseImgServerSocket(DEViseCmdServerSocket what) throws IOException
    {
        super(DEViseGlobals.IMGPORT);
        cmdServer = what;
    } 
    
    public void run()
    { 
        System.out.println("DEVise IMG Server started ...\n");
        System.out.println("DEVise Server waiting for client connection ...");
        System.out.println("(" + cmdServer.getClients().size() + " out of " + cmdServer.maxClients + " channels already been used)\n");
        while (isListen)  {
            Socket socket = null;
            try  {
                socket = accept();
            }  catch (IOException e)  {
                System.out.println("DEVise IMG Server can not listen on port " + DEViseGlobals.IMGPORT + "\nDEVise IMG Server is aborted!");
                isListen = false;
            }
            
            String hostName = socket.getInetAddress().getHostName();
            Vector clients = cmdServer.getClients();
            DEViseSDataChannel client = null;
            for (int i = 0; i < clients.size(); i++)  {
                if (hostName.equals(((DEViseSDataChannel)clients.elementAt(i)).getHostName())) {
                    client = (DEViseSDataChannel)clients.elementAt(i);
                    client.putImgSocket(socket);
                    break;
                }
            }
            
            // this situation should not occurs
            if (client == null)  {
                System.out.println("Something is wrong!");
                System.exit(1);
            }
        }
    }    
}
