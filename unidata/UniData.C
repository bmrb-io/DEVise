// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
//
//   Universal Data Reader Definition
//   $Id$
//
//   Copyright 1997, Shaun Flisakowski
//
//   You may distribute under the terms of either the GNU General Public
//   License or the Artistic License, as specified in the README file.
//
// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o

#include  <stdlib.h>
#include  <string.h>
#include  <errno.h>
#include  <fcntl.h>
#include  <sys/stat.h>

#include  "Config.h"

#include  "UniData.h"
#include  "udIndex.h"
#include  "fast_atof.h"
#include  "getftime.h"

#undef    assert        // defined by perl includes
#include  <assert.h>

//#define   DEBUG_UNIDATA

extern SlideBuf *yyslbuf;

    // To try to avoid collisions with other UniData instances.
static unsigned int fifo_cnt = 0;

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
void UniData::init()
{
    _name = NULL;
    _schema = NULL;

    _status = UD_OK;

    _ownfd  =  0;
    _datafd = -1;

    _in_fifo  = NULL;
    _out_fifo = NULL;

    _slbuf  = NULL;

    _data_init = 0;

    _next_rid = 0;
    _last_rid = 0;

    _params     = new ParamStk();
    _sys_params = new ParamStk();

    _index = NULL;
    _getrec = (GetRecFunc) NULL;

    _cnvt_security = UD_CHK_SUSPICIOUS;
    _chkcnvt = (ChkCnvtFunc) NULL;

    _perl = NULL;
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
// If schema is NULL, it's at the begining of the data.
UniData::UniData(char *datafile, char *schemafile)
{
    init();

    _ownfd = 1;
    _datafd = open(datafile, O_RDONLY);

    _slbuf  = new SlideBuf(_datafd);
    _slbuf->slurp(_status,1);

    do {

        if (_datafd < 0) {
            _status = UD_FAIL;
            break;
        }

        _schema = new Schema();
        if (schemafile) {

            int schfd;
            schfd = open(schemafile, O_RDONLY);

            if (schfd < 0) {
                _status = UD_FAIL;
                break;
            }

            yyslbuf  = new SlideBuf(schfd);
            _schema->Parse(schfd,schemafile);
            delete yyslbuf;
            yyslbuf = NULL;

            if (!_schema->valid())
               _status = UD_FAIL;

            if (close(schfd) < 0)
                perror(__FUNCTION__);

        } else {
            yyslbuf = _slbuf;
            _data_init = _schema->Parse(_datafd,"<NoName>");
            yyslbuf = NULL;

              // Almost certainly in the buffer?
            _slbuf->Seek(_status,_data_init); 
        }

    } while(0);

    if (isOk()) {
        setup_converter(datafile);
        get_params();
        setup_perl();
    }
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
// If schema is NULL, it's at the begining of the data.
UniData::UniData(int datafd, Schema *schema)
{
    init();

    _datafd = datafd;
    _schema = schema;

    _slbuf  = new SlideBuf(_datafd);
    _slbuf->slurp(_status,1);

    if (isOk()) {
        setup_converter();
        get_params();
        setup_perl();
    }
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
UniData::~UniData()
{
      // close file descriptor
    if (_ownfd && (_datafd != -1))
        if (close(_datafd) < 0)
            perror(__FUNCTION__);

    if (_in_fifo)
        unlink(_in_fifo);

    if (_out_fifo)
        unlink(_out_fifo);

    delete [] _in_fifo;
    delete [] _out_fifo;

    delete [] _name;

    delete _schema;
    _params->empty();
    delete _params;

    _sys_params->empty();
    delete _sys_params;

    delete _index;

    delete _slbuf;

    if (_perl) {
        perl_destruct(_perl);
        perl_free(_perl);
    }
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
// A function to figure out the converter (if any)
// and do the setup for it.
int  UniData::setup_converter(char *filename)
{
    char *cnv = _schema->converter();

    if (!cnv || !*cnv)
        return 0;

      // These FIFO's could get large if the converter
      // is a blocking operation (like sorting).
    char *tmp_dir = getenv(TMP_DIR);
    int   mypid = (unsigned int) getpid();
    int   chldpid;

    if (!tmp_dir)
        tmp_dir = DFLT_TMP_DIR;

      // Make two FIFOs right away, though we may not need them both.
    _in_fifo = new char [MAXPATHLEN];
    sprintf(_in_fifo, "%s/udin_fifo.%d.%u", tmp_dir, mypid, fifo_cnt++);

    _out_fifo = new char [MAXPATHLEN];
    sprintf(_out_fifo, "%s/udout_fifo.%d.%u", tmp_dir, mypid, fifo_cnt++);

      // I hope someone wasn't using these...
    unlink(_in_fifo);
    unlink(_out_fifo);

    if ( mkfifo(_in_fifo, S_IRUSR | S_IWUSR ) < 0) {
        perror(__FUNCTION__);
        _status = UD_FAIL;
        return 0;
    }

    if ( mkfifo(_out_fifo, S_IRUSR | S_IWUSR ) < 0) {
        perror(__FUNCTION__);
        _status = UD_FAIL;
        return 0;
    }

    char *fname = strstr(cnv,CNV_FILE);
    char *cmmd;

    if (!fname || !filename) {

        // We need to stream the data into their stdin.
        // We can fork 'cat' to take care of this, so we don't
        // have to worry about deadlock. 

        if ( (chldpid = fork()) < 0) {

            perror(__FUNCTION__);
            _status = UD_FAIL;
            return 0;

        } else if (chldpid == 0) {

            // Child process
            int outfd = open(_in_fifo, O_WRONLY);

            if (outfd < 0) {
                cerr << "Cannot open pipe " << _in_fifo << endl;
                exit(-1);
            }

              // Dump the first data page (within the SlideBuf) right away.
              // This is the beginning of the data file, obtained by a
              // premature slurp.
            _slbuf->dump(outfd);

              // Now, let 'cat' handle the rest.
            dup2(_datafd,0);    // Make this stdin
            dup2(outfd,1);      // Make this stdout

            execlp(CNV_CAT, CNV_CAT, NULL);

                /* NOT REACHED */
            exit(0);
        }
    }


    if (close(_datafd) < 0)
        perror(__FUNCTION__);

    _datafd = -1;


    if (fname) {

        // They want a filename, replace '$file' with the actual name.

            // Give them the name of the FIFO if we don't have a filename.
        if (!filename)
            filename = _in_fifo;

        *fname = '\0';
        fname += strlen(CNV_FILE);

          // Single quotes are not allowed in file names.
        if (strchr(filename,'\'')) {
            _status = UD_FAIL;
            if (fname)
                delete [] cmmd;
            unlink(_in_fifo);
            unlink(_out_fifo);
            return 0;
        }

        cmmd = new char [strlen(cnv) + strlen(filename) + strlen(fname) + 3];

        strcpy(cmmd,cnv);
        strcat(cmmd,"'");
        strcat(cmmd,filename);
        strcat(cmmd,"'");
        strcat(cmmd,fname);

    } else
        cmmd = cnv;

    switch (_cnvt_security) {

        default:
        case UD_CHK_SUSPICIOUS:
            if (strpbrk(cmmd,SHELL_CHARS)) {
                if ( _chkcnvt != (ChkCnvtFunc) NULL ) {
                    if ( !(_chkcnvt)(cmmd) ) {
                        _status = UD_FAIL;
                        if (fname)
                            delete [] cmmd;
                        unlink(_in_fifo);
                        unlink(_out_fifo);
                        return 0; 
                    }
                } else {
                    _status = UD_FAIL;
                    if (fname)
                        delete [] cmmd;
                    unlink(_in_fifo);
                    unlink(_out_fifo);
                    return 0; 
                }
            }
            break;

        case UD_CHK_ALWAYS:
            if ( _chkcnvt != (ChkCnvtFunc) NULL ) {
                if ( !(_chkcnvt)(cmmd) ) {
                    _status = UD_FAIL;
                    if (fname)
                        delete [] cmmd;
                    unlink(_in_fifo);
                    unlink(_out_fifo);
                    return 0; 
                }
            } else {
                _status = UD_FAIL;
                if (fname)
                    delete [] cmmd;
                unlink(_in_fifo);
                unlink(_out_fifo);
                return 0; 
            }
            break;

        case UD_CHK_NEVER:
            break;
    }

    if ( (chldpid = fork()) < 0) {

        perror(__FUNCTION__);
        _status = UD_FAIL;
        if (fname)
            delete [] cmmd;
        unlink(_in_fifo);
        unlink(_out_fifo);
        return 0;

    } else if (chldpid > 0) {

        // Parent process
        _datafd = open(_out_fifo, O_RDONLY);

        if (_datafd < 0)
            _status = UD_FAIL;
        else
            _slbuf->slurp(_status,1);

    } else {

        // Child process
        int infd;

        if (!fname) {
            infd = open(_in_fifo, O_RDONLY);

            if (infd < 0) {
                cerr << "Cannot open pipe " << _in_fifo << endl;
                exit(-1);
            }
        }

        int outfd = open(_out_fifo, O_WRONLY);

        if (outfd < 0) {
            cerr << "Cannot open pipe " << _out_fifo << endl;
            exit(-1);
        }

        if (!fname)
            dup2(infd,0);     // Make this stdin

        dup2(outfd,1);        // Make this stdout

        execl(CNV_SHELL, CNV_SHELL, CNV_SH_CMMD_OPT, cmmd, NULL);

            /* NOT REACHED */
        exit(0);
    }

    if (fname)
        delete [] cmmd;

    return 1;
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
// Prepare for the use of perl, if it's needed.
int  UniData::setup_perl()
{
    char *embed[] = { "", "-e", "sub _eval_ { eval $_[0] }" };

    if (!_schema->uses_perl())
        return 0;

    _perl = perl_alloc();
    perl_construct(_perl);

        // Apparently, we need to parse _something_.
    perl_parse(_perl, NULL, 3, embed, NULL);

    _schema->setup_perl(_perl);

    if (_schema->_global)
        _schema->_global->Eval();

    return 1;
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
// Set the perl variables for this record.
void UniData::set_vars(char *rcrd)
{
    int j, lim = _schema->NumFlatAttrs();

    for (j=0; j < lim; j++) {
        Attr *at = _schema->_flat_attrs->ith(j);

        if (at->_perl_var)
            at->set_var( &(rcrd[at->offset()]) );
    }
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
// Get the perl variables for this record.
void UniData::grab_vars(char *rcrd)
{
    int j, lim = _schema->NumFlatAttrs();

    for (j=0; j < lim; j++) {
        Attr *at = _schema->_flat_attrs->ith(j);

        if (at->_perl_var)
            at->grab_var( &(rcrd[at->offset()]) );
    }
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
// Build parameters
int UniData::build_params(Attr *attr, ParamStk *params, int& fl_indx)
{
    udParam *prm = new udParam();

    prm->attr = attr;
    params->push(prm);

    AttrStk *subattr = attr->subattr();

    if (subattr && !subattr->is_empty()) {

        prm->subparam = new ParamStk();
        int have_pos = 1;

        for (int j=0; j < subattr->nAttrs(); j++) {
            build_params(subattr->ith(j),prm->subparam,fl_indx);
            if (! subattr->ith(j)->have_lpos())
                have_pos = 0;
        }

        // This is the only formatting currently supported, this needs
        // to be expanded to handle all kinds of formatting.

        if (prm->subparam->is_empty()) {
            // Didn't need it, maybe only system parameters below.
            delete prm->subparam;
            prm->subparam = NULL;
            prm = params->pop();
            delete prm;
        } else if (have_pos)
            prm->attrfunc = Split_Position;
        else if (_schema->attr0()->seperator())
            prm->attrfunc = Split_Seper;
        else
            prm->attrfunc = Split_White;

    } else {

        prm->flat_indx = fl_indx++;
        prm->dst_off = _schema->GetFlatAttr(prm->flat_indx)->offset();

        // These are the only types currently supported, this
        // section must handle all builtin types.

        if (attr->reader())
            prm->attrfunc = ReaderCall;

        else if (attr->value()) {

            prm = params->pop();
            prm->attrfunc = ValueCall;
            _sys_params->push(prm);

        } else if (_schema->getType() == Schema::BINARY)
            prm->attrfunc = BinCopy_Native;

        else
            switch (attr->_type) {
    
              case Int_Attr:
                  prm->attrfunc = TxtCopy_Int;
                  break;
    
              case Float_Attr:
                  prm->attrfunc = TxtCopy_Float;
                  break;
    
              case Double_Attr:
                  prm->attrfunc = TxtCopy_Double;
                  break;
    
              case String_Attr:
                  prm->attrfunc = TxtCopy_String;
                  break;
    
              case UnixTime_Attr:
                  prm->attrfunc = TxtCopy_UnixTime;
                  break;

              case Date_Attr:
                  // NYI - don't know yet.
                  prm->attrfunc = NullCopy;
                  break;

              case DateTime_Attr:
                  prm->attrfunc = TxtCopy_DateTime;
                  break;

              default:
                  prm->attrfunc = NullCopy;
                  break;
            }

              // Add a filter if we have one.
            if (attr->filter()) {
                udParam *sysprm = prm->dup();
                sysprm->attrfunc = FilterCall;
                _sys_params->push(sysprm);
            }
    }

    return 0;
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
// setup the parameters we'll need.
int  UniData::get_params()
{
    int flt_indx = 0;

    build_params(_schema->attr0(),_params,flt_indx);

    if (_schema->getType() == Schema::TEXT) {
        // Assume delimited for now?
        _getrec = getRec_delimit;
    } else
        _getrec = getRec_recsze;

    if (_getrec == (GetRecFunc) NULL)
        _status = UD_FAIL;

    return flt_indx;
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
void UniData::SetName(char *name)
{
    delete [] _name;
    int len = strlen(name) + 1;

    _name = new char[len];
    memcpy(_name,name,len); 
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
// Do we have an Rid field?
// (get one for "free" for data srcs with fixed-length records)
int  UniData::haveRid()
{
    int rl;

      // Do we have one for free?
    if (_schema->getRecLen(rl) == Schema::FIXED_LEN)
        return 1;

      // Did we build an index already?
    if (_index)
        return 1;

    return 0;
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
// Indicates you want an Rid interface,
// builds an internal index of record offsets if necessary.
// Can switch at any time, but turning it on causes a reset()
// to the beginning of file.
void UniData::useRid(int uR)
{
    if (uR) {
        if (haveRid())
            return;

        // Build an index here.
        struct stat statbuf;

        if (fstat(_datafd,&statbuf) == 0)
            _index = new udIndex(statbuf.st_size,_data_init);
        else
            _index = new udIndex();

        Seek(_data_init);
        _next_rid = 0;

    } else {
        delete _index;
        _index = NULL;
    }
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
// Get a single record (the next available)
UD_Status UniData::getRec(char *buff, off_t *offset)
{
    if (isOk()) {

        off_t off;
        (_getrec)(buff, &off);

        if (offset)
           *offset = off;
    }

    return _status;
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
// Get a single record, with random access.
UD_Status UniData::getRndRec(char *buff, off_t offset)
{
    Seek(offset);    // This turns off EOF, if it was set.

    if (isOk()) {
          // need to fix _next_rid if rid indexing is being used.
        if (_index)
            _index->revlookup(offset,_next_rid);

          // offset isn't intended to be passed back here,
          // we already have it anyhow.
        (_getrec)(buff, &offset);
    }

    return _status;
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
// Get a page of records, returns the number obtained.
// Trys to fill the buffer, but won't read
// more than maxRecs records in any case.
UD_Status UniData::getRecs(char *buff, int bufsze, int& numRecs,
                           off_t *offsets)
{
    int cnt = -1;
    int rsze = _schema->recSze();

    if  (!isOk() || (bufsze < rsze) || (numRecs <= 0))
        return UD_FAIL;

    while (isOk() && (cnt < numRecs) && (bufsze >= rsze)) {
        off_t off;
        cnt++;

        (_getrec)(buff, &off);

        if (offsets)
           offsets[cnt] = off;

        buff   += rsze;
        bufsze -= rsze;
    }

    numRecs = cnt;

    if (numRecs == 0)
        return _status;
    else
        return UD_OK;
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
// Moves the read pointer.  (make private?)
int UniData::Seek(off_t offset)
{
    _status = UD_OK;    // no more EOF
    _slbuf->Seek(_status,offset);
    return 0;
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
// Seek to beginning of data in this file;
// also causes perl "global" sections to be re-run, and clears any
// error conditions.
int UniData::reset()
{
    _status = UD_OK;
    _next_rid = 0;

    Seek(_data_init);

    return 0;
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
// Get a single record
UD_Status UniData::getRidRec(char *buff, RecId& rid)
{
    off_t     offset; 
    UD_Status stat;

    stat = translateRid(rid,offset);

    if (stat != UD_OK)
        return stat;

    _next_rid = rid;

      // No need to write the same code twice.
    return getRndRec(buff,offset);
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
// Get a few, starting at startRid
UD_Status UniData::getRidRecs(char *buff, int bufsze, int& numRecs,
                         RecId& startRid)
{
    off_t     offset; 
    UD_Status stat;

    stat = translateRid(startRid,offset);

    if (stat != UD_OK)
        return stat;

    _next_rid = startRid;
    Seek(offset);

      // No need to write the same code twice.
    return getRecs(buff,bufsze,numRecs,NULL);
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
// Translate an Rid into the offset to be used.
UD_Status UniData::translateRid(RecId& Rid, off_t& offset)
{
    if (_index) {
        if (! _index->lookup(Rid,offset))
            return UD_FAIL;
    } else {
#ifdef    DEBUG_UNIDATA
        // Should really just assume they know what they are doing.
        assert(haveRid());
#endif

        offset = _data_init + (_schema->recSze() * Rid);
    }

    return UD_OK;
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
// Stream printing (prints state, for debugging)
ostream& operator<< (ostream& out, const UniData& ud)
{
    out << " -- UniData Internals --\n";

    out << "Name: ";
    if (ud._name)
        out << ud._name << endl;
    else
        out << "<UnNamed>\n";

    out << "Status: ";
    switch (ud._status) {
      case UD_OK:
        out << "UD_OK\n";
        break;

      case UD_EOF:
        out << "UD_EOF\n";
        break;

      case UD_FAIL:
        out << "UD_FAIL\n";
        break;

      default:
        out << "Error, invalid status.\n";
        break;
    }

    out << "Data Starts at offset: " << ud._data_init << endl;

/*
    out << "Schema: ";
    if (ud._schema)
        out << endl << *(ud._schema);
    else
        out << "None\n";
*/

    out << "Index: ";
    if (ud._index)
        out << endl << *(ud._index);
    else
        out << "None\n";

    out << " -----------------------\n\n";

    out << "Parameters:\n";
    for (int j=0; j < ud._params->count(); j++)
        out << *(ud._params->ith(j));

    out << endl;

    out << " -----------------------\n\n";

    return out;
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
// Do a getRec using record size (text or binary).
int UniData::getRec_recsze(char *buff, off_t *offset)
{
    int ok = 0;
    int nxt_buf_init;

    if (isOk()) {

        while (!ok && isOk()) {

            _slbuf->room(_status,UD_BUFSLACK);

            nxt_buf_init = _slbuf->get_init() + _schema->_reclen;
        
           *offset = _slbuf->getoff();
    
            ok = (_params->ith(0)->attrfunc)(buff,_slbuf->getcur(),
                                                 _params->ith(0));
    
            if (ok)
                ok &= run_SysParams(buff);

            _slbuf->set_init(nxt_buf_init);
        }

        if (ok) {
            if (_next_rid >= _last_rid)
                _last_rid = ++_next_rid;
            else
                _next_rid++;
        }
    }

    return ok;
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
// Do a getRec using a delimitor (text-only).
int UniData::getRec_delimit(char *buff, off_t *offset)
{
    int ok = 0;

    if (isOk()) {

        while (!ok && isOk()) {
    
            _slbuf->SkipCmmts(_status, _schema->_cmmts,
                              _schema->attr0()->_white);
    
            _slbuf->room(_status,UD_BUFSLACK);
    
           *offset = _slbuf->getoff();
    
            udParam *prm = _params->ith(0);
            prm->buf_pos = *offset;
            ok = (prm->attrfunc)(buff,_slbuf->getcur(),prm);

            if (ok)
                ok &= run_SysParams(buff);

            if (ok) { 

                if (_next_rid >= _last_rid) {
                    if (_index)
                        _index->insert(_next_rid,*offset);
                    _last_rid = ++_next_rid;
                } else
                    _next_rid++;
            } 

              // Now locate the start of the next record.
            _slbuf->SkipDelimit(_status,_schema->attr0()->delimiter());
        }
    }

    return ok;
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
int UniData::run_SysParams(char *rcrd)
{
    int ok = 1;

    if (!_sys_params->is_empty()) {
        set_vars(rcrd);

        for (int j=0; j < _sys_params->count(); j++) {
            udParam *prm = _sys_params->ith(j);
            ok &= (prm->attrfunc)(rcrd,NULL,prm);
        }

        grab_vars(rcrd);
    }

    return ok;
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
//  This one splits on whitespace if we have it, otherwise
//  relying on the reading functions to take what they need.
int UniData::Split_White(char *dst, char * /* src */, udParam *ud)
{
    int ok;

    for (int j=0; j < ud->subparam->count(); j++) {

        udParam *param = ud->subparam->ith(j);

        _slbuf->SkipWhite(_status,ud->attr->whitespace());
        assert(param);
        _slbuf->setzero(param->buf_pos);
        _slbuf->room(_status,UD_BUFSLACK);
        _slbuf->room(_status,param->sze()+1);

        char *buf = _slbuf->getpos(param->buf_pos);

        ok = (param->attrfunc)(dst,buf,param);

        if (!ok)
            return 0;
    }

    return 1;
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
int UniData::Split_Seper(char *dst, char * /* src */, udParam *ud)
{
    int      ok, last;
    char     sentinel;
    AttrStk *subattr = ud->attr->subattr();

    for (int j=0; j < ud->subparam->count(); j++) {

        last = (j == subattr->nAttrs()-1);
        
        udParam *param = ud->subparam->ith(j);

        _slbuf->SkipWhite(_status,ud->attr->whitespace());
        _slbuf->setzero(param->buf_pos);
        _slbuf->room(_status,UD_BUFSLACK);
        _slbuf->room(_status,param->sze()+1);

        char *buf = _slbuf->getpos(param->buf_pos);
        char *sent = NULL;
        off_t soff;

        if ( !last && !param->attr->seperator) {

            sent = strchr(buf, *(ud->attr->seperator()) );

            if (sent) {
                soff = _slbuf->getoff(sent);    
                sentinel = *sent;
               *sent = '\0';
            }
        }

        ok = (param->attrfunc)(dst,buf,param);

        // This may have changed when calling someone.
        if (sent) {
            sent = _slbuf->getpos(soff);
           *sent = sentinel;
            _slbuf->set_init(soff+1);
        }

        if (!ok)
            return 0;
    }

    return 1;
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
int UniData::Split_Position(char *dst, char * /* src */, udParam *ud)
{
    int      ok;
    char     sentinel;
    AttrStk *subattr = ud->attr->subattr();

    for (int j=0; j < ud->subparam->count(); j++) {

        Attr *attr = subattr->ith(j);
        udParam *param = ud->subparam->ith(j);

        assert(attr->have_lpos());

        _slbuf->setzero(param->buf_pos,ud->buf_pos,attr->lpos());
        _slbuf->set_init(param->buf_pos);

        _slbuf->room(_status,param->sze()+1);
        _slbuf->room(_status,UD_BUFSLACK);

        if (attr->have_rpos()) {
            _slbuf->room(_status, (attr->rpos() - attr->lpos() + 1));

            char *sent = _slbuf->getpos(ud->buf_pos,attr->rpos()+1);
            // Should check that sent_pos is within the buffer, NYI.
            sentinel = *sent;
            *sent = '\0';

            ok = (param->attrfunc)(dst, _slbuf->getpos(ud->buf_pos,
                                   attr->lpos()), param);

              // This may have changed when calling someone.
            sent = _slbuf->getpos(ud->buf_pos,attr->rpos()+1);
            *sent = sentinel;
        } else
            ok = (param->attrfunc)(dst,_slbuf->getpos(ud->buf_pos,
                                   attr->lpos()), param);

        if (!ok)
            return 0;
    }

    return 1;
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
int UniData::NullCopy(char *dst, char *, udParam *ud)
{
    char *mydst = &(dst[ud->dst_off]);
    memset(mydst,'\0',ud->sze());
    return 1;
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
int UniData::BinCopy_Native(char *dst, char *src, udParam *ud)
{
    char *mydst = &(dst[ud->dst_off]);
    memcpy(mydst,src,ud->sze());
    _slbuf->set_init(src + ud->sze());
    return 1;
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
int UniData::TxtCopy_Int(char *dst, char *src, udParam *ud)
{
    char *ptr;
                    // guarenteed to be aligned
    int *i = (int*) &(dst[ud->dst_off]);

#ifdef DEBUG_UNIDATA
    if (((int)dst % INT_ALIGN) != 0)  // but we'll check to be sure.
        return 0;
    cout << "In " << __FUNCTION__ << " for string '" << src << "'\n";
#endif

    src += strspn(src, ud->attr->whitespace());
    *i = strtol(src, &ptr, 10);
    _slbuf->set_init(ptr);

    return (errno != ERANGE) && (ptr != src);
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
int UniData::TxtCopy_Float(char *dst, char *src, udParam *ud)
{
    char *ptr;
                    // guarenteed to be aligned
    float *f = (float*) &(dst[ud->dst_off]);

#ifdef DEBUG_UNIDATA
    if (((int)dst % FLT_ALIGN) != 0)  // but we'll check to be sure.
        return 0;
    cout << "In " << __FUNCTION__ << " for string '" << src << "'\n";
#endif

    src += strspn(src, ud->attr->whitespace());
    *f = UtilStrtod(src, &ptr);
    _slbuf->set_init(ptr);

    return (errno != ERANGE) && (ptr != src);
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
int UniData::TxtCopy_Double(char *dst, char *src, udParam *ud)
{
    char *ptr;
                    // guarenteed to be aligned
    double *d = (double*) &(dst[ud->dst_off]);

#ifdef DEBUG_UNIDATA
    if (((int)dst % DOUB_ALIGN) != 0)  // but we'll check to be sure.
        return 0;
#endif

    src += strspn(src, ud->attr->whitespace());
    *d = UtilStrtod(src, &ptr);
    _slbuf->set_init(ptr);

    return (errno != ERANGE) && (ptr != src);
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
int UniData::TxtCopy_String(char *dst, char *src, udParam *ud)
{
    int n, tmpn;
    char *str = &(dst[ud->dst_off]);
    char *delim;

    n = ud->sze()-1;

    if (ud->attr->whitespace()) {
      src += strspn(src, ud->attr->whitespace());
      tmpn = strcspn(src, ud->attr->whitespace());
      n = (tmpn < n) ? tmpn : n;
    }

    delim = ud->attr->delimiter();
    if (!delim)
        delim = "\n";

    tmpn = strcspn(src,delim);
    n = (tmpn < n) ? tmpn : n;

    strncpy(str,src,n);
    str[n] = '\0';
    _slbuf->set_init(src + n);

    return 1;
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
int UniData::TxtCopy_DateTime(char *dst, char *src, udParam *ud)
{
                    // guarenteed to be aligned
    struct tm *t = (struct tm*) &(dst[ud->dst_off]);

    src += strspn(src, ud->attr->whitespace());
    int n = getftime(src, ud->attr->date_format(), t);

    _slbuf->set_init(_slbuf->get_init() + n);

    return (n != 0);
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
int UniData::TxtCopy_UnixTime(char *dst, char *src, udParam *ud)
{
    char *ptr;
                    // guarenteed to be aligned
    time_t *t = (time_t*) &(dst[ud->dst_off]);

    src += strspn(src, ud->attr->whitespace());
    *t = strtol(src, &ptr, 10);
    _slbuf->set_init(ptr);

    return (errno != ERANGE) && (ptr != src);
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
int UniData::ReaderCall(char *dst, char *src, udParam *ud)
{
    // NYI - needs to be changed.
    ud->attr->reader()->Eval();
    return 1;
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
int UniData::ValueCall(char *, char *, udParam *ud)
{
    ud->attr->value()->Eval();
    return 1;
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
int UniData::FilterCall(char *, char *, udParam *ud)
{
    ud->attr->filter()->Eval();
    return 1;
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
