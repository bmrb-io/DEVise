#ifndef DATETIME_H
#define DATETIME_H

#include <time.h>  // just for the definition of struct tm

class DateTime ;

bool isLeap(bool L_BC,int L_Year) ;
     
class IntervalYM {
	int year; 	// ?
	int month;	// can be negative, year = month / 12
	bool V_Plus ;
public:
	IntervalYM(int c_year, int c_month, bool c_Plus) ;
	bool isValid() ;
	V_2_Month() const ;
	IntervalYM operator-(const IntervalYM& arg) ;
	IntervalYM operator+(const IntervalYM& arg);
	char* Write_Data();
};

class IntervalDT {
	int day;
	int hour;
	int min;
	int sec;
	int msec;
	int V_Plus ;
public:
	IntervalDT() ;
	IntervalDT(int c_day, int c_hour, int c_min, int c_sec, int c_msec,int c_V_Plus) ;
     bool isValid() ;
	DateTime Elapsed_2_Date() ;
	long long Value_2_Sec() const ;
	int Value_SSec() const ; 
	IntervalDT operator-(const IntervalDT& arg);
	IntervalDT operator+(const IntervalDT& arg);
	bool operator==(const IntervalDT& arg) ;
	char* Write_Data() ;
};

// const int JAN11 = 0x00000221;

class EncodedD {
friend class EncodedDT;
friend class EncodedDTF;
	int date;
public:
	EncodedD(int d){
		date = d;
	}
};

class EncodedT {
friend class EncodedTF;
friend class EncodedDTF;
friend class EncodedDT;
	int time;
public:
     EncodedT(int t) {
		time = t ;
	}
};

class EncodedTF {
friend class EncodedDTF;
	int time;
	int nanosec;
public:
     EncodedTF(int t, int n) {
		time = t ;
		nanosec = n ;
	}
	EncodedTF(EncodedT et) {
		time = et.time;
		nanosec = 0 ;
	}
	operator EncodedT(){
		return EncodedT(time);
	}
};

class EncodedDT {
friend class EncodedDTF;
	int date;
	int time;
public:
	EncodedDT(int d, int t) {
		date = d ;
		time = t ;
	}
	EncodedDT(EncodedD ed) {
		date = ed.date;
		time = 0 ;
     }
	EncodedDT(EncodedT et) {
		date = 0;
		time = et.time;
	}
	operator EncodedD(){
		return EncodedD(date);
	}
	operator EncodedT() {
		return EncodedT(time) ;
	}
};

class EncodedDTF {
	int date;
	int time;
	int nanosec;
friend ostream& operator <<(ostream& our, const EncodedDTF& edtf);
public:
     EncodedDTF() {
		date = 0;
		time = 0 ;
		nanosec  = 0 ;
	}
	EncodedDTF(int d, int t, int n) {
		date = d ;
		time = t ;
		nanosec = n ;
	}
	EncodedDTF(EncodedD ed){
		date = ed.date;
		time = 0;
		nanosec = 0;
	}
	EncodedDTF(EncodedDT edt){
		date = edt.date;
		time = edt.time;
		nanosec = 0;
	}
	EncodedDTF(EncodedT et) {
		date = 0;
		time = et.time;
		nanosec = 0 ;
	}
	EncodedDTF(EncodedTF etf) {
		date = 0;
		time = etf.time;
		nanosec = etf.nanosec;
	}
	operator ==(const EncodedDTF& x) const {
		return date == x.date && time == x.time && nanosec == x.nanosec;
	}
	int compare(const EncodedDTF& x) const {
		if(date < x.date){
			return -1;
		}
		else if(date > x.date){
			return 1;
		}
		else if(time < x.time){
			return -1;
		}
		else if(time > x.time){
			return 1;
		}
		else if(nanosec < x.nanosec){
			return -1;
		}
		else if(nanosec > x.nanosec){
			return 1;
		}
		else{
			return 0;
		}
	}
	operator <(const EncodedDTF& x) const {
		return compare(x) < 0;
	}
	operator >(const EncodedDTF& x) const {
		return compare(x) > 0;
	}
	IntervalDT operator-(const EncodedDTF& x) const;
	operator EncodedD() {
		return EncodedD(date);
	}
	operator EncodedT() {
		return EncodedT(time) ;
	}
	operator EncodedTF() {
		return EncodedTF(time,nanosec) ;
	}
	operator EncodedDT() {
		return EncodedDT(date,time) ;
	}
	int getYear() const {	// returns signed year
		int i = date >> 9;
		if(date < 0){
			return i | 0xFF800000;	// for machines with logical shift
		}
		else{
			return i;
		}
	}
	int getMonth() const {
		int i = date >> 5;
		return i & 0x0000000F;
	}
	int getDay() const {
		return date & 0x0000001F;
	}
	int getHour() const {
		return time >> 12;	// leading bit must be zero
	}
	int getMin() const {
		int i = time >> 6;
		return i & 0x0000003F;
	}
	int getSec() const {
		return time & 0x0000003F;
	}
	int getNanoSec() const {
		return nanosec;
	}
	void setDate(int year, int month, int day){
		date = year << 4;
		date |= month;
		date = date << 5;
		date |= day;
	}
	void setTime(int hour, int min, int sec){
		time = hour << 6;
		time |= min;
		time = time << 6;
		time |= sec;
	}
	void setNanoSec(int nanosec){
		this->nanosec = nanosec;
	}
	void maketm(struct tm& t){
		t.tm_year = getYear() - 1900;
		t.tm_mon = getMonth() - 1;
		t.tm_mday = getDay();
		t.tm_hour = getHour();
		t.tm_min = getMin();
		t.tm_sec = getSec();
		t.tm_isdst = 0;
	}
};

ostream& operator <<(ostream& out, const EncodedDTF& arg);

/*
-- 1 is subtracted from month to supply consistency of arrays and month variable, don't forget to add 1 to month while returning actual date
*/

class DateTime {
private:
	bool isBC;
	int year;
	int month;
	int day;
	int hour;
	int min;
	int sec;
	int msec;
	int elap_day ;
	int elap_month ;
	int current_day ;
public:
	int Get_Elap() const ;
	DateTime(EncodedDTF encodedDateTime);
	DateTime(
		bool c_isBC,
		int c_year,
		int c_month,
		int c_day,
		int c_hour,
		int c_min,
		int c_sec,
		int c_msec
	);
	operator EncodedDTF() ;
	char* Write_Data() ;
     int Find_Elapsed() ;
	bool isValid();
	IntervalDT operator-(const DateTime& arg);
	DateTime operator-(const IntervalDT& arg);
	DateTime operator-(const IntervalYM& arg);
	DateTime operator+(const IntervalDT& arg);
	DateTime operator+(const IntervalYM& arg);
};

#endif
