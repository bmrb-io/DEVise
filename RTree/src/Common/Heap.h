#ifndef SIMPLE_HEAP_FILE__
#define SIMPLE_HEAP_FILE__

#define ARRAY_HEAP_SIZE 1024

class ArrayHeap {
  private :
	int priority[ARRAY_HEAP_SIZE]; 
	int value[ARRAY_HEAP_SIZE];
	int size;

	void correct(int place);
  public :
	ArrayHeap();
	~ArrayHeap();
	void insert(int v,int p);
	int pop();
	int top_value();
	int top_priority();
	int get_size();

        void reset();
	void display();
};

#endif
