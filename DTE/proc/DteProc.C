// (C) 1998- by Kevin S. Beyer

#include "DTE/proc/DteProc.h"
#include "DTE/util/Del.h"
#include "DTE/comm/DteAdtPage.h"
#include "Iterator.h"

//---------------------------------------------------------------------------

PrintIteratorProc::PrintIteratorProc(Iterator* iter)
: iter(iter)
{
}


PrintIteratorProc::~PrintIteratorProc()
{
  delete iter;
}


void PrintIteratorProc::run()
{
  const DteTupleAdt& tupAdt = iter->getAdt();
  cerr << "return type: " << tupAdt.getTypeSpec() << endl;
  int n = 0;
  const Tuple* tup = iter->getFirst();
  while(tup) {
    n++;
    tupAdt.toAscii(cout, tup);
    //tupAdt.print(cout, tup);
    cout << '\n';
    tup = iter->getNext();
  }
  cerr << "\nread " << n << " records\n";
}


//---------------------------------------------------------------------------


ConsumeIteratorProc::ConsumeIteratorProc(Iterator* iter)
: iter(iter)
{
}


ConsumeIteratorProc::~ConsumeIteratorProc()
{
  delete iter;
}


void ConsumeIteratorProc::run()
{
  cerr << "starting consume\n";
  const Tuple* tup = iter->getFirst();
  int n = 0;
  while(tup) {
    n++;
    tup = iter->getNext();
  }
  cerr << "read " << n << " records\n";
}

//---------------------------------------------------------------------------

WriteIteratorProc::WriteIteratorProc(Iterator* iter, const string& filename)
  : iter(iter), filename(filename)
{
}


WriteIteratorProc::~WriteIteratorProc()
{
  delete iter;
}


void WriteIteratorProc::run()
{
  //kb: use HeapInserter
  int fd = ::open(filename.c_str(), O_WRONLY|O_CREAT|O_TRUNC, 0666);
  assert(fd >= 0 && "error opening output file");
  const DteTupleAdt& tupAdt = iter->getAdt();
  cerr << "return type: " << tupAdt.getTypeSpec() << endl;
  Del<PageBuf> pageBuf = new PageBuf;
  DteAdtPage page(tupAdt);
  page.initPage(pageBuf);
  int n = 0;
  const Tuple* tup = iter->getFirst();
  while(tup) {
    n++;
    const Tuple* newTup = page.addTuple(tup);
    if( !newTup ) {             // tuple didn't fit on page
      page.write(fd);
      page.reinitPage();
      newTup = page.addTuple(tup);
      assert(newTup && "tuple didn't fit on an empty page!");
    }
    tup = iter->getNext();
  }
  if( page.getNumSlots() > 0 ) { // write last page
    page.write(fd);
  }
  close(fd);
  cerr << "\nwrote " << n << " records\n";
}


#if 0
//---------------------------------------------------------------------------

#include "thread_page_pipe.h"
#include "TupleStream.h"


dte_pipe_t::dte_pipe_t(Iterator* iter, thread_page_pipe_t& pipe)
: _td(pipe.tuple_desc()), _pipe(pipe), iter(iter)
{
  assert(iter);
}


dte_pipe_t::~dte_pipe_t()
{
  delete iter;
}


void dte_pipe_t::run()
{
  //kb: replace switch with function pointers later?
  cerr << "starting writer" << endl;
  //_pipe.start();
  {
//cerr << "here1!!\n";
    thread_page_pipe_writer_t page_stream(_pipe);
//cerr << "here2!!\n";
    OTupleStream ts(_td, page_stream);
    int attrs = _td.NumAttrs();
    int n = 0;
    const Tuple* dtetup = iter->getFirst();
    char* cp;
    while( dtetup ) {
      n++;
      for(int i = 0 ; i < attrs ; i++) {
        uint type = _td.GetAttrType(i);
        switch( type ) {
        case Mql::INT4_TYPE:
          ts.WriteInt4((int4)dtetup[i]);
          break;
        case Mql::BOOL_TYPE:
          ts.WriteBool((int4)dtetup[i]);
          break;
        case Mql::REAL8_TYPE:
          ts.WriteReal8(*(real8*)dtetup[i]);
          break;
        case Mql::STRING_TYPE:
          cp = (char*)dtetup[i];
          ts.WriteString(cp, strlen(cp)+1);
          break;
        case Mql::VARSTRING_TYPE:
          cp = (char*)dtetup[i];
          ts.WriteVarString(cp, strlen(cp)+1);
          break;
        default:
          assert(!"unknown type");
        }
      }
      assert(ts.EndTuple());
      dtetup = iter->getNext();
    }
    cerr << "Wrote " << n << " tuples" << endl;
  }
  //_pipe.end();
  cerr << "done writing" << endl;
}


