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
        
	    showStatus("Successfully get symbols of DEVise1");

        try  {
            cmdSocket = new DEViseCmdSocket(DEViseGlobals.DEVISEHOST, DEViseGlobals.CMDPORT);
            try {
                String rsp = cmdSocket.receiveRsp(false);
                if (rsp.equals("JAVAC_Fail")) {
                    showStatus("Connection to DEVise Server is rejected!");
		            return;
                }
            } catch (DEViseNetException e) {
                showStatus("Communication error: " + e.getMessage());
                return;
            }
            
            imgSocket = new DEViseImgSocket(DEViseGlobals.DEVISEHOST, DEViseGlobals.IMGPORT);
            try {
                String rsp = cmdSocket.receiveRsp(false);
                if (rsp.equals("JAVAC_Fail")) {
                    showStatus("Can not connect to DEVise Image Server!");
                    return;
                }
            } catch (DEViseNetException e) {
                showStatus("Communication error: " + e.getMessage());
                return;
            }
        }  catch (UnknownHostException e)  {
            showStatus("Can not find " + DEViseGlobals.DEVISEHOST);
            return;
        }  catch (IOException e)  {
            showStatus("Can not open connection to DEVise server at " + DEViseGlobals.DEVISEHOST);
            return;
        }
        
        showStatus("Successfully connect to Server command and image socket!");

        client = new jsdevisec(cmdSocket, imgSocket, images); 
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
