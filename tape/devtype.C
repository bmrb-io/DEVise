#include <iostream.h>
#include <stdlib.h>

extern "C" void perror(char *);

#define UXCALL(c,s) {if ((c) < 0) {cerr << __FILE__ << ":" << __LINE__ \
				        << ": "; perror(s); exit(1);}}

#include <sys/stat.h>

int isTapeDevice(char *fname)
{
  struct stat buf;
  UXCALL(stat(fname, &buf), "stat");

  cout << "st_dev = " << buf.st_dev << endl;
  cout << "st_ino = " << buf.st_ino << endl;
  cout << "st_mode = " << buf.st_mode << endl;
  cout << "st_nlink = " << buf.st_nlink << endl;
  cout << "st_uid = " << buf.st_uid << endl;
  cout << "st_gid = " << buf.st_gid << endl;
  cout << "st_rdev = " << buf.st_rdev << endl;
  cout << "st_size = " << buf.st_size << endl;
  cout << "st_atime = " << buf.st_atime << endl;
  cout << "st_spare1 = " << buf.st_spare1 << endl;
  cout << "st_mtime = " << buf.st_mtime << endl;
  cout << "st_spare2 = " << buf.st_spare2 << endl;
  cout << "st_ctime = " << buf.st_ctime << endl;
  cout << "st_spare3 = " << buf.st_spare3 << endl;
  cout << "st_blksize = " << buf.st_blksize << endl;
  cout << "st_blocks = " << buf.st_blocks << endl;

  if (S_ISCHR(buf.st_mode))
    return 1;
  return 0;
}

#ifdef DEVIOCGET
#include <sys/ioctl.h>

void deviceType(char *fname)
{
  int device;                           /* file descriptor for device */
  struct devget dget;                   /* device information */

  UXCALL(device = open(fname, O_RDONLY, "r"), "open");
  UXCALL(ioctl(device, DEVIOCGET, (char *)&dget), "ioctl");

  switch(dget.category) {
  case DEV_TAPE:     cout << "tape" << endl; break;
  case DEV_DISK:     cout << "disk" << endl; break;
  case DEV_TERMINAL: cout << "terminal" << endl; break;
  case DEV_PRINTER:  cout << "printer" << endl; break;
  case DEV_SPECIAL:  cout << "special" << endl; break;
  default:           cout << "unknown" << endl; break;
  }

  UXCALL(close(device));
}
#endif

int main(int argc, char **argv)
{
  if (argc < 2) {
    cerr << "Usage: " << argv[0] << " filename" << endl;
    exit(1);
  }

  int type = isTapeDevice(argv[1]);
  cout << argv[1] << " is " << type << endl;

  return 0;
}

