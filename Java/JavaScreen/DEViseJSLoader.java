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

import java.applet.*;
import java.awt.*;


/**
* This applet loads the applet specified in the appletToLoad
* String. Used to print a short message that Java Screen is 
* Loading, when the network connection is slow.
**/


public class DEViseJSLoader extends Applet implements Runnable, AppletStub
{
String appletToLoad;
Label label;
Thread appletThread;

public void init()
{
appletToLoad = "jsa";
if (appletToLoad == null) {
label = new Label("No applet to load.");
} else {
 label = new Label( " *** Please wait - Loading Applet for DEVise JavaScreen. ***  ");
 label.setFont(new Font("Helvetica", Font.BOLD, 16));
 label.setForeground(new Color(0.4f, 0.3f, 0.6f));
 label.setBackground(new Color(0.9f, 0.9f, 0.9f));

add(label);
}
}

public void run()
{
if (appletToLoad == null) return;

try {
  try{
     Thread.sleep(1000);
  }
  catch(InterruptedException e){
  }
Class appletClass = Class.forName(appletToLoad);
Applet realApplet = (Applet)appletClass.newInstance();
realApplet.setStub(this);

remove(label);
setLayout(new GridLayout(1, 0));
add(realApplet);
realApplet.init();
realApplet.start();
} catch (Exception e) {
label.setText("Error loading applet.");
}
validate();
}

public void start()
{
if(appletThread == null){
appletThread = new Thread(this);
}
appletThread.start();
}

public void stop()
{
appletThread.stop();
}

public void appletResize(int width, int height)
{
resize(width, height);
}

}
