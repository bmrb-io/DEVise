
This is not for yacc, but rather a grammar to attempt to
formalize what's acceptable.

    <e> is epsilon, which means nothing.
    <symbol> is some standard identifier.
    <inum> is an integer
    <rnum> is a real
    <date> is some date format(s)
    unbracketed items are terminals.

<schema> :- schema <schema_name> { <schema_part_list> }

<schema_part_list> :- <e>
                   |  <schema_part> <schema_part_list>

<schema_part> :-
             <schema_stmnt>
          |  <enum_defn>
          |  <type_defn>
          |  <attr_defn>

<schema_stmnt> :- 
        type = <schema_type> ;
    |   char_set = <char_set> ;
    |   binary_order = <bin_order> ;
    |   recsize = <inum> ;
    |   comment = <string_or_pattern> ;
    |   comment = <string_or_pattern> .. <string_or_pattern> ;
    |   separator = <string_or_pattern> ;
    |   delimiter = <string_or_pattern> ;
    |   converter = <string> ;
    |   format = <pattern> ;
    |   filter = <perl_code> ;
    |   reader = <perl_code> ;
    |   global = <perl_code> ;

<attr_defn> :- attr <attr_name> { <attr_stmnt_list> }
<type_defn> :- typedef <type_name> { <attr_stmnt_list> }
<enum_defn> :- enum <type_name> { <enum_line_list> }

<attr_stmnt_list> :- <attr_stmnt>
                  |  <attr_stmnt> <attr_stmnt_list>

<enum_line_list> :- <enum_line>
                 |  <enum_line> <enum_line_list>

<enum_line>  :- <symbol> ;
             |  <symbol> = <const_value> ;
             |  <symbol> = <pattern> ;
             
<attr_stmnt> :-
        <attr_defn>
    |   type = <attr_type> ;
    |   <trait> ;
    |   min = <const_value> ;
    |   max = <const_value> ;
    |   maxlen = <inum> ;
    |   position = <range> ;
    |   format = <pattern> ;
    |   function_of = <attr_list> ;
    |   value  = <perl_code> ;
    |   filter = <perl_code> ;
    |   reader = <perl_code> ;

<perl_code> :-  <perl_expr>
            |   {  <any_perl_code> }

<range> :-
    [ <inum> ]             # A single column
    [ <inum> .. ]          # Only know starting column
    [ <inum> .. <inum> ]   
    [        .. <inum> ]   # Only know ending column

<attr_list> :-
              <attr_name>
            | <attr_name> , <attr_list>

<string_or_pattern> :-  <string>
                    |   <pattern>

<const_value> :- <inum>
              |  <rnum>
              |  <string>
              |  <date>

<schema_type> :- text
              |  binary

<char_set> :- ascii
   
<bin_order> :- native
            |  network

<trait> :- sorted
        |  unique
        |  consecutive
        |  null_allowed
 
<schema_name> :- <symbol>
<attr_name> :- <symbol> 
<type_name> :- <symbol> 

