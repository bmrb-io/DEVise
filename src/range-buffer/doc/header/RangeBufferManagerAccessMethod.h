class RangeBufferManagerAccessMethod: public RangeAccessMethod
{

public:

    /********************************************************************/
    /* Constructor.							*/
    /* 									*/
    /* Specifies the object to be queried on as well as	a handle to a	*/
    /* low level RangeAccessMethod where this				*/
    /* RangeBufferManagerAccessMethod can get more records if what is	*/
    /* currently cache doesn't include the entire answer to the query.	*/
    /*									*/
    /* Must be called before any query (scan) can be issued.		*/
    /********************************************************************/
    RangeBufferManagerAccessMethod(RBMObject obj,	
						/* IN: Object 		*/

				   RangeAccessMethod *ram); 
						/* IN: pointer to RAM	*/
						/*     in lower level 	*/

    /********************************************************************/
    /* Destructor							*/
    /*									*/
    /* Must be called when there is no scan outstanding.		*/
    /********************************************************************/
    virtual ~RangeBufferManagerAccessMethod();

    /********************************************************************/
    /* The open scan function. Specifies the query composed of the      */
    /* 'in' range and the 'not in' list.                                */
    /*                                                                  */
    /* The query result would be all records whose bounding box         */
    /* attribute value is in the 'in' range but is not in any of the    */
    /* elements of the 'not in' list.                                   */
    /*                                                                  */
    /* return 0 if OK. return negtive number if something is wrong      */
    /********************************************************************/
    virtual int
    openScan(BoundingBox *in,               /* IN: The "in" range      	*/
             BoundingBoxList *notIn);       /* IN: The "not in" ranges 	*/

    /********************************************************************/
    /* The iterating function. Attempting to get the next record if     */
    /* there is any.                                                    */
    /*                                                                  */
    /* return 0 if OK                                                   */
    /* return NO_MORE_RECORD (-1) if no more record                     */
    /********************************************************************/
    virtual int
    nextRec(void *&record);                  /* OUT: pointer to record  */

    /********************************************************************/
    /* Close the scan.                                                  */
    /*                                                                  */
    /* No matter what state the scan is in now and no matter whether    */
    /* there is actually more record or not, close it. Do clean up      */
    /* as necessary.                                                    */
    /********************************************************************/
    virtual void
    closeScan(); 

    /********************************************************************/
    /* Print out the state of this RangeBufferManagerAccessMethod.      */
    /********************************************************************/
    void output(int verbose);
};
