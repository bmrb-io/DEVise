#ifndef _SELECT32_H_
#define _SELECT32_H_

#if !defined(FD_SET)

typedef int fd_set;
#define FD_SETSIZE 32
inline void FD_SET(int fd, fd_set* fdset) { (*fdset) |= 1 << fd; }
inline void FD_CLR(int fd, fd_set* fdset) { (*fdset) &= ~(1 << fd); }
inline void FD_ZERO(fd_set* fdset) { *fdset = 0; }
inline bool FD_ISSET(int fd, fd_set* fdset) { return (*fdset) & (1 << fd); }

#endif
#endif // _SELECT32_H_
