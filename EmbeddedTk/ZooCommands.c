
/*
   This file is NOT part of the standard ETk server. It is only
   necessary to create the version of ETk that supports the Zoo
   object browser
   
   IMPORTANT: This is a COPY of the ZooCommands.c file in the
   zoo-tclsh module of the Zoo code repository.
   
*/

#include "ZooCommands.h"
#if defined(ZOOQUERY)
#include "client.h"
#endif
#include <string.h>

extern "C"
{
    int strcasecmp(const char *s1, const char *s2);
}

static bool
ExecQuery(int port, const STRING &server,
	  Ship_ObjList *Oin,
	  Ship_ObjList *Oout,
	  STRING &errmsg);
	       
static Ship_ComplexObj *
AddArcsToQuery(Ship_ObjList *objList, Ship_ComplexObj *parent, 
	       char *pathexpr, bool select_all);

static bool
AddObjectToList(Tcl_DString *dstr, Ship_ComplexObj *obj, 
		Ship_ObjList *objList);
		
static int
SetValue(Tcl_Interp *interp, Ship_ObjList *theQuery,
	 Ship_ComplexObj *parent, char *value);
		    
/*
 * ZooQuery command
 *
 * A query looks like:
 *   ZooQuery <server> <port> { <root class> [ <operator> <value> ] }
 *                            { <path> [ <operator> <value> ] } ...
 *                            [ -debug ] [ -deep ]
 *
 * Server is the name of a machine where Horse is running.
 * Port is the port number on which Horse is listening.
 * Operator can be = < > <= >= .
 * Path can be any path expression beginning at the root class.
 * If -debug is specified, the shipping lists are printed to stdout.
 * If -deep is specified, the query will be marked as a "deep" query.
 *
 * Example:
 *
 *    To retrieve all A objects:
 *        ZooQuery <server> <port> A 
 *
 *    To retrieve a single A object, with oid = 100:
 *        ZooQuery <server> <port> {A = 100} 
 *
 *    To retrieve names and addresses of everyone over 40:
 *        ZooQuery <server> <port> Person name address {age > 40}
 *
 *    To retrieve names of employees and their managers in the Sales dept:
 *        ZooQuery <server> <port> Employee name mgr.name
 *                 {dept.name = \"Sales\"}  (Stray quote for emacs colors: ")
 *        NOTE: the backslashes before the double-quotes were necessary
 *              because of the way Tcl parses commands)
 *
 * Results are returned as a Tcl list, with one list element for each
 * result object. Values for all sub-objects are nested inside each
 * result object. For example: OIDs, names, and ages of people would be
 * returned like this:
 * 
 * Query: ZooQuery <server> <port> Person name age
 * Result: {{<oid> {name Jack} {age 10}} {<oid> {name Jill} {age 11}} ...}
 *
 */

static const char *QueryUsage =
"Usage: ZooQuery server port {type [op val]} [{path [op val]}...] [-debug]\n         [-deep]";

