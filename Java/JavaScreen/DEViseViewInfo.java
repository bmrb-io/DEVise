import java.awt.*;
import java.awt.event.*;

public class DEViseViewInfo extends Panel
{   
    private jsdevisec jsc = null;
    
    TextField winName = new TextField(15);
    TextField viewName = new TextField(15);
    TextField mouseX = new TextField(4), mouseY = new TextField(4);
    TextField recordValue = new TextField(15);
    TextField recordName = new TextField(15);
     
    public DEViseViewInfo(jsdevisec what)
    {
        jsc = what;
      
        setBackground(UIGlobals.uibgcolor);
        setForeground(UIGlobals.uifgcolor);
        setFont(UIGlobals.uifont);

        // set layout manager
        GridBagLayout  gridbag = new GridBagLayout();
        GridBagConstraints  c = new GridBagConstraints();       
        setLayout(gridbag); 
        //c.gridx = GridBagConstraints.RELATIVE;
        //c.gridy = GridBagConstraints.RELATIVE;
        //c.gridwidth = GridBagConstraints.REMAINDER;
        //c.gridheight = 1;
        c.fill = GridBagConstraints.NONE;
        c.insets = new Insets(5, 10, 5, 0);
        //c.ipadx = 0;
        //c.ipady = 0;
        //c.anchor = GridBagConstraints.CENTER;
        c.weightx = 1.0;
        c.weighty = 1.0;

        //setLayout(new FlowLayout(FlowLayout.CENTER));
        winName.setBackground(UIGlobals.textbgcolor);
        winName.setForeground(UIGlobals.textfgcolor);
        winName.setFont(UIGlobals.textfont);
        viewName.setBackground(UIGlobals.textbgcolor);
        viewName.setForeground(UIGlobals.textfgcolor);
        viewName.setFont(UIGlobals.textfont);
        mouseX.setBackground(UIGlobals.textbgcolor);
        mouseX.setForeground(UIGlobals.textfgcolor);
        mouseX.setFont(UIGlobals.textfont);
        mouseY.setBackground(UIGlobals.textbgcolor);
        mouseY.setForeground(UIGlobals.textfgcolor);
        mouseY.setFont(UIGlobals.textfont);
        recordName.setBackground(UIGlobals.textbgcolor);
        recordName.setForeground(UIGlobals.textfgcolor);
        recordName.setFont(UIGlobals.textfont);
        recordValue.setBackground(UIGlobals.textbgcolor);
        recordValue.setForeground(UIGlobals.textfgcolor);
        recordValue.setFont(UIGlobals.textfont);
        
        c.insets = new Insets(5, 15, 5, 0);
        Label label = new Label("Active Window:");
        gridbag.setConstraints(label, c);
        add(label);
        c.insets = new Insets(5, 0, 5, 10);
        gridbag.setConstraints(winName, c);
        add(winName);
        c.insets = new Insets(5, 15, 5, 0);
        label = new Label("Active View:");
        gridbag.setConstraints(label, c);
        add(label);
        c.insets = new Insets(5, 0, 5, 10);
        gridbag.setConstraints(viewName, c);
        add(viewName);
        c.insets = new Insets(5, 15, 5, 0);
        label = new Label("Mouse X:");
        gridbag.setConstraints(label, c);
        add(label);
        c.insets = new Insets(5, 0, 5, 10);
        gridbag.setConstraints(mouseX, c);
        add(mouseX);
        c.insets = new Insets(5, 15, 5, 0);
        label = new Label("Mouse Y:");
        gridbag.setConstraints(label, c);
        add(label);
        c.gridwidth = GridBagConstraints.REMAINDER;
        c.insets = new Insets(5, 0, 5, 15);
        gridbag.setConstraints(mouseY, c);
        add(mouseY);
        c.gridwidth = 1;        
        c.insets = new Insets(5, 15, 5, 0);
        label = new Label("Record Name:");
        gridbag.setConstraints(label, c);
        add(label);
        c.insets = new Insets(5, 0, 5, 10);
        gridbag.setConstraints(recordName, c);
        add(recordName);
        c.insets = new Insets(5, 15, 5, 0);
        label = new Label("Record Value:");
        gridbag.setConstraints(label, c);
        add(label);
        c.insets = new Insets(5, 0, 5, 15);
        gridbag.setConstraints(recordValue, c);
        add(recordValue);
    }

    public void updateInfo(String wname, String vname, int x, int y, String rname, String rvalue)
    {   
        if (wname == null)
            winName.setText("");
        else
            winName.setText(wname);

        if (vname == null)
            viewName.setText("");
        else
            viewName.setText(vname);
                
        mouseX.setText(String.valueOf(x));
        mouseY.setText(String.valueOf(y));

        if (rname == null)
            recordName.setText("");
        else
            recordName.setText(rname);

        if (rvalue == null)
            recordValue.setText("");
        else
            recordValue.setText(rvalue);
        
        validate();
    } 
    
    public void updateInfo(String wname, String vname, int x, int y)
    {
        updateInfo(wname, vname, x, y, null, null);
    }
    
    public void updateInfo(int x, int y)
    { 
        mouseX.setText(String.valueOf(x));
        mouseY.setText(String.valueOf(y));
        validate();
    }
    
    public void updateInfo(String rname, String rvalue)
    {
        if (rname == null)
            recordName.setText("");
        else
            recordName.setText(rname);

        if (rvalue == null)
            recordValue.setText("");
        else
            recordValue.setText(rvalue);
        
        validate();
    }
    
    public void updateInfo()
    {   
        winName.setText("");
        viewName.setText("");
        mouseX.setText("");
        mouseY.setText("");
        recordName.setText("");
        recordValue.setText("");
        validate();
    }    
}
