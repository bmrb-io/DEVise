// (C) 1997- by Kevin S. Beyer

#ifndef _TIMEVAL_H_
#define _TIMEVAL_H_

#include <iostream.h>
#include <iomanip.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <assert.h>

#define USEC_TO_SEC 1000000


//#if defined(sparc)
//extern "C" int gettimeofday(timeval*, timezone*);
//#endif


// NOTE: Timevals can only represent times >= 0

class Timeval
: public timeval
{
  public:

    inline Timeval();

    inline Timeval(double secs);

    inline ~Timeval();

    inline Timeval(const Timeval& other);

    inline Timeval& operator=(const Timeval& other);

    inline Timeval& SetToCurrentTime();
    inline Timeval ElapsedTime() const;

    inline Timeval& SetSecs(double secs);
    inline double AsSecs() const;

    inline Timeval& operator+=(const Timeval& other);
    inline Timeval& operator-=(const Timeval& other);
    
    inline Timeval operator+(const Timeval& other) const;
    inline Timeval operator-(const Timeval& other) const;

    inline bool operator>(const Timeval& other) const;
    inline bool operator<(const Timeval& other) const;
    inline bool operator==(const Timeval& other) const;
    inline bool operator>=(const Timeval& other) const;
    inline bool operator<=(const Timeval& other) const;
    inline bool operator!=(const Timeval& other) const;

    friend inline ostream& operator<<(ostream& outs, const Timeval& x);

  protected:

};



inline Timeval& Timeval::SetToCurrentTime()
{
    struct timezone tz;
    gettimeofday(this, &tz);
    return *this;
}


inline Timeval& Timeval::SetSecs(double secs)
{
    assert(secs >= 0);
    tv_sec = (int)secs;
    tv_usec = (int)((secs - (int)secs) * 1e6 + 0.5);
    return *this;
}


inline Timeval::Timeval()
{
    SetToCurrentTime();
}


inline Timeval::Timeval(double secs)
{
    SetSecs(secs);
}


inline Timeval::~Timeval()
{
}


inline Timeval::Timeval(const Timeval& other)
{
    tv_sec = other.tv_sec;
    tv_usec = other.tv_usec;
}


inline Timeval& Timeval::operator=(const Timeval& other)
{
    tv_sec = other.tv_sec;
    tv_usec = other.tv_usec;
    return *this;
}


inline double Timeval::AsSecs() const
{
    return tv_sec + (tv_usec / 1e6);
}


inline bool Timeval::operator>(const Timeval& other) const
{
    return tv_sec > other.tv_sec || (tv_sec == other.tv_sec &&
				     tv_usec > other.tv_usec);
}


inline bool Timeval::operator<(const Timeval& other) const
{
    return tv_sec < other.tv_sec || (tv_sec == other.tv_sec &&
				     tv_usec < other.tv_usec);
}


inline bool Timeval::operator==(const Timeval& other) const
{
    return tv_sec == other.tv_sec && tv_usec == other.tv_usec;
}


inline bool Timeval::operator<=(const Timeval& other) const
{
    return tv_sec <= other.tv_sec || (tv_sec == other.tv_sec &&
				      tv_usec <= other.tv_usec);
}


inline bool Timeval::operator>=(const Timeval& other) const
{
    return tv_sec >= other.tv_sec || (tv_sec == other.tv_sec &&
				      tv_usec >= other.tv_usec);
}


inline bool Timeval::operator!=(const Timeval& other) const
{
    return tv_sec != other.tv_sec || tv_usec != other.tv_usec;
}


inline Timeval& Timeval::operator+=(const Timeval& other)
{
    tv_usec += other.tv_usec;
    tv_sec  += other.tv_sec;
    if( tv_usec >= USEC_TO_SEC ) {
	tv_usec -= USEC_TO_SEC;
	tv_sec++;
    }
    return *this;
}


inline Timeval& Timeval::operator-=(const Timeval& other)
{
    assert(*this > other);
    tv_usec -= other.tv_usec;
    tv_sec  -= other.tv_sec;
    if( tv_usec < 0 ) {
	tv_usec += USEC_TO_SEC;
	tv_sec--;
    }
    return *this;
}


inline Timeval Timeval::operator+(const Timeval& other) const
{
    Timeval result(*this);
    result += other;
    return result;
}


inline Timeval Timeval::operator-(const Timeval& other) const
{
    Timeval result(*this);
    result -= other;
    return result;
}


inline Timeval Timeval::ElapsedTime() const
{
    Timeval now;
    return now - *this;
}


inline ostream& operator<<(ostream& outs, const Timeval& x)
{
    return outs << setprecision(9) << x.AsSecs();
#ifdef FORMATTED_TIME
    long tm = x.tv_sec;
    int sec = tm % 60;
    tm /= 60;
    int min = tm % 60;
    tm /= 60;
    int hr = tm % 24;
    tm /= 24;
    long day = tm;
    if( day )
      outs << day << " days ";
    char fill = outs.fill('0');
    return outs << hr << ':'<< setw(2) << min << ':' << setw(2) << sec
      << '.' << setw(6) << x.tv_usec << setfill(fill);
#endif
}


#endif // _TIMEVAL_H_
