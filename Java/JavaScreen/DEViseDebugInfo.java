import  java.awt.*;
import  java.io.*;

// Frame window used to output debug information 
public final class DEViseDebugInfo extends Frame
{
    private static TextArea textArea = null;
    private static BufferedWriter logFile = null;
    private static boolean isLog = false;
    private static boolean isDebug = true;
    private static boolean isGUI = true;
    
    public DEViseDebugInfo(boolean arg1, boolean arg2, boolean arg3, String name)
    {   
        isDebug = arg1;
        isGUI = arg2;
        isLog = arg3;
        
        if (isDebug && isGUI)  {            
            textArea = new TextArea(10, 50);            
            textArea.setFont(new Font("Monospaced", Font.PLAIN, 14));
            textArea.setEditable(false);
            add("Center", textArea);

            setTitle("DEVise Client Debug Information");
            pack();
            show();
        }
        
        if (isLog) {
            if (name == null)
                name = new String("devise.log");
                            
            try  {
                logFile = new BufferedWriter(new FileWriter(name));
            }  catch (IOException e)  {
                logFile = null;                
                println("Can not open logfile " + name + "\n" + e.getMessage());
            }
        }
    }        
    
    public static void println(String msg)
    {   
        if (isDebug) {
            if (isGUI) {
                textArea.append(msg + "\n");
            } else {
                System.out.println(msg);
            }
        } 
        
        if (logFile != null && isLog) {
            try  {
                logFile.write(msg, 0, msg.length());
                logFile.newLine(); 
            }  catch (IOException e) {
                closeLogfile("Can not write to logfile!");
            }    
        } 
    }
    
    public static void print(String msg)
    {
        if (isDebug) {
            if (isGUI) {
                textArea.append(msg);
            } else {
                System.out.print(msg);
            }
        } 
        
        if (logFile != null && isLog) {
            try  {
                logFile.write(msg, 0, msg.length());
            }  catch (IOException e) {
                closeLogfile("Can not write to logfile!");
            }    
        }
    }
        
    public static void closeLogfile(String msg)
    {    
        isLog = false;
        if (msg != null)        
            println(msg);

        if (logFile == null)  {
            return;
        }  else  {
            try  {
                logFile.close();
            }  catch (IOException e)  {
                println("Can not close logfile!\n" + e.getMessage());                    
            }
                
            logFile = null;
            return;
        }
    }
    
    public void close()
    {
        closeLogfile(null);
        dispose();
    }               
}
