// (C) 1999- by Kevin S. Beyer

#include <iostream.h>
#include <assert.h>
#include "DTE/proc/RadixCube.h"
//#include "MQLTypes.h"


const static int MAX_RECS = 2000000;
//const static int MAX_PAGES = (200*1024*1024) / mql_pagesize;
static int numDims = 0;
static int countThreshold = 1;
#if defined(RC_MAXDEPTH)
static int maxDepth = 0;
#endif

static int numMeasures = 0;
static int numInputFields = 0;
static int numOutputFields = 0;
static int numOutputRecords = 0;
static int* outputRec = NULL;
// these point into outputRec
static int* outputMin = NULL;
static int* outputMax = NULL;
static int* outputSum = NULL;
static int* outputCount = NULL;

static int* card = NULL;
static int* sortCount = NULL;
static int** dataCount = NULL;


static int depth = 0;
#if 0
void PrintDimHeaders()
{
  for(int i = 0 ; i < indent ; i++) cout << ' ';
  for(int i = 0 ; i < numDims ; i++) {
    if( outputRec[i] >= 0 ) {
      cout << (char)('A'+i);
    }
  }
}
#endif

static void PrintDimValues()
{
  for(int i = 0 ; i < numDims ; i++) {
    if( outputRec[i] >= 0 ) {
      cout << ' ' << outputRec[i];
    }
  }
}

static int** sortTable = NULL;

static void CountingSort(int** in, int N, int dim, int& buckets, int* counts)
{
  int B = buckets;

  memset(sortCount, 0, B * sizeof(int));
  //    for(int i = 0 ; i < B ; i++) { // clear counts
  //      sortCount[i] = 0;
  //    }
  int x = 0;
  for(int i = 0 ; i < N ; i++) { // count number in each bucket
    x = in[i][dim];
    //assert(x >= 0 && x < card[dim]);
    sortCount[x]++;
  }
#if 1
  if( sortCount[x] == N ) {     // all duplicates!
    counts[0] = N;
    buckets = 1;
    return;
  }
  int C = 0;
  if( sortCount[0] ) counts[C++] = sortCount[0]; // copy for parent
  for(int j = 1 ; j < B ; j++) {
    if( sortCount[j] ) counts[C++] = sortCount[j]; // copy for parent
    sortCount[j] += sortCount[j-1]; // make cumulative counts
  }
  buckets = C;
#else
  counts[0] = sortCount[0]; // copy for parent
  for(int j = 1 ; j < B ; j++) {
    counts[j] = sortCount[j]; // copy for parent
    sortCount[j] += sortCount[j-1]; // make cumulative counts
  }
  buckets = B;
#endif
  for(int i = N-1 ; i >= 0 ; i--) { // put pointers in order
    sortTable[--sortCount[in[i][dim]]] = in[i];
  }
  memcpy(in, sortTable, N * sizeof(int));
  //    //ksb: do it this way?
  //    for(int i = 0 ; i < N ; i++) { // copy pointers back to input
  //      in.setRec(i, sortTable[i]);
  //    }
}

static void CountingSort2(int** in, int N, int dim, int& buckets, int* counts)
{
  int B = buckets;

  memset(sortCount, 0, B * sizeof(int));
  //    for(int i = 0 ; i < B ; i++) { // clear counts
  //      sortCount[i] = 0;
  //    }
  int x = 0;
  for(int i = 0 ; i < N ; i++) { // count number in each bucket
    x = in[i][dim];
    //assert(x >= 0 && x < card[dim]);
    sortCount[x]++;
  }
#if 0
  if( sortCount[x] == N ) {     // all duplicates!
    counts[0] = N;
    buckets = 1;
    return;
  }
  int C = 0;
  if( sortCount[0] ) counts[C++] = sortCount[0]; // copy for parent
  for(int j = 1 ; j < B ; j++) {
    if( sortCount[j] ) counts[C++] = sortCount[j]; // copy for parent
    sortCount[j] += sortCount[j-1]; // make cumulative counts
  }
  buckets = C;
#else
  //counts[0] = sortCount[0]; // copy for parent
  for(int j = 1 ; j < B ; j++) {
    //counts[j] = sortCount[j]; // copy for parent
    sortCount[j] += sortCount[j-1]; // make cumulative counts
  }
#endif
  for(int i = N-1 ; i >= 0 ; i--) { // put pointers in order
    sortTable[--sortCount[in[i][dim]]] = in[i];
  }
  //memcpy(in, sortTable, N * sizeof(int));
  //    //ksb: do it this way?
  //    for(int i = 0 ; i < N ; i++) { // copy pointers back to input
  //      in.setRec(i, sortTable[i]);
  //    }
}


