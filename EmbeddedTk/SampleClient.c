
#include <iostream.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>
#include "EmbeddedTk.h"
#include "ETkPatron.h"
#include "ETkCompDate.h"

void Help();
void Demo(char *server, int port);
istream &eatwhite(istream &is);
static int MyWrite(int fd, char *buff, int num);
static int MyRead(int fd, char *buff, int num);
static int newlinefd(char *s, int fd, int maxc);

void
Help()
{
    cout << "ETk commands:" << endl;
    cout << "  demo (runs a demo)" << endl;
    cout << "  show file window x y width height anchor argc" << endl;
    cout << "    (use window = 0 to create a toplevel window)" << endl;
    cout << "  free handle" << endl;
    cout << "  eval handle argc" << endl;
    cout << "  move handle x y" << endl;
    cout << "  resize handle w h" << endl;
    cout << "  moveresize handle x y w h" << endl;
    cout << "  map handle" << endl;
    cout << "  unmap handle" << endl;
    cout << "  status" << endl;
    cout << "  quit (kills server)" << endl;
    cout << "  exit" << endl;
    cout << "  help (or \"?\")" << endl;
}

void
Demo(char *server, int port)
{
    int fd;
    char cmd[ETK_MAX_STRLEN];
    char errbuf[ETK_MAX_STRLEN];
    char reply[ETK_MAX_STRLEN];
    char c;
    int handle, handle2;
    int delay = 3;
    int windowID;

    //
    // Test the status message
    //
    if ((fd = ETkPatron(server, port, errbuf)) < 0)
    {
	cout << "Could not connect to port " << port << " on "
	    << server << endl;
	return;
    }
    sprintf(cmd, "status");
    cout << "\nSending command: " << cmd << endl;
    MyWrite(fd, cmd, strlen(cmd));
    MyWrite(fd, "\n", 1);
    newlinefd(reply, fd, ETK_MAX_STRLEN);
    cout << "Reply: " << reply << endl;
    sleep(delay);
    
    //
    // Create a window
    //
    if ((fd = ETkPatron(server, port, errbuf)) < 0)
    {
	cout << "Could not connect to port " << port << " on "
	    << server << endl;
	return;
    }
    sprintf(cmd, "show ETkSample.tcl 0 100 100 200 200 c 3");
    cout << "\nSending command: " << cmd << endl;
    cout << "  argv: \"ABC\" \"123\" \"Do Re Mi\"" << endl;
    MyWrite(fd, cmd, strlen(cmd));
    MyWrite(fd, "\n", 1);
    MyWrite(fd, "ABC\n", 4);
    MyWrite(fd, "123\n", 4);
    MyWrite(fd, "Do Re Mi\n", 9);
    newlinefd(reply, fd, ETK_MAX_STRLEN);
    cout << "Reply: " << reply << endl;
    sscanf(reply, "%*s %d", &handle);
    sleep(delay);
    
    //
    // Move the window
    //
    if ((fd = ETkPatron(server, port, errbuf)) < 0)
    {
	cout << "Could not connect to port " << port << " on "
	    << server << endl;
	return;
    }
    sprintf(cmd, "move %d 200 500", handle);
    cout << "\nSending command: " << cmd << endl;
    MyWrite(fd, cmd, strlen(cmd));
    MyWrite(fd, "\n", 1);
    newlinefd(reply, fd, ETK_MAX_STRLEN);
    cout << "Reply: " << reply << endl;
    sleep(delay);
    
    //
    // Resize the window
    //
    if ((fd = ETkPatron(server, port, errbuf)) < 0)
    {
	cout << "Could not connect to port " << port << " on "
	    << server << endl;
	return;
    }
    sprintf(cmd, "resize %d 300 300", handle);
    cout << "\nSending command: " << cmd << endl;
    MyWrite(fd, cmd, strlen(cmd));
    MyWrite(fd, "\n", 1);
    newlinefd(reply, fd, ETK_MAX_STRLEN);
    cout << "Reply: " << reply << endl;
    sleep(delay);
    
    //
    // Unmap the window
    //
    if ((fd = ETkPatron(server, port, errbuf)) < 0)
    {
	cout << "Could not connect to port " << port << " on "
	    << server << endl;
	return;
    }
    sprintf(cmd, "unmap %d", handle);
    cout << "\nSending command: " << cmd << endl;
    MyWrite(fd, cmd, strlen(cmd));
    MyWrite(fd, "\n", 1);
    newlinefd(reply, fd, ETK_MAX_STRLEN);
    cout << "Reply: " << reply << endl;
    sleep(delay);
    
    //
    // Map the window
    //
    if ((fd = ETkPatron(server, port, errbuf)) < 0)
    {
	cout << "Could not connect to port " << port << " on "
	    << server << endl;
	return;
    }
    sprintf(cmd, "map %d", handle);
    cout << "\nSending command: " << cmd << endl;
    MyWrite(fd, cmd, strlen(cmd));
    MyWrite(fd, "\n", 1);
    newlinefd(reply, fd, ETK_MAX_STRLEN);
    cout << "Reply: " << reply << endl;
    sleep(delay);
    
    //
    // Eval a tcl command in the remote interpreter
    //
    if ((fd = ETkPatron(server, port, errbuf)) < 0)
    {
	cout << "Could not connect to port " << port << " on "
	    << server << endl;
	return;
    }
    sprintf(cmd, "eval %d 1", handle);
    cout << "\nSending command: " << cmd << endl;
    cout << "  argv: \"set ETk(handle)\"" << endl;
    MyWrite(fd, cmd, strlen(cmd));
    MyWrite(fd, "\n", 1);
    MyWrite(fd, "set ETk(handle)\n", 16);
    newlinefd(reply, fd, ETK_MAX_STRLEN);
    cout << "Reply: " << reply << endl;
    sleep(delay);
    
    //
    // Eval a tcl command in the remote interpreter
    //
    if ((fd = ETkPatron(server, port, errbuf)) < 0)
    {
	cout << "Could not connect to port " << port << " on "
	    << server << endl;
	return;
    }
    sprintf(cmd, "eval %d 1", handle);
    cout << "\nSending command: " << cmd << endl;
    cout << "  argv: \"ETk_GetCoords\"" << endl;
    MyWrite(fd, cmd, strlen(cmd));
    MyWrite(fd, "\n", 1);
    MyWrite(fd, "ETk_GetCoords\n", 14);
    newlinefd(reply, fd, ETK_MAX_STRLEN);
    cout << "Reply: " << reply << endl;
    sleep(delay);
    
    //
    // Get the window id of the new window
    //
    if ((fd = ETkPatron(server, port, errbuf)) < 0)
    {
	cout << "Could not connect to port " << port << " on "
	    << server << endl;
	return;
    }
    sprintf(cmd, "eval %d 1", handle);
    cout << "\nSending command: " << cmd << endl;
    cout << "  argv: \"winfo id .\"" << endl;
    MyWrite(fd, cmd, strlen(cmd));
    MyWrite(fd, "\n", 1);
    MyWrite(fd, "winfo id .\n", 11);
    newlinefd(reply, fd, ETK_MAX_STRLEN);
    cout << "Reply: " << reply << endl;
    sscanf(reply, "%*s %x", &windowID);
    sleep(delay);
    
    //
    // Create another window inside the first
    //
    if ((fd = ETkPatron(server, port, errbuf)) < 0)
    {
	cout << "Could not connect to port " << port << " on "
	    << server << endl;
	return;
    }
    sprintf(cmd, "show ETkSample.tcl 0x%x 150 75 200 100 c 0", windowID);
    cout << "\nSending command: " << cmd << endl;
    MyWrite(fd, cmd, strlen(cmd));
    MyWrite(fd, "\n", 1);
    newlinefd(reply, fd, ETK_MAX_STRLEN);
    cout << "Reply: " << reply << endl;
    sscanf(reply, "%*s %d", &handle2);
    sleep(delay);
    
    //
    // Change the color of the child window
    //
    if ((fd = ETkPatron(server, port, errbuf)) < 0)
    {
	cout << "Could not connect to port " << port << " on "
	    << server << endl;
	return;
    }
    sprintf(cmd, "eval %d 1", handle2);
    cout << "\nSending command: " << cmd << endl;
    cout << "  argv: \".b config -bg yellow\"" << endl;
    MyWrite(fd, cmd, strlen(cmd));
    MyWrite(fd, "\n", 1);
    MyWrite(fd, ".b config -bg yellow\n", 21);
    newlinefd(reply, fd, ETK_MAX_STRLEN);
    cout << "Reply: " << reply << endl;
    sleep(delay);
    
    //
    // Move the child window
    //
    if ((fd = ETkPatron(server, port, errbuf)) < 0)
    {
	cout << "Could not connect to port " << port << " on "
	    << server << endl;
	return;
    }
    sprintf(cmd, "move %d 150 225", handle2);
    cout << "\nSending command: " << cmd << endl;
    MyWrite(fd, cmd, strlen(cmd));
    MyWrite(fd, "\n", 1);
    newlinefd(reply, fd, ETK_MAX_STRLEN);
    cout << "Reply: " << reply << endl;
    sleep(delay);
    
    //
    // Eval a tcl command in the child's interpreter
    //
    if ((fd = ETkPatron(server, port, errbuf)) < 0)
    {
	cout << "Could not connect to port " << port << " on "
	    << server << endl;
	return;
    }
    sprintf(cmd, "eval %d 1", handle2);
    cout << "\nSending command: " << cmd << endl;
    cout << "  argv: \"set ETk(handle)\"" << endl;
    MyWrite(fd, cmd, strlen(cmd));
    MyWrite(fd, "\n", 1);
    MyWrite(fd, "set ETk(handle)\n", 16);
    newlinefd(reply, fd, ETK_MAX_STRLEN);
    cout << "Reply: " << reply << endl;
    sleep(delay);
    
    //
    // Free the child window
    //
    if ((fd = ETkPatron(server, port, errbuf)) < 0)
    {
	cout << "Could not connect to port " << port << " on "
	    << server << endl;
	return;
    }
    sprintf(cmd, "free %d", handle2);
    cout << "\nSending command: " << cmd << endl;
    MyWrite(fd, cmd, strlen(cmd));
    MyWrite(fd, "\n", 1);
    newlinefd(reply, fd, ETK_MAX_STRLEN);
    cout << "Reply: " << reply << endl;
    sleep(delay);
    
    //
    // Free the parent window
    //
    if ((fd = ETkPatron(server, port, errbuf)) < 0)
    {
	cout << "Could not connect to port " << port << " on "
	    << server << endl;
	return;
    }
    sprintf(cmd, "free %d", handle);
    cout << "\nSending command: " << cmd << endl;
    MyWrite(fd, cmd, strlen(cmd));
    MyWrite(fd, "\n", 1);
    newlinefd(reply, fd, ETK_MAX_STRLEN);
    cout << "Reply: " << reply << endl;
    sleep(delay);
    
}