int
ZooQueryCmd(ClientData clientData,    /* Not used          */
	    Tcl_Interp *interp,       /* Tcl interpreter   */
	    int argc, char *argv[])   /* The Tcl command   */
{
    bool debug;
    char *server;
    int port;
    int listArgc;
    char **listArgv;
    char *qual;
    int i;
    ERRORS e;
    Ship_ObjList theQuery;
    Ship_ObjList theResult;
    Ship_ComplexObj comp(&theQuery);
    Ship_ComplexObj *leaf;
    
    // To avoid compiler warning
    clientData = clientData;

    //
    // Enough arguments specified?
    //
    if (argc < 4)
    {
	Tcl_SetResult(interp, (char *) QueryUsage, TCL_STATIC);
	return TCL_ERROR;
    }
    
    debug = false;
    theQuery.setmode(SOL_QUERY_MODE);

    //
    // Initialize the root shipping object
    //
    comp.iterate();
    comp.select(identity);
    
    // First two arguments are server, port
    server = argv[1];
    if (Tcl_GetInt(interp, argv[2], &port) != TCL_OK)
    {
	return TCL_ERROR;
    }

    //
    // The next argument is a class name, or a {class op value} list.
    // Use Tcl_SplitList to determine the length of the list. If 
    // length is 1, then we have a class name. If length is 3, then
    // we have a {class op value} list and we need to set the 
    // qualification of the root object.
    //
    listArgc = 0;
    listArgv = NULL;
    if (Tcl_SplitList(interp, argv[3], &listArgc, &listArgv) != TCL_OK)
    {
	return TCL_ERROR;
    }
    if (listArgc == 1)
    {
	comp.settype(listArgv[0]);
    }
    else if (listArgc == 3)
    {
	comp.settype(listArgv[0]);
	if (strcmp("", listArgv[1]) && strcmp("", listArgv[2])) 
	{
	    qual = new char[strlen(listArgv[1]) + strlen(listArgv[2]) + 2];
	    sprintf(qual, "%s %s", listArgv[1], listArgv[2]);
	    comp.setqual(qual);
	    delete [] qual;
	}
    }
    else
    {
	free((char *) listArgv);
	Tcl_AppendResult(interp, "Invalid object: ", argv[3], NULL);
	return TCL_ERROR;
    }
    free((char *) listArgv);

    for (i = 4; i < argc; i++)
    {
	if (!strcasecmp(argv[i], "-debug"))
	{
	    debug = true;
	    continue;
	}
	if (!strcasecmp(argv[i], "-deep"))
	{
	    comp.select(deep);
	    continue;
	}

	//
	// argv[i] is either a path expression (ex: a.b.c), or a list of the
	// form {pathexpr op value} (ex: pjpeg.maxpsnr > 35).
	//
	// Split the value list to see if we have just a path, or a 
	// {path op val} list.
	//
	if (Tcl_SplitList(interp, argv[i], &listArgc, &listArgv) != TCL_OK)
	{
	    return TCL_ERROR;
	}
	if (listArgc != 1 && listArgc != 3)
	{
	    free((char *) listArgv);
	    Tcl_AppendResult(interp, "Invalid expression: ", argv[i], NULL);
	    return TCL_ERROR;
	}
	
	//
	// Add sub-objects to the query for each component of the
	// path expression. leaf will be a pointer to the leaf 
	// object that was created.
	//
	leaf = AddArcsToQuery(&theQuery, &comp, listArgv[0], true);
	if (leaf == NULL)
	{
	    free((char *) listArgv);
	    Tcl_AppendResult(interp, "Error adding arcs for expression: ",
			     argv[i], NULL);
	    return TCL_ERROR;
	}
	
	//
	// Set the qualification of the leaf object
	//
	if (listArgc == 3)
	{
	    // expression looks like: {path op val}
	    char *op, *value, *temp;
	    op = listArgv[1];
	    value = listArgv[2];
	    temp = new char[strlen(op) + strlen(value) + 2];
	    sprintf(temp, "%s %s", op, value);
	    leaf->setqual(temp);
	    delete [] temp;
	}
	
	free((char *) listArgv);

    }

    if (debug)
    {
	cout << "The query:" << endl;
	theQuery.display();
    }
    
    QueryError qerr;
    List<OBJECTINDEX> *indexList;
    Node<OBJECTINDEX> *node;
    Ship_ComplexObj *objPtr;
    Tcl_DString dstr;
    Tcl_DStringInit(&dstr);
    
    //
    // Get the result objects from Horse
    //
    STRING errmsg;
    if (!ExecQuery(port, server, &theQuery, &theResult, errmsg))
    {
	Tcl_SetResult(interp, (char *) errmsg, TCL_VOLATILE);
	return TCL_ERROR;
    }
    
    if (debug)
    {
	cout << "The result:" << endl;
	theResult.display();
    }
    
    objPtr = (Ship_ComplexObj*) theResult[0];

    //
    // Build a Tcl list, one element for each result object.
    //
    if ((indexList = objPtr->relatedobjs("answers")) != NULL)
    {
	for (node = indexList->getheader();
	     node != NULL;
	     node = node->successor())
	{
	    objPtr = (Ship_ComplexObj*) theResult[node->getvalue()];
	    Tcl_DStringStartSublist(&dstr);
	    AddObjectToList(&dstr, objPtr, &theResult);
	    Tcl_DStringEndSublist(&dstr);
	    
	}
    }

    Tcl_DStringResult(interp, &dstr);
    return TCL_OK;
   
}

