//#include <iostream.h>   erased for sysdep.h
//#include <strstream.h>   erased for sysdep.h
#include "sysdep.h"
#include "DateTime.h"
#include <string>

const char *months[] = {
	"January", "February", "March", "April", "May", "June", "July",
	"August", "September", "October", "November", "December"
};
const char *Days[] = {
	"Saturday" , "Sunday", "Monday", "Tuesday", "Wednesday",
	"Thursday", "Friday"
};
const int month_days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31} ;
const int s_sec = 1000000 ;
     
char* IntervalYM::Write_Data(){
	ostringstream tmp;
	tmp << "Year : " << year << " Month: " << month << " Plus ?: " << V_Plus << ends ;
	string retst = tmp.str() ;
	return strdup(retst.c_str()) ;
}

IntervalYM::IntervalYM(int c_year, int c_month, bool c_Plus) {
	year = c_year ;
	month = c_month ;
	V_Plus = c_Plus ;
	return ;
}

bool IntervalYM::isValid() {
	if (month>12 || month<1) return 0;
	return 1 ;
}

int IntervalYM::V_2_Month() const {
	return ( year * 12 + month ) * (!V_Plus ? -1 : 1) ;
}

IntervalYM IntervalYM::operator -(const IntervalYM& arg) {
	int t_val1 = arg.V_2_Month() ;
	int t_val2 = V_2_Month() ;
	int r_y,r_m,r_p ;
	t_val1 = t_val1 - t_val2 ;
	r_p = (t_val1 >= 0 ? 1 : 0) ;
	t_val1 *= (t_val1 >=0 ? 1 : -1) ;
	r_y = int(t_val1/12) ;
	r_m = t_val1 % 12 ;
	IntervalYM ret_IYM(r_y,r_m,!r_p) ;
	return ret_IYM ;
}

IntervalYM IntervalYM::operator +(const IntervalYM& arg) {
	int t_val1 = arg.V_2_Month() ;
	int t_val2 = V_2_Month() ;
	int r_y,r_m,r_p ;
	t_val1 = t_val1 + t_val2 ;
	r_p = (t_val1 >= 0 ? 1 : 0) ;
	t_val1 *= (t_val1 >=0 ? 1 : -1) ;
	r_y = int(t_val1/12) ;
	r_m = t_val1 % 12 ;
	IntervalYM ret_IYM(r_y,r_m,bool(r_p)) ;
	return ret_IYM ;
}

char* IntervalDT::Write_Data(){
	ostringstream tmp;
	tmp << "Day : " << day  << " Hour : " << hour << " Minute : " << min << " Seconds : " << sec << " N Seconds : " << msec << " Min-Pos : " << V_Plus;
	string retst = tmp.str() ;
	return strdup(retst.c_str()) ;
}

IntervalDT::IntervalDT() {
	day = 0;
	hour = 0 ;
	min = 0 ;
	sec = 0 ;
	msec = 0 ;
	V_Plus = 1 ;
}

IntervalDT::IntervalDT(int c_day, int c_hour, int c_min, int c_sec, int c_msec,int c_V_Plus) {
	day = c_day ;
	hour = c_hour ;
	min = c_min ;
	sec = c_sec ;
	msec = c_msec ;
	V_Plus = c_V_Plus ;
}

IntervalDT::IntervalDT(EncodedIDT x_IDT) {
	day = x_IDT.getDay() ;
	V_Plus = x_IDT.get_V_Plus() ;
	hour = x_IDT.getHour() ;
	min = x_IDT.getMin() ;
	sec = x_IDT.getSec() ;
	msec = x_IDT.getNanoSec() ;
}

IntervalDT::operator EncodedIDT() {
	int d,t=0 ;
	d = ( V_Plus ? day : -day) ;
	t = hour << 6 ;
	t |= min ;
	t = t << 6 ;
	t |= sec ;
	EncodedIDT ret_IDT(d, t, msec) ;
	return ret_IDT ;
}