/********************************************************
   read a \n-terminated line from fd into s, replacing
   \n by \0. do not read more than maxc-1 characters
   return the number of characters read (excluding \n). 
*********************************************************/
static int
newlinefd(char *s, int fd, int maxc)
{
    int i, lim = maxc-1;
    char c;
    i = 0;
    while ((i < lim) && (read(fd, &c, 1) == 1))
    {
	if (c == '\n')
	{
	    break;
	}
	s[i++] = c;
    }
    
    s[i] = '\0';
    return i;
}

/******************************************************
  Read num bytes from fd into buff. Return
  # bytes actually read, which may be less only when
  EOF is prematurely reached.
*******************************************************/ 
static int
MyRead(int fd, char *buff, int num)
{
    char *temp;
    int tocome = num;
    int thistime;
    temp = buff;
    while (tocome > 0)
    {
	thistime = read(fd,temp,tocome);
	if (thistime <= 0)
	    return (num - tocome);
	tocome -= thistime;
	temp = temp + thistime;
    }
    return (num);
}

/******************************************************
  Write num bytes into fd from buff. Return
  # bytes actually written, which may be less only when
  write(fd,..,..) returns <=0 at some point. 
*********************************************************/ 
static int
MyWrite(int fd, char *buff, int num)
{
    char *temp;
    int tocome = num;
    int thistime;
    temp = buff;
    while (tocome > 0)
    {
	thistime = write(fd, temp, tocome);
	if (thistime <= 0)
	    return (num-tocome);
	tocome -= thistime;
	temp = temp + thistime;
    }
    return (num);
}

