#ifndef _DTEALGO_H_
#define _DTEALGO_H_

// (C) 1998- by Kevin S. Beyer

#include <iostream.h>
#include <iterator>

template <class InputIterator>
void delete_all(InputIterator first, InputIterator last)
{
  for ( ; first != last; ++first) {
    delete *first;
  }
}


template <class Container>
void delete_all(Container& container)
{
  delete_all(container.begin(), container.end());
}


template <class InputIterator>
void print_list(ostream& out, InputIterator first, InputIterator last,
                const char* sep = ", ")
{
  if( first != last ) {
    out << *first;
    for (++first ; first != last; ++first) {
      out << sep << *first;
    }
  }
}

template <class InputIterator>
void print_ptr_list(ostream& out, InputIterator first, InputIterator last,
                    const char* sep = ", ")
{
  if( first != last ) {
    out << **first;
    for (++first ; first != last; ++first) {
      out << sep << **first;
    }
  }
}

template <class Container>
void print_list(ostream& out, const Container& c, const char* sep = ", ")
{
  print_list(out, c.begin(), c.end(), sep);
}

template <class Container>
void print_ptr_list(ostream& out, const Container& c, const char* sep = ", ")
{
  print_ptr_list(out, c.begin(), c.end(), sep);
}


template <class Container>
void append(Container& c1, const Container& c2)
{
  c1.insert(c1.end(), c2.begin(), c2.end());
}


#endif // _DTEALGO_H_
