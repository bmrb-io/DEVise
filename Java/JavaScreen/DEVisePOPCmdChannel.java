import  java.net.*;
import  java.io.*;
import  java.util.*;

public class DEVisePOPCmdChannel implements Runnable
{
    DEVisePOPCmdServerSocket cmdServer = null;
    DEViseCmdSocket cmdSocket = null;
    String hostName = null;
    int portNum = 0;
    boolean isExit = true;
    //boolean isError = false;

    public DEVisePOPCmdChannel(DEVisePOPCmdServerSocket what, Socket socket) throws IOException
    {
        cmdServer = what;
        cmdSocket = new DEViseCmdSocket(socket);
        hostName = socket.getInetAddress().getHostName();
    }

    public void close()
    {
        setStatus(true);
        cmdServer.decreaseCount(this);

        try {
            if (cmdSocket != null) {
                cmdSocket.closeSocket();
                cmdSocket = null;
            }
        } catch (YError e) {
            System.out.println("Can not close socket connection to client from host " + hostName);
        }
    }

    public int getNumber()
    {
        return portNum;
    }

    public void setNumber(int num)
    {
        portNum = num;
    }

    public String getHostName()
    {
        if (hostName != null)
            return hostName;
        else
            return "No Host";
    }

    public synchronized boolean getStatus()
    {
        return isExit;
    }

    public synchronized void setStatus(boolean flag)
    {
        isExit = flag;
    }

    public void run()
    {
        if (!cmdServer.increaseCount(this)) {
            try {
                setStatus(true);
                cmdSocket.sendCmd("JAVAC_Fail", Globals.API_JAVA);
                close();
            } catch (YError e) {
                System.out.println("Communication Error with " + hostName +" : " + e.getMessage());
            }
        } else {
            try {
                setStatus(false);
                cmdSocket.sendCmd("JAVAC_Connect " + cmdServer.getCmdPort() + " " + cmdServer.getImgPort(), Globals.API_JAVA);
            } catch (YError e) {
                System.out.println("Communication Error with " + hostName +" : " + e.getMessage());
            }
        }

        while (!getStatus()) {
            try {
                String cmd = cmdSocket.receiveRsp(false);

                System.out.println("Received: " + cmd);
                if (cmd.equals("JAVAC_Exit")) {
                    System.out.println("Client from host " + hostName + " quitting!");
                    close();
                } else if (cmd.equals("JAVAC_Done")) {
                    // Last operation is successful, do nothing
                } else if (cmd.equals("JAVAC_Fail")) {
                    // Last operation is failed
                    System.out.println("Client from host " + hostName + " send failing message!");
                    close();
                } else {
                    // do nothing
                }
            } catch (YError e) {
                System.out.println("Communication Error with " + hostName + " : " + e.getMessage());
                close();
            }
        }
    }

}