istream &
eatwhite(istream &is)
{
    char c;
    while (is.get(c))
    {
	if (isspace(c) == 0 || c == '\n')
	{
	    is.putback(c);
	    break;
	}
    }
    return is;
}

int
main()
{
    int fd;
    char errbuf[ETK_MAX_STRLEN];
    char cmd[ETK_MAX_STRLEN];
    char reply[ETK_MAX_STRLEN];
    char c;
    char *server = "localhost";
    int port = ETK_PORT;
    int argc;
    int i;
    
    cout << "-----------------------------------------------------" << endl;
    cout << " Sample client for the EmbeddedTk server" << endl;
    cout << " Compile date: " << ETkCompDate::Get() << endl;
    cout << "-----------------------------------------------------" << endl;
    cout << endl;
    cout << "    Type \"?\" for help" << endl;
    cout << "    Type \"demo\" to see a demo" << endl;
    cout << endl;
    
    while (cin)
    {
	cout << "% " << flush;
	eatwhite(cin);
	cin.get(cmd, ETK_MAX_STRLEN, '\n');
	if (cin.get(c) && c != '\n')
	{
	    cout << "Input string too long. max = "
		<< ETK_MAX_STRLEN-1 << " characters." << endl;
	    while (cin.get(c) && c != '\n')
	    {
	    }
	}
	if (!strcmp(cmd, "exit"))
	{
	    break;
	}
	else if (!strcmp(cmd, "help") || !strcmp(cmd, "?"))
	{
	    Help();
	}
	else if (!strcmp(cmd, "demo"))
	{
	    Demo(server, port);
	}
	else if (strlen(cmd) > 0)
	{
	    if ((fd = ETkPatron(server, port, errbuf)) < 0)
	    {
		cout << "Could not connect to port " << port << " on "
		    << server << endl;
	    }
	    else
	    {
		c = '\n';
		MyWrite(fd, cmd, strlen(cmd));
		MyWrite(fd, &c, 1);
		argc = 0;
		if (!strncmp(cmd, "show", 4))
		{
		    sscanf(cmd, "%*s %*s %*s %*s %*s %*s %*s %*s %d", &argc);
		}
		else if (!strncmp(cmd, "eval", 4))
		{
		    sscanf(cmd, "%*s %*s %d", &argc);
		}
		for (i = 0; i < argc; i++)
		{
		    cout << "(arg " << i << ") " << flush;
		    eatwhite(cin);
		    cin.get(cmd, ETK_MAX_STRLEN, '\n');
		    if (cin.get(c) && c != '\n')
		    {
			cout << "Input string too long. max = "
			    << ETK_MAX_STRLEN-1 << " characters." << endl;
			while (cin.get(c) && c != '\n')
			{
			}
		    }
		    MyWrite(fd, cmd, strlen(cmd));
		    MyWrite(fd, &c, 1);
		}
		newlinefd(reply, fd, ETK_MAX_STRLEN);
		cout << reply << endl;
	    }
	}
    }
    
    return 0;
}