#include "DTE/util/SortAlgo.h"
static int cmpDim;
template <class T>
struct ptr_less : public binary_function<T, T, bool> {
  bool operator()(const T* x, const T* y) const {
    return x[cmpDim] < y[cmpDim];
  }
};

template<int card>
struct TypeT
{
  int a[card];
  bool operator<(const TypeT& x) const {
    return a[cmpDim] < x.a[cmpDim];
  }
};


static int Cmp(int** a, int** b)
{
  return (*a)[cmpDim] - (*b)[cmpDim];
}

// #define RC_SORTS
#if defined(RC_SORTS)
static long long sorts = 0;
static long long recsSorted = 0;
#endif

static void Partition(int** in, int N, int dim, int& buckets, int* counts)
{
#if defined(RC_SORTS)
  sorts++;
  recsSorted += N;
#endif
#if 0
  cout << "sort " << N << " on " << char('A'+dim) << ": ";
  outputRec[dim] = 9;
  PrintDimHeaders();
  PrintDimValues();
  cout << endl;
#endif

  //cout << dim << ' ' << depth << ' ' << N << ' ' << numOutputRecords
  // << ' ' << in.addr() << ' ' << out.addr();
  //if( N < 100 || depth == 4 ) {
  //cout << "\nxxx\n";
  //for(int i = 0 ; i < N ; i++) {
  //for(int j = 0 ; j < numDims ; j++) {
  //cout << ' ' << in[i][j];
  //}
  //cout << endl;
  //}
  //cout << endl;
  //}
  //if( numOutputRecords > 1000 ) exit(1);

  //if( true ) {
  //if( false ) {
  //if( N <= card[dim]/2 ) {
  if( N <= card[dim]/4 ) {
  //if( N <= 12 ) {
    //cout << " *** ";
    cmpDim = dim;
    //cout << " N = " << N << " dim = " << dim << '\n';
    quickSort(in, N, ptr_less<int>());
    //quickSort((TypeT<5>*)(*in), N);
    //InsertSort(in.addr(), N, sizeof(int**), Cmp);
    // set dataCounts
    int prevX = in[0][dim];
    int B = 0;
    int c = 1;
    for(int i = 1 ; i < N ; i++) {
      int x = in[i][dim];
      if( x != prevX ) {
        counts[B++] = c;
        c = 1;
        prevX = x;
      } else {
        c++;
      }
    }
    counts[B++] = c;
    buckets = B;
  } else {
    CountingSort(in, N, dim, buckets, counts);
  }
  //cout << ' ' << buckets;
  //for(int i = 0 ; i < buckets ; i++) if( counts[i] != 0 ) cout << ' ' << counts[i];
  //cout << endl;
}


inline
void InitAggregate(int* rec)
{
  outputCount[0] = rec[numDims];
  for(int i = 1 ; i <= numMeasures ; i++) {
    outputSum[i] = rec[numDims+i];
    // note: not right with dedup:
    //outputSum[i] = outputMin[i] = outputMax[i] = in[numDims+i];
  }
}

inline
void Aggregate(int* rec)
{
  outputCount[0] += rec[numDims];
  for(int i = 1 ; i <= numMeasures ; i++) {
    int m = rec[numDims+i];
    outputSum[i] += m;
    // note: not right with dedup:
    //if( m < outputMin[i] ) outputMin[i] = m;
    //if( m > outputMax[i] ) outputMax[i] = m;
  }
}


#if defined(RC_STATS)
static int gbId = 0;
static struct { int nonZero; int nonOne; } oc[1<<9] = {0};

#include "/u/b/e/beyer/olap/cube/IntMath.h"
#endif


inline void OutputRecord()
{
  numOutputRecords++;
#if defined(RC_STATS)
  //oc[depth][intlog2(outputCount[0])]++;
  oc[gbId].nonZero++;
  if( outputCount[0] > 1 ) oc[gbId].nonOne++;
#endif
#if 0
  if( depth <= 200 ) {
    //PrintDimHeaders();
    for(int i = 0 ; i < numOutputFields ; i++) {
      cout << ' ' << outputRec[i];
    }
    cout << endl;
  }
#endif
}

static int* ancestorRecord = NULL;

static void WriteAncestors(int dim)
{
  OutputRecord();
#if defined(RC_MAXDEPTH)
  if( depth >= maxDepth ) return; // temp
#endif
  for( ; dim < numDims ; dim++ ) {
    outputRec[dim] = ancestorRecord[dim];
#if defined(RC_STATS)
    gbId |= (1<<dim);
#endif
#if defined(RC_MAXDEPTH) || defined(RC_STATS)
    depth++;
#endif
    WriteAncestors(dim+1);
#if defined(RC_MAXDEPTH) || defined(RC_STATS)
    depth--;
#endif
#if defined(RC_STATS)
    gbId &= ~(1<<dim);
#endif
    outputRec[dim] = -1;
  }
}


