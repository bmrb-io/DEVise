#ifndef _SOCKET_H_
#define _SOCKET_H_

// (C) 1997- by Kevin S. Beyer

class Socket
{
public:
  // static RString local_addr(int port);
  static int connect(const char* host, int port);
  //static int connect(sockaddr_in* sock_addr);
  static int connect(const char* addr);
  static int connect_samehost(int sd, int port);
  static int bind(int& port);
  static int port_by_name(char* service);
  static int accept(int sockd);
  static int serve(int sockd, bool do_fork = false);
  static int close(int sockd); // shutdown socket too
};


#endif // _SOCKET_H_
