#ifndef _DTEPROC_H_
#define _DTEPROC_H_

// (C) 1998- by Kevin S. Beyer

#include <vector>
#include <string>
#include "DTE/proc/DteThread.h"


//---------------------------------------------------------------------------

// runs a set of threads in parallel
class DteProcList
{
public:

  DteProcList();

  ~DteProcList();

  void push_back(DteThread* step);

  void clear();

  void run();

  int size() { return steps.size(); }

protected:

  vector<DteThread*> steps;

private:

  DteProcList(const DteProcList& x);
  DteProcList& operator=(const DteProcList& x);
};


inline
DteProcList::DteProcList()
{
}


inline
void DteProcList::clear()
{
  vector<DteThread*>::iterator i;
  for(i = steps.begin() ; i != steps.end() ; i++) {
    delete *i;
  }
  steps.clear();
}


inline
DteProcList::~DteProcList()
{
  clear();
}


inline
void DteProcList::push_back(DteThread* step)
{
  steps.push_back(step);
}


inline
void DteProcList::run()
{
  //vector<DteThread*>::iterator i;
  int n = steps.size();
  for(int i = 0 ; i < n ; i++) {
    steps[i]->fork();
  }
  cerr << "all threads forked" << endl;
  // must wait in reverse because pipe readers are after pipe writers
  // and pipe writers wont end until the pipe reader closes
  for(int i = n - 1 ; i >= 0 ; i--) {
    steps[i]->wait();
    delete steps[i];            // pipe closed on delete
  }
  //kb: I would prefer to not delete the plan so it can be re-evaluated,
  //kb: but pipes don't support it yet...
  steps.clear();
}



//---------------------------------------------------------------------------

class Iterator;

class PrintIteratorProc
: public DteThread
{
public:

  PrintIteratorProc(Iterator* iter);

  ~PrintIteratorProc();

  void run();

protected:

  Iterator* iter;

private:

  PrintIteratorProc(const PrintIteratorProc& x);
  PrintIteratorProc& operator=(const PrintIteratorProc& x);
};


//---------------------------------------------------------------------------

class ConsumeIteratorProc
: public DteThread
{
public:

  ConsumeIteratorProc(Iterator* iter);

  ~ConsumeIteratorProc();

  void run();

protected:

  Iterator* iter;

private:

  ConsumeIteratorProc(const ConsumeIteratorProc& x);
  ConsumeIteratorProc& operator=(const ConsumeIteratorProc& x);
};


//---------------------------------------------------------------------------

class WriteIteratorProc
: public DteThread
{
public:

  WriteIteratorProc(Iterator* iter, const string& filename);

  ~WriteIteratorProc();

  void run();

protected:

  Iterator* iter;

  string filename;

private:

  WriteIteratorProc(const WriteIteratorProc& x);
  WriteIteratorProc& operator=(const WriteIteratorProc& x);
};


#if 0
//---------------------------------------------------------------------------
#include "thread_page_pipe.h"

class dte_pipe_t
: public DteThread
{
public:

  dte_pipe_t(Iterator* iter, thread_page_pipe_t& pipe);

  ~dte_pipe_t();

  void run();

  //thread_page_pipe_t& pipe() { return _pipe; }

protected:

  MqlTupleDesc _td;
  thread_page_pipe_t& _pipe;
  Iterator* iter;

private:

  dte_pipe_t(const dte_pipe_t& x);
  dte_pipe_t& operator=(const dte_pipe_t& x);
};


//---------------------------------------------------------------------------

class write_pipe_t
: public DteThread
{
public:

  write_pipe_t(thread_page_pipe_t& pipe, string filename);

  ~write_pipe_t();

  void run();

protected:

  thread_page_pipe_t& _pipe;

  string _filename;

  int _recv_id;

private:

  write_pipe_t(const write_pipe_t& x);
  write_pipe_t& operator=(const write_pipe_t& x);
};


//---------------------------------------------------------------------------

class load_pipe_t
: public DteThread
{
public:

  //load_pipe_t(MqlTupleDesc& td, int num_pages, string filename);

  load_pipe_t(string filename, thread_page_pipe_t& pipe);

  ~load_pipe_t();

  void run();

  //thread_page_pipe_t& pipe() { return _pipe; }

protected:

  thread_page_pipe_t& _pipe;

  string _filename;

private:

  load_pipe_t(const load_pipe_t& x);
  load_pipe_t& operator=(const load_pipe_t& x);
};


//---------------------------------------------------------------------------

class build_rtree_proc_t
: public DteThread
{
public:

  build_rtree_proc_t(string filename, vector<int>* key_pos,
                     vector<int>* add_pos, Iterator* input);
  
  ~build_rtree_proc_t();

  void run();

protected:

  string _filename;

  vector<int>* _key_pos;

  vector<int>* _add_pos;

  Iterator* _input;

private:

  build_rtree_proc_t(const build_rtree_proc_t& x);
  build_rtree_proc_t& operator=(const build_rtree_proc_t& x);
};


//---------------------------------------------------------------------------


class send_proc_t
: public DteThread
{
  public:

    send_proc_t(Iterator* iter, int stream_id, int fd, mql_mutex_t& mutex);

    ~send_proc_t();

    void run();

  protected:

    Iterator* iter;
    MqlTupleDesc _td;
    int _stream_id;
    int _fd;
    mql_mutex_t& _mutex;

  private:

    send_proc_t(const send_proc_t& x);
    send_proc_t& operator=(const send_proc_t& x);
};



//---------------------------------------------------------------------------


class plot_proc_t
: public DteThread
{
  public:

    plot_proc_t(Iterator* iter, int stream_id,
                int fd, mql_mutex_t& mutex,
                int xsize, int ysize,
                double xmin, double ymin,
                double xmax, double ymax);

    ~plot_proc_t();

    void run();

  protected:

    Iterator* iter;
    MqlTupleDesc _td;
    int _stream_id;
    int _fd;
    mql_mutex_t& _mutex;

    int _xsize;
    int _ysize;
    double _xmin;
    double _ymin;
    double _xmax;
    double _ymax;

  private:

    plot_proc_t(const plot_proc_t& x);
    plot_proc_t& operator=(const plot_proc_t& x);
};



//---------------------------------------------------------------------------


class ChiSquaredProc
: public DteThread
{
  public:

    ChiSquaredProc(Iterator* iter, int cardA, int cardB);

    ~ChiSquaredProc();

    void run();

  protected:

    Iterator* iter;
    const int cardA;
    const int cardB;
    int* A;
    int* B;
    int* AB;

  private:

    ChiSquaredProc(const ChiSquaredProc& x);
    ChiSquaredProc& operator=(const ChiSquaredProc& x);
};
#endif

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

#endif // _DTEPROC_H_