bool IntervalDT::isValid() {
	if ((hour > 23) || (hour < 0)) return 0;
	if ((min > 59) || (min < 0)) return 0 ;
	if ((sec > 59) || (sec < 0)) return 0 ;
	if ((msec > 999999) || (msec <0)) return 0 ;
	return true;
}

bool IntervalDT::operator ==(const IntervalDT& arg) {
	return ((day == arg.day) && ( hour == arg.hour ) && ( min == arg.min) && (sec == arg.sec) && ( msec == arg.msec ) && ( V_Plus == arg.V_Plus)); 
}

IntervalDT IntervalDT::operator -(const IntervalDT& arg) {
	bigint DT1,DT2 ;
	int DTS1, DTS2,t_day,t_hour,t_min ;
	bool T_P = 1;
	DT1 = Value_2_Sec() ;
	DTS1 = Value_SSec() ;
	DT2 = arg.Value_2_Sec() ;
	DTS2 = arg.Value_SSec() ;
	DT1 = DT1 - DT2 ;
	DTS1 = DTS1 -DTS2 ;
	if (DT1 >0) {
		if (DTS1  < 0) {
			DT1 -- ;
			DTS1 += s_sec ;
	   	}
	}
	else if (DT1 < 0 ) {
		DT1 *= (bigint)(-1) ;
		T_P = 0 ;
		if (DTS1 > 0) {
			DT1 -- ;
			DTS1 = s_sec - DTS1 ;
	   	}
	   	else if (DTS1 < 0) DTS1 *= (bigint)(-1) ;
	}
	else if (DTS1 < 0) {
		T_P = 0 ;
		DTS1 *= (bigint)(-1) ;
	}
	DTS2 = DT1 % (bigint)(86400);
	t_day = int(DT1 / (bigint)(86400)) ;
	t_hour = int(DTS2 / (bigint)(3600)) ;
	DTS2 = DTS2 % 3600 ;
	t_min = int(DTS2 / 60) ;
	DTS2 = DTS2 % 60 ;
	IntervalDT Tem_ID(t_day,t_hour,t_min,DTS2,DTS1,T_P) ;
	return Tem_ID ;
}

IntervalDT IntervalDT::operator +(const IntervalDT& arg) {
	bigint DT1,DT2 ;
	int DTS1, DTS2,t_day,t_hour,t_min ;
	bool T_P = 1;
	DT1 = Value_2_Sec() ;
	DTS1 = Value_SSec() ;
	DT2 = arg.Value_2_Sec() ;
	DTS2 = arg.Value_SSec() ;
	DT1 = DT1 + DT2 ;
	DTS1 = DTS1 +DTS2 ;
	if (DT1 >0) {
		if (DTS1  < 0) {
		 	DT1 -- ;
		 	DTS1 += s_sec ;
	   	}
	}
	else if (DT1 < 0 ) {
	   	DT1 *= (bigint)(-1) ;
	   	T_P = 0 ;
	   	if (DTS1 > 0) {
		 	DT1 -- ;
			DTS1 = s_sec - DTS1 ;
	   	}
	   	else if (DTS1 < 0) DTS1 *= (bigint)(-1) ;
	}
	else {
	   	T_P = 0 ;
	   	DT1 *= (bigint)(-1) ;
	}
	DTS2 = DT1 % (bigint)(86400);
	t_day = int(DT1 / (bigint)(86400)) ;
	t_hour = int(DTS2 / (bigint)(3600)) ;
	DTS2 = DTS2 % 3600 ;
	t_min = int(DTS2 / 60) ;
	DTS2 = DTS2 % 60 ;
	IntervalDT Tem_ID(t_day,t_hour,t_min,DTS2,DTS1,T_P) ;
	return Tem_ID ;
}

char* DateTime::Write_Data(){
	ostringstream tmp;
	tmp << "isBC : " << isBC << " year : " << year  << " month : " << month << " day : " << day << " elap_day : " << elap_day << " elap_month : " << elap_month << " current_day : " << Days[current_day] << " hour : " << hour << " minute : " << min << " seconds : " << sec << " m.seconds : "<< msec << ends ;
	string retst = tmp.str() ;
	return strdup(retst.c_str());
}

