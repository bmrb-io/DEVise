// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
//
//   Get Formatted Time (the antithesis of strftime())
//   $Id$
//
//   Copyright 1997, Shaun Flisakowski
//
//   You may distribute under the terms of either the GNU General Public
//   License or the Artistic License, as specified in the README file.
//
// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o

/*
  getftime() is a function that can read date/time strings similar
  to the ones produced by the standard function strftime.

  See the header getftime.h for the specifics.
*/

#include   <stdlib.h>
#include   <string.h>
#include   <ctype.h>
#include   <iostream.h>

#include   "getftime.h"

/*  #define    VERBOSE_ERROR  */

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o

// Strings to be used during conversions.

static char *am_or_pm[] = {
                       "AM",
                       "PM",
                        NULL
                      };

static char *day_names[] = {
                            "Sunday",
                            "Monday",
                            "Tuesday",
                            "Wednesday",
                            "Thursday",
                            "Friday",
                            "Saturday",
                             NULL
                           };

static char *day_abbrs[] = {
                            "Sun",
                            "Mon",
                            "Tue",
                            "Wed",
                            "Thu",
                            "Fri",
                            "Sat",
                             NULL
                           };
static char *mnth_names[] = {
                                "January",
                                "Febuary",
                                "March",
                                "April",
                                "May",
                                "June",
                                "July",
                                "August",
                                "September",
                                "October",
                                "November",
                                "December",
                                 NULL
                            };

static char *mnth_abbrs[] = {
                                "Jan",
                                "Feb",
                                "Mar",
                                "Apr",
                                "May",
                                "Jun",
                                "Jul",
                                "Aug",
                                "Sep",
                                "Oct",
                                "Nov",
                                "Dec",
                                 NULL
                            };

static int days_per_mnth[] = {
                                31, 28, 31, 30, 31, 30,
                                31, 31, 30, 31, 30, 31,
                                0
                             };

    // Need to figure out what all these could be.
