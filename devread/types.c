#include "types.h"

Type* intAdd(Type* arg1, Type* arg2){
     return new IInt(((IInt*)arg1)->value + ((IInt*)arg2)->value);
}

Type* intEq(Type* arg1, Type* arg2){
     if(((IInt*)arg1)->value == ((IInt*)arg2)->value){
		return new IInt(1);
	}
	else{
		return new IInt(0);
	}
}

Type* intLT(Type* arg1, Type* arg2){
     if(((IInt*)arg1)->value < ((IInt*)arg2)->value){
		return new IInt(1);
	}
	else{
		return new IInt(0);
	}
}
