#ifndef TYPES_H
#define TYPES_H

#include <iostream.h>
#include <assert.h>
#include <String.h>
#include <sys/types.h>
#include <netinet/in.h>

typedef void Type;
typedef String TypeID;
typedef Type* (*OperatorPtr)(Type*, Type*);

Type* intAdd(Type* arg1, Type* arg2);
Type* intEq(Type* arg1, Type* arg2);
Type* intLT(Type* arg1, Type* arg2);

class IInt {
public:
     int value;
     IInt() {}
     IInt(int i) : value(i) {}
	void display(ostream& out){
		out << value;
	}
	int packSize(){
		return sizeof(IInt);
	}
	void marshal(char* to){
		int tmp = htonl(value);
		memcpy(to, &tmp, sizeof(int));
	}
	void unmarshal(const char* from){
		int tmp;
		memcpy(&tmp, from, sizeof(int));
		value = ntohl(tmp);
	}
	static OperatorPtr getOperatorPtr(
		String name, TypeID arg, TypeID& retType){
		if(arg != "IInt"){
			return NULL;
		}
		if(name == "+"){
			retType = "IInt";
			return intAdd;
		}
		else if(name == "="){
			retType = "IInt"; // should be "MyBool";
			return intEq;
		}
		else if(name == "<"){
			retType = "IInt"; // should be "MyBool";
			return intLT;
		}
		else{
			return NULL;
		}
	}
	bool isNull(){
		return value == 0;
	}
};

class IDouble {
private:
	double value;
public:
	IDouble(double val) : value(val){}
};

class IString {
     int length;
	char* string;
public:
     IString() : length(0), string(NULL){}
	IString(char* s) : length(strlen(s) + 1), string(s){}
     IString(char* s, int l) : length(l), string(s){}
	void display(ostream& out){
		assert(string);
		out << string;
	}
	int packSize(){
		return sizeof(int) + length;
	}
	void marshal(char* to){
		int tmp = htonl(length);
		memcpy(to, &tmp, sizeof(int));
		memcpy(to + sizeof(int), string, length);
	}
	void unmarshal(char* from){
		int tmp;
		memcpy(&tmp, from, sizeof(int));
		length = ntohl(tmp);
		string = new char[length];
		memcpy(string, from + sizeof(int), length);
	}
	static OperatorPtr getOperatorPtr(
		String name, TypeID arg, TypeID& retType){
		return NULL;
	}
};

inline void displayAs(ostream& out, void* adt, String type){
	if(type == "IInt"){
		((IInt*) adt)->display(out);
	}
	else if(type == "IString"){
		((IString*) adt)->display(out);
	}
	else{
		cout << "No such type: " << type << endl;
		assert(0);
	}
}

inline int packSize(void* adt, String type){
	if(type == "IInt"){
		return ((IInt*) adt)->packSize();
	}
	else if(type == "IString"){
		return ((IString*) adt)->packSize();
	}
	else{
		cout << "No such type: " << type << endl;
		assert(0);
	}
}

inline void marshal(Type* adt, char* to, String type){
	if(type == "IInt"){
		((IInt*) adt)->marshal(to);
	}
	else if(type == "IString"){
		((IString*) adt)->marshal(to);
	}
	else{
		cout << "No such type: " << type << endl;
		assert(0);
	}
}

inline Type* unmarshal(char* from, String type){
	if(type == "IInt"){
		IInt* adt = new IInt;
		adt->unmarshal(from);
		return adt;
	}
	else if(type == "IString"){
		IString* adt = new IString;
		adt->unmarshal(from);
		return adt;
	}
	else{
		cout << "No such type: " << type << endl;
		assert(0);
	}
}

inline OperatorPtr getOperatorPtr(
	String name, TypeID root, TypeID arg, TypeID& retType){
	if(root == "IInt"){
		return IInt::getOperatorPtr(name, arg, retType);
	}
	else if(root == "IString"){
		return IString::getOperatorPtr(name, arg, retType);
	}
	else{
		cout << "No such type: " << root << endl;
		assert(0);
	}
}

#endif