/*
 * ZooInsert command
 *
 *
 */

static const char *InsertUsage =
"Usage: ZooInsert server port type [value] [-debug]";

int 
ZooInsertCmd(ClientData clientData, Tcl_Interp *interp,
	     int argc, char *argv[])
{
    // Just to avoid unused parameter warning:
    clientData = clientData;
    
    if (argc < 4 || argc > 6)
    {
	Tcl_SetResult(interp, (char *) InsertUsage, TCL_STATIC);
	return TCL_ERROR;
    }

    ERRORS e;
    Ship_ObjList theQuery;
    Ship_ObjList theResult;
    theQuery.setmode(SOL_INSERT_MODE);
    
    // The first three args are server, port, type
    char *server = argv[1];
    int port;
    if (Tcl_GetInt(interp, argv[2], &port) != TCL_OK)
    {
	return TCL_ERROR;
    }
    char *type = argv[3];
    char *object_value = NULL;
    bool debug = false;
    
    // Last two args are either -debug, or the object value. Order
    // is not important.
    int i;
    for (i = 4; i < argc; i++)
    {
	if (!strcasecmp("-debug", argv[i]))
	{
	    debug = true;
	}
	else
	{
	    if (object_value == NULL)
	    {
		object_value = argv[i];
	    }
	}
    }

    //
    // Create the root shipping object
    //
    Ship_ComplexObj *comp = new Ship_ComplexObj(&theQuery);
    comp->select(deep);
    comp->iterate();
    comp->settype(type);

    if (object_value != NULL)
    {
	if (SetValue(interp, &theQuery, comp, object_value) != TCL_OK)
	{
	    return TCL_ERROR;
	}
    }

    if (debug)
    {
	cout << "The query:" << endl;
	theQuery.display();
    }
    
    //
    // Submit the query
    //
    STRING errmsg;
    if (!ExecQuery(port, server, &theQuery, &theResult, errmsg))
    {
	Tcl_SetResult(interp, (char *) errmsg, TCL_VOLATILE);
	return TCL_ERROR;
    }

    if (debug)
    {
	cout << "The result:" << endl;
	theResult.display();
    }
    
    //
    // Return the OID of the new object
    //
    Tcl_AppendResult(interp, theResult[0]->getqual(), NULL);
    
    return TCL_OK;
   
}

/*
 * ZooUpdate command
 *
 *
 */

static const char *UpdateUsage =
"Usage: ZooUpdate server port type oid {{arc type val}...} [-debug]";

int
ZooUpdateCmd(ClientData clientData, Tcl_Interp *interp,
	     int argc, char *argv[])
{
    ERRORS e;
    Ship_ObjList theQuery;
    Ship_ObjList theResult;
    Ship_ComplexObj root(&theQuery);
    char *server;
    int port;
    bool debug = false;
    
    // Just to avoid unused parameter warning:
    clientData = clientData;
    
    if (argc < 6)
    {
	Tcl_SetResult(interp, (char *) UpdateUsage, TCL_STATIC);
	return TCL_ERROR;
    }

    theQuery.setmode(SOL_UPDATE_MODE);
    
    root.iterate();
    root.select(identity);
    
    // First 4 args are server, port, type, oid
    server = argv[1];
    if (Tcl_GetInt(interp, argv[2], &port) != TCL_OK)
    {
	return TCL_ERROR;
    }
    root.settype(argv[3]);
    root.setqual(argv[4]);
    
    // Next arg is the value of the object
    if (SetValue(interp, &theQuery, &root, argv[5]) != TCL_OK)
    {
	return TCL_ERROR;
    }

    // Last arg may be -debug
    if (argc > 6 && !strcasecmp("-debug", argv[6]))
    {
	debug = true;
    }

    if (debug)
    {
	cout << "The query:" << endl;
	theQuery.display();
    }

    //
    // Submit the query
    //
    STRING errmsg;
    if (!ExecQuery(port, server, &theQuery, &theResult, errmsg))
    {
	Tcl_SetResult(interp, (char *) errmsg, TCL_VOLATILE);
	return TCL_ERROR;
    }

    if (debug)
    {
	cout << "The result:" << endl;
	theResult.display();
    }
    
    // Return the OID of the new object
    Tcl_AppendResult(interp, theResult[0]->getqual(), NULL);
    
    return TCL_OK;
   
}

