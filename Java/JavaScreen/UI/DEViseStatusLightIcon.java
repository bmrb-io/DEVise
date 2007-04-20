// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2007
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// TEMP -- Need to write description.

// ------------------------------------------------------------------------

package JavaScreen.UI;

import javax.swing.*;

public final class DEViseStatusLightIcon extends IconImage {
	
	private Image onImage;
	private Image offImage;
	private Boolean status;
	
    public DEViseStatusLightIcon(Image[] images) {
        onImage = images[0];
		offImage = images [1];
		super(onIcon);
    }

/*	public setStatus(Boolean s) {
		if(s!=status) {
			if(status) {
				this.setImage(onImage);
				this.
			} else {
			
			}
		}
	}*/
}
