package xman.xnav;

import java.awt.*;
import java.awt.event.*;
import java.io.*;
import javax.swing.*;

class ThreadRun extends Thread {

    protected SingleRunInterface singleRun;

    // Constructor
    public ThreadRun(ThreadGroup group, String name, int portNumber, String machineName) {
	super(group, name);
	this.singleRun = new SingleRunInterface(portNumber, machineName);
	singleRun.setSize(750, 600);
    }

    public SingleRunInterface getSingleRun() {
	return singleRun;
    }

    public void run() {}


}
