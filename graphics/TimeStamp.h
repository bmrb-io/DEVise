/* TimeStamp.h */
#ifndef TimeStamp_h
#define TimeStamp_h

class TimeStamp {
public:
	static int NextTimeStamp() { return _timeStamp++; }
private:
	static _timeStamp;
};
#endif
