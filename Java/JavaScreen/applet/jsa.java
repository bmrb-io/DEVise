import  java.applet.*;
import  java.awt.*;
import  java.net.*;
import  java.io.*;
import  java.util.*;

public class jsa extends Applet
{
    jsdevisec client = null;

    public void init()    
    {
        DEViseCmdSocket cmdSocket = null;
        DEViseImgSocket imgSocket = null;
        URL url = getDocumentBase();           
        DEViseGlobals.DEVISEHOST = url.getHost();
	    showStatus(DEViseGlobals.DEVISEHOST);
                            
        MediaTracker tracker = new MediaTracker(this);
        //Toolkit toolkit = this.getToolkit();
        Vector images = new Vector();
        Image image = null;
        for (int i = 0; i < 4; i++)  {        
            image = getImage(url, "../images/devise" + i + ".gif");
            tracker.addImage(image, 0);
            try  {
                tracker.waitForID(0);
            }  catch (InterruptedException e)  {
            }
        
            if (tracker.isErrorID(0)) {
                showStatus("Can not get symbols of DEVise!");
		        return;
            }
                                                      
            images.addElement(image);
        }
        
	    showStatus("Successfully get symbols of DEVise");
        
        int myID = 0;
        try  {
            cmdSocket = new DEViseCmdSocket(DEViseGlobals.DEVISEHOST, DEViseGlobals.CMDPORT);
            try {   
                myID = cmdSocket.receiveInt();
                
                if (myID == 0) {
                    System.out.println("Connection to DEVise Server is rejected!");
                    System.exit(0);
                }
            } catch (DEViseNetException e) {
                System.out.println(e.getMessage());
                System.exit(1);
            }
            
            imgSocket = new DEViseImgSocket(DEViseGlobals.DEVISEHOST, DEViseGlobals.IMGPORT);
            try {
                imgSocket.sendInt(myID);
                String rsp = cmdSocket.receiveRsp(false);
                if (!rsp.equals("JAVAC_Done")) {
                    System.out.println("Can not connect to DEVise Image Server!");
                    System.exit(0);
                }
            } catch (DEViseNetException e) {
                System.out.println("Communication error: " + e.getMessage());
                System.exit(1);
            }
        }  catch (UnknownHostException e)  {
            System.out.println("Can not find " + DEViseGlobals.DEVISEHOST);
            System.exit(1);
        }  catch (IOException e)  {
            System.out.println("Can not open connection to DEVise server at " + DEViseGlobals.DEVISEHOST);
            System.exit(1);
        }
        
        showStatus("Successfully connect to Server command and image socket!");

        client = new jsdevisec(cmdSocket, imgSocket, images, myID); 
    }

    public void destroy()
    {
        if (client != null) {
            client.quit();
	        client.dispose();
	        client = null;
	    }

	    super.destroy();
    }
}
