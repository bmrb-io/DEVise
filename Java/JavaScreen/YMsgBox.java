// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 1999-2000
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// ADD COMMENT: overall description of the function of this class

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.5  1999/12/10 15:37:02  wenger
// Added standard headers to source files.
//
// ========================================================================


import java.awt.*;
import java.awt.event.*;
import java.util.*;

// for JDK 1.1, we can not build a dialog without a Frame as its parent, what
// if we want to display a dialog within an applet? there are basically three
// solutions:
// 1: extended message box from a Frame instead of a dialog
//    problem: might run into deadlock if we want the message box to be modal
//             because we can not create an event dispatch thread manually,
//             it is done automatically while you open a modal dialog, it will
//             block the calling thread(may be an event dispatch thread) and
//             create a new event dispatch thread to handling events
// 2: start another thread to display the message box
//    problem:
// 3: create an empty, temporary Frame as the parent of the message box
//    In my humble opinion, this is the best way to create a new event dispatch
//    thread and also block the calling thread, the only thing need to be careful
//    is to make the message box relocation to center of the screen instead of
//    the parent frame, because in this case the position and size of the parent
//    frame is meaningless
public class YMsgBox extends Dialog
{
    public static final String YIDNO = new String("No"), YIDYES = new String("Yes"),
                               YIDOK = new String("OK"), YIDCANCEL = new String("Cancel"),
                               YIDRETRY = new String("Retry"), YIDABORT = new String("Abort"),
                               YIDIGNORE = new String("Ignore");
    public static final int YMBXOK = 0, YMBXYESNO = 1, YMBXOKCANCEL = 2, YMBXYESNOCANCEL = 3,
                            YMBXRETRYCANCEL = 4, YMBXABORTRETRYIGNORE = 5;

    private Label label[] = null;
    private Button button[] = null;
    private String result = null;
    private boolean status = false;

