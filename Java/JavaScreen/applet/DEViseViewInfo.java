import java.awt.*;
import java.awt.event.*;

public class DEViseViewInfo extends Panel
{   
    private jsdevisec jsc = null;
    
    TextField viewName = new TextField(15);
    TextField mouseX = new TextField(4), mouseY = new TextField(4);
    TextField recordValue = new TextField(15);
     
    public DEViseViewInfo(jsdevisec what)
    {
        jsc = what;
      
        setBackground(DEViseGlobals.uibgcolor);
        setForeground(DEViseGlobals.uifgcolor);
        setFont(DEViseGlobals.uifont);
        setLayout(new FlowLayout(FlowLayout.CENTER));
        viewName.setBackground(DEViseGlobals.textbgcolor);
        viewName.setForeground(DEViseGlobals.textfgcolor);
        viewName.setFont(DEViseGlobals.textfont);
        mouseX.setBackground(DEViseGlobals.textbgcolor);
        mouseX.setForeground(DEViseGlobals.textfgcolor);
        mouseX.setFont(DEViseGlobals.textfont);
        mouseY.setBackground(DEViseGlobals.textbgcolor);
        mouseY.setForeground(DEViseGlobals.textfgcolor);
        mouseY.setFont(DEViseGlobals.textfont);
        recordValue.setBackground(DEViseGlobals.textbgcolor);
        recordValue.setForeground(DEViseGlobals.textfgcolor);
        recordValue.setFont(DEViseGlobals.textfont);
        
        add(new Label("Current View Name:"));
        add(viewName);
        add(new Label("      Mouse Position:"));
        add(new Label("X"));
        add(mouseX);
        add(new Label("Y"));
        add(mouseY);
        add(new Label("      Record Value:"));
        add(recordValue);
    }

    public void updateInfo(String name, int x, int y)
    {
        viewName.setText(name);
        mouseX.setText(String.valueOf(x));
        mouseY.setText(String.valueOf(y));
        validate();
    } 
    
    public void updateInfo(int x, int y)
    { 
        mouseX.setText(String.valueOf(x));
        mouseY.setText(String.valueOf(y));
        validate();
    }
    
    public void updateInfo(float value)
    {
        recordValue.setText(String.valueOf(value));
        validate();
    }    
}
