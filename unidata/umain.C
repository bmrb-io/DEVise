
#include <iostream.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

#include "UniData.h"

#define BUFSZE 2048
char buff[BUFSZE+1];

void showRec(char *buf, UniData *ud, UD_Status stat)
{
    if (!ud)
        return;

    if (stat != UD_OK) {
        if (stat == UD_EOF)
            cout << "Status: EOF\n";
        else if (stat == UD_FAIL)
            cout << "Status: Fail\n";
        else
            cout << "Unknown Status.\n";
        return;
    }

    int j, n = ud->schema()->NumFlatAttrs();
    AttrStk *stk = ud->schema()->GetFlatAttrs();

    int    *i;
    double *d;
    float  *f;
    char   *c;
    time_t *t;
    ushort *us;

    struct tm *t2;
    char tmebuf[200];

    for (j=0; j < n; j++) {

        Attr *at = stk->ith(j);

        switch (at->type()) {

          case Int_Attr:
            i = (int*) &(buf[at->offset()]);
            cout << " " << *i << " ";
            break;

          case Float_Attr:
            f = (float*) &(buf[at->offset()]);
            cout << " " << *f << " ";
            break;

          case Double_Attr:
            d = (double*) &(buf[at->offset()]);
            cout << " " << *d << " ";
            break;

          case String_Attr:
            c = (char*) &(buf[at->offset()]);
            cout << " '" << c << "' ";
            break;

          case UnixTime_Attr:
            t = (time_t*) &(buf[at->offset()]);
            cout << " '" << *t << "' ";
            break;

          case DateTime_Attr:
            t2 = (struct tm*) &(buf[at->offset()]);
            strftime(tmebuf, 200, " '%m/%d/%y at %H:%M:%S' ", t2);
            cout << tmebuf;
            break;

          default:
            if (at->type() > UserDefined_Attr) {
                us = (ushort*) &(buf[at->offset()]);
                cout << " " << *us << " ";
                break;
            }

          case Invalid_Attr:
          case UserDefined_Attr:
            cout << "This type isn't handled yet: " << at->type() << endl;
            break;
        }
    }

    cout << endl;
}

int main(int ac, char **av)
{
    //int fd;
    UniData *ud = NULL;

    buff[BUFSZE] = '\0';;

    if (ac == 3) {
        ud = new UniData(av[1],av[2]);
        //cout << *ud << endl;
    } else if (ac == 2) {
        ud = new UniData(av[1]);
        //cout << *ud << endl;
    } else {
        cerr << "Try:\n\tumain <datafile> <schemafile>\n";
    }

//#define SPEEDTEST

#ifdef SPEEDTEST
    if (ud && ud->isOk()) {

        off_t off;
        UD_Status stat;

        while (ud->isOk()) {
            stat = ud->getRec(buff,&off);
        }
    }
#else
    if (ud && ud->isOk()) {

        //ud->useRid();

        off_t off;
        UD_Status stat;
        int rsze;
        int cnt = 0;

        rsze = ud->schema()->recSze();

        while (ud->isOk() && (cnt < 5)) {

            stat = ud->getRec(buff,&off);
        
            if (ud->isOk())
                cout << "[" << off << "]: ";
            showRec(buff,ud,stat);
            cnt++;
        }
    }
#endif

    delete ud;
    return(0);
}

