Introduction:
-------------
Welcome to the GJT1.1.  This is a version of the 1.1 GJT that works with 
the 1.1 JDK, and will not work or compile with previous versions of the JDK.

The GJT is a collection of custom components built on the AWT - it is
not a replacement for the AWT, but instead augments the capabilities
the AWT provides.  The GJT provides gauges, rubberbanding, splash screens,
image filters, sprite animation, convenience dialogs, borders, image buttons, 
toolbars, bubble help, custom layout managers, and more.

The GJT is freely available.  Although we recommend that you buy the
book Graphic Java, it is not necessary to buy the book in order to use the
GJT.  We have included unit tests for every component in the GJT, so you
have simple, working examples that you can use to guide you in using
the capabilities the GJT offers.

Known Bugs:
-----------

	Windows95:  Applets aren't laid out when the window is shown.

	Solaris:  bubble help for image buttons in ToolbarTest do not show
	          up until the 2nd time the image button is entered.  Also,
              the right and bottom pixels of the bubble do not get drawn.

	Solaris:  DoubleList doesn't layout properly when a scrollbar is added to 
	          the left-hand list.

	Solaris:  Splash screen appears intermittently.

Bug Reports And Enhancement Requests:
-------------------------------------
Please send bug reports and enhancement requests to sabreware@earthlink.com

What's New:
-----------
See the whatsNew file in this directory to find out what's new with the 
1.1 GJT, including new capabilities and bug fixes.

Future Plans:
-------------
Graphic Java Beans is under development!  We expect to have a beanified
version of the GJT out summer of 1997.  Watch comp.lang.java.announce for 
details.

License:
--------
The original license in the book Graphic Java did not pertain to the GJT.
We have included a new license that will appear in the 2nd edition of
Graphic Java that covers the GJT - see the license file in this directory.  
In plain English, you may use the GJT in any manner you desire - even 
distributed as part of a commercial product, as long as you do not sell the GJT 
as a standalone product.

See the license file in this directory for more information.

For Macintosh Users:
--------------------
Graphic Java supports the Mac!  Included on the CD is a 1.02 version of the
GJT (at the time we went to press the 1.1 JDK for the Mac was not out).  When
the Mac 1.1 JDK comes out, 1.1 GJT on the CD will work on the Mac.

DIRECTORY STRUCTURE:
-------------------
  The directory structure for the GJT and its packages looks like so:

	graphicJava_1.1
		gifs
		gjt
			animation
			image
			rubberband
			test
		javadoc
			images	
		lib

  The graphicJava_1.1 directory contains html files for all of the
Graphic Java Toolkit unit tests, 

  The graphicJava_1.1/javadoc directory contains all of the javadoc
output for the following packages: gjt, gjt.animation, 
gjt.image, gjt.rubberband, gjt.test.

  graphicJava_1.1/gifs contains gif files necessary to run the
unit tests contained in the graphicJava_1.1 directory.  Note that
the directory contains a bunch of way-cool images from Keith 
Ohlf's Pixelsite web site.  You can see more images from Keith 
at: http://www.pixelsight.com:80/PS/pixelsight/pixelsight.html.

  graphicJava_1.1/javadoc/images contains the images needed by the html
files that are output from javadoc.

  graphicJava_1.1/gjt, graphicJava_1.1/gjt/animation, 
graphicJava_1.1/gjt/image, graphicJava_1.1/gjt/rubberband and
graphicJava_1.1/gjt/test contain source code for the gjt, 
gjt.animation, gjt.image, gjt.rubberband, and gjt.test packages,
respectively.

  graphicJava_1.1/lib contains a zipped classes file:  classes.zip,
which contains the .class files for all of the gjt packages.

COMPILING THE GJT:
-----------------
  You must have the directory behind the gjt directory in your CLASSPATH.
For instance, if you have a directory structure: C:\java\graphicJava_1.1\gjt,
then you must have C:\java\graphicJava_1.1 in your CLASSPATH.

  This is due to the fact that whenever you do "import gjt.*;", the
compiler looks in each directory in your CLASSPATH for a directory with
the same name as the package - in this case gjt.

RUNNING UNIT TEST APPLETS:
-------------------------
  The Graphic Java Toolkit comes complete with unit tests for
every custom component contained in the toolkit.  In 
the graphicJava_1.1 directory, you will find the following html
files:

	BargaugeTest.html
	BlackAndWhiteFilterTest.html
	BleachImageFilterTest.html
	BorderTest.html
	BoxTest.html
	BulletinLayoutTest.html
	BumpAnimationTest.html
	ChoiceCardPanelTest.html
	ColumnLayoutTest.html
	ComponentScrollerTest.html
	DialogTest.html
	DoubleBufferedContainerTest.html
	DrawnRectangleTest.html
	FontDialogTest.html
	IconCardPanelTest.html
	ImageButtonTest.html
	ImageDissolverTest.html
	ImageScrollerTest.html
	LabelCanvasTest.html
	MessageDialogTest.html
	RubberbandTest.html
	SeparatorTest.html
	SimpleAnimationTest.html
	SimpleBargaugeTest.html
	StateButtonTest.html
	StopwatchTest.html
	ToolbarTest.html
	ToolboxTest.html
	TwoDrinkersAnimationTest.html
	VerticalBargaugeTest.html

	To run SplashScreenTest:  run java gjt.test.SplashScreenTest
	from the same directory you run the applets from.

  All of the html files listed above are designed to be used with
appletviewer.  Your CLASSPATH must include the classes.zip file
in graphicJava_1.1/lib AND the current directory.  Note that including the 
directory graphicJava_1.1/lib will not suffice:  you must include the file 
graphicJava_1.1/lib/classes.zip (along with the current directory)
