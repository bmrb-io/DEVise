// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
//
//   Attribute and Type Definitions
//   $Id$
//
//   Copyright 1997, Shaun Flisakowski
//
//   You may distribute under the terms of either the GNU General Public
//   License or the Artistic License, as specified in the README file.
//
// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o

#include  "Attr.h"
#include  "udParam.h"
#include  "UniData.h"    // For definitions of copying functions.
#include  "getftime.h"

#undef    assert        // defined by perl includes
#include  <assert.h>

// perl will not take const char*
char* DATE_PART_NAMES[NUM_DATE_PARTS] =        
     {"year", "month", "day", "hour", "min", "sec", "nanosec"};


// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
// Make a duplicate (deep-copy)
FuncOf *FuncOf::dup()
{
    if (!this)
        return NULL;

    char *my_attr = new char [strlen(attr)+1];
    strcpy(my_attr,attr);

    FuncOf *that = new FuncOf(my_attr,next->dup());
    that->indx = indx;

    return that;
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
// This class is for both attributes and typedef's
Attr::Attr(char *name)
{
    _type = Invalid_Attr;
    _name = name;
    _flat_name = name;
    _offset = 0;

    _size = 0;
    _nattrs = 0;

    _subattr = NULL;

    _min_type = Invalid_Attr;
    _max_type = Invalid_Attr;
    _min = _max = NULL;

    _maxlen = 0;
    _qte_chr[0] = '"';    // Default quote char
    _qte_chr[1] = '\0';
    _date_frmt = NULL;

    _own_df = 0;
    _own_ws = 0;

    _have_lpos = _have_rpos = 0;
    _lpos = _rpos = 0;

    _delimit = NULL;
    _seper   = NULL;
    _white   = NULL;

    _traits = new Traits();

    _format = NULL;
    _value  = NULL;
    _filter = NULL;
    _reader = NULL;

    _func_of = NULL;

    _perl_var = (SV*) NULL;
    _perl_date = (HV*) NULL;

    for(int i = 0; i < NUM_DATE_PARTS; i++){
        _perl_date_parts[i] = (SV*) NULL;
    }    

    _sublen  = NULL;
    _sublpos = NULL;
    _subrpos = NULL;
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
Attr::~Attr()
{
    delete _subattr;
    delete _traits;

    if (_flat_name != _name)
        delete [] _flat_name;

    delete [] _name;

    delete [] _delimit;
    delete [] _seper;

    if (_own_ws)
        delete [] _white;

    if (_own_df)
        delete [] _date_frmt;

    FuncOf *f, *prev = NULL;
    for (f=_func_of; f; f=f->next) {
        delete prev;
        prev = f;
    }
    delete prev;

    delete _format;
    delete _value;
    delete _filter;
    delete _reader;

    delete _sublen;
    delete _sublpos;
    delete _subrpos;

    if (_perl_var)
        SvREFCNT_dec(_perl_var);

    delete [] _min;
    delete [] _max;

    // _perl_date and _perl_date_parts should be deleted too,
    // but I am not sure how to do this.   DD	
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
size_t  Attr::determ_size()
{
    switch(_type) {

      case Invalid_Attr:
        _size = (size_t) 0;
        break;
     
      case Int_Attr:
        _size = sizeof(int);
        break;

      case Float_Attr:
        _size = sizeof(float);
        break;

      case Double_Attr:
        _size = sizeof(double);
        break;

      case String_Attr:
        _size = (size_t) _maxlen;
        break;

      case UnixTime_Attr:
        _size = sizeof(time_t);
        break;

      case DateTime_Attr:
        _size = sizeof(TimeT);
        break;

      case UserDefined_Attr:
        _size = (size_t) 0;
        if (_subattr)
            for (int j=0; j < _subattr->nAttrs(); j++)
                _size += _subattr->ith(j)->size_of();

      default:
        // An Enum
        _size = sizeof(ushort);
    }

    return _size;
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
// Make a duplicate (deep-copy)
Attr *Attr::dup()
{
    if (!this)
        return NULL;

    cerr << "Warning: " __FUNCTION__ " - Not yet implemented.\n";
    return NULL;
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
// Does this attr or it's children use perl at all?
int Attr::uses_perl()
{
    int ret = 0;

      // Do I use perl?
    if (_format || _value || _filter || _reader)
        return 1;

      // What about my children?
    if (_subattr)
        for (int j=0; j < _subattr->nAttrs(); j++) {
            ret |= _subattr->ith(j)->uses_perl();

            if (ret)
                break;
        }

    return ret;
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
// Provide a Perl Interpreter to use.
void Attr::set_interp(PerlInterpreter *perl)
{
    _format->set_interp(perl);
    _value->set_interp(perl);
    _filter->set_interp(perl);
    _reader->set_interp(perl);

    if (_subattr)
        for (int j=0; j < _subattr->nAttrs(); j++)
            _subattr->ith(j)->set_interp(perl);
    else {
        // leaf values get a corresponding perl var
        _perl_var = perl_get_sv(_flat_name, TRUE);
        assert(_perl_var);

        // create and init vars that contain date parts

        for(int i = 0; i < NUM_DATE_PARTS; i++){
            _perl_date_parts[i] = newSViv(DATE_PART_DEF[i]);
            assert(_perl_date_parts[i]);
        }

        // creating _perl_date

        _perl_date = perl_get_hv(_flat_name, TRUE);
        assert(_perl_date);
        for(int i = 0; i < NUM_DATE_PARTS; i++){
            char* name = DATE_PART_NAMES[i];		// should be const char*
            SV** ret =  hv_store(
                _perl_date, name, strlen(name), _perl_date_parts[i], 0);
            assert(ret);
        }
    }
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
// Set the perl variable for this attribute.
void Attr::set_var(char *field)
{
    switch (_type) {

      default:
      case Invalid_Attr:
        break;

      case Int_Attr: {
            int *i = (int*) field;
            sv_setiv(_perl_var, *i);
        }
        break;

      case Float_Attr: {
            float *f = (float*) field;
            double d = *f;
            sv_setnv(_perl_var, d);
        }
        break;

      case Double_Attr: {
            double *d = (double*) field;
            sv_setnv(_perl_var, *d);
        }
        break;

      case String_Attr:
        sv_setpv(_perl_var, field);
        break;

      case UnixTime_Attr: {
            time_t *t = (time_t*) field;
            sv_setiv(_perl_var, *t);
        }
        break;

      case DateTime_Attr: {

            // This code is called even when
            // value option is defined, and then some of 
            // the _perl_date_parts may be dangling pointers
 
            if(_value){

                // this is computed atribute, nothing to read
                // has to break or core dump

                break;   
            }

            TimeT *dte = (TimeT*) field;

            sv_setiv(_perl_date_parts[0], dte->getYear());
            sv_setiv(_perl_date_parts[1], dte->getMonth());
            sv_setiv(_perl_date_parts[2], dte->getDay());
            sv_setiv(_perl_date_parts[3], dte->getHour());
            sv_setiv(_perl_date_parts[4], dte->getMin());
            sv_setiv(_perl_date_parts[5], dte->getSec());
            sv_setiv(_perl_date_parts[6], dte->getNanoSec());
        }
        break;
    }
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
// Get the perl variable for this attribute.
void Attr::grab_var(char *field)
{
    switch (_type) {

      default:
      case Invalid_Attr:
        break;

      case Int_Attr: {
            int *i = (int*) field;
            *i = SvIV(_perl_var);
        }
        break;

      case Float_Attr: {
            float *f = (float*) field;
            *f = SvNV(_perl_var);
        }
        break;

      case Double_Attr: {
            double *d = (double*) field;
            *d = SvNV(_perl_var);
        }
        break;

      case String_Attr:
        strncpy(field, SvPV(_perl_var, na), _maxlen);
        field[_maxlen-1] = '\0';
        break;

      case UnixTime_Attr: {
            time_t *t = (time_t*) field;
            *t = SvIV(_perl_var);
        }
        break;

      case DateTime_Attr: {
            TimeT *dte = (TimeT*) field;
            int values[NUM_DATE_PARTS];
            for(int i = 0; i < NUM_DATE_PARTS; i++){
			 // perl will not take const char*
                char* name = DATE_PART_NAMES[i];
                SV** ret = hv_fetch(_perl_date, name, strlen(name), FALSE);
                if(ret){
                    values[i] = SvIV(*ret);
                }
                else{
                    values[i] = DATE_PART_DEF[i];
                }
            }
		  dte->setDate(values[0], values[1], values[2]);
		  dte->setTime(values[3], values[4], values[5]);
		  dte->setNanoSec(values[6]);
        }
        break;
    }
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
// compile all perl subroutines for later use.
void Attr::compile(unsigned int& subrcnt)
{
    if(_flat_name){
        int length = strlen(_flat_name) + 2;
	   char name[length];  
        name[0] = (_type == DateTime_Attr ? '%' : '$');
        strcpy(&name[1], _flat_name);
        _format->compile(subrcnt, name);
        _value->compile(subrcnt, name);
        _filter->compile(subrcnt, name);
        _reader->compile(subrcnt, name);
    }
    else{
        char* name = NULL;
        _format->compile(subrcnt, name);
        _value->compile(subrcnt, name);
        _filter->compile(subrcnt, name);
        _reader->compile(subrcnt, name);
    }

    if (_subattr)
        for (int j=0; j < _subattr->nAttrs(); j++)
            _subattr->ith(j)->compile(subrcnt);
}
    
// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
// Flatten the attribute into the AttrStk provided.
int  Attr::flatten(AttrStk *stk, char *prename)
{
    char *my_name;

    if (prename) {
        my_name = new char [strlen(prename)+strlen(_name)+2];

        strcpy(my_name,prename);
        strcat(my_name,"_");
        strcat(my_name,_name);
    } else 
        my_name = _name;

    if (!_subattr || _subattr->is_empty()) {

        this->_flat_name = my_name;
        stk->push(this);

    } else {

        for (int j=0; j < _subattr->nAttrs(); j++)
            _subattr->ith(j)->flatten(stk,my_name);

        if (prename)
            delete [] my_name; 
    }

    return 0;
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
// Propagate info between _sublpos,_subrpos,_sublen
// returns true if we have completely determined _sublpos at end.
int  Attr::propagate_pos()
{
    int  j;
    AttrStk *stk = subattr();

    // Try to propagate positioning info both forwards and
    // backwards to other fields, butting them up against each other.

    if (stk && _sublen) {

        if (_sublen) {
            Attr *prev_sa = NULL;

            for (j=0; j < stk->nAttrs(); j++) {
                int len;
                Attr *sa = stk->ith(j);

                if (j < _sublen->count())
                    len = _sublen->get_ith(j);
                else
                    len = -1;

                if (sa->have_lpos() && !sa->have_rpos()
                            && (len != -1)) {
                    sa->set_rpos(sa->lpos()+len-1);

                } else if (!sa->have_lpos()) {
                    if (sa->have_rpos() && (len != -1))
                        sa->set_lpos(sa->rpos()-len+1);
                    else if (prev_sa) {
                        // try to butt up to the last one.
                        if (prev_sa->have_rpos())
                            sa->set_lpos(prev_sa->rpos()+1);
                    }

                    // try to set rpos again.
                    if (sa->have_lpos() && !sa->have_rpos()
                                && (len != -1))
                        sa->set_rpos(sa->lpos()+len-1);
                }

                prev_sa = sa;
            }

              // Now try it backwards
            Attr *next_sa = NULL;

            for (j= stk->nAttrs()-1; j >= 0; j--) {
                int len;
                Attr *sa = stk->ith(j);

                if (j < _sublen->count())
                    len = _sublen->get_ith(j);
                else
                    len = -1;

                // try to butt up to the next one.
                if (!sa->have_rpos() && next_sa
                        && next_sa->have_lpos())
                    sa->set_rpos(next_sa->lpos()-1);

                // try to set lpos again.
                if (!sa->have_lpos()
                        && sa->have_rpos() && (len != -1))
                    sa->set_lpos(sa->rpos()-len+1);

                next_sa = sa;
            }
        }
    }

    return 0;
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
// Copy the top level of src attribute (a typedef?) to this.
void Attr::assign(Attr *src, AttrStk *owner, int usename)
{
    // Don't want flat name, but take everything else.
    if (usename) {
      delete [] _name;
      _name = new char [strlen(src->_name)+1];
      strcpy(_name,src->_name);
    }

    _type   = src->_type;
    _size   = src->_size;
    _nattrs = src->_nattrs;

    delete _subattr;
    _subattr = src->_subattr->dup();
    
    // Need to duplicate all subattributes as well, to avoid ptr-sharing.
    if (_subattr)
      for (int j=0; j < _subattr->nAttrs(); j++) {
        Attr *tmp = new Attr();
        owner->push(tmp);
        tmp->assign(_subattr->ith(j),owner,1);
        _subattr->set_ith(j,tmp);
      }
    
    _min_type = src->_min_type;
    _max_type = src->_max_type;

    delete [] _min;
    delete [] _max;

    // Duplicate _min and _max
    switch (_min_type) {

        default:
        case Invalid_Attr:
            _min = NULL;
            break;

        case Int_Attr:
            _min = new char [sizeof(int)];
            memcpy(_min,src->_min,sizeof(int));
            break;

        case Double_Attr:
            _min = new char [sizeof(double)];
            memcpy(_min,src->_min,sizeof(double));
            break;

        case String_Attr:
            _min = new char [strlen(src->_min)+1];
            strcpy(_min,src->_min);
            break;
    }

    switch (_max_type) {

        default:
        case Invalid_Attr:
            _max = NULL;
            break;

        case Int_Attr:
            _max = new char [sizeof(int)];
            memcpy(_max,src->_max,sizeof(int));
            break;

        case Double_Attr:
            _max = new char [sizeof(double)];
            memcpy(_max,src->_max,sizeof(double));
            break;

        case String_Attr:
            _max = new char [strlen(src->_max)+1];
            strcpy(_max,src->_max);
            break;
    }

    _maxlen = src->_maxlen;

    _have_lpos = src->_have_lpos;
    _have_rpos = src->_have_rpos;
    _lpos = src->_lpos;
    _rpos = src->_rpos;

    delete _traits;
    _traits = src->_traits->dup();

    delete _func_of;
    _func_of = src->_func_of->dup();


    delete _format;
    if (src->_format)
        _format = src->_format->dup();
    else
        _format = NULL; 

    delete _value;
    if (src->_value)
        _value = src->_value->dup();
    else
        _value = NULL; 

    delete _filter;
    if (src->_filter)
        _filter = src->_filter->dup();
    else
        _filter = NULL; 

    delete _reader;
    if (src->_reader)
        _reader = src->_reader->dup();
    else
        _reader = NULL; 
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
// Stream printing (for debugging)
ostream& operator<< (ostream& out, const Attr& attr)
{
    out << "Name: " << attr._name << endl;
    out << "Flat Name: " << attr._flat_name << endl;
    out << "Type: ";

    switch(attr._type) {
        default:
            if (attr._type == UserDefined_Attr)
                out << "User-Defined\n";
            else if (attr._type > UserDefined_Attr)
                out << "Enum: " << (int) attr._type << endl;
            else
                out << "<Unknown>\n";
            break;

        case Invalid_Attr:
            out << "<Invalid>\n";
            break;

        case Int_Attr:
            out << "Int\n";
            break;

        case Float_Attr:
            out << "Float\n";
            break;

        case Double_Attr:
            out << "Double\n";
            break;

        case String_Attr:
            out << "String\n";
            out << "MaxLen: " << attr._maxlen << endl;
            break;


        case UnixTime_Attr:
            out << "UnixTime\n";
            break;

        case DateTime_Attr:
            out << "DateTime\n";
            break;
    }

    out << "Size: " << attr._size << endl;

    out << "Total attributes: " << attr._nattrs << endl;

    if (attr._have_lpos || attr._have_rpos) {
        out << "Position: [";
        if (attr._have_lpos)
            out << attr._lpos;
        out << "..";
        if (attr._have_rpos)
            out << attr._rpos;
        out << "]\n";
    }


    if (attr._delimit)
        out << "Delimiter: '" << attr._delimit << "'" << endl;

    if (attr._seper)
        out << "Seperator: '" << attr._seper << "'" << endl;

    if (attr._white)
        out << "WhiteSpace: '" << attr._white << "'" << endl;


    out << "Traits: " << *(attr._traits) << endl;

    if (attr._func_of) {
        out << "Function Of: ";

        for (FuncOf *f=attr._func_of; f; f=f->next)
            out << f->attr << " ";

        out << endl;
    }

    if (attr._format)
        out << "Format: " << *(attr._format) << endl;

    if (attr._value)
        out << "Value: " << *(attr._value) << endl;

    if (attr._filter)
        out << "Filter: " << *(attr._filter) << endl;

    if (attr._reader)
        out << "Reader: " << *(attr._reader) << endl;

    if (attr._sublen)
        out << "Sub-Lengths: " << attr._sublen << endl;

    if (attr._sublpos)
        out << "Sub-Left Positions: " << attr._sublpos << endl;

    if (attr._subrpos)
        out << "Sub-Right Positions: " << attr._subrpos << endl;

    if (attr._subattr && attr._subattr->nAttrs()) {
        out << "\nSub Attributes:\n";
        out << "-----------------\n";
        for (int j=0; j < attr._subattr->nAttrs(); j++) {
            Attr *sa = attr._subattr->ith(j);
            out << *sa << endl;
        }
        out << "-----------------\n";
    }

    return out;
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
AttrStk::AttrStk(int hint)
{
    _num = 0;
    _sze = hint;

    if (_sze <= 0)
        _sze = 1;

    _stk = new Attr* [_sze]; 
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
AttrStk::~AttrStk()
{
    delete [] _stk;
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
// Empty out the stack, if del then delete the Attr* being stored.
void AttrStk::empty(int del)
{
    if (del)
      for (int j=0; j < _num; j++)
        delete _stk[j];

    _num = 0;
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
int  AttrStk::push(Attr *attr)
{
    if (_num >= _sze) {

        int len = _sze;
        _sze += _sze;
        Attr **tmp_stk = _stk;
        _stk = new Attr* [_sze];
        memcpy(_stk, tmp_stk, len * sizeof(Attr*));
        delete [] tmp_stk;
    }

    _stk[_num++] = attr;

    return(_num-1);
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
Attr *AttrStk::pop()
{
    if (_num > 0) {
        return _stk[--_num];
    } else {
        return NULL;
    }
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
// Make a duplicate (shallow-copy)
AttrStk *AttrStk::dup()
{
    if (!this)
        return NULL;

    AttrStk *that = new AttrStk(_sze);

    that->_num = _num;

    for (int j=0; j < _num; j++)
        that->_stk[j] = _stk[j];

    return that;
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
// Find an attribute with this name;
// return it's position, or -1 if not found.
int AttrStk::find(char *name)
{
    for (int j=0; j < _num; j++) {
        if (_stk[j] && _stk[j]->name() 
                    && (strcmp(name,_stk[j]->name()) == 0))
            return j;
    }

    return -1;
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