//---------------------------------------------------------------------------

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


write_pipe_t::write_pipe_t(thread_page_pipe_t& pipe, string filename)
: _pipe(pipe), _filename(filename)
{
  _recv_id = _pipe.open();
}


write_pipe_t::~write_pipe_t()
{
}


void write_pipe_t::run()
{
  cerr << "starting pipe file writer: " << _filename << endl;
  int fd;
  //W_COERCE(open(_filename.c_str(), OPEN_CREATE|OPEN_WRONLY, 0666, fd));
  fd = ::open(_filename.c_str(), O_CREAT|O_TRUNC|O_WRONLY, 0666);
  assert(fd >= 0);
  thread_page_pipe_t::ref_page_t* page;
  //int n = 0;
  int s = 0;
  Page p;
  while( (page = _pipe.get_page(_recv_id)) != NULL ) {
    //n++;
    p.SetPage(page->p);
    s += p.NumSlots();
    //W_COERCE( write(fd, page, sizeof(*page)) );
    assert( ::write(fd, page->p, mql_pagesize) == mql_pagesize );
    _pipe.free_page(page);
  }
  //W_COERCE(close(fd));
  ::close(fd);
  cerr << "done writing pipe (" << s << " tuples) to " << _filename << endl;
  _pipe.close();
  cerr << "pipe closed\n";
}


//---------------------------------------------------------------------------

/*
load_pipe_t::load_pipe_t(MqlTupleDesc& td, int num_pages, string filename)
: _pipe(td, num_pages), _filename(filename)
{
}
*/

load_pipe_t::load_pipe_t(string filename, thread_page_pipe_t& pipe)
: _pipe(pipe), _filename(filename)
{
}


load_pipe_t::~load_pipe_t()
{
}


void load_pipe_t::run()
{
  cerr << "starting pipe file loader" << endl;
  int fd;
  //W_COERCE(open(_filename.c_str(), O_RDONLY, 0666, fd));
  fd = ::open(_filename.c_str(), O_RDONLY);
  assert(fd >= 0);
  _pipe.start();
  thread_page_pipe_t::ref_page_t* page;
  //w_rc_t rc;
  int rc;
  //int n = 0;
  int s = 0;
  Page p;
  while(1) {
    //n++;
    page = _pipe.alloc_page();
    //rc = read(fd, page, sizeof(*page));
    rc = ::read(fd, page->p, mql_pagesize);
    if( rc <= 0 ) {
      assert(rc == 0);
      _pipe.free_page(page);
      break;
    } else {
      assert(rc == mql_pagesize);
      p.SetPage(page->p);
      s += p.NumSlots();
      _pipe.send_page(page);
    }
  //} while( rc == RCOK );
  }
  //W_COERCE(close(fd));
  ::close(fd);
  _pipe.shutdown();
  cerr << "done loading pipe (" << s << " tuples) from " << _filename << endl;
  //cerr << "done loading pipe from " << _filename << endl;
}


//---------------------------------------------------------------------------
#include "RTreeRead.h"

build_rtree_proc_t::build_rtree_proc_t(string filename, vector<int>* key_pos,
                                       vector<int>* add_pos, Iterator* input)
: _filename(filename), _key_pos(key_pos), _add_pos(add_pos), _input(input)
{
}
  
build_rtree_proc_t::~build_rtree_proc_t()
{
  delete _key_pos;
  delete _add_pos;
}

void build_rtree_proc_t::run()
{
  create_rtree(_filename, _input, *_key_pos, *_add_pos);
}


//---------------------------------------------------------------------------
#include "MqlExternalStream.h"

send_proc_t::send_proc_t(Iterator* iter, int stream_id, 
                         int fd, mql_mutex_t& mutex)
: iter(iter), _td(convert_to_mql_types(iter->getTypes())),
  _stream_id(stream_id), _fd(fd), _mutex(mutex)
{
}


send_proc_t::~send_proc_t()
{
    delete iter;
}


void send_proc_t::run()
{
  extern int dataPageSize;
  int n = 0;
  {
    MqlExternalOutputStream out(_stream_id, _fd, dataPageSize, _mutex);
    const Tuple* tup = iter->getFirst();
    int fields = _td.NumAttrs();
    while(tup) {
      n++;
      for(int i = 0 ; i < fields ; i++){
        uint type = _td.GetAttrType(i);
        switch( type ) {
        case Mql::INT4_TYPE:
          out.writeInt((int4)tup[i]);
          break;
          //case Mql::BOOL_TYPE:
          //  out.writeBool((int4)dtetup[i]);
          //  break;
        case Mql::REAL8_TYPE:
          out.writeDouble(*(real8*)tup[i]);
          break;
        case Mql::STRING_TYPE:
        case Mql::VARSTRING_TYPE:
          out.writeString((char*)tup[i]);
          break;
        default:
          assert(!"unknown type");
        }
      }
      tup = iter->getNext();
    }
  }
  cerr << "\nsent " << n << " records\n";
}


