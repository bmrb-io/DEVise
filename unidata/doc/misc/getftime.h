  getftime() is a function that can read date/time strings similar
  to the ones produced by the standard function strftime.

  You must supply getftime with a format string that indicates
  the format of the date/time being expected:

  Code            Replaced By
  ----            -----------
   %a      abbreviated weekday name, eg, "Mon"
   %A      full weekday name, eg, "Monday"
   %b      abbreviated month name, eg, "Feb"
   %B      full month name, eg, "Febuary"
   %d      day of the month as a decimal integer (01-31)
   %H      the hour (24-hour clock) as a decimal integer (00-23)
   %I      the hour (12-hour clock) as a decimal integer (01-12)
   %j      day of the year as a decimal number (001-366)
   %m      month, as a decimal number (01-12)
   %M      minute, as a decimal number (00-59)
   %P      AM/PM designation for 12-hour clock (AM)
   %S      second, as a decimal number (00-61)
               (allows for up to 2 leap-seconds - 60 and 61)
   %U      week number of the year (00-53)
               (Week number 1 has the first Sunday; previous days
                are week 0.) 
   %w      weekday as a decimal number (0-6, Sunday=0)
   %W      week number of the year (00-53)
               (Week number 1 has the first Monday; previous days
                are week 0.) 
   %y      year without century as a decimal number (00-99)
   %Y      year with century as a decimal number, eg, 1952
   %z      time zone abbreviation:
                (CST, GMT, EST, CDT, ...)
   %%      A single %

   %?      Any single character, to be ignored      
   %n?     Ignore any n characters, eg, "%3?" 
 <any>     A literal character, to be ignored. 

   %c, %x, %X, %p: Reserved, as they are used in strftime().

   Misc Notes:
     %a, %A, %b, %B, %P, and %z are case insensitive.

     While strftime prints most numbers zero-filled, getftime
     will accept values that are not.  (ie, 1/1/97 is okay,
     it doesn't need to be 01/01/97)  To require a specific
     number of chars, use a repeat count, eg "%2m%2d%2y"
     will read a number of the form, 010197, which otherwise
     cannot be interpreted properly.