int DateTime::Get_Elap() const {
return elap_day ;
}

DateTime::DateTime(EncodedDTF encodedDateTime) {
	isBC = false;
	year = encodedDateTime.getYear();
	month = encodedDateTime.getMonth();
	day = encodedDateTime.getDay();
	if(year < 0){
		year = - year;
		isBC = true;
	}
	if(year == 0){  // date part is missing
		year = 1;
	}
	if(month > 0){
		month--;	// month range here is 0 - 11
	}
	if(day == 0){
		day = 1;
	}
	hour = encodedDateTime.getHour();
	min = encodedDateTime.getMin();
	sec = encodedDateTime.getSec();
	msec = encodedDateTime.getNanoSec();
     elap_day = Find_Elapsed() ;
	elap_month = ( isBC ? ((year-1)*12+(12-month)) : ((year-1)*12+month)) ;
	current_day = ( isBC ? (7-(elap_day % 7))%7 : (elap_day % 7) ) ;
}

DateTime::DateTime( bool c_isBC, int c_year, int c_month, int c_day, int c_hour, int c_min, int c_sec, int c_msec) {
	isBC = c_isBC ;
	year = c_year ;
	month = c_month-1 ;
	day = c_day ;
	hour = c_hour ;
	min = c_min ;
	sec = c_sec ;
	msec = c_msec ;
	elap_day = Find_Elapsed() ;
	elap_month = ( isBC ? ((year-1)*12+(12-month)) : ((year-1)*12+month)) ;
	current_day = ( isBC ? (7-(elap_day % 7))%7 : (elap_day % 7) ) ;
}

bool DateTime::isValid() {
	bigint Date_Base = (bigint)(year)*(bigint)(10000) + (bigint)(month+1)*(bigint)(100) + (bigint)(day) ;
	if (!isBC && (Date_Base>17520902) && (Date_Base<17520914)) return 0 ;
	if ((month>11) || (month < 0)) return 0 ;
	if (year>292451) return 0 ;
	if ((day>29) && (month == 1)) return 0 ;
	if ((Date_Base <= 17520902) || isBC) {
		if (month == 1) {
			if (((isBC ? (year-1) : year) % 4 ) != 0) return (day<29) ; 
	   	}
	   	else {
			return (day<(month_days[month]+1)) ;
		}
	}
	else
	{
		if (month == 1) {
			if (!(((year % 4) == 0) && (((year % 100) != 0) || ((year % 400) == 0)))) return (day<29) ;
	   	}
	   	else return (day<(month_days[month]+1)) ;
	}
	return 1 ;
}

DateTime::operator EncodedDTF() {
	int y = (isBC ? -year : year);
	EncodedDTF ret_ETF;
	ret_ETF.setDate(y, month + 1, day);
	ret_ETF.setTime(hour, min, sec);
	ret_ETF.setNanoSec(msec);
	return ret_ETF ;
}