/* Add a shipping object to the query for each component of the path
 * expression. Returns the leaf object for the last path component.
 */
static Ship_ComplexObj *
AddArcsToQuery(Ship_ObjList *objList, Ship_ComplexObj *parent,
	       char *pathexpr, bool select_all)
	       // char *type, char *op, char *value, bool select)
{
    char *head = NULL;
    char *tail = NULL;
    Ship_ComplexObj *child = NULL;
    char *delim = strpbrk(pathexpr, ".");
    
    // Split the path expression into head and tail where head is the
    // first component and tail is the rest.
    if (delim == NULL)
    {
	// Only one component in the path expression. 
	head = new char[strlen(pathexpr) + 1];
	strcpy(head, pathexpr);
	tail = NULL;
    }
    else
    {
	// Split the string into head and tail (string = head.tail)
	int len = delim - pathexpr;
	head = new char[len + 1];
	strncpy(head, pathexpr, len);
	head[len] = '\0';
	tail = delim + 1;
    }
    
    // Add an arc to the parent object, if it's not there already
    List<OBJECTINDEX> *indexList;
    Node<OBJECTINDEX> *node;
    if (!(indexList = parent->relatedobjs(head)))
    {
	child = new Ship_ComplexObj(objList);
	parent->Create_and_set(head, child);
	if (child && select_all)
	    child->select(identity);
    }
    else if (!(node = indexList->getheader()))
    {
	cerr << "Error accessing relatedobjs list" << endl;
    }
    else if (!(child = (Ship_ComplexObj *) (*objList)[node->getvalue()]))
    {
	cerr << "Error accessing relatedobjs list" << endl;
    }
    
    if (child == NULL)
    {
	delete [] head;
	return NULL;
    }

    if (tail != NULL)
    {
	child = AddArcsToQuery(objList, child, tail, select_all);
    }
    
    delete [] head;
    return child;
    
#if 0    
    if (tail == NULL)
    {
	if (type != NULL)
	{
	    child->settype(type);
	}
	if (value != NULL)
	{
	    if (op != NULL)
	    {
		char *temp = new char[strlen(op) + strlen(value) + 2];
		sprintf(temp, "%s %s", op, value);
		child->setqual(temp);
		delete [] temp;
	    }
	    else
	    {
		child->setqual(value);
	    }
	}
    }
    else
    {
	AddArcsToQuery(objList, child, tail, type, select);
    }
#endif
    
}

static bool 
AddObjectToList(Tcl_DString *dstr, Ship_ComplexObj *obj, 
		Ship_ObjList *objList)
{
    
    Tcl_DStringAppendElement(dstr, obj->getqual());
    
    if (!obj->isatomic()) {

	char *relname;
	for (relname = obj->getfirstrelname();
	     relname != NULL;
	     relname = obj->getnextrelname()) {
	    
	    int j;
	    for(j = obj->getfirstInsideRelname();
		j >= 0;
		j = obj->getNextInsideRelname()) {
		
		Tcl_DStringStartSublist(dstr);
		Tcl_DStringAppendElement(dstr, relname);
		AddObjectToList(dstr, (Ship_ComplexObj *) (*objList)[j], 
				objList);
		Tcl_DStringEndSublist(dstr);
		
	    }
	    
	}
	
    }
    
    return true;

}

