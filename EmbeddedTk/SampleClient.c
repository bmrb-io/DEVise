
#include <iostream.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>
#include "EmbeddedTk.h"
#include "ETkPatron.h"
#include "CompDate.h"

void
Help()
{
    cout << "ETk commands:" << endl;
    cout << "  show file window x y width height anchor argc" << endl;
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
    
    cout << "Sample client for the EmbeddedTk server" << endl;
    cout << "Compile date: " << CompDate::Get() << endl;
    
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

