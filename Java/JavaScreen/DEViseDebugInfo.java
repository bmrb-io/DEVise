import  java.awt.*;
import  java.io.*;

// Frame window used to output debug information 
public final class DEViseDebugInfo extends Frame
{
    private static TextArea textArea = null;
    private static BufferedWriter debugFile = null;
    
    public DEViseDebugInfo()
    {   
        if (DEViseGlobals.ISDEBUG)  {            
            textArea = new TextArea(10, 50);            
            textArea.setFont(new Font("Monospaced", Font.PLAIN, 14));
            textArea.setEditable(false);
            add("Center", textArea);

            setTitle("DEVise Client Debug Information");
            pack();
            show();
            
            try  {
                debugFile = new BufferedWriter(new FileWriter("debug.out"));
            }  catch (IOException e)  {
                debugFile = null;
                DEViseGlobals.showMsg(this, "Can not open debug.out for writing\n" + e.getMessage(), "Warning");
            }
        }
    }        
            
    public static void println(String msg)
    {   
        if (DEViseGlobals.ISDEBUG)
            textArea.append(msg + "\n");
    }
    
    public static void printfile(String msg)
    {
        if (DEViseGlobals.ISDEBUG)  {
            if (debugFile == null)  {
                return;
            }  else  {
                try  {
                    debugFile.write(msg, 0, msg.length());
                    debugFile.newLine();
                }  catch (IOException e)  {
                    println("Can not write to debugfile with message: " + msg);
                }    
            }
        }
    }
    
    public static void closefile()
    {
        if (DEViseGlobals.ISDEBUG)  {
            if (debugFile == null)  {
                return;
            }  else  {
                try  {
                    debugFile.close();
                }  catch (IOException e)  {
                    System.out.println("Can not close debugfile!\n");                    
                }
                
                debugFile = null;
                return;
            }
        }
    }
    
    public static void clearAll()
    {
        if (DEViseGlobals.ISDEBUG)
            textArea.setText("");
    }
}
