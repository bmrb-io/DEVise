proc Puts { str } {
   global quiet
   if { $quiet == 0 } { puts $str }
}


