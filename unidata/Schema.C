// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
//
//   Schema Definition
//   $Id$
//
//   Copyright 1997, Shaun Flisakowski
//
//   You may distribute under the terms of either the GNU General Public
//   License or the Artistic License, as specified in the README file.
//
// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o

#include  <string.h>

#include  "Schema.h"
#include  "Token.h"

#include  "Config.h" 

#undef    YYSTYPE        // perl has it's own lexer.
#include  "PerlFrag.h"
#include "getftime.h"

#undef    assert        // defined by perl includes
#include  <assert.h>

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o 

static int lineno, colno;

#define MATCH(tok,ult)  do { if (!match((tok),(ult))) return(0); } while(0)


// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
Schema::Schema()
{
    init();
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
void Schema::init()
{
    _name     = NULL;
    _valid    = 1;

    _type     = TEXT;
    _char_set = ASCII;
    _bin_type = NATIVE;

    _rli      = VAR_LEN;
    _reclen   = 0;

    _cmmts     = new CmmtStk();
    _converter = NULL;
    _global    = NULL;

    _nattrs    = 0;
    _recsze    = 0;

    _attr_own  = new AttrStk(16);

    _enums     = NULL;
    _typedefs  = new AttrStk();

    _attr0     = NULL;
    _flat_attrs= new AttrStk();

    _ordering  = new IntStack();

    _subr_cnt  = 0;
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
Schema::~Schema()
{
    clear();
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
void Schema::clear()
{
    delete [] _name;
    _name = NULL;

    delete [] _converter;
    _converter = NULL;

    delete _global;

    if (_enums)
        _enums->empty(1);

    delete _enums;
    _enums = NULL;

    delete _typedefs;
    _typedefs = NULL;

    delete _attr0;
    _attr0 = NULL;

    delete _flat_attrs;
    _flat_attrs = NULL;

    _attr_own->empty(1);    // Delete all attributes.
    delete _attr_own;
    _attr_own = NULL;

    delete _ordering;
    _ordering = NULL;

    delete _cmmts;

    _valid = 1;
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
int Schema::get_token(UniLexType *ult)
{
    lineno = yylineno;
    colno  = yycolno;

    return uni_yylex(ult);
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
void Schema::dispose_token(int tok, UniLexType *ult)
{
    switch (tok) {

      default:
        // Most tokens use no extra space.
        break;

      case PERL_EXPR:
      case PERL_SUBR:
        delete [] ult->PerlCode;
        break;

      case REG_EXPR:
        delete [] ult->RegExpr;
        break;

      case SYMBOL:
      case STRING:
        delete [] ult->Str;
        break;
    }
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
// Match the expected token, return 1 if match succeeds,
// or 0 if it fails, and prints an error.
int Schema::match(int exp_tok, UniLexType *ult)
{
    int tok;

    tok    = get_token(ult);

/*  // For debugging
    cout << "match: expecting token ";
    show_token(&cout,exp_tok,NULL);
    cout << " and got token "; 
    show_token(&cout,tok,NULL);
    cout << endl;
*/

    if (tok != exp_tok) {

        *perr << filename << ":" << lineno
              << ": Expecting token: ";

        show_token(perr,exp_tok,NULL);
        *perr << " but found token ";

        show_token(perr,tok,ult);
        *perr << endl;

        dispose_token(tok,ult);
        return(0);
    }

    return(1);
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
// Build a literal string, ie convert all \t,\n, etc to
// there real equivilents. (allocates memory)
char *Schema::mkLiteral(char *in)
{
    char *ret = NULL;

    if (in && *in) {

        int j,k,len;

        k = 0;
        len = strlen(in);
        ret = new char[len+1];

        for (j=0; j < len; j++) {
            if (in[j] != '\\') 
                ret[k++] = in[j];
            else switch (in[++j]) {

                default:
                      // This case handles literal quote,
                      // backslash, and everything else not mentioned.
                    ret[k++] = in[j];   // \x -> x
                    break;

                case 't':
                    ret[k++] = '\t';    // Tab
                    break;

                case 'n':
                    ret[k++] = '\n';    // NewLine
                    break;

                case 'r':
                    ret[k++] = '\r';    // Carriage Return
                    break;

                case 'b':
                    ret[k++] = '\b';    // Backspace
                    break;

                case 'f':
                    ret[k++] = '\f';    // Form Feed
                    break;

                case 'v':
                    ret[k++] = '\v';    // Vertical Tab
                    break;

                case 'a':
                    ret[k++] = '\a';    // Alert (bell)
                    break;
            }
        }

        ret[k] = '\0';
    }

    return ret;
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
// Parse a position-style range (ie: [5..13]).
int Schema::parse_range(int& init, int& end)
{
    UniLexType ult;
    int        tok;

    init = -1;
    end  = -1;    // Invalid values

    tok = get_token(&ult);

    switch(tok) {
        default:
            *perr << filename << ":" << lineno
                  << ": Expecting integer range: ";
            show_token(perr,tok,&ult);
            *perr << endl;
            dispose_token(tok,&ult);
            return 0;

        case INUM:
            init = ult.Int;
            break;

        case KY_TO:
            break;
    }

    tok = get_token(&ult);
    switch(tok) {
        default:
            *perr << filename << ":" << lineno
                  << ": Expecting integer range: ";
            show_token(perr,tok,&ult);
            *perr << endl;
            dispose_token(tok,&ult);
            return 0;

        case INUM:
            end = ult.Int;
            break;

        case KY_TO:
            break;

        case KY_RBRCKT:
            return 1;
    }

    tok = get_token(&ult);
    switch(tok) {
        default:
            *perr << filename << ":" << lineno
                  << ": Expecting integer range: ";
            show_token(perr,tok,&ult);
            *perr << endl;
            dispose_token(tok,&ult);
            return 0;
    
        case INUM:
            end = ult.Int;
            break;
    
        case KY_RBRCKT:
            return 1;
    }

    MATCH(KY_RBRCKT, &ult);
    return 1;
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
// Parse a list of integers, if the 2nd stack is non-null,
// allow position-style also (ie: [5..13]).
int Schema::parse_ilist(IntStack *init, IntStack *end)
{
    UniLexType ult;
    int        tok;
    int        n1, n2, sep;

    sep = 1;    // An initial comma causes an empty value right away.

    do {
        n1 = n2 = -1;    // Invalid values.

        tok = get_token(&ult);
        switch(tok) {
            default:
                *perr << filename << ":" << lineno
                      << ": Expecting integer range: ";
                show_token(perr,tok,&ult);
                *perr << endl;
                dispose_token(tok,&ult);
                return 0;
        
            case INUM:
                n1 = ult.Int;
                sep = 0;
                break;
        
            case KY_LBRCKT:
                if (end) {
                    if (!parse_range(n1,n2))
                        return 0;
                } else {
                    *perr << filename << ":" << lineno
                    << ": A numerical range is not allowed in this context.\n";
                    return 0;
                }
                sep = 0;
                break;

            case KY_RPAREN:
                return 1;

            case KY_COMMA:
                sep++;
                break;
        }

        if ((n1 != -1) || (sep > 1)) {
            init->push(n1);
            if (end)
                end->push(n2);
            if (sep > 1)
                sep = 1;
        }

    } while(1);

    return 1;  // Not reached.
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
// Fill in the attribute given.
// Recursively calls itself to parse the lower levels. 
// If attr is NULL, the top-level is being parsed.
int Schema::subParse(Attr *attr)
{
    UniLexType ult;
    int        tok;
    Attr      *subattr = NULL;
    Enum      *enm = NULL;
    Comment   *cmmt;
    PerlFrag  *pf;
    int        m = 0, pos;
    int        j, ipos, epos;

#define    TOP_LEVEL(s) do { \
     if (attr != _attr0) { \
        *perr << filename << ":" << lineno \
              << ": " << s << " is only allowed at the top-level.\n"; \
         dispose_token(tok,&ult); \
         return(0); \
     } } while(0)

#define    NOT_TOP_LEVEL(s) do { \
     if (attr == _attr0) { \
        *perr << filename << ":" << lineno \
              << ": " << s << " is not allowed at the top-level.\n"; \
         dispose_token(tok,&ult); \
         return(0); \
     } } while(0)

  
    while(1) { 

        tok = get_token(&ult);

        switch (tok) {
            default:
                *perr << filename << ":" << lineno
                      << ": Invalid token at this point in file: ";
                show_token(perr,tok,&ult);
                *perr << endl;
                dispose_token(tok,&ult);
                return(0);
    
            case KY_RBRACE:    // Done parsing this bracketed section.
                  // Keep a count of the number of flat attributes
                  // you're composed of.
                {
                    AttrStk *stk = attr->subattr();
                    attr->_nattrs = 0; 

                    if (stk) {
                        if (stk->is_empty()) {
                            if (attr != _attr0)
                                attr->_nattrs = 1;   // count yourself
                        } else
                            for (j=0; j < stk->nAttrs(); j++)
                                attr->_nattrs += stk->ith(j)->nattrs();
                    } else 
                        if (attr != _attr0)
                            attr->_nattrs = 1;   // count yourself

                    if (attr == _attr0)
                        _nattrs = attr->_nattrs;

                    // Need to distribute this positioning info
                    // to the subattributes.  We try to propagate
                    // info both forwards and backwards to other
                    // fields, butting them up against each other.
                    // If the first lpos is not found, it's set to 0.

                    if (stk && (attr->_sublpos || attr->_sublen)) {

                        // Push position list down into subattributes.
                        if (attr->_sublpos)
                            for (j=0; j < attr->_sublpos->count(); j++) {
                                ipos = attr->_sublpos->get_ith(j);
                                epos = attr->_subrpos->get_ith(j);
                                Attr *sa = stk->ith(j);

                                if (sa) {
                                    if (!sa->have_lpos() && (ipos != -1))
                                        sa->set_lpos(ipos);
                                    if (!sa->have_rpos() && (epos != -1))
                                        sa->set_rpos(epos);
                                }
                            }
                    }

                    if (stk) {
                        attr->propagate_pos();

                        if (stk->ith(0)) {
                            if (!stk->ith(0)->have_lpos()) {
                                stk->ith(0)->set_lpos(0);
                                attr->propagate_pos();
                            }
                        }
                    }
                }

                return 1;
    
            case KY_SEMICOLON:    // Extra semicolon, ignore.
                break;

            case KY_ATTR:
                MATCH(SYMBOL, &ult);
    
                subattr = new Attr(ult.Str);

                // Want these things to scope.
                subattr->set_white(attr->whitespace(),0);
                subattr->set_dtefmt(attr->date_format(),0);
                subattr->set_quote(attr->quote());
                subattr->set_maxlen(attr->maxlen());
 
                if (!match(KY_LBRACE,&ult)) {
                    delete subattr;
                    dispose_token(tok,&ult);
                    return(0);
                }
    
                _attr_own->push(subattr);
   
                if (!attr->_subattr)
                    attr->_subattr = new AttrStk();
 
                attr->subattr()->push(subattr);
                subParse(subattr);
			 
// This Part is added for checking maxlen entry (strings) in schema  file

			 if ((subattr->_type == String_Attr) && (subattr->_maxlen < 1 )) {
				*perr << filename << " maxlen is not indicated in : " << (subattr->_flat_name) << " " ; 
				show_token(perr,tok,&ult);
				*perr << endl ;
				exit(1) ;
			}

				 if (!subattr->subattr()->is_empty())
					subattr->set_type(UserDefined_Attr);

                subattr->determ_size();
                break;
    
            case KY_TYPEDEF:
                TOP_LEVEL("typedef");
                MATCH(SYMBOL, &ult);

                subattr = new Attr(ult.Str);

                // Want these things to scope.
                subattr->set_white(attr->whitespace(),0);
                subattr->set_dtefmt(attr->date_format(),0);
                subattr->set_quote(attr->quote());
                subattr->set_maxlen(attr->maxlen());
    
                if (!match(KY_LBRACE,&ult)) {
                    delete subattr;
                    dispose_token(tok,&ult);
                    return(0);
                }
    
                _attr_own->push(subattr);
                _typedefs->push(subattr);
    
                subParse(subattr);
                subattr->determ_size();

                break;
    
            case KY_ENUM:
                // NYI - SPF
                TOP_LEVEL("enum");
                MATCH(SYMBOL, &ult);
                enm = new Enum(ult.Str);

                if (!_enums)
                    _enums = new EnumStk();

                if (!match(KY_LBRACE,&ult)) {
                    delete enm;
                    dispose_token(tok,&ult);
                    return(0);
                }

                enm->set_type(UserDefined_Attr + _enums->nEnums() + 1);

                {
                    int done = 0, saw_eq = 0;
                    char *sym = NULL;
                    char *val = NULL;

                    while (!done) {
                        // Read each line of the enum.
                        tok = get_token(&ult);
                        switch (tok) {
                          default:
                            *perr << filename << ":" << lineno
                                  << ": Unexpected token: ";
                            show_token(perr,tok,&ult);
                            *perr << endl;
                            dispose_token(tok,&ult);
                            delete [] sym;
                            delete [] val;
                            delete enm;
                            return(0);

                          case SYMBOL:
                          case STRING:
                            if (saw_eq) {
                              if (!val)
                                  val = ult.Str;
                              else {
                                  *perr << filename << ":" << lineno
                                        << ": Expecting semicolon token: ";
                                  show_token(perr,tok,&ult);
                                  *perr << endl;
                                  dispose_token(tok,&ult);
                                  delete [] sym;
                                  delete [] val;
                                  delete enm;
                                  return(0);
                              }
                            } else {
                              if (!sym)
                                  sym = ult.Str;
                              else {
                                *perr << filename << ":" << lineno
                                      << ": Expecting '=' token: ";
                                show_token(perr,tok,&ult);
                                *perr << endl;
                                dispose_token(tok,&ult);
                                delete [] sym;
                                delete [] val;
                                delete enm;
                                return(0);
                              }
                            }
                            break;

                          case KY_EQ:
                            if (sym)
                                saw_eq = 1;
                            else {
                                *perr << filename << ":" << lineno
                                      << ": Expecting symbol token: ";
                                show_token(perr,tok,&ult);
                                *perr << endl;
                                dispose_token(tok,&ult);
                                delete [] sym;
                                delete [] val;
                                delete enm;
                                return(0);
                            }
                            break;

                          case KY_COMMA:
                          case KY_SEMICOLON:
                            if (!sym || (saw_eq && !val)) {
                                *perr << filename << ":" << lineno
                                      << ": Expecting symbol token: ";
                                show_token(perr,tok,&ult);
                                *perr << endl;
                                dispose_token(tok,&ult);
                                delete [] sym;
                                delete [] val;
                                delete enm;
                                return(0);
                            }

                            if (saw_eq)
                                enm->add(sym,val,String_Attr);
                            else
                                enm->add(sym,sym,String_Attr);

                            saw_eq = 0;
                            sym    = NULL;
                            val    = NULL;
                            break;

                          case KY_RBRACE:
                            done = 1;
                            break;
                        }
                    }
                }

                _enums->push(enm);
                break;
    
            case KY_TYPE:
                MATCH(KY_EQ, &ult);
                tok = get_token(&ult);
                switch (tok) {
                  default:
                    *perr << filename << ":" << lineno
                          << ": Expecting type token: ";
                    show_token(perr,tok,&ult);
                    *perr << endl;
                    dispose_token(tok,&ult);
                    return(0);

                  case SYMBOL:    // A previous typedef or enum
                    NOT_TOP_LEVEL("user-defined type");
                    attr->set_type(UserDefined_Attr);

                    // Need to find type, set this to a copy of it.
                    // If not a type, check for an enum.
                    pos = _typedefs->find(ult.Str);

                    if (pos >= 0) {
                        dispose_token(tok,&ult);
                        subattr = _typedefs->ith(pos);
                        attr->assign(subattr,_attr_own);
                    } else {

                        pos = _enums->find(ult.Str);

                        if (pos < 0) {
                            *perr << filename << ":" << lineno
                                  << ": Invalid type name: ";
                            show_token(perr,tok,&ult);
                            *perr << endl;
                            dispose_token(tok,&ult);
                            return(0);
                        }

                        attr->set_type((attr_t) _enums->ith(pos)->type());
                        dispose_token(tok,&ult);
                    }

                    break;

                  case KY_TEXT:
                    TOP_LEVEL("text");
                    _type = TEXT;
                    break;

                  case KY_BINARY:
                    TOP_LEVEL("binary");
                    _type = BINARY;
                    break;

                  case KY_INT:
                    NOT_TOP_LEVEL("int");
                    attr->set_type(Int_Attr);
                    attr->set_seper(NULL);
                    break;

                  case KY_FLOAT:
                    NOT_TOP_LEVEL("float");
                    attr->set_type(Float_Attr);
                    attr->set_seper(NULL);
                    break;

                  case KY_DOUBLE:
                    NOT_TOP_LEVEL("double");
                    attr->set_type(Double_Attr);
                    attr->set_seper(NULL);
                    break;

                  case KY_STRING:
                    NOT_TOP_LEVEL("string");
                    attr->set_type(String_Attr);
                    attr->set_seper(NULL);
                    break;

                  case KY_DATETIME:
                    NOT_TOP_LEVEL("datetime");
                    attr->set_type(DateTime_Attr);
                    attr->set_seper(NULL);
                    break;

                  case KY_UNIXTIME:
                    NOT_TOP_LEVEL("unixtime");
                    attr->set_type(UnixTime_Attr);
                    attr->set_seper(NULL);
                    break;
                }
                MATCH(KY_SEMICOLON, &ult);
                break;
    
            case KY_CHAR_SET:
                TOP_LEVEL("char_set");
                MATCH(KY_EQ, &ult);
                tok = get_token(&ult);
                switch(tok) {
                    default:
                        *perr << filename << ":" << lineno
                              << ": Expecting char_set choice: ";
                        show_token(perr,tok,&ult);
                        *perr << endl;
                        dispose_token(tok,&ult);
                        return(0);

                    case KY_ASCII:
                        _char_set = ASCII;
                        break;
                }
                MATCH(KY_SEMICOLON, &ult);
                break;
    
            case KY_BIN_ORDER:
                TOP_LEVEL("binary_order");
                MATCH(KY_EQ, &ult);
                tok = get_token(&ult);
                switch(tok) {
                    default:
                        *perr << filename << ":" << lineno
                              << ": Expecting binary_order choice: ";
                        show_token(perr,tok,&ult);
                        *perr << endl;
                        dispose_token(tok,&ult);
                        return(0);

                    case KY_NATIVE:
                        _bin_type = NATIVE;
                        break;

                    case KY_NETWORK:
                        _bin_type = NETWORK;
                        break;
                }

                MATCH(KY_SEMICOLON, &ult);
                break;
    
            case KY_RECSZE:
                TOP_LEVEL("recsze");
                MATCH(KY_EQ, &ult);
                MATCH(INUM, &ult);
                _rli = FIXED_LEN;
                _reclen = ult.Int;
                MATCH(KY_SEMICOLON, &ult);
                break;
    
            case KY_COMMENT:
                TOP_LEVEL("comment");
                MATCH(KY_EQ, &ult);

                MATCH(STRING,&ult);
                cmmt = new Comment();
                cmmt->init = ult.Str;

                tok = get_token(&ult);
                switch(tok) {
                    default:
                        *perr << filename << ":" << lineno
                              << ": Expecting '..' or ';': ";
                        show_token(perr,tok,&ult);
                        *perr << endl;
                        dispose_token(tok,&ult);
                        return(0);

                    case KY_TO:
                        MATCH(STRING, &ult);
                        cmmt->end = ult.Str;
                        MATCH(KY_SEMICOLON, &ult);
                        break;

                    case KY_SEMICOLON:
                        break;
                }

                  // Add the default end as end-of-line.
                if (! cmmt->end) {
                    cmmt->end = new char [2];
                    strcpy(cmmt->end,"\n");
                }

                _cmmts->push(cmmt);
                break;
    
            case KY_SEPARATOR:
                MATCH(KY_EQ, &ult);
                MATCH(STRING, &ult);

                attr->set_seper(mkLiteral(ult.Str));

                dispose_token(STRING,&ult);
                MATCH(KY_SEMICOLON, &ult);
                break;

            case KY_WHITESPACE:
                MATCH(KY_EQ, &ult);
                MATCH(STRING, &ult);

                attr->set_white(mkLiteral(ult.Str));

                dispose_token(STRING,&ult);
                MATCH(KY_SEMICOLON, &ult);
                break;

            case KY_DELIMITER:
                MATCH(KY_EQ, &ult);
                MATCH(STRING, &ult);

                attr->set_delimit(mkLiteral(ult.Str));

                dispose_token(STRING,&ult);
                MATCH(KY_SEMICOLON, &ult);
                break;
    
            case KY_CONVERTER:
                TOP_LEVEL("converter");
                MATCH(KY_EQ, &ult);
                MATCH(STRING, &ult);
                _converter = ult.Str;
                MATCH(KY_SEMICOLON, &ult);
                break;
    
            case KY_DATE_FRMT:
                MATCH(KY_EQ, &ult);
                MATCH(STRING, &ult);
                attr->set_dtefmt(ult.Str);
                MATCH(KY_SEMICOLON, &ult);
                break;

            case KY_FORMAT:
                MATCH(KY_EQ, &ult);
                MATCH(REG_EXPR, &ult);
                attr->set_format(ult.RegExpr);
                MATCH(KY_SEMICOLON, &ult);
                break;
    
            case KY_FILTER:
                MATCH(KY_EQ, &ult);

                tok = get_token(&ult);
                switch(tok) {
                    default:
                        *perr << filename << ":" << lineno
                              << ": Expecting perl code: ";
                        show_token(perr,tok,&ult);
                        *perr << endl;
                        dispose_token(tok,&ult);
                        return(0);

                    case PERL_EXPR:
                        attr->set_filter(FT_EXPR,ult.PerlCode);
                        break;

                    case PERL_SUBR:
                        attr->set_filter(FT_SUBR,ult.PerlCode);
                        break;
                }
                break;
    
            case KY_READER:
                MATCH(KY_EQ, &ult);

                tok = get_token(&ult);
                switch(tok) {
                    default:
                        *perr << filename << ":" << lineno
                              << ": Expecting perl code: ";
                        show_token(perr,tok,&ult);
                        *perr << endl;
                        dispose_token(tok,&ult);
                        return(0);

                    case PERL_EXPR:
                        attr->set_reader(FT_EXPR,ult.PerlCode);
                        break;

                    case PERL_SUBR:
                        attr->set_reader(FT_SUBR,ult.PerlCode);
                        break;
                }
                break;
    
            case KY_GLOBAL:
                TOP_LEVEL("global");
                MATCH(KY_EQ, &ult);

                tok = get_token(&ult);
                switch(tok) {
                    default:
                        *perr << filename << ":" << lineno
                              << ": Expecting perl code: ";
                        show_token(perr,tok,&ult);
                        *perr << endl;
                        dispose_token(tok,&ult);
                        return(0);

                    case PERL_EXPR:
                    case PERL_SUBR:
                        pf = new PerlFrag(NULL,FT_GLOB);
                        pf->set_src(ult.PerlCode);
                        if (_global)
                            _global->enlist(pf);
                        else
                            _global = pf;
                        break;
                }
                break;
    
            case KY_VALUE:
                NOT_TOP_LEVEL("value");
                MATCH(KY_EQ, &ult);

                tok = get_token(&ult);
                switch(tok) {
                    default:
                        *perr << filename << ":" << lineno
                              << ": Expecting perl code: ";
                        show_token(perr,tok,&ult);
                        *perr << endl;
                        dispose_token(tok,&ult);
                        return(0);

                    case PERL_EXPR:
                        attr->set_value(FT_EXPR,ult.PerlCode);
                        break;

                    case PERL_SUBR:
                        attr->set_value(FT_SUBR,ult.PerlCode);
                        break;
                }
                break;
    
            case KY_MIN:
                NOT_TOP_LEVEL("min");
                MATCH(KY_EQ, &ult);
                tok = get_token(&ult);
                switch(tok) {
                    default:
                        *perr << filename << ":" << lineno
                              << ": Expecting constant: ";
                        show_token(perr,tok,&ult);
                        *perr << endl;
                        dispose_token(tok,&ult);
                        return(0);

                    case INUM:
                        delete [] attr->min();
                        attr->_min_type = Int_Attr;
                        attr->_min = new char [sizeof(int)];
                        memcpy(attr->_min, &(ult.Int), sizeof(int));
                        break;

                    case RNUM:
                        delete [] attr->min();
                        attr->_min_type = Double_Attr;
                        attr->_min = new char [sizeof(double)];
                        memcpy(attr->_min, &(ult.Float), sizeof(double));
                        break;

                    case STRING:
                        delete [] attr->min();
                        attr->_min_type = String_Attr;
                        attr->_min = ult.Str;
                        break;
                }
                MATCH(KY_SEMICOLON, &ult);
                break;
    
            case KY_MAX:
                NOT_TOP_LEVEL("max");
                MATCH(KY_EQ, &ult);
                tok = get_token(&ult);
                switch(tok) {
                    default:
                        *perr << filename << ":" << lineno
                              << ": Expecting constant: ";
                        show_token(perr,tok,&ult);
                        *perr << endl;
                        dispose_token(tok,&ult);
                        return(0);

                    case INUM:
                        delete [] attr->max();
                        attr->_max_type = Int_Attr;
                        attr->_max = new char [sizeof(int)];
                        memcpy(attr->_max, &(ult.Int), sizeof(int));
                        break;

                    case RNUM:
                        delete [] attr->max();
                        attr->_max_type = Double_Attr;
                        attr->_max = new char [sizeof(double)];
                        memcpy(attr->_max, &(ult.Float), sizeof(double));
                        break;

                    case STRING:
                        delete [] attr->max();
                        attr->_max_type = String_Attr;
                        attr->_max = ult.Str;
                        break;
                }
                MATCH(KY_SEMICOLON, &ult);
                break;
    
            case KY_MAXLEN:
                MATCH(KY_EQ, &ult);
                MATCH(INUM, &ult);
                attr->set_maxlen(ult.Int);
                MATCH(KY_SEMICOLON, &ult);
                break;
    
            case KY_QUOTE:
                MATCH(KY_EQ, &ult);
                MATCH(STRING, &ult);
                attr->set_quote(ult.Str);
                dispose_token(STRING,&ult);

                MATCH(KY_SEMICOLON, &ult);
                break;
    
            case KY_POSITION:
                MATCH(KY_EQ, &ult);

                tok = get_token(&ult);
                switch(tok) {

                    default:
                        *perr << filename << ":" << lineno
                              << ": Expecting integer range: ";
                        show_token(perr,tok,&ult);
                        *perr << endl;
                        dispose_token(tok,&ult);
                        return(0);

                    case KY_LPAREN:
                        if (attr->_sublpos || attr->_subrpos) {
                            *perr << filename << ":" << lineno
                                  << ": Duplicate position command.\n";
                            return 0;
                        }

                        attr->_sublpos = new IntStack();
                        attr->_subrpos = new IntStack();

                        if (!parse_ilist(attr->_sublpos,attr->_subrpos))
                            return 0;

                        break;

                    case KY_LBRCKT:
                        if (!parse_range(ipos,epos))
                            return 0;

                        if (ipos >= 0)
                            attr->set_lpos(ipos);
                        if (epos >= 0)
                            attr->set_rpos(epos);

                        break;

                    case INUM:
                        attr->set_lpos(ult.Int);
                        break;
                }

                MATCH(KY_SEMICOLON, &ult);
                break;

            case KY_LENGTH:
                MATCH(KY_EQ, &ult);

                tok = get_token(&ult);
                switch(tok) {

                    default:
                        *perr << filename << ":" << lineno
                              << ": Expecting integer, or list: ";
                        show_token(perr,tok,&ult);
                        *perr << endl;
                        dispose_token(tok,&ult);
                        return(0);

                    case KY_LPAREN:
                        if (attr->_sublen) {
                            *perr << filename << ":" << lineno
                                  << ": Duplicate length command.\n";
                            return 0;
                        }

                        attr->_sublen = new IntStack();

                        if (!parse_ilist(attr->_sublen))
                            return 0;

                        break;

                    // Not sure what to do here yet.
                    //case INUM:
                        //attr->set_lpos(ult.Int);
                        //break;
                }

                MATCH(KY_SEMICOLON, &ult);
                break;

            case KY_FUNC_OF:
                NOT_TOP_LEVEL("function_of");
                MATCH(KY_EQ, &ult);

                m = 1;
                MATCH(SYMBOL,&ult);

                while (m) {

                    FuncOf *f = new FuncOf(ult.Str,attr->func_of());
                    attr->set_func_of(f); 

                    tok = get_token(&ult);
                    switch(tok) {
                        default:
                            *perr << filename << ":" << lineno
                                  << ": Expecting list of attribute names: ";
                            show_token(perr,tok,&ult);
                            *perr << endl;
                            dispose_token(tok,&ult);
                            return(0);
    
                        case KY_COMMA:
                            MATCH(SYMBOL,&ult);
                            break;
    
                        case KY_SEMICOLON:
                            m = 0;
                            break;
                    }
                }

                break;
    
            case KY_SORTED:
                NOT_TOP_LEVEL("sorted");
                attr->traits()->set(Sorted_Trait);
                MATCH(KY_SEMICOLON,&ult);
                break;
    
            case KY_UNIQUE:
                NOT_TOP_LEVEL("unique");
                attr->traits()->set(Unique_Trait);
                MATCH(KY_SEMICOLON,&ult);
                break;
    
            case KY_CONSEC:
                NOT_TOP_LEVEL("consecutive");
                attr->traits()->set(Consecutive_Trait);
    
                  // These traits are implied by Consecutive
                attr->traits()->set(Sorted_Trait);
                attr->traits()->set(Unique_Trait);
                MATCH(KY_SEMICOLON,&ult);
                break;
    
            case KY_NULL_ALLOW:
                NOT_TOP_LEVEL("null_allowed");
                attr->traits()->set(Null_Allowed_Trait);
                MATCH(KY_SEMICOLON,&ult);
                break;
        }
    }

    return(1);

#undef    TOP_LEVEL
#undef    NOT_TOP_LEVEL
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
// Parse a schema, return the offset the "data" starts at.
off_t Schema::Parse(int fd, char *fname, ostream *err)
{
    UniLexType ult;
    char tmpbuf[2];

      // Get rid of previous Schema, if there was one.
    clear();
    init();

    lineno = colno = 0;
    unilex_init(fname);
    yyin_fd = fd; 
    perr = err;

    MATCH(KY_SCHEMA, &ult);
    MATCH(SYMBOL, &ult);
    _name = ult.Str;

    MATCH(KY_LBRACE, &ult);

      // Top-level "attribute" for the entire schema.
    _attr0 = new Attr();
    _attr0->_subattr = new AttrStk();

    if (!subParse(_attr0)) {
        unilex_end();
        _valid = 0;
        return 0;
    }

    unilex_end();

      // get rid of the '\n' following the last '}'
    read(yyin_fd,tmpbuf,1);
    yyoffset++;

    _recsze = 0;
    if (_attr0->subattr())
        for (int j=0; j < _attr0->subattr()->nAttrs(); j++)
            _recsze += _attr0->subattr()->ith(j)->size_of();

      // Need to round up _recsze to handle alignment restrictions.
    _recsze += (_recsze % MAX_ALIGN) ? (MAX_ALIGN - (_recsze % MAX_ALIGN)) : 0;
 
      // Make the flat version also.
    if (_attr0->subattr())
        for (int j=0; j < _attr0->subattr()->nAttrs(); j++)
            _attr0->subattr()->ith(j)->flatten(_flat_attrs);


      // Now, compute field offsets, ordering, etc.
    DeepThought(err);

      // Check for validity.
    _valid &= RealityCheck(err);

    return(yyoffset);
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
// Does this schema use perl at all?
int Schema::uses_perl()
{
    if (_global)
        return 1;

    return _attr0->uses_perl();
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
// Provide a Perl Interpreter to use.
void Schema::set_interp(PerlInterpreter *perl)
{
    if (_global)
        _global->set_interp(perl);

    _attr0->set_interp(perl);
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
void Schema::setup_perl(PerlInterpreter *perl)
{
    set_interp(perl);

    if (_global)
        _global->compile(_subr_cnt,NULL);

    _attr0->compile(_subr_cnt);
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
// duplicate this schema (deep-copy)
Schema *Schema::dup()
{
    cerr << "Warning: " __FUNCTION__ " - Not yet implemented.\n";
    return NULL;
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
// Check that this schema is reasonable.
// Return 0 for some problem, 1 otherwise.
int Schema::RealityCheck(ostream *err)
{
    if (_type == BINARY) {

        if (_rli != FIXED_LEN) {
            *err << "Error: Binary data must have fixed length records.\n"; 
            return 0;
        }

        if (_reclen <= 0) {
            *err << "Error: Invalid record length: " << _reclen << endl; 
            return 0;
        }
    }

    if (_rli == FIXED_LEN) {
        if (_cmmts->nCmmts() != 0) {
            *err << "Warning: Comments are not allowed in files with "
                 << "fixed-length records.\n";
            _cmmts->empty(1);
        }
    }

    return 1; 
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
// Pre-compute as much about the attributes as we can:
//   - ordering
//   - field offset (in the output tuple)
// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
void Schema::DeepThought(ostream *err)
{
    int j;

      // Ordering: start with the default, the order listed.
      // with the caveat that all "real" attributes come before
      // computed ones.
    for (j=0; j < _nattrs; j++)
        if (! _flat_attrs->ith(j)->value())
            _ordering->push(j);

    for (j=0; j < _nattrs; j++)
        if (_flat_attrs->ith(j)->value())
            _ordering->push(j);

      // Now process all function_of statements:
      // First, preprocess all function_of statements to set
      // the indexes into the _flat_attrs array.

    FuncOf *fo;
    for (j=0; j < _nattrs; j++)
        for (fo = _flat_attrs->ith(j)->func_of(); fo; fo=fo->next)
            fo->indx = _flat_attrs->find(fo->attr);

      // We'll iterate a few times, but if the ordering doesn't 
      // stabilize in _nattrs+1 iterations we'll assume there 
      // must be a circularity. (It should normally damp out fairly
      // fast, a single function_of statement that is independent of
      // the others is fixed in a single iteration. I suspect that
      // the number of iterations required for no circularities is
      // probably bounded by the number of function_of statements.
      // In any case, _nattrs+1 will give us a little fudge on top of that.
      // The "+1" is so that we exit with done true whenever we got
      // an ordering; imagine a single attribute with a function_of,
      // (even though that doesn't make sense, a function of what? :^)
      // it will be fixed on the first iteration, but we need another
      // iteration to check it so that done is set.)

    int iters = 0;
    int done;

    do {
        done = 1;

        for (j=0; j < _nattrs; j++)
            if ((fo = _flat_attrs->ith(j)->func_of())) {

                int max_pos = _ordering->find(fo->indx);
                int pos;

                for (fo=fo->next; fo; fo=fo->next) {
                    pos = _ordering->find(fo->indx);
                    max_pos = (max_pos >= pos) ? max_pos : pos; 
                }

                if (max_pos > (pos = _ordering->find(j))) {
                    done = 0;
                    _ordering->swap(pos,max_pos);
                }
            }

    } while(!done && (iters++ < _nattrs+1));

    if (!done) {
        *err << "Error: circularity caused by function_of statements\n";
    }

    // o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o

      // Compute offsets in the output data record, if we do this
      // right, fields should always be aligned.

      // We'll organize the fields by most restrictive alignment,
      // ie:  double, float, int, string

      // Of course, the architecture needs to roughly correspond
      // the machine model we're imagining.

      // A quick check that the person editing Config.h
      // can follow instructions.
    assert(DOUB_ALIGN >= sizeof(double));
    assert(FLT_ALIGN >= sizeof(float));
    assert(INT_ALIGN >= sizeof(int));

      // Check that we're ordering things properly.
    assert(DOUB_ALIGN >= FLT_ALIGN);
    assert(FLT_ALIGN >= TM_ALIGN);
    assert(TM_ALIGN >= INT_ALIGN);

      // Check that no padding is required when we switch between types.
    assert((DOUB_ALIGN % FLT_ALIGN) == 0);
    assert((DOUB_ALIGN % INT_ALIGN) == 0);
    assert( (FLT_ALIGN % INT_ALIGN) == 0);
    assert(  (TM_ALIGN % INT_ALIGN) == 0);

      // Check that no padding is required when packing a single type
      // (that would be _really_ weird.
    assert((sizeof(double) % DOUB_ALIGN) == 0);
    assert((sizeof(float) % FLT_ALIGN) == 0);
    assert((sizeof(int) % INT_ALIGN) == 0);

      // This one is actually required to be true by ISO C
    assert((sizeof(TimeT) % TM_ALIGN) == 0);

    int offst = 0;

    // First, do all the doubles
    for (j=0; j < _nattrs; j++)
        if (_flat_attrs->ith(j)->type() == Double_Attr) {
            _flat_attrs->ith(j)->_offset = offst;
            offst += DOUB_ALIGN;
        }

    // now floats
    for (j=0; j < _nattrs; j++)
        if (_flat_attrs->ith(j)->type() == Float_Attr) {
            _flat_attrs->ith(j)->_offset = offst;
            offst += FLT_ALIGN;
        }

    // now unixtime
    for (j=0; j < _nattrs; j++)
        if (_flat_attrs->ith(j)->type() == UnixTime_Attr) {
            _flat_attrs->ith(j)->_offset = offst;
            offst += sizeof(time_t);
        }

    // now struct tm's
    for (j=0; j < _nattrs; j++)
        if (_flat_attrs->ith(j)->type() == DateTime_Attr) {
            _flat_attrs->ith(j)->_offset = offst;
            offst += sizeof(TimeT);
        }

    // now ints
    for (j=0; j < _nattrs; j++)
        if (_flat_attrs->ith(j)->type() == Int_Attr) {
            _flat_attrs->ith(j)->_offset = offst;
            offst += INT_ALIGN;
        }

    // now enums
    for (j=0; j < _nattrs; j++)
        if (_flat_attrs->ith(j)->type() > UserDefined_Attr) {
            _flat_attrs->ith(j)->_offset = offst;
            offst += sizeof(ushort);
        }

    // and strings
    for (j=0; j < _nattrs; j++)
        if (_flat_attrs->ith(j)->type() == String_Attr) {
            _flat_attrs->ith(j)->_offset = offst;
            offst += _flat_attrs->ith(j)->size_of();
        }
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
// Stream printing (for debugging)
ostream& operator<< (ostream& out, const Schema& schema)
{
    out << " Schema\n--------\n";
    out << "Name: " << schema._name << endl;

    out << "Type: ";
    switch (schema._type) {
        default:
            out << "<Unknown>\n";
            break;

        case Schema::TEXT:
            out << "Text\n";
            break;

        case Schema::BINARY:
            out << "Binary\n";
            break;
    }

    out << "Char Set: ";
    switch (schema._char_set) {
        default:
            out << "<Unknown>\n";
            break;

        case Schema::ASCII:
            out << "Ascii\n";
            break;
    }

    if (schema._type == Schema::BINARY) {
        out << "Binary Ordering: ";
        switch (schema._bin_type) {
            default:
                out << "<Unknown>\n";
                break;

            case Schema::NATIVE:
                out << "Native\n";
                break;

            case Schema::NETWORK:
                out << "Network\n";
                break;
        }
    }

    out << "Record Length: ";
    if (schema._rli == Schema::FIXED_LEN)
        out << schema._reclen << endl;
    else
        out << "Variable\n";

    int len = schema._cmmts->nCmmts();
    for (int j=0; j < len; j++) {
        Comment *cmmt = schema._cmmts->ith(j);
        out << *cmmt << endl;
    }

    out << "Size needed to store record (internally): "
        << schema._recsze << endl;

    if (schema._converter)
        out << "Converter: " << schema._converter << endl;

    // print _global

    out << "Number of Attributes @ top-level: ";
    if (schema._attr0->subattr())
        out << schema._attr0->subattr()->nAttrs() << endl;
    else
        out << 0 << endl;

    out << "Number of Attributes (flat): "
        << schema._nattrs << endl;

    out << "Order attributes will be computed in:\n    "
        << *(schema._ordering) << endl;

    for (int j=0; j < schema._nattrs; j++) {
        out << "\nAttribute " << j << ":\n";
        out << *(schema._flat_attrs->ith(j));
    }

    /*
    if (schema._attr0->subattr())
        for (int j=0; j < schema._attr0->subattr()->nAttrs(); j++) {
            out << "\nAttribute " << j << ":\n";
            out << *(schema._attr0->subattr()->ith(j));
        }
    */

    return out;
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