static void RadixCubeNoDedup(int** in, int N, int dim)
{
#if 1
  InitAggregate(in[0]);
  for(int i = 1 ; i < N ; i++) {
    Aggregate(in[i]);
  }
#endif
  outputCount[0] = N;

#if 1
  if( N == 1 ) {
    ancestorRecord = in[0];
    WriteAncestors(dim);
    return;
  }
#endif

  OutputRecord();

#if defined(RC_MAXDEPTH)
  if( depth >= maxDepth ) return;
#endif

  for( ; dim < numDims ; dim++) {
    int B = card[dim];
    int* counts = dataCount[dim];
    Partition(in, N, dim, B, counts);
    //int B = card[dim];
    int k = 0;
    for(int b = 0 ; b < B ; b++) {
      int c = counts[b];
      //cout << depth << ' ' << dim << ' ' << b << ' ' << B << ' ' << c << '\n';
      if( c >= countThreshold ) {
        outputRec[dim] = in[k][dim];
#if defined(RC_STATS)
        gbId |= (1<<dim);
#endif
#if defined(RC_MAXDEPTH) || defined(RC_STATS)
        depth++;
#endif
        RadixCubeNoDedup(in + k, c, dim + 1);
#if defined(RC_MAXDEPTH) || defined(RC_STATS)
        depth--;
#endif
#if defined(RC_STATS)
        gbId &= ~(1<<dim);
#endif
      }
      k += c;
    }
    //assert(k == N);		// temp
    outputRec[dim] = -1;
  }
}

static void RadixCubeDedupAux(int** in, int N, int dim)
{
#if 1
  InitAggregate(in[0]);
  for(int i = 1 ; i < N ; i++) {
    Aggregate(in[i]);
  }
#endif

  if( outputCount[0] < countThreshold ) {
    return;
  }

#if 1
  if( N == 1 ) {
    ancestorRecord = in[0];
    WriteAncestors(dim);
    return;
  }
#endif

  OutputRecord();

#if defined(RC_MAXDEPTH)
  if( depth >= maxDepth ) return;
#endif

  for( ; dim < numDims ; dim++) {
    int B = card[dim];
    int* counts = dataCount[dim];
    Partition(in, N, dim, B, counts);
    //int B = card[dim];
    int k = 0;
    for(int b = 0 ; b < B ; b++) {
      int c = counts[b];
      //cout << depth << ' ' << dim << ' ' << b << ' ' << B << ' ' << c << '\n';
      outputRec[dim] = in[k][dim];
#if defined(RC_STATS)
      gbId |= (1<<dim);
#endif
#if defined(RC_MAXDEPTH) || defined(RC_STATS)
      depth++;
#endif
      RadixCubeDedupAux(in + k, c, dim + 1);
#if defined(RC_MAXDEPTH) || defined(RC_STATS)
      depth--;
#endif
#if defined(RC_STATS)
      gbId &= ~(1<<dim);
#endif
      k += c;
    }
    //assert(k == N);		// temp
    outputRec[dim] = -1;
  }
}


static void Dedup(int** in, int& N, int dim, int& buckets, int* counts)
{
  if( N <= 1 ) {
    buckets = N;
    counts[0] = N;
    return;
  }

  for(int d = numDims - 1 ; d >= dim ; d--) {
    int C = card[d];
    int* counts = dataCount[d];
    //cout << "DDCS: N:" << N << " d:" << d << " card:" << C << endl;
    CountingSort(in, N, d, C, counts);
  }

  int cur = 0;
  int* curRec = in[cur];
  for(int i = 1 ; i < N ; i++) {
    int* r = in[i];
    for(int d = dim ; d < numDims ; d++) { // check the current key
      if( curRec[d] != r[d] ) {
        // this rec is different
        in[++cur] = r;
        curRec = in[cur];
        goto nextRec;
      }
    }

    // this rec is a duplicate
    curRec[numDims] += r[numDims]; // count
    for(int i = 1 ; i <= numMeasures ; i++) {
      curRec[numDims+i] += r[numDims+i]; // sum
    }

  nextRec:
    ; // on to the next record
  }

  N = cur + 1;

#if 1
  // set counts for first dim
  int prevX = in[0][dim];
  int B = 0;
  int c = 1;
  for(int i = 1 ; i < N ; i++) {
    int x = in[i][dim];
    if( x != prevX ) {
      counts[B++] = c;
      c = 1;
      prevX = x;
    } else {
      c++;
    }
  }
  counts[B++] = c;
  buckets = B;
  //cout << "DD: B:" << B << " c0:" << counts[0] << " c1:" << counts[1] << endl;
#endif
}

