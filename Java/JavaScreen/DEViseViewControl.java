import java.awt.*;
import java.awt.event.*;

public class DEViseViewControl extends Panel
{   
    private jsdevisec jsc = null;
    
    Button [] button = null;
    ComponentPanel panel = null;
     
    public DEViseViewControl(jsdevisec what, String[] label)
    {
        jsc = what;
      
        setBackground(DEViseGlobals.uibgcolor);
        setForeground(DEViseGlobals.uifgcolor);
        setFont(DEViseGlobals.uifont);
        setLayout(new GridLayout(1, 0));
        
        if (label == null) {
            button = new Button[1];
            button[0] = new Button("No Gdata Available");
        } else {            
            button = new Button[label.length];
            for (int i = 0; i < label.length; i++) 
                button[i] = new Button(label[i]);
        }

        panel = new ComponentPanel(button, "Vertical", 2);
        add(panel);                    
    }

    public void updateControl(String label[])
    {
        remove(panel);

        if (label == null) {
            button = new Button[1];
            button[0] = new Button("No Gdata Available");
        } else {            
            button = new Button[label.length];
            for (int i = 0; i < label.length; i++) 
                button[i] = new Button(label[i]);
        }

        panel = new ComponentPanel(button, "Vertical", 2);
        add(panel);
        validate();
    }
}