IntervalDT DateTime::operator -(const DateTime& arg) {
	int IDT_A[4][2] = {{hour,24}, {min,60}, {sec,60},{msec,s_sec}} ;
	int IDT_B[4][2] = {{arg.hour,24},{arg.min,60},{arg.sec,60},
		{arg.msec,s_sec} } ;
	int add_val = 0 , add_val2 = 0;
	if (isBC && ((hour != 0 ) || (min != 0 ) || (sec != 0) || (msec != 0))) {
		for (int idt=3;idt>=0 ; idt--) {
		  	if (add_val != 0) IDT_A[idt][0] = IDT_A[idt][1] - IDT_A[idt][0] - 1 ;
		  	if ((IDT_A[idt][0] != 0) && (add_val == 0)) {
		  		IDT_A[idt][0] = IDT_A[idt][1] - IDT_A[idt][0] ;
		  		add_val = 1 ;
		  	}
	   	}
	   	add_val = elap_day - 1; 
	}
	else add_val = elap_day ;
	if (arg.isBC && ((arg.hour != 0 ) || (arg.min != 0 ) || (arg.sec != 0) || (arg.msec != 0))) {
	   	for (int idt=3;idt>=0 ; idt--) {
		  	if (add_val2 != 0) IDT_B[idt][0] = IDT_B[idt][1] - IDT_B[idt][0] - 1 ;
		  	if ((IDT_B[idt][0] != 0) && (add_val2 == 0)) {
		  		IDT_B[idt][0] = IDT_B[idt][1] - IDT_B[idt][0] ;
		  		add_val2 = 1 ;
		  	}
	   	}
	   	add_val2 = arg.Get_Elap() - 1; 
	}
	else add_val2 = arg.Get_Elap() ; 

	IntervalDT OP_M1(add_val, IDT_A[0][0], IDT_A[1][0], IDT_A[2][0], IDT_A[3][0],!isBC) ;
	IntervalDT OP_M2(add_val2, IDT_B[0][0], IDT_B[1][0], IDT_B[2][0], IDT_B[3][0],!arg.isBC) ;

	OP_M1 = OP_M1 - OP_M2 ;
	return OP_M1 ;
}

DateTime DateTime::operator -(const IntervalDT& arg) {
	bigint T_D1,T_I1 ;
	int T_DM1 , T_IM1, T_Plus = 1, T_day , T_hour, T_min,T_sec ;

	T_D1 = ((bigint)(elap_day) * (bigint)(86400) + (bigint)(hour) * (bigint)(3600) + (bigint)(min) * (bigint)(60) + (bigint)(sec)) * (isBC ? (bigint)(-1) : (bigint)(1)) ;
	T_DM1 = msec ;
	T_I1 = arg.Value_2_Sec() ;
	T_IM1 = arg.Value_SSec() ;
	T_D1 = T_D1 - T_I1 ;
	T_DM1 = T_DM1 - T_IM1 ;
	if ( T_D1 < 0 ) {
		if ( T_DM1 < 0 ) {
		 	T_D1 -- ;
		 	T_DM1 = msec - T_DM1 ;
	   	}
	   	T_D1 *= (bigint)(-1);
	   	T_Plus = 0 ;
	}
	else if (( T_D1 > 0 ) && (T_DM1<0)) {
		T_D1 -- ;
		T_DM1 = msec - T_DM1 ;
	}
	else if ((T_D1 == 0) && ( T_DM1 < 0 )) {
		T_Plus -- ;
		T_DM1 *= -1 ;
	}
	T_day = int(T_D1 / (bigint)(86400)) ;
	T_D1 = T_D1 % (bigint)(86400);
	T_hour = int(T_D1 / 3600) ;
	T_D1 = T_D1 % (bigint)(3600);
	T_min = int(T_D1 / (bigint)(60)) ;
	T_D1 = T_D1 % 60 ;
	T_sec = int(T_D1) ;
	IntervalDT Tem_IDT(T_day, T_hour, T_min, T_sec , T_DM1, T_Plus) ;
	DateTime Tem_DT(0,1,1,1,0,0,0,0) ;
	Tem_DT = Tem_IDT.Elapsed_2_Date() ;
	return Tem_DT ;
}


DateTime DateTime::operator -(const IntervalYM& arg) {
	int t_mon = arg.V_2_Month() ;
	int t_year = (isBC ? (year * -1) + 1 : year) , t_month = month + 1 ;
	bool t_BC = 0 ;
	t_year -= int(t_mon/12) ;
	t_month -= t_mon % 12 ;
	if (t_month < 1) {
		t_month = 12 + t_month ;
	   	t_year -- ;
	}
	if (t_month > 12) {
	   	t_month = t_month - 12 ;
	   	t_year ++ ;
	}
	if ( t_year < 1 ) {
	   	t_year -- ;
	   	t_year *= -1 ;
	   	t_BC = 1 ;
	}
	DateTime Ret_D(t_BC, t_year, t_month, day,hour,min,sec,msec) ;
	return Ret_D ;

}

