#ifndef TYPE_COMPAT_H
#define TYPE_COMPAT_H

// Must define int2 to be a 2 byte int in the DB
#define int2 short

// Must define int4 to be a 4 byte int in the DB
#define int4 int

// Must define the return code type for error checking. An assignment method and
// copy constructor must be defined.
#define rc_t int
// Must define the return code that signifies all is ok
#define RCOK 0
// Must define the return code that indicates things are amiss
#define RCERR 1

// Must define type of the lock constant used. A copy constructor and assignment must
// be defined for this type.
#define lock_t int

// Must define lock constants for read and write locks
#define WRITE_LOCK 1
#define READ_LOCK 1
#define MEM_AVAIL (5*1024*1024)

// Must define page_id_t as the appropriate type in the DB. Note that this type must
// have an assignment method and a copy constructor.
class page_id_t
{
  public:
    // Default constructor
    page_id_t();


    // Copy constructor
    page_id_t(const page_id_t& pid_to_copy_from);


    // Default destructor
    ~page_id_t();


    // Returns the bit representation of the page_id. Returns pointer to memory
    // allocated in this structure. Will be freed in this structure.
    void *page_repr() const;


    // Returns the size of the bit representation stored on the page.
    static const page_repr_size = 4;


    // Interprets a bit pattern produced from page_repr() and assigns this page_id_t
    // to the appropriate value (ie. the inverse of page_repr())
    void interp_pid_str (void *inp_data);


    // Assignment definition
    page_id_t& operator =(const page_id_t& pid_to_assign_from);


    // Equality comparison definition
    bool operator ==(const page_id_t& pid_to_assign_from);

    
//  ****************** Unix file system implementation specific stuff *************
    int pid;
};


// This function must be written and should reside in the same file as the
// derived rtree manager.
extern rc_t allocate_and_clear_new_page (page_id_t& new_page_id);

#endif

