/* StringArray.h: implements array of strings */
#ifndef StringArray_h
#define StringArray_h
#include "libc.h"

unsigned const int StringArrayIncrement = 20;

class StringArray {
public:
	StringArray(){
		_numStrings = 0;
		_arraySize = 20;
		_array = new char *[20];
	};

	/* append a string */
	void Append(char *string){
		if (_numStrings == _arraySize){
			char **temp = new char *[_arraySize +StringArrayIncrement];
			for (int i=0; i < _numStrings; i++){
				temp[i] = _array[i];
			}
			delete _array;
			_array = temp;
		}
		_array[_numStrings++] = string;
	};

	/* delete a string */
	void Delete(int index){
		if (index < 0 || index >= _numStrings){
			fprintf(stderr,"StringArray:Delete invalid index %d\n",index);
			exit(2);
		}
		/* move rest of the array up.*/
		if (index < _numStrings-1){
			for (int i = index+1; i < _numStrings-1; i++)
				_array[i] = _array[i+1];
		}
		_numStrings--;
	};

	/* find 1st index of a string, or -1 if not found  */
	int Find(char *string){
		for (int i=0; i < _numStrings;i++)
			if (strcmp(string,_array[i])==0)
				return i;
		return -1;
	};

	void Get(char **&strings, int &num){
		strings = _array; 
		num = _numStrings;
	}

private:
	int _numStrings ;
	int _arraySize;
	char **_array;
};
#endif
