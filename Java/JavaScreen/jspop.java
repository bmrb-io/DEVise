import  java.io.*;
import  java.net.*;
import  java.util.*;

public class jspop
{   
    public static void main(String[] args)    
    {
        String version = System.getProperty("java.version");
        if (version.compareTo("1.1") < 0)  {
            System.out.println("Error: Java version 1.1 or greater is needed to run this program\n" 
                               + "The version you used is " + version);
            System.exit(1);
        }
        
        String usage = new String("usage: java jspop [-CP[port]] [-IP[port]] [-DP[port]]");
        
        if (args.length > 3)  {
            System.out.println(usage);
            System.exit(1);
        }
        
        for (int i = 0; i < args.length; i++)  {
            if (args[i].startsWith("-CP"))  {
                if (!args[i].substring(3).equals(""))  {
                    try  {
                        Globals.POPCMDPORT = Integer.parseInt(args[i].substring(3));
                        if (Globals.POPCMDPORT < 1 || Globals.POPCMDPORT > 65535)
                            throw new NumberFormatException();
                    }  catch (NumberFormatException e)  {
                        System.out.println("Invalid port number " + Globals.POPCMDPORT + " used in -CP field!\n");
                        System.out.println(usage);
                        System.exit(1);
                    }
                }
            }  else if (args[i].startsWith("-IP"))  {
                if (!args[i].substring(3).equals(""))  {
                    try  {
                        Globals.POPIMGPORT = Integer.parseInt(args[i].substring(3));
                        if (Globals.POPIMGPORT < 1 || Globals.POPIMGPORT > 65535)
                            throw new NumberFormatException();
                    }  catch (NumberFormatException e)  {
                        System.out.println("Invalid port number " + Globals.POPIMGPORT + " used in -IP field!\n");
                        System.out.println(usage);
                        System.exit(1);
                    }
                }
            }  else if (args[i].startsWith("-DP"))  {        
                if (!args[i].substring(3).equals(""))  {
                    try  {
                        Globals.POPDATAPORT = Integer.parseInt(args[i].substring(3));
                        if (Globals.POPDATAPORT < 1 || Globals.POPDATAPORT > 65535)
                            throw new NumberFormatException();
                    }  catch (NumberFormatException e)  {
                        System.out.println("Invalid port number " + Globals.POPDATAPORT + " used in -DP field!\n");
                        System.out.println(usage);
                        System.exit(1);
                    }
                }
            }  else  {
                System.out.println(usage);
                System.exit(1);
            }
        }
    
        DEVisePOPCmdServerSocket cmdServerSocket = null;
        //DEViseImgServerSocket imgServerSocket = null;
        try  {
            cmdServerSocket = new DEVisePOPCmdServerSocket();
            //imgServerSocket = new DEViseImgServerSocket(cmdServerSocket);
        }  catch (IOException e)  {
            System.out.println("Can not open server socket at local host");
            System.exit(1);
        }
        
        Thread cmdServerThread = new Thread(cmdServerSocket);
        cmdServerThread.start();
        //Thread imgServerThread = new Thread(imgServerSocket);
        //imgServerThread.start();       
    }
}
