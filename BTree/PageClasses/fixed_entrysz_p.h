#ifndef _FIXED_ENJTRYSZ_P_H
#define _FIXED_ENJTRYSZ_P_H

#include"stackpage.h"
#include"setup.h"
#include"bit_vec.h"

class fep_hdr_t
{
  public:
    int2 first_sz; // Size of first value in pair in BITS
    int2 sec_sz;   // Size of second value in pair in BYTES
    int4 num_entries;

    fep_hdr_t():first_sz(0),sec_sz(0),num_entries(0) {};
    virtual ~fep_hdr_t() {};


    // Returns a byte array of the representation of this header stored on a page.
    // Note that memory is allocated in this function and must be freed outside.
    virtual void *page_repr();


    // Returns the size of the data contained in the array returned by page_repr()
    virtual int unaligned_page_repr_size();


    // Returns the size of the byte array returned by page_repr()
    virtual int page_repr_size();


    // Interprets a page representation of a header passed through hdr_str and stores
    // the information in this object.
    virtual void interp_hdr_str(const void *hdr_str);
};


class fep_cursor_t; // forward declaration of cursor class


class fixed_entrysz_p_t : public stack_page_t
{
  friend fep_cursor_t;
  public:
    fixed_entrysz_p_t(db_page_t &in_page);

    virtual ~fixed_entrysz_p_t();


    virtual void init_p(int first_size,
			int sec_size);



    // modification routines
    virtual int insert_entry(bit_vec_t &first_str,
			     void      *sec_str);


    virtual void remove_last_entry();


    virtual void remove_entry(int index_num);


    virtual void modify_entry(      int       index_num,
			            bit_vec_t &new_first,
			      const void      *new_sec);


    virtual void first_modify_entry(int       index_num,
				    bit_vec_t &new_first);


    virtual void sec_modify_entry(      int  index_num,
				  const void *new_sec);



    // access routines
    virtual int num_entries();


    virtual int max_num_entries();


    virtual int first_sz();


    virtual int sec_sz();


    virtual fep_hdr_t *get_fep_hdr();


    virtual void get_entry(int       index_num,
			   bit_vec_t &first,
			   void      *sec);


    virtual void first_get_entry(int       index_num,
				 bit_vec_t &first);


    virtual void sec_get_entry(int  index_num,
			       void *sec);


//inherited:
//  page_id_t page_id() const;


    // stuff likely to be overriden by children
    virtual fep_hdr_t *alloc_fep_hdr();
    

  protected:
    virtual void set_hdr(fep_hdr_t *new_hdr);
};


class fep_cursor_t
{
  public:
    fep_cursor_t(int first_offset_amount=0,             // in bits
		 int first_outside_absorption_amount=0, // in bits
		 int sec_offset_amount=0,               // in bytes
		 int sec_outside_absorption_amount=0);  // in bytes
   

    virtual ~fep_cursor_t();


    void advance(); // This routine is inlined later in this file


    void first_advance();  // This routine is inlined later in this file


    void sec_advance();    // This routine is inlined later in this file


    bool more_first_entries(); // This routine is inlined later in this file


    bool more_sec_entries(); // This routine is inlined later in this file


    virtual void connect_to_page(fixed_entrysz_p_t &page);

  protected:
    int first_off, first_abs, 
        sec_off, sec_abs;    // Place to store info from constructor;

    int first_bytes_adv_amt; // # of bytes to advance first
    int first_bits_adv_amt;  // # of bits beyond first_bytes_adv_amt to advance
    int sec_adv_amt;         // advancement amount in bytes
    bit_vec_t first_vec;     // bit vector used to iterate over the first
    char *place_in_sec;      // character array used to iterate over the second
    int first_entries_left;  // number of first entries that haven't been processed
    int sec_entries_left;    // number of second entries that haven't been processed
};


inline bool fep_cursor_t::more_first_entries()
{
  return (first_entries_left > 0);
}


inline bool fep_cursor_t::more_sec_entries()
{
  return (sec_entries_left > 0);
}


inline void fep_cursor_t::first_advance()
{
  first_vec.advance(first_bytes_adv_amt, first_bits_adv_amt);
  first_entries_left--;
}


#include<stdio.h>

inline void fep_cursor_t::sec_advance()
{
  place_in_sec += sec_adv_amt;
  sec_entries_left--;
}


inline void fep_cursor_t::advance()
{
  first_advance();
  sec_advance();
}

#endif