static void RadixCubeDedup(int** in, int N, int dim)
{
// HEY, WAKE UP! THIS IS THE DEDUP VERSION!!
#if 1
  InitAggregate(in[0]);
  for(int i = 1 ; i < N ; i++) {
    Aggregate(in[i]);
  }
#endif

  if( outputCount[0] < countThreshold ) {
    return;
  }

#if 1
  if( N == 1 ) {
    ancestorRecord = in[0];
    WriteAncestors(dim);
    return;
  }
#endif

  OutputRecord();

  for( ; dim < numDims ; dim++) {
    int B = card[dim];
    int* counts = dataCount[dim];
    Dedup(in, N, dim, B, counts);
    int k = 0;
    for(int b = 0 ; b < B ; b++) {
      int c = counts[b];
      //cout << depth << ' ' << dim << ' ' << b << ' ' << B << ' ' << c << '\n';
      outputRec[dim] = in[k][dim];
#if defined(RC_STATS)
      gbId |= (1<<dim);
      depth++;
#endif
      RadixCubeDedupAux(in + k, c, dim + 1);
#if defined(RC_STATS)
      depth--;
      gbId &= ~(1<<dim);
#endif
      k += c;
    }
    //assert(k == N);		// temp
    outputRec[dim] = -1;
  }
}


//  #include <sys/types.h>
//  #include <unistd.h>
//  #include <sys/stat.h>
//  #include <fcntl.h>
//  #include <sys/mman.h>

#include "DTE/types/DteIntAdt.h"

static int ReadData(Iterator* iter, int** table)
{
  const Tuple* tup = iter->getFirst();
  int n = 0;
  while(tup) {
    //if( n >= countThreshold ) break; //kb: delete me!
    assert(n < MAX_RECS);
    for(int i = 0 ; i < numDims ; i++) {
      int4 x = DteIntAdt::cast(tup[i]);
      if( i < numDims && (x < 0 || x >= card[i])) {
        cerr << "bad dimension! N=" << n << " dim=" << i+1
             << " val=" << x << " card=" << card[i] << endl;
        assert(0);
      }
      table[n][i] = x;
    }
    table[n][numDims] = 1;      // count
    for(int m = 1 ; m <= numMeasures ; m++) {
      int4 x = DteIntAdt::cast(tup[numDims + m - 1]);
      table[n][numDims+m] = x;  // sum
      //table[n][i++] = x;          // min
      //table[n][i++] = x;          // max
    }
    n++;
    tup = iter->getNext();
  }
  return n;
}



#include "DTE/util/Timeval.h"



RadixCubeProc::RadixCubeProc(Iterator* iter, vector<int> dimCard,
                             int measures, int minCount, bool doDedup)
{
  this->iter = iter;
  this->doDedup = doDedup;

  numDims = dimCard.size();
  if( numDims <= 0 ) {
    cerr << "Bad numDims: " << numDims << endl;
    exit(1);
  }

  numMeasures = measures;
  if( numMeasures < 0 ) {
    cerr << "Bad numMeasures: " << numMeasures << endl;
    exit(1);
  }

#if defined(RC_MAXDEPTH)
  cerr << "warning: using maxdepth instead of countThreshold\n";
  countThreshold = 1;
  maxDepth = minCount;
#else
  countThreshold = minCount;
#endif
  if( countThreshold <= 0 ) {
    cerr << "Bad count threshold: " << countThreshold << endl;
    exit(1);
  }

  numInputFields = numDims + numMeasures;
  assert(numInputFields == iter->getNumFields());

  numOutputFields = numDims + 1 + 3*numMeasures;

  outputRec = new int[numOutputFields];
  outputCount = &outputRec[numDims];
  outputSum = &outputRec[numDims+1];
  outputMin = &outputRec[numDims+numMeasures+1];
  outputMax = &outputRec[numDims+2*numMeasures+1];

  card = new int[numDims];
  dataCount = new int*[numDims];
  int maxCard = 0;
  for(int i = 0 ; i < numDims ; i++) {
    int c = dimCard[i];
    if( c <= 0 ) {
      cerr << "Bad cardinality: " << c << endl;
      exit(1);
    }
    card[i] = c;
    if( c > maxCard ) maxCard = c;
    dataCount[i] = new int[c];
    outputRec[i] = -1;
  }
  sortCount = new int[maxCard];

  numOutputRecords = 0;
  depth = 0;
#if defined(RC_STATS)
  cerr << "warning: stats not reset\n";
#endif
}


