// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
//   Perl Code Interface
//   $Id$
//
//   Copyright 1997, Shaun Flisakowski
//
//   You may distribute under the terms of either the GNU General Public
//   License or the Artistic License, as specified in the README file.
//
// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o

#include "PerlFrag.h"
#include "Attr.h"

#include <iostream.h>

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
PerlFrag::PerlFrag(Attr *owner, FragType type)
{
    _perl  = NULL;

    _attr  = owner;
    _type  = type;

    _src   = NULL;
    _flags = G_DISCARD | G_NOARGS | G_EVAL;
    _nrets = 0;

    _code  = (SV*) NULL;

    _args  = (AV*) NULL;
    _rets  = (AV*) NULL;

    next   = NULL;
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
PerlFrag::~PerlFrag()
{
    delete next;

    if (_code)
        SvREFCNT_dec(_code);

    if (_args)
        av_undef(_args);

    if (_rets)
        av_undef(_rets);

    delete [] _src;
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
// Add this to the end of the list.
void PerlFrag::enlist(PerlFrag *pf)
{
    if (next)
        next->enlist(pf);
    else
        next = pf;
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
// Make a routine for formats.
// Have to do the match twice, as we need $` and $& to be set.
void PerlFrag::set_fmt(char *fmt)
{
  delete [] _src;
  _src = new char[2*strlen(fmt) + XTRA_SPCE + 150];

  memset(_src, ' ', XTRA_SPCE);
  sprintf( &(_src[XTRA_SPCE]),
  "{my($in)=@_;my($len,@flds);\nif($in=~%s){$len=length($`)+length($&);\n@flds=$in=~%s;\nreturn($len,@flds);}\nreturn();}",
    fmt,fmt);

  _nrets = 2;
  delete [] fmt;
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
static void perl_eval(char *code)
{
    char *argv[2];

    argv[0] = code;
    argv[1] = NULL;

    perl_call_argv("_eval_", 0, argv);
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
// Compile the code stored.
void PerlFrag::compile(unsigned int& subrcnt, char *flat_name)
{
    if (!this)
        return;

    switch (_type) {

        case FT_GLOB:
            // Put the code into an SV
            _code = newSVpv(_src,0);
            break;

        case FT_EXPR:
            if (flat_name) {
                int len = strlen(flat_name) + 3;
                char *cmmd = new char[len+strlen(_src)+1];

                sprintf(cmmd, "$%s =%s", flat_name, _src);
                _code = newSVpv(cmmd,0);
                delete [] cmmd;
            } else
                // Trust the user, the code might have side-effects.
                _code = newSVpv(_src,0);
            break;

        case FT_SUBR: {
                // If everything is working properly, there is extra space
                // (XTRA_SPCE) at the front of this string for us.
            
                char nme[10];
                char head[20];
             
                sprintf(nme, "UNI%06u", subrcnt++);
                sprintf(head, "sub %s ", nme);
            
                memcpy(_src, head, strlen(head));
            
                // Compile the subroutine.    
#if 1
                // Work-around for an apparent bug in the perl library
                perl_eval(_src);
#else
                _code = newSVpv(_src,0);
                perl_eval_sv(_code, G_DISCARD|G_NOARGS|G_EVAL);
                SvREFCNT_dec(_code);
#endif
            
                  // Save the subroutine's name, for later use.
                _code = newSVpv(nme,0);

                switch (_nrets) {
                  case 0:
                    _flags = G_DISCARD | G_NOARGS | G_EVAL | G_SCALAR;
                    break;

                  case 1:
                    _flags = G_NOARGS | G_EVAL | G_SCALAR;
                    break;

                  default:
                    _flags = G_EVAL | G_ARRAY;
                    break;
                }

                if (_nrets > 0) {
                    if (_args)
                      av_clear(_args);
                    else
                      _args = newAV();
                    
                    if (_rets)
                      av_clear(_rets);
                    else
                      _rets = newAV();
                }
            }
            break;

        default:
        case FT_NONE:
            break;
    }

    if (next)
        next->compile(subrcnt,flat_name);
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
// Set a argument to pass in as the @_ parameter.
void PerlFrag::set_arg(char *line)
{
    av_clear(_args);
    av_push(_args, newSVpv(line,0));
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
I32  PerlFrag::Eval()
{
    if (!this)
        return 0;

    int j;
    I32 n;

    switch (_type) {

        case FT_GLOB:
        case FT_EXPR:
            perl_eval_sv(_code, G_DISCARD|G_NOARGS|G_EVAL);
            break;

        case FT_SUBR: {
                // NYI, need to have args and return values.
                switch (_nrets) {
                  case 0:
                    perl_eval_sv(_code, _flags);
                    break;

                  case 1: {
                        dSP;
                        ENTER;
                        SAVETMPS;
                        PUSHMARK(sp);

                        n = av_len(_args) + 1;
                        for (j=0; j < n; j++) {
                            XPUSHs(sv_2mortal( av_shift(_args) ));
                        }

                        PUTBACK;
                        n = perl_call_sv(_code, _flags);
                        SPAGAIN;
                        if ((n > 0) && _attr->perl_var())
                            sv_setsv( _attr->perl_var(), POPs );
                        PUTBACK;
                        FREETMPS;
                        LEAVE;
                    }
                    break;

                  default: {
                    // need to pass in info, and accept a list back.
                    //_flags = G_EVAL | G_ARRAY;
                        av_clear(_rets);
                        dSP;
                        ENTER;
                        SAVETMPS;
                        PUSHMARK(sp);

                        n = av_len(_args) + 1;
                        for (j=0; j < n; j++)
                            XPUSHs(sv_2mortal( av_shift(_args) ));

                        PUTBACK;
                        n = perl_call_sv(_code, _flags);
                        SPAGAIN;

                        while (n > 0) {
                          av_push(_rets, newSVsv(POPs));
                          n--;
                        }

                        PUTBACK;
                        FREETMPS;
                        LEAVE;
                    }
                    break;
                }
            }
            break;

        default:
        case FT_NONE:
            break;
    }

    if (next)
        next->Eval();

    return 1;
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
// Duplicate this perl fragment.
PerlFrag *PerlFrag::dup()
{
    PerlFrag *pf = new PerlFrag(_attr,_type);

    pf->_src = new char [strlen(_src)+1];
    strcpy(pf->_src,_src);

    pf->_perl = _perl;

    // Assume we have not been compiled yet.
    // (so don't need to duplicate SV.)

    return pf;
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
// Stream printing (for debugging)
ostream& operator<< (ostream& out, const PerlFrag& pf)
{
    out << "Code: " << pf._src << endl;
    return out;
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o