DateTime DateTime::operator +(const IntervalDT& arg) {
	bigint T_D1,T_I1 ;
	int T_DM1 , T_IM1, T_Plus = 1, T_day , T_hour, T_min,T_sec ;

	T_D1 = ((bigint)(elap_day) * (bigint)(86400) + (bigint)(hour) * (bigint)(3600) + (bigint)(min) * (bigint)(60) + (bigint)(sec)) * (isBC ? (bigint)(-1) : (bigint)(1)) ;
	T_DM1 = msec ;
	T_I1 = arg.Value_2_Sec() ;
	T_IM1 = arg.Value_SSec() ;
	T_D1 = T_D1 + T_I1 ;
	T_DM1 = T_DM1 + T_IM1 ;
	if ( T_D1 < 0 ) {
		if ( T_DM1 < 0 ) {
			T_D1 -- ;
		 	T_DM1 = msec - T_DM1 ;
	   	}
	   	T_D1 *= (bigint)(-1);
	   	T_Plus = 0 ;
	}
	else if ( T_D1 > 0 ) {
		if ( T_DM1 < 0) {
		   T_D1 -- ;
		   T_DM1 = msec - T_DM1 ;
		}
	}
	else if ((T_D1 == 0) && ( T_DM1 < 0 )) {
		T_Plus -- ;
		T_DM1 *= -1 ;
	}
	T_day = int(T_D1 / (bigint)(86400)) ;
	T_D1 = T_D1 % (bigint)(86400);
	T_hour = int(T_D1 / 3600) ;
	T_D1 = T_D1 % (bigint)(3600);
	T_min = int(T_D1 / (bigint)(60)) ;
	T_D1 = T_D1 % 60 ;
	T_sec = int(T_D1) ;
	IntervalDT Tem_IDT(T_day, T_hour, T_min, T_sec , T_DM1, T_Plus) ;
	DateTime Tem_DT(0,1,1,1,0,0,0,0) ;
	Tem_DT = Tem_IDT.Elapsed_2_Date() ;
	return Tem_DT ;
}

DateTime DateTime::operator +(const IntervalYM& arg) {

	int t_mon = arg.V_2_Month() ;
	int t_year = (isBC ? (year * -1) + 1 : year) , t_month = month + 1 ;
	bool t_BC = 0 ;
	t_year += int(t_mon/12) ;
	t_month += t_mon % 12 ;
	if (t_month < 1) {
		t_month = 12 + t_month ;
		t_year -- ;
	}
	if (t_month > 12) {
	   	t_month = t_month - 12 ;
	   	t_year ++ ;
	}
	if ( t_year < 1 ) {
	   	t_year -- ;
	   	t_year *= -1 ;
	   t_BC = 1 ;
	}
	DateTime Ret_D(t_BC, t_year, t_month, day,hour,min,sec,msec) ;
	return Ret_D ;
}

int DateTime::Find_Elapsed() {

/* This function calculates the day difference between given date and January
      1st of 1st year, concerns BC AND AD .
*/

	bigint Date_Base = (bigint)(year)*(bigint)(10000) + (bigint)(month+1)*(bigint)(100) + (bigint)(day) ; //converts number to base 100 
	int Temp_Days = 0 ;

// calculates total days of previous months

	for (int fd_c=0 ; fd_c < month ; fd_c++) Temp_Days += month_days[fd_c] ;
	Temp_Days += day-1 ;

// if BC subtract number of days from 366 to find reverse

	Temp_Days = ( isBC ? (365 - Temp_Days) : Temp_Days) ;

// Find number of days in the previous years, find days gained from February 29th 's 
// if current year is dividable to 4 and month is after february, add 1 for AD
// for BC if month is before february add 1

	Temp_Days += ( (year - 1) * 365) ;
	Temp_Days += ((year - (isBC ? 2 : 1)) /4) + (isBC && year>1? 1 : 0) ;
	Temp_Days += ( isBC ? ( (month<2) && ( ((year) % 4) == 1 ) ? 1 : 0 ) : ( (month>1) && ( (year % 4) == 0) ? 1 : 0 )) ;
	if (!isBC && (Date_Base > 17520902)) {
		Temp_Days -= 11 ;
		if ((month <2) && (month>=0) && (year%100 == 0)) {
			Temp_Days -= (year/100) - 18 ;
		}
		else {
			if (Date_Base>=18000301) Temp_Days -= (year/100) - 17 ;
		}
		Temp_Days += int(((year/100.0) - 20.0)/4 + ((year%400 == 0) && (month>=0) && (month<2)? 0 : 1) ) ;
	}
	return Temp_Days ;
}