RadixCubeProc::~RadixCubeProc()
{
  delete [] sortCount;
  delete [] dataCount;
  for(int i = 0 ; i < numDims ; i++) {
    delete [] dataCount[i];
  }

  delete [] outputRec;
  delete iter;
}

#if defined(RC_STATS)
static double gbSize = 1;
static void WriteOC(int dim)
{
  cout << depth << ' ';
  for(int i = 0 ; i < numDims ; i++) {
    if( outputRec[i] > -1 ) {
      cout << (char)outputRec[i];
    }
  }
  cout << ' ' << gbId << ' ' << gbSize << ' ' 
    << oc[gbId].nonZero << ' ' << oc[gbId].nonOne << endl;

  for( ; dim < numDims ; dim++ ) {
    outputRec[dim] = dim + 'A';
    gbSize *= card[dim];
    gbId |= (1<<dim);
    depth++;
    WriteOC(dim+1);
    depth--;
    gbId &= ~(1<<dim);
    gbSize /= card[dim];
    outputRec[dim] = -1;
  }
}
#endif

static void sortTest(int** in, int N)
{
  int** saveIn = new int*[N];
  memcpy(saveIn, in, sizeof(int*)*N);
  { // dummy run
    int i = 0;
    int B = card[i];
    CountingSort2(in, N, i, B, dataCount[i]);
    memcpy(in, saveIn, sizeof(int*)*N);
  }
  for(int i = 0 ; i < numDims ; i++) {
    Timeval timer;
    int B = card[i];
    CountingSort2(in, N, i, B, dataCount[i]);
    double secs = timer.ElapsedTime().AsSecs();
    cout << "cs- " << i
         << " time: " << secs << endl;
    memcpy(in, saveIn, sizeof(int*)*N);
  }
  for(int i = 0 ; i < numDims ; i++) {
    Timeval timer;
    cmpDim = i;
    quickSort(in, N, ptr_less<int>());
    double secs = timer.ElapsedTime().AsSecs();
    cout << "qs- " << i
         << " time: " << secs << endl;
    memcpy(in, saveIn, sizeof(int*)*N);
  }
  for(int i = 0 ; i < numDims ; i++) {
    Timeval timer;
    int B = card[i];
    int B2 = B;
    CountingSort(in, N, i, B, dataCount[i]);
    double secs = timer.ElapsedTime().AsSecs();
    cout << "cs+ " << i << " time: " << secs 
         << "  B2:" << B2 << " B:" << B << " N:" << N << endl;
    memcpy(in, saveIn, sizeof(int*)*N);
  }
  delete [] saveIn;
}

void RadixCubeProc::run()
{
  int** ptrs = new int*[MAX_RECS];
  sortTable = new int*[MAX_RECS];
  //int* table = new int[MAX_RECS * numInputFields];
  int* table = new int[MAX_RECS * numOutputFields];
  for(int i = 0 ; i < MAX_RECS ; i++) {
    //ptrs[i] = &table[i*numInputFields];
    ptrs[i] = &table[i*numOutputFields];
  }
  
  int N = ReadData(iter, ptrs);
  cerr << "read " << N << " recs\n";
  //memcpy(t2,t1,sizeof(int*)*N);

  //sortTest(ptrs, N);
  //return;

  Timeval timer;
  //  Dedup(table1, N, 0, 0, NULL); // temp
  if( doDedup ) {
    RadixCubeDedup(ptrs, N, 0);
  } else {
    RadixCubeNoDedup(ptrs, N, 0);
  }
  double secs = timer.ElapsedTime().AsSecs();
  cerr << "time: " << secs << endl;;
  cerr << "output records: " << numOutputRecords << endl;
#if defined(RC_SORTS)
  cerr << "sorts         : " << sorts << endl;
  cerr << "records sorted: " << recsSorted << endl;
#endif
#if defined(RC_STATS)
  cerr << "# depth, groupby, numCells, nonZero, nonOne\n";
  WriteOC(0);
#endif
#if 0
  //cerr << "dims sum(log2(count))\n";
  for(int d = 0 ; d < numDims ; d++) {
    for(int i = 0 ; i < 32 ; i++) {
      if( oc[d][i] != 0 ) {
        cerr << d << ' ' << (1<<i) << ' ' << oc[d][i] << endl;
      }
    }
  }
#endif
  delete [] table;
  delete [] ptrs;
  delete [] sortTable;
}
