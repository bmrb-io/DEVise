#ifndef _INT_KEYED_P_H
#define _INT_KEYED_P_H

#include"fixed_entrysz_p.h"
#include"int_key.h"

class ikp_hdr_t : public fep_hdr_t
{
  public:
    int2 dim;
    bool point_keys;

    ikp_hdr_t():dim(0),point_keys(false) {};
    virtual ~ikp_hdr_t() {};


    // Returns a byte array of the representation of this header stored on a page.
    // Note that memory is allocated in this function and must be freed outside.
    virtual void *page_repr();


    // Returns the size of the data contained in the array returned by page_repr()
    virtual int unaligned_page_repr_size();


//  inherited:
//  // Returns the size of the byte array returned by page_repr()
//  virtual int page_repr_size();


    // Interprets a page representation of a header passed through hdr_str and stores
    // the information in this object.
    virtual void interp_hdr_str(const void *hdr_str);
};


class int_keyed_p_t : public fixed_entrysz_p_t
{
  public:
    int_keyed_p_t(db_page_t &in_page);

    virtual ~int_keyed_p_t();

    virtual void init_p(int  num_dim,
			int  data_size,
			bool pt_keys);

    // modification routines
    virtual int insert_entry(const int_key_t &key,
			     const void      *data);


//inherited:
//  virtual void remove_last_entry();
//
//  virtual void remove_entry(int index_num);

    virtual void modify_entry(      int       index_num,
			      const int_key_t &new_key,
			      const void      *new_data);

    virtual void modify_key(      int       index_num,
			    const int_key_t &new_key);

    virtual void modify_data(      int  index_num,
			     const void *new_data);

    virtual bool fix_key(const int_key_t &new_key,
			 void            *data);
    
    // access routines
//inherited:
//  page_id_t page_id() const;
//
//  virtual int num_entries()
//
//  virtual int max_num_entries();
//
//  db_page_t *db_page();

    virtual int dim();

    virtual bool point_keys();

    virtual int data_sz();

    virtual int_key_t bbox();

    virtual fep_hdr_t *get_fep_hdr();

    virtual ikp_hdr_t *get_ikp_hdr();

    virtual void get_entry(int       index_num,
			   int_key_t &key,
			   void      *data);

    virtual void get_key(int       index_num,
			 int_key_t &key);

    virtual void get_data(int  index_num,
			  void *data);

    // stuff likely to be overriden by children
    virtual fep_hdr_t *alloc_fep_hdr();

    virtual ikp_hdr_t *alloc_ikp_hdr();

//  inherited:
//protected:
//  virtual void set_hdr(fep_hdr_t *new_hdr);
};


class ikp_cursor_t : fep_cursor_t
{
  public:
    ikp_cursor_t();


    virtual ~ikp_cursor_t();


    virtual void connect_to_ikp(int_keyed_p_t &page);
    
    
    int_key_t *next(); // Inlined later


    void next_noret(); // Inlined later


    void *data(); // Inlined later


    int_key_t *key(); // Inlined later
    

    bool more_entries(); // Inlined later


  protected:
    int_key_t *cur_key;
    int       dim;
    bool      point_keys;
};


inline int_key_t *ikp_cursor_t::next()
{
  cur_key->interpret(first_vec, point_keys);
  sec_advance();
  return cur_key;
}


inline void ikp_cursor_t::next_noret()
{
  cur_key->interpret(first_vec, point_keys);
  sec_advance();
}


inline void *ikp_cursor_t::data()
{
  return place_in_sec;
}


inline int_key_t *ikp_cursor_t::key()
{
  return cur_key;
}


inline bool ikp_cursor_t::more_entries()
{
  return more_sec_entries();
}
#endif
