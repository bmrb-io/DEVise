import java.io.*;
import java.net.*;
import java.util.*;

public class DEViseClient
{
    private DEViseUser user = null;
    private Integer connectID = null;
    private String hostname = null;
    private DEViseCmdSocket cmdSocket = null;
    private DEViseImgSocket imgSocket = null;

    public String savedSessionName = null;
    public boolean isSessionSaved = false;
    public boolean isSessionOpened = false;
    public boolean isSwitched = false;
    public boolean isSwitchSuccessed = false;

    public int dimx = 640;
    public int dimy = 480;

    private Vector cmdBuffer = new Vector();

    private long tot = 0; // total online time
    private long tst = 0; // total suspend time
    private long lot = 0; // last active time
    private long lst = 0; // last suspend time

    // status = -1, not connected
    // status = 0, not active
    // status = 1, active
    private int status = 0;

    public DEViseClient(DEViseUser u, DEViseCmdSocket cmd, DEViseImgSocket img, String h, int id)
    {
        user = u;
        cmdSocket = cmd;
        imgSocket = img;
        hostname = h;
        connectID = new Integer(id);

        savedSessionName = "jstmp_" + id;

        lst = YGlobals.getTime();
        lot = lst;
        status = 0;
    }

    public synchronized int getStatus()
    {
        return status;
    }

    private synchronized void setStatus(int s)
    {
        status = s;
    }

    public DEViseUser getUser()
    {
        return user;
    }

    public Integer getID()
    {
        return connectID;
    }

    public String getHost()
    {
        return hostname;
    }

    public void setSockets(DEViseCmdSocket cmd, DEViseImgSocket img)
    {
        cmdSocket = cmd;
        imgSocket = img;
    }

    public synchronized void insertCmd(String cmd, int pos) throws YException
    {
        if (cmd == null)
            return;

        if (pos < 0 || pos > cmdBuffer.size())
            pos = cmdBuffer.size();

        if (cmd.startsWith("JAVAC_Abort")) {
            cmdBuffer.removeAllElements();
            cmdBuffer.insertElementAt(cmd, 0);
        } else {
            cmdBuffer.insertElementAt(cmd, pos);
            // tell client that command has been received
            sendCmd(new String[] {"JAVAC_Ack"});
        }
    }

    public synchronized void insertCmd(String cmd) throws YException
    {
        insertCmd(cmd, -1);
    }

    public synchronized String getCmd()
    {
        if (cmdBuffer.size() > 0)
            return (String)cmdBuffer.firstElement();
        else
            return null;
    }

    public synchronized void removeCmd()
    {
        if (cmdBuffer.size() > 0)
            cmdBuffer.removeElementAt(0);
    }

    public synchronized void removeAllCmds()
    {
        cmdBuffer.removeAllElements();
    }

    public void receiveCmd(boolean isWait) throws YException, InterruptedIOException
    {
        if (cmdSocket != null) {
            if (isWait) {
                String cmd = cmdSocket.receiveRsp();
                insertCmd(cmd);                
                while (!cmdSocket.isEmpty()) {
                    cmd = cmdSocket.receiveRsp();
                    insertCmd(cmd);
                }
            } else {
                if (!cmdSocket.isEmpty()) {
                    String cmd = cmdSocket.receiveRsp();
                    insertCmd(cmd);
                    while (!cmdSocket.isEmpty()) {
                        cmd = cmdSocket.receiveRsp();
                        insertCmd(cmd);
                    }
                }
            }
        } else {
            throw new YException("Null cmd socket of client " + hostname + "!", 2);
        }
    }

    public void sendCmd(String[] cmds) throws YException
    {
        if (cmds == null)
            return;

        if (cmdSocket == null)
            throw new YException("Null cmd socket of client " + hostname + "!", 2);

        for (int i = 0; i < cmds.length; i++) {
            if (cmds[i] != null) {
                cmdSocket.sendCmd(cmds[i]);
            }
        }
    }

    public void sendImg(Vector images) throws YException
    {
        if (images == null)
            return;

        if (imgSocket == null)
            throw new YException("Null img socket of client " + hostname + "!", 4);

        for (int i = 0; i < images.size(); i++) {
            byte[] image = (byte[])images.elementAt(i);
            if (image != null && image.length > 0) {
                imgSocket.sendImg(image);
            }
        }
    }

    public synchronized void setSuspend()
    {
        if (status == 1) {
            long time = YGlobals.getTime();
            tot += time - lot;
            lst = time;

            status = 0;
        }
    }

    public synchronized void setActive()
    {
        if (status == 0) {
            long time = YGlobals.getTime();
            tst += time - lst;
            lot = time;

            status = 1;
        }
    }

    public synchronized void closeSocket()
    {
        if (status < 0)
            return;

        if (cmdSocket != null) {
            cmdSocket.closeSocket();
            cmdSocket = null;
        }

        if (imgSocket != null) {
            imgSocket.closeSocket();
            imgSocket = null;
        }

        long time = YGlobals.getTime();
        if (status == 1) {
            tot += time - lot;
        } else {
            tst += time - lst;
        }

        lst = time;

        status = -1;
        isSessionOpened = false;
        removeAllCmds();
    }

    public synchronized void close()
    {
        if (cmdSocket != null) {
            cmdSocket.closeSocket();
            cmdSocket = null;
        }

        if (imgSocket != null) {
            imgSocket.closeSocket();
            imgSocket = null;
        }

        if (status == 1) {
            long time = YGlobals.getTime();
            tot += time - lot;
        }

        if (user != null && connectID != null) {
            user.addTOT(tot);
            user.removeClient(connectID);
        }

        isSessionSaved = false;
        isSessionOpened = false;
        savedSessionName = null;
        removeAllCmds();
        status = -1;
    }

    // 0: not request
    // 1: request
    // -1: socket is dead
    public synchronized int isRequest()
    {
        if (cmdSocket == null)
            return -1;

        try {
            if (!cmdSocket.isEmpty()) {
                return 1;
            } else {
                if (cmdBuffer.size() > 0)
                    return 1;
                else
                    return 0;
            }
        } catch (YException e) {
            return -1;
        }
    }
}