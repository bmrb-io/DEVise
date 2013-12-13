// This example is from the book "Java in a Nutshell, Second Edition".
// Written by David Flanagan.  Copyright (c) 1997 O'Reilly & Associates.
// You may distribute this source code for non-commercial purposes only.
// You may study, modify, and use this example for any purpose, as long as
// this notice is retained.  Note that this example is provided "as is",
// WITHOUT WARRANTY of any kind either expressed or implied.

import java.applet.*;
import java.awt.*;

public class Scribble extends Applet {
  private int last_x, last_y;                // Store the last mouse position.
  private Color current_color = Color.black; // Store the current color.
  private Button clear_button;               // The clear button.
  private Choice color_choices;              // The color dropdown list.

  // This method is called to initialize the applet.
  // Applets don't have a main() method.
  public void init() {
    // Set the background color
    this.setBackground(Color.white);

    // Create a button and add it to the applet.  Set the button's colors
    clear_button = new Button("Clear");
    clear_button.setForeground(Color.black);
    clear_button.setBackground(Color.lightGray);
    this.add(clear_button);

    // Create a menu of colors and add it to the applet.
    // Also set the menus's colors and add a label.
    color_choices = new Choice();
    color_choices.addItem("black");
    color_choices.addItem("red");
    color_choices.addItem("yellow");
    color_choices.addItem("green");
    color_choices.setForeground(Color.black);
    color_choices.setBackground(Color.lightGray);
    this.add(new Label("Color: "));
    this.add(color_choices);
  }

  // This method is called when the user clicks the mouse to start a scribble.
  public boolean mouseDown(Event e, int x, int y)
  {
    last_x = x; last_y = y;
    return true;
  }

  // This method is called when the user drags the mouse.
  public boolean mouseDrag(Event e, int x, int y)
  {
    Graphics g = this.getGraphics();
    g.setColor(current_color);
    g.drawLine(last_x, last_y, x, y);
    last_x = x;
    last_y = y;
    return true;
  }

  // This method is called when the user clicks the button or chooses a color
  public boolean action(Event event, Object arg) {
    // If the Clear button was clicked on, handle it.
    if (event.target == clear_button) {
      Graphics g = this.getGraphics();
      Rectangle r = this.bounds();
      g.setColor(this.getBackground());
      g.fillRect(r.x, r.y, r.width, r.height);
      return true;
    }
    // Otherwise if a color was chosen, handle that
    else if (event.target == color_choices) {
      if (arg.equals("black")) current_color = Color.black;
      else if (arg.equals("red")) current_color = Color.red;
      else if (arg.equals("yellow")) current_color = Color.yellow;
      else if (arg.equals("green")) current_color = Color.green;
      return true;
    }
    // Otherwise, let the superclass handle it.
    else return super.action(event, arg);
  }
}
