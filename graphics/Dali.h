

/*****************************************************************************************
**                        Dali: Display Arbitrarily Large Images                        **
**                                                                                      **
** Clients can use the function "DaliPatron" to connect to remote servers. They can     **
** then send image display commands on the connection. Must get a new connection for    **
** each command.                                                                        **
**                                                                                      **
**All commands and return messages should/will have a trailing \n character.            **
**All commands and return messages should/will be less than DALI_MAX_STR_LENGTH         **
**characters long.                                                                      **
**Commands accepted:                                                                    **
**                                                                                      **
**1. show <imagefile> <xwindow> <topleftx> <toplefty> <botrightx> <botrighty> [options] **
**   * <imagefile> can be -, meaning image data will also be sent on the connection     **
**   * <xwindow> can be - or 0, implying root window of screen. Otherwise, <xwindow>    **
**     has to be the X Window identifier in hexadecimal for a valid X window.           **
**   * Within the <xwindow>, the image (after shrinking if necessary) will be           **
**     displayed in the rectangle specified by the coordinates.                         **
**   * Dali will return a string, in one of these two formats:                          **
**     1. OK <handle> <num-colors-used>                                                 **
**     2. ERROR <error-message>                                                         **
**     In the first case, everything went well. <handle> will be an integer             **
**     in decimal format, which is your unique identifier for the displayed             ** 
**     image, and should be used in the "free" command described later.                 **
**     <num-colors-used> is the number of colors used in displaying the image,          **
**     again a decimal integer.                                                         **
**     In case of ERROR, the error-message will describe what went wrong.               **
**   * options can be:                                                                  **
**     -refresh   Dali will get Exposure events for the window where it                 **
**                displays the image (a subwindow of <xwindow>) and                     **
**                redisplay the image. If this is used, then Dali will be               **
**      	  using up some extra memory to store the ximage,                       **
**      	  which will be freed with the "free" command. By default,              **
**      	  Dali will not manage refreshing.                                      **
**     -xbacking  If the Xserver supports backing stores, then Dali will ask X to       **
**                retain copies of window images. Expose events will not be generated   **
**                and Dali will be ignoring any -refresh option. Dali will also         **
**                not have to retain image date after displaying. The X reference       **
**                says that using backing stores imposes big penalties on server        **
**                performance.. if this seems to be the case then you'll be better      **
**                off using -refresh. If you want backing under any circs, you can:     **
**                  a) Use the command "xbacking" which will return OK or ERROR to      **
**                     find out if X can do backing, and then use -xbacking or -refresh **
**                     appropriately.                                                   **
**                  b) Simply use both -xbacking and -refresh options. X backing will   **
**                     be tried first, if that fails, then Dali would backup images.    **
**     -quitonq   Dali will get KeyPress events for the window displaying the           **
**      	  image, and will automatically call "free" when 'q' is                 **
**      	  pressed in the window. By default, Dali will not intercept            **
**      	  any KeyPress.                                                         **
**     (Note that with these two options, your <xwindow> may not get KeyPress           **
**      and Exposure XEvents.. if you need these events, then don't use the             **
**      -refresh and/or -quitonq options, but call Dali yourself to redisplay           **
**      the image using a fresh "show" command, and/or call Dali to "free".)            **
**                                                                                      **
**     -raw <W> <H> <n> Indicates to Dali that the image data has no headers.           **
**             <W> is the width, <H> is the height, and <n> is the number of            **
**             components (1 for gray, 3 for RGB images). The image data should be      **
**             organized in row-major order, one byte per pixel. For RGB images (n=3),  **
**             R values of all pixels should come before G of all, followed by B.       **
**     -maxcols <mc> Tell dali to use at most <mc> colors when displaying the image.    **
**      	     Default is 256.                                                    **
**     -ccube <c> Used only for color images (non-grayscale). Dali will allocate        **
**                colors using <c> bits only from each of R,G, and B. Default is 4      **
**                5 is a good value, 6 the best.                                        **
**     -dither    Do Floyd-Steinberg dithering. Very useful, especially when using      **
**                a small number of colors                                              **
**     -quickreturn Dali will respond immediately, after checking the command           **
**                syntax, and reading the image (only if the image is being sent        **
**                on the connection too, with "-" as file name). The return message     **
**                will again be either ERROR <error_mesg>, or OK <handle> <num_colors>, **
**                but in the latter case, <num_colors> may be arbitrary, as the         **
**                return will occur before color allocation. To really find out if      **
**                if the image was displayed properly, use a "query <handle>"           **
**                command subsequently.                                                 **
**                                                                                      **
**2. query <handle>  Find if a handle is valid. Return message will either be           **
**                   OK <num-colors-used>, or ERROR <error msg>. This is useful         **
**                   for verifying if a "show" with "-quickreturn" went all right.      **
**                                                                                      **
**3. free <handle>                                                                      **
**   * all storage associated with the displayed image will be free'd. If -refresh      **
**     was not used in "show", then this only involves free'ing the XColors used,       **
**     otherwise the XImage for the window and some auxiliary data-structs that         **
**     were used up by "show" will also be free'd.                                      **
**   * Dali will return a string, in one of these two formats:                          **
**     1. OK                                                                            **
**     2. ERROR <error-message>                                                         **
**                                                                                      **
**The following commands need not be used by applications.                              **
**                                                                                      **
**4. quit                                                                               **
**   * Terminate Dali: reply will be "OK"                                               **
**                                                                                      **
**5. logfile <logfilename>                                                              **
**   * Previous logfile will be closed and a new one started. On                        **
**     start-up, Dali starts with stderr as its logfile. Reply will be "OK" or          **
**     "ERROR error-msg"                                                                **
**                                                                                      **
**6. reset                                                                              **
**   * free all active handle: reply will be "OK"                                       **
**                                                                                      **
**7. status                                                                             **
**   * returns "OK <num-images> <num-colormaps>"                                        **
**                                                                                      **
**8. xbacking                                                                           **
**   * returns OK if the X Display of the Dali server allows backing stores for X       **
**     windows, else it returns ERROR. Useful to decide if the -xbacking or -refresh    **
**     option should be used in "show".                                                 **
**                                                                                      **
**                                                                                      **
*****************************************************************************************/

#define DALI_MAX_STR_LENGTH 200
#define DALI_PORT 6600

/***********************************************************
**  Dali client: opens a connection to the Dali server on **
**  the server "servName" and returns a file descriptor.  **
**  In case of errors, it will return a negative value,   **
**  and a brief description of the error in "errmsg".     **
**  errmsg must point to a buffer of DALI_MAX_STR_LENGTH  **
**  bytes.                                                **
***********************************************************/

extern int DaliPatron(char *servName, char *errmsg, int port = DALI_PORT);

