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
      
        setBackground(UIGlobals.uibgcolor);
        setForeground(UIGlobals.uifgcolor);
        setFont(UIGlobals.uifont);
        setLayout(new FlowLayout(FlowLayout.CENTER));
        viewName.setBackground(UIGlobals.textbgcolor);
        viewName.setForeground(UIGlobals.textfgcolor);
        viewName.setFont(UIGlobals.textfont);
        mouseX.setBackground(UIGlobals.textbgcolor);
        mouseX.setForeground(UIGlobals.textfgcolor);
        mouseX.setFont(UIGlobals.textfont);
        mouseY.setBackground(UIGlobals.textbgcolor);
        mouseY.setForeground(UIGlobals.textfgcolor);
        mouseY.setFont(UIGlobals.textfont);
        recordValue.setBackground(UIGlobals.textbgcolor);
        recordValue.setForeground(UIGlobals.textfgcolor);
        recordValue.setFont(UIGlobals.textfont);
        
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