//---------------------------------------------------------------------------
#include "TinyPlot.h"

plot_proc_t::plot_proc_t(Iterator* iter, int stream_id, 
                         int fd, mql_mutex_t& mutex,
                         int xsize, int ysize,
                         double xmin, double ymin,
                         double xmax, double ymax)
: iter(iter), _td(convert_to_mql_types(iter->getTypes())),
  _stream_id(stream_id), _fd(fd), _mutex(mutex)
{
  int fields = _td.NumAttrs();
  assert(fields == 4);
  for(int i = 0 ; i < fields ; i++) {
    uint type = _td.GetAttrType(i);
    assert( type == Mql::REAL8_TYPE );
  }
  _xsize = xsize;
  _ysize = ysize;
  _xmin = xmin;
  _ymin = ymin;
  _xmax = xmax;
  _ymax = ymax;
}


plot_proc_t::~plot_proc_t()
{
    delete iter;
}


void plot_proc_t::run()
{
  extern int dataPageSize;
  int n = 0;
  {
    MqlExternalOutputStream out(_stream_id, _fd, dataPageSize, _mutex);
    TinyPlot plot(_xsize, _ysize);
    plot.setBounds(_xmin, _ymin, _xmax, _ymax);
    
    const Tuple* tup = iter->getFirst();
    while(tup) {
      n++;
      double x = *(real8*)tup[0];
      double y = *(real8*)tup[1];
      double xlen = *(real8*)tup[2];
      double ylen = *(real8*)tup[3];
      plot.fillRect(x, y, xlen, ylen);
      tup = iter->getNext();
    }
    out.writeBytes(plot.getPixmap(), plot.getNumBytes());

    //char buf[100];
    //sprintf(buf, "/tmp/tinyplot.%d", _stream_id);
    //ofstream of(buf);
    //of.write(plot.getPixmap(), plot.getNumBytes());
  }
  cerr << "plotted " << n << " records on stream " << _stream_id << endl;
}


//---------------------------------------------------------------------------

ChiSquaredProc::ChiSquaredProc(Iterator* iter, int cardA, int cardB)
: iter(iter), cardA(cardA), cardB(cardB)
{
  assert(iter != NULL);
  assert(cardA > 0);
  assert(cardB > 0);
  A = new int[cardA];
  B = new int[cardB];
  AB = new int[cardA*cardB];
  memset(A, 0, cardA*sizeof(int));
  memset(B, 0, cardB*sizeof(int));
  memset(AB, 0, cardA*cardB*sizeof(int));
}


ChiSquaredProc::~ChiSquaredProc()
{
  delete AB;
  delete B;
  delete A;
  delete iter;
}


void ChiSquaredProc::run()
{
  int n = 0;
  const Tuple* tup = iter->getFirst();
  while(tup) {
    n++;
    int a = (int)tup[0];
    int b = (int)tup[1];
    assert(a >= 0 && a < cardA);
    assert(b >= 0 && b < cardB);
    A[a]++;
    B[b]++;
    AB[a + b*cardA]++;
    tup = iter->getNext();
  }
  double chisq = 0.0;
  double chisq2 = 0.0;
  double maxDiff = 0.0;
  int maxDiffa, maxDiffb, maxDiffA, maxDiffB, maxDiffAB;
  double posDiff = 0;
  int numPosDiff = 0;
  for(int a = 0 ; a < cardA ; a++) {
    if( A[a] > 0 ) {
      for(int b = 0 ; b < cardB ; b++) {
        if( B[b] > 0 ) {
          double EAB = double(A[a])/n * B[b];
          int ab = AB[a + b*cardA];
          double diff = ab - EAB;
          chisq += (diff * diff) / EAB;
          if( diff > 0 ) {
            numPosDiff++;
            posDiff += diff;
            chisq2 += (diff * diff) / EAB;
            if( diff > maxDiff ) {
              maxDiff = diff;
              maxDiffa = a;
              maxDiffb = b;
              maxDiffA = A[a];
              maxDiffB = B[b];
              maxDiffAB = ab;
            }
          }
        }
      }
    }
  }
  cerr << "Chi-Squared = " << chisq << ", only positive = " << chisq2 << endl;
  cerr << "Max diff = " << maxDiff << " at " << maxDiffa << ',' << maxDiffb
    << " counts = " << maxDiffA << ',' << maxDiffB << ',' << maxDiffAB << endl;
  cerr << "positive differences: " << numPosDiff << ", avg = " << 
    posDiff / numPosDiff << endl;
}
#endif

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
