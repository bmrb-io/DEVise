/*
 * works with "blue_sidemenu.html" and some other pages.
 * Expand/collapse chunks of the page, also highlight the 
 * header of a chunk to show it's clickable.
 *
 * see /software/wishart/index.html for example.
 *
 * Requires jquery.
 */
$( document ).ready( function() {

    $( ".expandingBody" ).hide();

    $( ".expandingHeader" ).click( function() {
        if( $( this ).hasClass( "Selected" ) ) {
            $( this ).removeClass( "Selected" );
            $( this ).addClass( "NotSelected" );
        }
        else {
            $( this ).removeClass( "NotSelected" );
            $( this ).addClass( "Selected" );
        }
        $( this ).next( ".expandingBody" ).toggle();
    } );

    $( ".progname" ).hover( function() {
        $( this ).toggleClass( "progname_hover" );
    } );

   $( ".hiliteonhover" ).hover( function() {
        if( ! $( this ).hasClass( "highlight_hover" ) )
    	    $( this ).addClass( "highlight_hover" );
    }, function() {
        if( $( this ).hasClass( "highlight_hover" ) )
    	    $( this ).removeClass( "highlight_hover" );
    } );

} );
