import java.io.*;
import java.net.*;

public class jsd
{
    public static String usage = new String("java jsd -port <cmdport> -imageport <imgport> -switchport <switchport>");
    public int cmdPort, imgPort, switchPort;
    public ServerSocket cmdSocket = null;
    public ServerSocket imgSocket = null;
    public ServerSocket switchSocket = null;
    public Thread cmdThread = null;
    public Thread imgThread = null;
    public Thread switchThread = null;

    public static void main(String[] args)
    {
        if (args.length != 6) {
            System.out.println("Incorrect number of arguments!");
            System.out.println(usage);
            System.exit(1);
        }

        if (!args[0].equals("-port") || !args[2].equals("-imageport") || !args[4].equals("-switchport")) {
            System.out.println("Incorrect command line option!");
            System.out.println(usage);
            System.exit(1);
        }

        try {
            int cp = Integer.parseInt(args[1]);
            int ip = Integer.parseInt(args[3]);
            int sp = Integer.parseInt(args[5]);

            if (cp < 1024 || cp > 65535 || ip < 1024 || ip > 65535 || sp < 1024 || sp > 65535)
                throw new NumberFormatException();

            jsd dd = new jsd(cp, ip, sp);
        } catch (NumberFormatException e) {
            System.out.println("Incorrect command line option!");
            System.out.println(usage);
            System.exit(1);
        }
    }

    public jsd(int cp, int ip, int sp)
    {
        cmdPort = cp;
        imgPort = ip;
        switchPort = sp;

        try {
            cmdSocket = new ServerSocket(cmdPort);
            imgSocket = new ServerSocket(imgPort);
            switchSocket = new ServerSocket(switchPort);
        } catch (IOException e) {
            System.out.println("Can not start server socket at port: " + cp + " " + ip + " " + sp);
            System.exit(1);
        }

        cmdThread = new Thread(new CmdServer(cmdSocket));
        cmdThread.start();
        imgThread = new Thread(new ImgServer(imgSocket));
        imgThread.start();
        switchThread = new Thread(new SwitchServer(switchSocket));
        switchThread.start();
    }
}

class CmdServer implements Runnable
{
    ServerSocket server = null;

    public CmdServer(ServerSocket socket)
    {
        server = socket;
    }

    public void run()
    {
        boolean isListen = true;

        System.out.println("CmdServer is started ...");
        while (isListen)  {
            Socket socket = null;
            try  {
                socket = server.accept();
                String name = socket.getInetAddress().getHostName();
                System.out.println("Client connection request to CmdServer from " + name + " is received.");
            }  catch (IOException e)  {
                System.out.println("CmdServer can not listen on port!");
                isListen = false;
            }
        }
    }
}

class ImgServer implements Runnable
{
    ServerSocket server = null;

    public ImgServer(ServerSocket socket)
    {
        server = socket;
    }

    public void run()
    {
        boolean isListen = true;

        System.out.println("ImgServer is started ...");
        while (isListen)  {
            Socket socket = null;
            try  {
                socket = server.accept();
                String name = socket.getInetAddress().getHostName();
                System.out.println("Client connection request to ImgServer from " + name + " is received.");
            }  catch (IOException e)  {
                System.out.println("ImgServer can not listen on port!");
                isListen = false;
            }
        }
    }
}

class SwitchServer implements Runnable
{
    ServerSocket server = null;

    public SwitchServer(ServerSocket socket)
    {
        server = socket;
    }

    public void run()
    {
        boolean isListen = true;

        System.out.println("SwitchServer is started ...");
        while (isListen)  {
            Socket socket = null;
            try  {
                socket = server.accept();
                String name = socket.getInetAddress().getHostName();
                System.out.println("Client connection request to SwitchServer from " + name + " is received.");
            }  catch (IOException e)  {
                System.out.println("SwitchServer can not listen on port!");
                isListen = false;
            }
        }
    }
}