DateTime IntervalDT::Elapsed_2_Date() {

	int tem_year = 0 ; 
	int tem_elap , tem_diff , tem_month,t_day=day ;
	if ((day == 0) && (min == 0) && (sec == 0) && (msec == 0)) {
		DateTime Ret_D(0,1,1,1,0, 0, 0 , 0) ;
		return Ret_D ;
	}
	if (V_Plus) {
		t_day = (t_day > 639797 ? t_day+11 : t_day) ;
		t_day ++ ;
		tem_year = int(t_day / 365.2425 +1 ) ;
		DateTime tem_Date(0,tem_year,1,1,0,0,0,0) ;
		tem_elap = tem_Date.Get_Elap() ;
		tem_elap = (tem_elap > 639797 ? tem_elap + 11 : tem_elap ) ;
		tem_diff = (isLeap(0,tem_year) ? 366 : 365) ;
		if ((t_day - tem_elap) > tem_diff ) {
			tem_elap += tem_diff ;
			tem_year ++ ;
		}
		if ((tem_elap - t_day) >= 0 ) {
			tem_diff = (isLeap(0,tem_year-1) ? 366 : 365) ;
			tem_year -- ;
			tem_elap -= tem_diff ;
		}
		tem_elap = t_day - tem_elap ;
		tem_month = 0 ;
		for (int tem_i = 0 ; tem_i < 12 ; tem_i ++) {
			tem_diff = (tem_i==1 ? (isLeap(0,tem_year) ? 29 : 28) : month_days[tem_i] ) ; 
			if ( tem_elap > tem_diff) tem_elap -= tem_diff ;
			else {
				tem_month = tem_i + 1 ;
				break ;
			}
		} 
	}
	else {
		tem_year = int(t_day / 365.25 + 1) ;
		if ((hour > 0) || (min > 0) || (sec > 0) || (msec > 0))  t_day ++ ;
		DateTime tem_Date(1,tem_year,12,31,0,0,0,0) ;
		tem_elap = tem_Date.Get_Elap() ;
		tem_diff = (isLeap(1,tem_year) ? 366 : 365) ;
		if ((t_day - tem_elap) > tem_diff) {
			tem_elap += tem_diff ;
			tem_year ++ ;
		}
		if ((tem_elap - t_day) > 0 ) {
			tem_diff = (isLeap(1,tem_year-1) ? 365 : 366 ) ;
			tem_year -- ;
			tem_elap -= tem_diff ;
		}
		tem_elap = tem_diff - t_day + tem_elap ;
		tem_month = 0 ;
		for (int tem_i = 0 ; tem_i < 12 ; tem_i ++ ) {
			tem_diff = (tem_i == 1 ? (isLeap(1,tem_year) ? 29 : 28) : month_days[tem_i] ) ;
			if (tem_elap > tem_diff) tem_elap -= tem_diff ;
			else {
				tem_month = tem_i + 1 ;
				break ;
			}
		}
		if ((hour > 0) || (min > 0) || (sec > 0) || (msec > 0)) { 
			int TT[4][2] = 
				{{hour, 24}, {min, 60}, {sec, 60}, {msec, s_sec}} ;
			int add_val = 0 ;
			for (int I_T = 3 ; I_T>=0 ; I_T --) {   
				if (add_val !=0) TT[I_T][0] = TT[I_T][1] -  TT[I_T][0] ;
				if ((TT[I_T][0] !=0) && (add_val==0)) {
					TT[I_T][0] = TT[I_T][1] - TT[I_T][0] ;
					add_val=1 ;
				}
			}
			hour = TT[0][0] ;
			min = TT[1][0] ;
			sec = TT[2][0] ;
			msec = TT[3][0] ;
		}
	}
	DateTime Ret_D(!V_Plus,tem_year,tem_month,tem_elap,hour, min, sec, msec) ;
	return Ret_D ;
}