int
SetValue(Tcl_Interp *interp, Ship_ObjList *theQuery,
	 Ship_ComplexObj *parent, char *value)
{
    int valueArgc;
    char **valueArgv;
    int i;
    int arcArgc;
    char **arcArgv;
    Ship_ComplexObj *child;
    
    // Split the value list
    if (Tcl_SplitList(interp, value, &valueArgc, &valueArgv) != TCL_OK)
    {
	return TCL_ERROR;
    }

    if (valueArgc == 0)
    {
	Tcl_AppendResult(interp, "Invalid value: ", value, NULL);
	return TCL_ERROR;
    }
    
    // Split the first element of the value list
    if (Tcl_SplitList(interp, valueArgv[0], &arcArgc, &arcArgv) 
	!= TCL_OK)
    {
	free((char *) valueArgv);
	return TCL_ERROR;
    }
    
    // The value may just be an OID. If so, just set the oid of the
    // parent object and return.
    if (valueArgc == 1 && arcArgc == 1)
    {
	parent->setqual(arcArgv[0]);
	free((char *) valueArgv);
	free((char *) arcArgv);
	return TCL_OK;
    }
    
    //
    // Otherwise value is a list of arcs, each arc being a {name type val}
    // list. Create a new child shipping object and call SetValue recursively
    // for each arc in the list.
    //
    // To accomodate set updates, the list could have the form
    // {arc type set-operator value} where set-operator is "+=" or "-="
    // (and maybe there are others--not sure).
    //
    for (i = 0; i < valueArgc; i++)
    {
	char *child_value;
	
	if (arcArgc != 3 && arcArgc != 4)
	{
	    Tcl_AppendResult(interp, "Invalid arc: ", valueArgv[i], NULL);
	    free((char *) valueArgv);
	    free((char *) arcArgv);
	    return TCL_ERROR;
	}
    
	child = new Ship_ComplexObj(theQuery);
	
	if (arcArgc == 3)
	{
	    // List looks like: {arc type value}
	    parent->Create_and_set(arcArgv[0], child);
	    child->settype(arcArgv[1]);
	    child_value = arcArgv[2];
	}
	else
	{
	    // List looks like: {arc type set-operator value}
	    parent->Create_and_set(arcArgv[0], child);
	    child->settype(arcArgv[1]);
	    parent->Set_operator(arcArgv[0], arcArgv[2]);
	    child_value = arcArgv[3];
	}
	
	if (SetValue(interp, theQuery, child, child_value) != TCL_OK)
	{
	    free((char *) valueArgv);
	    free((char *) arcArgv);
	    return TCL_ERROR;
	}
    
	// Split the next arc before executing the loop again
	free((char *) arcArgv);

	if (i < valueArgc - 1)
	{
	    if (Tcl_SplitList(interp, valueArgv[i+1], &arcArgc, &arcArgv) 
		!= TCL_OK)
	    {
		free((char *) valueArgv);
		return TCL_ERROR;
	    }
	}

    }
    
    free((char *) valueArgv);
    
    return TCL_OK;

}

static bool
ExecQuery(int port,
	  const STRING &server,
	  Ship_ObjList *Oin,
	  Ship_ObjList *Oout,
	  STRING &errmsg)
{
    
    ERRORS errors;
    QueryError qerr;
    ostrstream os;
    
    os << port << ends;

    qerr = ClientExecQuery(os.str(), server,
			   Oin, Oout, errors);
    
    delete os.str();
    
    if (qerr != qe_Success)
    {
	if (qerr == qe_DBError)
	{
	    errmsg = STRING("Horse query failed: ") + errors.error_msg();
	    if (errors.error_extra() != NULL)
	    {
		errmsg = errmsg + STRING(" (") + errors.error_extra()
		    + STRING(")");
	    }
	}
	else if (qerr == qe_NetworkError)
	{
	    errmsg = "Horse query failed due to a network error";
	}
	
	cout << "\nWARNING: Error in query execution:\n    "
	    << errmsg << endl;
	cout << "The following query produced the error:\n" << endl;
	Oin->display();
	cout << endl;
	
	return false;
    
    }
    
    return true;

}

