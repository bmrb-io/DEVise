// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 1999-2000
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// This class keeps track of user information (the JS supplies a username
// when it connects to the jspop).  Right now the only thing we are doing
// with this is allowing different usernames to have different priorities
// for getting connected to a devised.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.6  2000/03/23 16:26:15  wenger
// Cleaned up headers and added requests for comments.
//
// Revision 1.5  1999/06/23 20:59:17  wenger
// Added standard DEVise header.
//

// ========================================================================

// DEViseUser.java

import java.util.*;

public class DEViseUser
{
    private String username = null;
    private String password = null;
    private int priority;

    public DEViseUser(String[] data) throws YException
    {
        if (data == null) {
            throw new YException("Invalid user data: \"NULL\"");
        }

        if (data.length != 3) {
            throw new YException("Incorrect number of items in user data");
        }

        try {
            username = data[0];
            password = data[1];
            priority = Integer.parseInt(data[2]);
            if (priority < 1 || username == null || password == null) {
                throw new YException("Illegal value in user data");
	    }
        } catch (NumberFormatException e) {
            throw new YException("Incorrect data format in user data");
        }
    }

    public String getName()
    {
        return username;
    }

    public String getPassword()
    {
        return password;
    }

    public int getPriority()
    {
        return priority;
    }

    public boolean equals(DEViseUser user)
    {
        if (user == null) {
            return false;
	}

        if (username.equals(user.getName()) &&
	  password.equals(user.getPassword())) {
            return true;
        } else {
            return false;
        }
    }
}