static char *time_zones[] = {
                              "EST",
                              "EDT",
                              "CST",
                              "CDT",
                              "GMT",
                              "UST",
                               NULL
                            };

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
//   Returns 1 if strings are equal, ignoring case, 0 otherwise.
//   Only checks for equality up to the shorter length of
//   the two strings, so "blat" equals "BlaTheR".
//   (requires ISO standard tolower function, which is
//    well-defined for all letters.)
static int strcaseeq(char *s, char *t)
{
    for (; *s && *t; s++, t++) {
          // first condition is just a faster check.
        if ((*s != *t) && (tolower(*s) != tolower(*t)))
            return 0;
    }

    return 1;
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
// returns -1 if s not an element of strs, its offset otherwise.
static int findstr( char *s, char **strs )
{
    for (int j=0; strs[j]; j++)
        if (strcaseeq(strs[j],s))
            return j;

    return -1;
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
// Read in an integer from the string, using up at most n chars.
// n == 0 means no limit.
static int atoin( char *s, int n )
{
    int ret;
    char sent;

    sent = s[n];
    if (n)
      s[n] = '\0';
    ret = atoi(s);
    s[n] = sent;

    return ret;
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
// returns the number of characters consumed from buf,
// or 0 on an error.
int getftime(char *buf, char *format, struct tm *tme)
{
    int  len, tot=0;          // The total chars consumed
    int  v, got_fn=0, repeat;
         // For building a 24-hour clock from 12-hour plus am/pm.
    int  have_hour = 0, hour_sum = 0;
    int  weeknum = -1, weeknum1 = -1;

    if (!format)
        format = ISO_TIME;

    // Initialize fields to zero.
    tme->tm_sec   =  0;
    tme->tm_min   =  0;
    tme->tm_hour  =  0;
    tme->tm_mday  =  0;    // invalid, on purpose
    tme->tm_mon   =  0;
    tme->tm_year  =  0;
    tme->tm_wday  =  0;
    tme->tm_yday  =  0;
    tme->tm_isdst = -1;
 
    for (; *format && *buf; format++) {

        if (got_fn) {

            got_fn = 0;

            switch (*format) {
                     // %a - abbreviated weekday name, eg, "Mon"
              case 'a':
                v = findstr(buf,day_abbrs);
                if ((v < 0) || (v > 6)) {
#ifdef    VERBOSE_ERROR
                    cout << "Day abbr '" << buf << "' not found.\n";
#endif
                    return 0;
                }
                tme->tm_wday = v;
                buf += 3;
                tot += 3;
                break;

                    // %A - full weekday name, eg, "Monday"
              case 'A':
                v = findstr(buf,day_names);
                if ((v < 0) || (v > 6)) {
#ifdef    VERBOSE_ERROR
                    cout << "Day name '" << buf << "' not found.\n";
#endif
                    return 0;
                }
                tme->tm_wday = v;
                len = strlen(day_names[v]);
                buf += len;
                tot += len;
                break;

                    // %b - abbreviated month name, eg, "Feb"
              case 'b':
                v = findstr(buf,mnth_abbrs);
                if ((v < 0) || (v > 11)) {
#ifdef    VERBOSE_ERROR
                    cout << "Month abbr '" << buf << "' not found.\n";
#endif
                    return 0;
                }
                tme->tm_mon = v;
                buf += 3;
                tot += 3;
                break;

                    // %B - full month name, eg, "Febuary"
              case 'B':
                v = findstr(buf,mnth_names);
                if ((v < 0) || (v > 11)) {
#ifdef    VERBOSE_ERROR
                    cout << "Month name '" << buf << "' not found.\n";
#endif
                    return 0;
                }
                tme->tm_mon = v;
                len = strlen(mnth_names[v]);
                buf += len;
                tot += len;
                break;

                    // %d - day of the month as an int (01-31)
              case 'd':
                v = atoin(buf,repeat);
                if ((v < 1) || (v > 31)) {
#ifdef    VERBOSE_ERROR
                    cout << "Invalid day of month (" << v << "): '"
                         << buf << "'.\n";
#endif
                    return 0;
                }
                tme->tm_mday = v;

                if (repeat)
                    len = repeat;
                else if ((v < 10) && (*buf != '0'))
                    len = 1;
                else
                    len = 2;

                buf += len;
                tot += len;
                break;

                    // %H - the hour (24-hour clock) as an int (00-23)
              case 'H':
                v = atoin(buf,repeat);
                if ((v < 0) || (v > 23)) {
#ifdef    VERBOSE_ERROR
                    cout << "Invalid 24-hour clock hour (" << v << "): '"
                         << buf << "'.\n";
#endif
                    return 0;
                }
                tme->tm_hour = v;
                have_hour = 1;

                if (repeat)
                    len = repeat;
                else if ((v < 10) && (*buf != '0'))
                    len = 1;
                else
                    len = 2;

                buf += len;
                tot += len;
                break;

                    // %I - the hour (12-hour clock) as an int (01-12)
              case 'I':
                v = atoin(buf,repeat);
                if ((v < 1) || (v > 12)) {
#ifdef    VERBOSE_ERROR
                    cout << "Invalid 12-hour clock hour (" << v << "): '"
                         << buf << "'.\n";
#endif
                    return 0;
                }
                hour_sum += v-1;   // Need AM/PM too.

                if (repeat)
                    len = repeat;
                else if ((v < 10) && (*buf != '0'))
                    len = 1;
                else
                    len = 2;

                buf += len;
                tot += len;
                break;

                    // %j - day of the year as a decimal number (001-366)
              case 'j':
                v = atoin(buf,repeat);
                if ((v < 1) || (v > 366)) {
#ifdef    VERBOSE_ERROR
                    cout << "Invalid day of year (" << v << "): '"
                         << buf << "'.\n";
#endif
                    return 0;
                }
                tme->tm_yday = v-1;

                if (repeat)
                    len = repeat;
                else if ((v < 10) && (*buf != '0'))
                    len = 1;
                else if ((v < 100) && (*buf != '0'))
                    len = 2;
                else
                    len = 3;

                buf += len;
                tot += len;
                break;

                    // %m - month, as a decimal number (01-12)
              case 'm':
                v = atoin(buf,repeat);
                if ((v < 1) || (v > 12)) {
#ifdef    VERBOSE_ERROR
                    cout << "Invalid month (" << v << "): '"
                         << buf << "'.\n";
#endif
                    return 0;
                }
                tme->tm_mon = v-1;

                if (repeat)
                    len = repeat;
                else if ((v < 10) && (*buf != '0'))
                    len = 1;
                else
                    len = 2;

                buf += len;
                tot += len;
                break;

                    // %M - minute, as a decimal number (00-59)
              case 'M':
                v = atoin(buf,repeat);
                if ((v < 0) || (v > 59)) {
#ifdef    VERBOSE_ERROR
                    cout << "Invalid minute (" << v << "): '"
                         << buf << "'.\n";
#endif
                    return 0;
                }
                tme->tm_min = v;

                if (repeat)
                    len = repeat;
                else if ((v < 10) && (*buf != '0'))
                    len = 1;
                else
                    len = 2;

                buf += len;
                tot += len;
                break;

                    // %P - AM/PM designation for 12-hour clock (AM)
              case 'P':
                v = findstr(buf,am_or_pm);
                if ((v < 0) || (v > 1)) {
#ifdef    VERBOSE_ERROR
                    cout << "Invalid AM/PM designation " << buf << "'.\n";
#endif
                    return 0;
                }
                hour_sum += 12 * v;   // Need 12-hour clock too.

                len = 2;
                buf += len;
                tot += len;
                break;

                    // %S - second, as an int (00-61)
                    //      (allows for up to 2 leap-seconds - 60 and 61)
              case 'S':
                v = atoin(buf,repeat);
                if ((v < 0) || (v > 61)) {
#ifdef    VERBOSE_ERROR
                    cout << "Invalid second (" << v << "): '"
                         << buf << "'.\n";
#endif
                    return 0;
                }
                tme->tm_sec = v;

                if (repeat)
                    len = repeat;
                else if ((v < 10) && (*buf != '0'))
                    len = 1;
                else
                    len = 2;

                buf += len;
                tot += len;
                break;

                    // %U - week number of the year (00-53)
                    //     (Week number 1 has the first Sunday; previous
                    //      days are week 0.) 
              case 'U':
                v = atoin(buf,repeat);
                if ((v < 0) || (v > 53)) {
#ifdef    VERBOSE_ERROR
                    cout << "Invalid week of year (" << v << "): '"
                         << buf << "'.\n";
#endif
                    return 0;
                }

                weeknum = v;

                if (repeat)
                    len = repeat;
                else if ((v < 10) && (*buf != '0'))
                    len = 1;
                else
                    len = 2;

                buf += len;
                tot += len;
                break;

                    // %w - weekday as a decimal number (0-6, Sunday=0)
              case 'w':
                v = atoin(buf,repeat);
                if ((v < 0) || (v > 6)) {
#ifdef    VERBOSE_ERROR
                    cout << "Invalid weekday number (" << v << "): '"
                         << buf << "'.\n";
#endif
                    return 0;
                }
                tme->tm_wday = v;

                if (repeat)
                    len = repeat;
                else
                    len = 1;

                buf += len;
                tot += len;
                break;

                    // %W - week number of the year (00-53)
                    //     (Week number 1 has the first Monday; previous
                    //      days are week 0.) 
              case 'W':
                v = atoin(buf,repeat);
                if ((v < 0) || (v > 53)) {
#ifdef    VERBOSE_ERROR
                    cout << "Invalid week number (" << v << "): '"
                         << buf << "'.\n";
#endif
                    return 0;
                }

                weeknum1 = v;

                if (repeat)
                    len = repeat;
                else if ((v < 10) && (*buf != '0'))
                    len = 1;
                else
                    len = 2;

                buf += len;
                tot += len;
                break;

                    // %y - year without century as a decimal number (00-99)
              case 'y':
                v = atoin(buf,repeat);
                if ((v < 0) || (v > 99)) {
#ifdef    VERBOSE_ERROR
                    cout << "Invalid 2-digit year (" << v << "): '"
                         << buf << "'.\n";
#endif
                    return 0;
                }
                  // We should really smite them for not being
                  // more specific, but for now, round up for < 50.
                tme->tm_year = v + ((v <= 50) ? 100 : 0);

                if (repeat)
                    len = repeat;
                else if ((v < 10) && (*buf != '0'))
                    len = 1;
                else
                    len = 2;

                buf += len;
                tot += len;
                break;

                    // %Y - year with century as a decimal number, eg, 1952
              case 'Y':
                v = atoin(buf,repeat);
                if (v < 0) {
#ifdef    VERBOSE_ERROR
                    cout << "Invalid 4-digit year (" << v << "): '"
                         << buf << "'.\n";
#endif
                    return 0;
                }
                  // struct tm is offset for some reason.
                tme->tm_year = v - 1900;  

                if (repeat)
                    len = repeat;
                else
                    len = 4;    // Change here for year 10000+, :^)

                buf += len;
                tot += len;
                break;

                    // %z - time zone abbreviation:
                    //      (CST, GMT, EST, CDT, ...)
              case 'z':
                v = findstr(buf,time_zones);
                if (v < 0) {
#ifdef    VERBOSE_ERROR
                    cout << "Invalid timezone (" << v << "): '"
                         << buf << "'.\n";
#endif
                    return 0;
                }
                // NYI - What to do with this info?

                len = strlen(time_zones[v]);
                buf += len;
                tot += len;
                break;

                    // %% - A single %
              case '%':
                if (*buf != *format) {
#ifdef    VERBOSE_ERROR
                    cout << "Literal char mismatch '" << *buf
                         << "' vs '" << *format << "'.\n";
#endif
                    return 0;
                }
                tot++;
                buf++;
                break;

                    // %? - Any single char(s), to be ignored      
              case '?':
                if (repeat) {
                  buf += repeat;
                  tot += repeat;
                } else {
                  buf++;
                  tot++;
                }
                break;

                    // %<n> - A repeat count.
              case '0':    // Handle 0 as leading zero indicator?
              case '1': case '2': case '3':
              case '4': case '5': case '6':
              case '7': case '8': case '9':
                repeat = atoi(format);
                format += strspn( format, "0123456789" ) - 1;
                got_fn = 1;    // Not done yet.
                break;

              default:
#ifdef    VERBOSE_ERROR
                    cout << "Unknown conversion char '" << *format << "'.\n";
#endif
                return 0;    // Unknown conversion char.
            }
 
        } else if (*format == '%') {
            got_fn = 1;
            repeat = 0;
        } else {
            if (*buf != *format) {
#ifdef    VERBOSE_ERROR
                    cout << "Literal char mismatch '" << *buf
                         << "' vs '" << *format << "'.\n";
#endif
                return 0;
            }
            tot++;
            buf++;
        }
    }

      // Build a 24-hour clock from 12-hour plus am/pm.
    if (!have_hour) {
        if (hour_sum > 23) {
#ifdef    VERBOSE_ERROR
                    cout << "Error converting to 24-hour clock\n";
#endif
            return 0;
        }
        tme->tm_hour = hour_sum;
    }


    if (tme->tm_mday == 0) {

        // use mktime() with a 1/1/YY struct tm to
        // figure out how this year begins.
        struct tm tm1;

        tm1.tm_mday  =  1;
        tm1.tm_mon   =  0;
        tm1.tm_year  =  tme->tm_year;

        (void) mktime(&tm1);

          // Tranform the Monday-starting weeks to Sunday-starting.
        if (weeknum1 >= 0) {

            if (tm1.tm_wday == 1)
                weeknum1--;

            if (tme->tm_wday == 0)
                weeknum = weeknum1 + 1;
            else
                weeknum = weeknum1;
        }

          // Figure out the number of days from the week number,
          // and day-of-week.
        if (weeknum >= 0) {
   
          if (tm1.tm_wday == 0)
              weeknum--;
 
          tme->tm_yday = (tme->tm_wday - tm1.tm_wday) + (7 * weeknum);
        }
    
          // They gave the day number of the year, but no day of the month.
          // Figure out the month/day from the number of days.
        if (tme->tm_yday > 0) {
            int j, ndays = tme->tm_yday;
            int days_this_mnth;
            int yr = tme->tm_year + 1900;
    
            for (j=0; ndays > 0; j++) {
                days_this_mnth = days_per_mnth[j];
    
                  // Futz with day number if it's a leap year.
                if ((j == 1)
                    && ( ((yr % 4) == 0)
                       && ( ((yr % 100) != 0) || ((yr % 400) == 0))) )
                    days_this_mnth++;
    
                ndays -= days_this_mnth;
            }
    
            ndays += days_this_mnth; 
    
            tme->tm_mon = j;
            tme->tm_mday = ndays+1;
        }
    }

      // No date was given (only time?), I guess that's okay.
    if (tme->tm_mday == 0)
        tme->tm_mday = 1;

      // Sets wday & yday based on the other values.
    (void) mktime(tme);
    return tot;
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o