    public YMsgBox(Frame owner, boolean isCenterScreen, boolean isModal, String msg, String title, int style, Font font, Color bg, Color fg)
    {
        super(owner, isModal);

        if (bg == null)
            bg = Color.lightGray;

        if (fg == null)
            fg = Color.black;

        if (font == null)
            font = new Font("Serif", Font.PLAIN, 14);

        if (title == null) {
            title = new String("Program Information");
        }

        setTitle(title);
        setResizable(false);
        setBackground(bg);
        setForeground(fg);
        setFont(font);

        boolean isStyleCorrect = true;

        switch (style) {
        case YMsgBox.YMBXOK:
            button = new Button[1];
            button[0] = new Button(YMsgBox.YIDOK);
            result = YMsgBox.YIDOK;
            break;
        case YMsgBox.YMBXYESNO:
            button = new Button[2];
            button[0] = new Button(YMsgBox.YIDYES);
            button[1] = new Button(YMsgBox.YIDNO);
            result = YMsgBox.YIDNO;
            break;
        case YMsgBox.YMBXOKCANCEL:
            button = new Button[2];
            button[0] = new Button(YMsgBox.YIDOK);
            button[1] = new Button(YMsgBox.YIDCANCEL);
            result = YMsgBox.YIDCANCEL;
            break;
        case YMsgBox.YMBXYESNOCANCEL:
            button = new Button[3];
            button[0] = new Button(YMsgBox.YIDYES);
            button[1] = new Button(YMsgBox.YIDNO);
            button[2] = new Button(YMsgBox.YIDCANCEL);
            result = YMsgBox.YIDCANCEL;
            break;
        case YMsgBox.YMBXRETRYCANCEL:
            button = new Button[2];
            button[0] = new Button(YMsgBox.YIDRETRY);
            button[1] = new Button(YMsgBox.YIDCANCEL);
            result = YMsgBox.YIDCANCEL;
            break;
        case YMsgBox.YMBXABORTRETRYIGNORE:
            button = new Button[3];
            button[0] = new Button(YMsgBox.YIDABORT);
            button[1] = new Button(YMsgBox.YIDRETRY);
            button[2] = new Button(YMsgBox.YIDIGNORE);
            result = YMsgBox.YIDIGNORE;
            break;
        default:
            isStyleCorrect = false;
            button = new Button[1];
            button[0] = new Button(YMsgBox.YIDOK);
            result = YMsgBox.YIDOK;
            break;
        }

        if (isStyleCorrect) {
            if (msg == null) {
                label = new Label[1];
                label[0] = new Label("No Message Given!");
            } else {
                if (msg.equals("")) {
                    label = new Label[1];
                    label[0] = new Label("Empty Message Given!");
                } else {
                    StringTokenizer token = new StringTokenizer(msg, "\n", false);
                    Vector tokenList = new Vector();
                    while (token.hasMoreTokens()) {
                        try {
                            tokenList.addElement(token.nextToken());
                        } catch (NoSuchElementException e) {
                            // this should not be happening, but just in case
                            tokenList = new Vector();
                            break;
                        }
                    }

                    if (tokenList.size() > 0) {
                        label = new Label[tokenList.size()];
                        for (int i = 0; i < label.length; i++) {
                            String msgStr = (String)tokenList.elementAt(i);
                            if (msgStr != null) {
                                label[i] = new Label(msgStr);
                            } else {
                                label[i] = new Label("");
                            }
                        }
                    } else {
                        label = new Label[1];
                        label[0] = new Label("Invalid message given!");
                    }
                }
            }
        } else {
            label = new Label[1];
            label[0] = new Label("Unsupported YMsgBox style given!");
        }

        // building the panel that display messages
        Panel panel1 = new Panel();
        panel1.setLayout(new GridLayout(0, 1, 0, 0));
        for (int i = 0; i < label.length; i++) {
            panel1.add(label[i]);
        }

        // building the panel that display buttons
        Panel panel2 = new Panel();
        panel2.setLayout(new GridLayout(1, 0, 10, 0));
        for (int i = 0; i < button.length; i++) {
            button[i].setBackground(bg);
            button[i].setForeground(fg);
            button[i].setFont(font);
            button[i].setActionCommand(button[i].getLabel());
            panel2.add(button[i]);
        }

        // set new gridbag layout
        GridBagLayout gridbag = new GridBagLayout();
        GridBagConstraints c = new GridBagConstraints();
        //c.gridx = GridBagConstraints.RELATIVE;
        //c.gridy = GridBagConstraints.RELATIVE;
        c.gridwidth = GridBagConstraints.REMAINDER;
        c.gridheight = 1;
        c.fill = GridBagConstraints.NONE;
        c.insets = new Insets(10, 10, 0, 0);
        c.ipadx = 0;
        c.ipady = 0;
        c.anchor = GridBagConstraints.CENTER;
        //c.weightx = 1.0;
        //c.weighty = 1.0;

        setLayout(gridbag);

        gridbag.setConstraints(panel1, c);
        add(panel1);

        c.insets = new Insets(10, 10, 5, 10);
        gridbag.setConstraints(panel2, c);
        add(panel2);

        pack();

        // reposition the window
        Point parentLoc = null;
        Dimension parentSize = null;

        if (isCenterScreen) {
            Toolkit kit = Toolkit.getDefaultToolkit();
            parentSize = kit.getScreenSize();
            parentLoc = new Point(0, 0);
        } else {
            parentLoc = owner.getLocation();
            parentSize = owner.getSize();
        }

        Dimension mysize = getSize();
        parentLoc.y += parentSize.height / 2;
        parentLoc.x += parentSize.width / 2;
        parentLoc.y -= mysize.height / 2;
        parentLoc.x -= mysize.width / 2;
        setLocation(parentLoc);

        // add event handler
        for (int i = 0; i < button.length; i++)  {
            button[i].addActionListener(new ActionListener()
                    {
                        public void actionPerformed(ActionEvent event)  {
                            result = event.getActionCommand();
                            close();
                        }
                    });
        }

        // enable destroying window
        this.enableEvents(AWTEvent.WINDOW_EVENT_MASK);
    }

    protected void processEvent(AWTEvent event)
    {
        if (event.getID() == WindowEvent.WINDOW_CLOSING) {
            close();
            return;
        }

        super.processEvent(event);
    }

    // If this dialog is a modal dialog, the show() or setVisible(true) method
    // will block current thread(i.e. the thread that access this method, may
    // be the event dispatcher thread) until setVisible(false) or dispose() is
    // called
    // In JDK1.1, any thread that access AWT method is acting as a event
    // dispatcher thread
    public void open()
    {
        status = true;
        setVisible(true);
    }

    public synchronized void close()
    {
        if (status) {
            dispose();

            status = false;
        }
    }

    public synchronized boolean getStatus()
    {
        return status;
    }

    public synchronized String getResult()
    {
        return result;
    }
}


