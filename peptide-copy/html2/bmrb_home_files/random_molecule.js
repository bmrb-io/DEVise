/*
 * works with "blue_sidemenu.html"
 * displays a random picture (clickable link) of a "featured molecule".
 *
 * Requires jquery, should be included at the top of the file (want to
 * add this picture before the page's loaded).
 *
 * see /software/wishert/index.html for example
 */

$.get( "/featuredSys/random.php", function( data ) {
    var menu = $( "#leftcol" ).html();
    $( "#leftcol" ).html( data + menu );
} );
