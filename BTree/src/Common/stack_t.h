#ifndef _STACK_T_H
#define _STACK_T_H

template <class T> class jon_stack_t 
{
  private:
    T * stk;
    int top_ent;
    int max;


  public:
    jon_stack_t(int new_max)	
      { 
	top_ent = 0;
	max = new_max;
	stk = new T[max];
      }

    ~jon_stack_t() 
      {
	delete [] stk;
      }

    int size() 
      {
        return top_ent;
      }

    bool full() 	
      { 
	return top_ent >= max; 
      }

    bool empty()	
      { 
	return top_ent == 0; 
      }

    void push(T& t)  
      {
	if (!full())
	  stk[top_ent++] = t;
      }

    T& pop() 
      {
        if (!empty())
	  return stk[--top_ent];
	else
	  {
	    printf("Popped empty stack in jon_stack_t::pop\n");
	    return stk[0];
	  }
      }

    T& top() 
      {
        if (!empty())
	  return stk[top_ent - 1];
	else
	  {
	    printf("topped empty stack in jon_stack_t::top\n");
	    return stk[0];
	  }
      }
};

#endif