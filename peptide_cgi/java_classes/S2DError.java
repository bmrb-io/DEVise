// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2001
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// Error for Star2Devise.

// ------------------------------------------------------------------------

// $Id$

// $Log$

// ========================================================================

public class S2DError extends S2DException
{
    private static final int DEBUG = 0;

    public S2DError(String msg)
    {
        super(msg);

        if (DEBUG >= 1) {
            printStackTrace();
        }
    }
}

// ========================================================================