bigint IntervalDT::Value_2_Sec() const {
	return ((bigint)(day) * (bigint)(86400) + (bigint)(hour) * (bigint)(3600) + (bigint)(min) * (bigint)(60) + (bigint)(sec)) * (!V_Plus ? (bigint)(-1) : (bigint)(1)) ;
}

int IntervalDT::Value_SSec () const {
	return msec* (!V_Plus ? -1 : 1) ;
}

bool isLeap(bool L_BC,int L_Year) {
     if (L_BC) return (((L_Year-1) % 4 ) == 0 ? 1 : 0) ;
     if (L_Year <= 1752) return (((L_Year) % 4 ) == 0 ? 1 : 0) ;
     if (((L_Year % 4) == 0) && (((L_Year % 100 ) != 0) || ((L_Year % 400) == 0 ))) return 1 ;
     return 0 ;
}

ostream& operator <<(ostream& out, const EncodedDTF& arg){
	int year = arg.getYear();
	int month = arg.getMonth();
	int day = arg.getDay();
	bool bc = false;
	if(year < 0){
		bc = true;
		year = - year;
	}
	if(year){
		out << year << "-";
	}
	if(month){
		out << month << "-";
	}
	if(day){
		out << day << " ";
	}
	if(bc){
		out << "bc ";
	}
	out << arg.getHour() << ":";
	out << arg.getMin() << ":";
	out << arg.getSec();
	int nanosec = arg.getNanoSec();
	if(nanosec){
		out << "." << nanosec;
	}
	return out;
}

ostream& operator <<(ostream& out, const EncodedIDT& arg) {
	int o_day = arg.getDay() ;
	int o_hour = arg.getHour() ;
	int o_min = arg.getMin() ;
	int o_sec = arg.getSec() ;
	int o_msec = arg.getNanoSec() ;
	int o_V_Plus = arg.get_V_Plus() ; 
	if (!(o_V_Plus)) {
		out << "- " ;
	}
	if (o_day) {
		out << o_day << "d " ;
	}
	if (o_hour) {
		out << o_hour << "h " ;
	}
	if (o_min) {
		out << o_min << "m " ;
	}
	if (o_min) {
		out << o_sec << "s " ;
	}
	if (o_sec) {
		out << o_msec << "n " ;
	}
	return out ;
}

EncodedIDT EncodedDTF::operator-(const EncodedDTF& x) const {
	return (EncodedIDT)((DateTime)(*this) - (DateTime)(x)) ;
}

EncodedDTF EncodedDTF::operator-(const EncodedIDT& x) const {
	return (EncodedDTF)((DateTime)(*this) - (IntervalDT)(x)) ;
}

EncodedDTF EncodedDTF::operator+(const EncodedIDT& x) const {
	return (EncodedDTF)((DateTime)(*this) + (IntervalDT)(x)) ;
}

EncodedIDT EncodedIDT::operator-(const EncodedIDT& arg) {
	return (EncodedIDT)((IntervalDT)(*this) - (IntervalDT)(arg)) ;
}

EncodedIDT EncodedIDT::operator+(const EncodedIDT& arg) {
	return (EncodedIDT)((IntervalDT)(*this) + (IntervalDT)(arg)) ;
}
