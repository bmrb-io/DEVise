class DateTime ;

bool isLeap(bool L_BC,int L_Year) ;
     
class EncodedD {
	int date;
public:
	EncodedD(int d){
		date = d;
	}
	int getDate(){
		return date;
	}
};

class EncodedT {
	int time;
public:
     EncodedT(int t) {
		time = t ;
	}
	int getTime(){
		return time;
	}
};

class EncodedTF {
	int time;
	int nanosec;
public:
     EncodedTF(int t, int n) {
		time = t ;
		nanosec = n ;
	}
	EncodedTF(EncodedT et) {
		time = et.getTime() ;
		nanosec = 0 ;
	}
	int getTime() {
		return time ;
	}
	int getNano() {
		return nanosec ;
	}
	operator EncodedT(){
		return EncodedT(time);
	}
};

class EncodedDT {
	int date;
	int time;
public:
	EncodedDT(int d, int t) {
		date = d ;
		time = t ;
	}
	EncodedDT(EncodedD ed) {
		date = ed.getDate() ;
		time = 0 ;
     }
	EncodedDT(EncodedT et) {
		date = 0 ;
		time = et.getTime() ;
	}
	getDate() {
		return date ;
	}
	getTime() {
		return time ;
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
public:
     EncodedDTF() {
		date = 0 ;
		time = 0 ;
		nanosec  = 0 ;
	}
	EncodedDTF(int d, int t, int n) {
		date = d ;
		time = t ;
		nanosec = n ;
	}
	EncodedDTF(EncodedD ed){
		date = ed.getDate();
		time = 0;
		nanosec = 0;
	}
	EncodedDTF(EncodedDT edt){
		date = edt.getDate();
		time = edt.getTime();
		nanosec = 0;
	}
	EncodedDTF(EncodedT et) {
		date = 0 ;
		time = et.getTime() ;
		nanosec = 0 ;
	}
	EncodedDTF(EncodedTF etf) {
		date = 0 ;
		time = etf.getTime() ;
		nanosec = etf.getNano() ;
	}
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
	getDate() {
		return date ;
	}
	getTime() {
		return time ;
	}
	getNano() {
		return nanosec ;
	}
};

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
