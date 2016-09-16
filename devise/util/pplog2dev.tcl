#! /s/std/bin/tclsh

#  This program reads MPE log files (Alog format) and PICL V1.0 log files
#  and produces Devise data files adhering to the MPELOG schema.

#  Major portions of this file were taken from the MPE upshot source
#  code distribution. A small set of modifications were done locally.

############################################################

#  $Id$

#  $Log$

############################################################

proc SetGlobalVars {} {
   global barWidth maxOverlap

   # with of state bars
   # for overlapping states, first 16 wide, then 10, then 4, and hold at 4
   set barWidth(0) 8
   set barWidth(1) 5
   set barWidth(2) 2
   set maxOverlap 2
}

#Might as well document the Alog format, while we're here-

#Each line:
#  type process task data cycle timestamp [comment]

#    type - nonnegative integer representing a user-defined event type
#    process - an integer representing the process in which the event occurred
#    task - an integer representing a different notion of task.  Usually 
#           ignored.
#    data - an integer representing user data for the event
#    cycle - an integer representing a time cycle, used to distinguish
#            between time returned by a timer that "rolls over" during
#            the run
#    timestamp - an integer representing (when considered in conjuction
#                with the cycle number) a time for the event.  Upshot treats
#                the units as microseconds
#    comment - an optional character string representing user data.  Currently
#              12 character maximum, will soon hopefully be any length (really!)

#Reserved types:

#  Type Proc Task Data          cycle Timestamp      Comment
#  ---- ---- ---- ----          ----- ---------      -------
#Creation data
#    -1                                              Creator and date

#Number of events in the logfile
#    -2           #events

#Number of processors in the run
#    -3           #procs

#Number of tasks used in the run
#    -4           #tasks

#Number of event types used
#    -5           #event types

#Start time of the run
#    -6                               start time

#End time of the run
#    -7                               end time

#Number of timer cycles
#    -8           #timer cycles

#Decription of event types
#    -9           event type                         Description

#printf string for event types
#    -10          event type                         printf string

#Rollover point
#    -11                              rollover point

#State definition
#    -13     start end                               color:bitmap State name

#Send message
#    -14     size receiver    tag

#Receive message
#    -15     size sender      tag


set DEViseColors \
	{{1000 XOR} {0 black} {1 white} {2 red} {3 blue} {4 orange} \
	{5 tan} {6 green} {7 purple} {8 aquamarine} {9 peru} {10 chocolate} \
	{11 tomato} {12 pink} {13 plum} {14 azure1} {15 cyan1} {16 SeaGreen1} \
	{17 khaki1} {18 goldenrod1} {19 yellow} {20 sienna1} \
	{21 LightCoral} {22 AntiqueWhite} {23 LemonChiffon} {24 LightGray}\
	{25 Lavender} {26 LavenderBlush} {27 MistyRose} {28 NavyBlue} \
	{29 SlateBlue} {30 MediumBlue} {31 DeepSkyBlue} {32 SkyBlue} \
	{33 red1} {34 red2} {35 DarkSeaGreen} {36 green2} {37 blue1} \
	{38 blue2} {39 blue3} {40 blue4} {41 gold1} {42 gold2}}

proc FindDeviseColor {color} {
   global DEViseColors

   foreach citem $DEViseColors {
      set cindex [lindex $citem 0]
      set cname [lindex $citem 1]
      if {[string tolower $cname] == [string tolower $color]} {
         return $cindex
       }
   }

   puts "Color $color is not available in Devise"
   return 0
}

set colorlist {red blue green cyan yellow magenta orange3 maroon \
       gray25 gray75 purple4 darkgreen white black}

proc PreprocessLog(alog) {id logfilename} {
   global setting colorNo

   set logFileHandle [open $logfilename r]
   set type -1
   set lineNo 0
   set colorNo 0
   set setting($id,states,list) {}
   set setting($id,startEvents,list) {}
   set setting($id,endEvents,list) {}

   set nbytes [gets $logFileHandle string]
   while {$nbytes>=0} {
      incr lineNo
      if {[scan $string "%d %d %d %d %d %lf" type proc task data cycle \
	    timestamp]!=6} {
	 LogFormatError $logfilename $string $lineNo
	 return -1
      } else {
	 case $type in {
	    -3		{set setting($id,numProcs)   $data}
	    -6		{set setting($id,firstTime)  $timestamp}
	    -7		{set setting($id,lastTime)   $timestamp}
	    -11		{set setting($id,rolloverPt) $timestamp}
	    -13         {Alog_StateDef $id $string $logfilename $lineNo}
	 }
	 if {[lsearch $setting($id,startEvents,list) $type] != -1} {
	    set setting($id,states,$setting($id,startEvents,$type),used) 1
	    # puts "$setting($id,startEvents,$type) used"
	 }
	 # case $type
      }
      # else !format error
      set nbytes [gets $logFileHandle string]
   }
   # while reading

   foreach stateName $setting($id,states,list) {
      if {$setting($id,states,$stateName,used) == 0} {
	 # puts "removing $stateName"
	 RemoveState $id $stateName
      } else {
	 unset setting($id,states,$stateName,used)
      }
   }
	    

   close $logFileHandle

   return 0
}


proc Alog_StateDef {id line logfilename lineNo} {
   global setting colorlist colorNo

   if {[scan $line "%*d %*d %d %d %*d %*d %s %\[^\n\]" startEvt \
	 endEvt coloring stateName]!=4} {
      LogFormatError $logfilename $line $lineNo
   } else {
      if {[info exists setting($id,states,list)] && \
	    [lsearch $setting($id,states,list) $stateName]>=0} {
	 return
      }
      # stateArray(stateName) = {start event, end event, color}
      set color [lindex [split $coloring :] 0]
      if {![string length $color]} {
	 set color [lindex $colorlist $colorNo]
	 incr colorNo
	 if $colorNo==[llength $colorlist] {
	    set colorNo 0
	 }
      }
      # set default color

      lappend setting($id,states,list) $stateName
      set setting($id,states,$stateName,start) $startEvt
      set setting($id,states,$stateName,end) $endEvt
      set setting($id,states,$stateName,color) $color
      set setting($id,states,$stateName,used) 0
      # per state settings

      lappend setting($id,startEvents,list) $startEvt
      set setting($id,startEvents,$startEvt) $stateName
      lappend setting($id,endEvents,list) $endEvt
      set setting($id,endEvents,$endEvt) $stateName
#      puts "State $stateName defined:\
#	    $setting($id,states,$stateName,start)\
#	    $setting($id,states,$stateName,end)\
#	    $setting($id,states,$stateName,color)\
#	    setting($id,startEvents,$startEvt)=\
#	    $setting($id,startEvents,$startEvt)\
#	    setting($id,endEvents,$endEvt)=\
#	    $setting($id,endEvents,$endEvt)"
	    
   }
}


proc ProcessLog(alog) {id logFileName} {
   global setting

   set log [open $logFileName r]

   set lineNo 0
   # line number

   set nbytes [gets $log line]
   while {$nbytes>=0} {
      incr lineNo
      
      if [scan $line "%d %d %d %d %d %lf" type proc task data cycle \
	    timestamp]!=6 {
	       LogFormatError $logFileName $line $lineNo
      } else {
	 if {$type>=0 || $type<-100} {
	    if $type==-101 {
	       scan $line "%*d %*d %*d %*d %*d %*lf %d %d" tag size
	       DrawSend $id $proc $data $tag $size \
		     [expr $timestamp+$cycle*$setting($id,rolloverPt)]
		     
	       # pass sender and receiver
	    } elseif $type==-102 {
	       scan $line "%*d %*d %*d %*d %*d %*lf %d %d" tag size
	       DrawRecv $id $proc $data $tag $size \
		     [expr $timestamp+$cycle*$setting($id,rolloverPt)]
	       # pass receiver and sender
	    } else {
	       GatherEvent $id $type $proc \
		     [expr $timestamp+$cycle*$setting($id,rolloverPt)]
	       # gather lone events into states
	    }
	    # check type number
	 }
	 # filter out nonpositive events
      }
      # if !formaterror
      set nbytes [gets $log line]
   }
   # while reading
   
   NoMoreEvents $id
   close $log
}


proc RemoveState {id stateName} {
   global setting

   set start $setting($id,states,$stateName,start)
   set end   $setting($id,states,$stateName,end)

   unset setting($id,endEvents,$end)
   unset setting($id,startEvents,$start)

   set idx [lsearch $setting($id,states,list) $stateName]
   # puts "removing $idx ($stateName) from $setting($id,states,list)"
   set setting($id,states,list) \
	 [lreplace $setting($id,states,list) $idx $idx]

   set idx [lsearch $setting($id,startEvents,list) $start]
   # puts "removing $idx ($stateName) from $setting($id,startEvents,list)"
   set setting($id,startEvents,list) \
	 [lreplace $setting($id,startEvents,list) $idx $idx]

   set idx [lsearch $setting($id,endEvents,list) $end]
   # puts "removing $idx ($stateName) from $setting($id,endEvents,list)"
   set setting($id,endEvents,list) \
	 [lreplace $setting($id,endEvents,list) $idx $idx]
   
   EraseArrayElements setting "$id,states,$stateName"
}	 


proc EraseArrayElements {array_name idx_header} {
   upvar $array_name a

   set pattern "^$idx_header,"
   foreach idx [array names a] {
      if [regexp $pattern $idx] {
	 unset a($idx)
      }
   }
}


proc PreprocessLog(picl) {id logfilename} {
   global setting compactList verboseList numTasks colorlist

   Picl_GetCompactList compactList verboseList
   # return array of the compact forms of record types, indexed by 
   # verbose name

   set tracefile [open $logfilename r]
   # open the tracefile

   global numTasks

   set maxTime 0
   set maxProc 0
   set nevents 0
   set lineNo 0
   set setting($id,hostProc) 0
   set setting($id,states,list) {}
   set setting($id,startEvents,list) {}
   set setting($id,endEvents,list) {}

   set nchars [gets $tracefile line]
   while {$nchars>=0} {
      incr lineNo

      set type [lindex $line 0]
      if [regexp "^\[a-zA-Z_\]*$" $type] {
	 set verbose 1
	 set type $compactList($type)
      } else {
	 set verbose 0
         if {$type < 0} {
             puts "I think $logfilename is in PICL 2.0 format."
             puts "Cannot handle that format yet."
             exit 1
         }
      }
      set clock [expr {$verbose?(1000000*[lindex $line 2]+[lindex $line 3]): \
	    1000000*[lindex $line 1]+[lindex $line 2]}]
      if $clock>$maxTime {
	 set maxTime $clock
      }
      set node [lindex $line [expr $verbose?5:3]]
      if $node>$maxProc {
	 set maxProc $node
      } elseif $node==-32768 {
	 set setting($id,hostProc) 1
      }
      if $type==4 {
	 Picl_AddState $id send
      } elseif $type==6 {
	 Picl_AddState $id recv
      } elseif $type==7 {
	 Picl_AddState $id recv_blocking
      } elseif $type==10 {
	 Picl_AddState $id sync
      }

      if {$type==20} {
	 set blockType [lindex $line [expr $verbose?7:4]]
	 if $blockType==-1 {
	    set stateName barrier
	 } elseif $blockType==-2 {
	    set stateName bcast0
	 } elseif $blockType==-3 {
	    set stateName bcast1
	 } elseif $blockType==-4 {
	    set stateName globalOp
	 } else {
	    set stateName state_[expr $blockType%$numTasks]
	 }
	 Picl_AddState $id $stateName
      }
      set nchars [gets $tracefile line]
   }

   set setting($id,states,list) [lsort $setting($id,states,list)]
   set i 0
   foreach stateName [lsort $setting($id,states,list)] {
      set setting($id,states,$stateName,color) \
	    [lindex $colorlist [expr $i%[llength $colorlist]]]
      set setting($id,states,$stateName,bitmap) \
	    [lindex $bitmaplist [expr $i%[llength $bitmaplist]]]
      incr i
   }
      
   set setting($id,numProcs) [expr $maxProc+$setting($id,hostProc)+1]
   # add one to the number of processes if a host is involved
   set setting($id,firstTime) 0
   set setting($id,lastTime) $maxTime
   set setting($id,rolloverPt) 0

   close $tracefile
}


proc ProcessLog(picl) {id logfilename} {
   global setting compactList verboseList numTasks

   set tracefile [open $logfilename r]
   set lineNo 0
   set status(0) 0
   set nchars [gets $tracefile line]
   while {$nchars>=0} {
      incr lineNo
      Picl_ExtractVerbose compactList line
      Picl_GetTypeTimeNode $id $line type time node
      #puts "$verboseList($type): $type, time: $time, node: $node, line: $line"

      Picl_ReadRecordData($verboseList($type)) $id $time $node status $line \
	    $lineNo
      set nchars [gets $tracefile line]
   }

   NoMoreEvents $id
   close $tracefile
}

proc Picl_GetCompactList {compactList_name verboseList_name} {
   upvar $compactList_name compactList
   upvar $verboseList_name verboseList

   set i 1
   foreach type {trace_start open load send probe recv recv_blocking \
	 recv_waking message sync compstats commstats close \
	 trace_level trace_mark trace_message trace_stop trace_flush \
	 trace_exit block_begin block_end trace_marks} {
      set compactList($type) $i
      set verboseList($i) $type
      incr i 
   }
}



proc Picl_ExtractVerbose {compactList_name line_name} {
   upvar $line_name line
   upvar $compactList_name compactList

   set type [lindex $line 0]
   if {[regexp "^\[a-zA-Z_\]*$" $type]} {
      set line [lreplace $line 0 0 $compactList($type)]
        # replace the verbose type
      lremove line {1 4}
        # remove 'clock' and 'node'

      case $type in {
	 {trace_start send recv recv_waking trace_level block_begin block_end}
	    {lremove line {4 6 8}}
	    # remove three verbose names

	 {recv_blocking trace_mark trace_flush trace_exit}
	    {lremove line 4}
	 # remove one verbose name

	 {open} {
	    if {[llength $line]!=4} {
	       lremove line {4 6}
	    }
	 }

	 {load} {
	    lremove line {4 5}
	 }
      }
   }
}


proc Picl_AddState {id name} {
   global bitmaplist colorList setting

   if ![info exists setting($id,states,max_id)] {
      set setting($id,states,max_id) 0
      set setting($id,states,list) ""
   }
   if [lsearch $setting($id,states,list) $name]>=0 {
      return
   }

   set state_id [expr $setting($id,states,max_id)+1]
   set start [expr $state_id*2]
   set end  [expr $state_id*2+1]
   lappend setting($id,states,list) $name

   set setting($id,states,$name,start) $start
   lappend setting($id,startEvents,list) $start
   set setting($id,startEvents,$start) $name

   set setting($id,states,$name,end) $end
   lappend setting($id,endEvents,list) $end
   set setting($id,endEvents,$end) $name
   # state id, start and end events

   incr setting($id,states,max_id)

#   puts "Add state $name: $setting($id,states,$name,start)\
#	 $setting($id,states,$name,end)"
}

proc Picl_GetTypeTimeNode {id line type_name time_name node_name} {
   upvar $type_name type
   upvar $time_name time
   upvar $node_name node
   global setting

   set type [lindex $line 0]
   set time [expr 1000000*[lindex $line 1]+[lindex $line 2]]
   set node [lindex $line 3]
   if $setting($id,hostProc) {
      incr node
      if $node==-32767 {
	 set node 0
      }
   }
}


foreach doNothingFn {trace_start open load probe message \
      commstats close message \
      trace_level trace_mark trace_message trace_stop trace_flush \
      trace_exit trace_marks} {
   proc Picl_ReadRecordData($doNothingFn) {id time node status_name \
	 line lineNo} {}
}


proc Picl_ReadRecordData(send) {id time node status_name line lineNo} {
   # Send events are short events in the PICL traceformat; they are not
   # explicitly given length.  You must check for a matching 'compstats'
   # event that may point out overhead involved in the send.  For example:
   #   Send at 5.3 sec.
   #   Compstat at 5.3 sec. reporting x idle time
   #   Compstat at 5.7 sec. reporting x+.4 idle time
   #
   # Kind of a bass-ackwards way of doing it, but oh well...
   # So, we want to remember that we just did a 'send', and if a 'compstat'
   # occurs, check if the time matches the last send (or recv or sync), if so,
   # the next compstat should tell up how much idle time we have there.

   global setting
   upvar $status_name status

   set status($node,mode) send
   set status($node,time) $time

   if {[llength $line] == 7} {
      set receiver [lindex $line 4]
      set type [lindex $line 5]
      set len [lindex $line 6]
   } else {
      puts "Error in format of line $lineNo:\n$line"
      return -1
      # error in the format
   }

   # record time, receiver, type, and length of message

   set status($node,send,receiver) $receiver
   set status($node,send,type) $type
   set status($node,send,len) $len
   GatherEvent $id $setting($id,states,send,start) $node $time
}
   

proc Picl_ReadRecordData(recv) {id time node status_name line lineNo} {
   # see comments for Picl_ReadRecordData(send)

   global setting
   upvar $status_name status

   set status($node,mode) recv
   set status($node,time) $time

   if {[llength $line] == 7} {
      set sender [lindex $line 4]
      set type [lindex $line 5]
      set len [lindex $line 6]
   } else {
      puts "Error in format of line $lineNo:\n$line"
      return -1
      # error in the format
   }

   # record time, sender, type, and length of message
   set status($node,recv,sender) $sender
   set status($node,recv,type) $type
   set status($node,recv,len) $len
   GatherEvent $id $setting($id,states,recv,start) $node $time
}
   

proc Picl_ReadRecordData(recv_blocking) {id time node status_name line \
      lineNo} {
   # see comments for Picl_ReadRecordData(send)
   global setting
   upvar $status_name status

   set status($node,mode) recv_blocking
   set status($node,time) $time

   GatherEvent $id $setting($id,states,recv_blocking,start) $node $time
}
   

proc Picl_ReadRecordData(recv_waking) {id time node status_name line lineNo} {
   # see comments for Picl_ReadRecordData(send)
   global setting
   upvar $status_name status

   if {[llength $line] == 7} {
      set sender [lindex $line 4]
      set type [lindex $line 5]
      set len [lindex $line 6]
   } else {
      puts "Error in format of line: $line"
      return -1
      # error in the format
   }

   # record time, sender, type, and length of message
   DrawRecv $id $node $sender $type $len $time
   GatherEvent $id $setting($id,states,recv_blocking,end) $node $time
}


proc Picl_ReadRecordData(block_begin) {id time node status_name line lineNo} {
   # see comments for ReadRecordData(send)
   global numTasks setting

   if {[llength $line] == 7} {
      set blockType [lindex $line 4]
   } else {
      puts "Error in format of line $lineNo:\n$line"
      return -1
      # error in the format
   }

   set stateNum [expr $blockType%$numTasks]

   if $blockType==-1 {
      set stateName barrier
   } elseif $blockType==-2 {
      set stateName bcast0
   } elseif $blockType==-3 {
      set stateName bcast1
   } elseif $blockType==-4 {
      set stateName globalOp
   } else {
      set stateName state_[expr $blockType%$numTasks]
   }
   
   GatherEvent $id $setting($id,states,$stateName,start) $node $time
}
   

proc Picl_ReadRecordData(block_end) {id time node status_name line lineNo} {
   # see comments for ReadRecordData(send)
   global numTasks setting

   if {[llength $line] == 7} {
      set blockType [lindex $line 4]
   } else {
      puts "Error in format of line $lineNo:\n$line"
      return -1
      # error in the format
   }

   set stateNum [expr $blockType%$numTasks]

   if $blockType==-1 {
      set stateName barrier
   } elseif $blockType==-2 {
      set stateName bcast0
   } elseif $blockType==-3 {
      set stateName bcast1
   } elseif $blockType==-4 {
      set stateName globalOp
   } else {
      set stateName state_[expr $blockType%$numTasks]
   }

   GatherEvent $id $setting($id,states,$stateName,end) $node $time
}
   

proc Picl_ReadRecordData(sync) {id time node status_name line lineNo} {
   # see comments for Picl_ReadRecordData(send)
   global setting
   upvar $status_name status

   set status($node,mode) sync
   set status($node,time) $time

   # record time
   GatherEvent $id $setting($id,states,sync,start) $node $time
}
   

proc Picl_ReadRecordData(compstats) {id time node status_name line lineNo} {
   # see comments for Picl_ReadRecordData(send)
   global setting
   upvar $status_name status
   
   if {![info exists status($node,mode)]} {
      # if this node has never been called, give it a resting status and
      # null mode
      set status($node,status) 0
      set status($node,mode) ""
   }

   if {[lsearch {send recv sync} $status($node,mode)]>-1} {
      # the node is in send/recv/sync mode
      if {$status($node,status)} {
	 # active status
	 GatherEvent $id $setting($id,states,$status($node,mode),end) $node \
	       $time
	 # end the state
	 if ![string compare $status($node,mode) send] {
	    DrawSend $id $node $status($node,send,receiver) \
		  $status($node,send,type) $status($node,send,len) $time
	 } elseif ![string compare $status($node,mode) recv] {
	    DrawRecv $id $node $status($node,recv,sender) \
		  $status($node,recv,type) $status($node,recv,len) $time
	 }

	 set status($node,mode) ""
	 set status($node,status) 0
	 # clear the status
      } elseif {$time==$status($node,time)} {
	 # this time matches that time in the mode, thus this compstat
	 # if meant to time this mode
	 set status($node,status) 1
	 # active status
      } else {
	 puts "compstat at $time does not match $status($node,mode) at \
	       $status($node,time)."
      }
   } else {
      # puts "compstat at $time is for what?"
   }
}


set gatherEvt(proclist) ""
proc GatherEvent {id type procnum time} {
   # Gather single events into states
   # one list is kept for each process (gatherEvt(0-x)), new start events
   # are added to the end; when removed, succeeding events slide down;
   # when drawn, the index is taken as the overlap level
   #
   # gatherEvt(proclist) - list of process numbers encountered
   # gatherEvt($procnum,nstates) - size of the list of states on each \
	 this process
   # gatherEvt($procnum,states,x) - list of states the process is in
   # gatherEvt($procnum,times,x) - times each of the current states started
   global setting gatherEvt

   # might need to check if the array has been created yet

   # puts "GatherEvent $id $type $procnum $time"

   set idx [lsearch $setting($id,startEvents,list) $type]
   if $idx==-1 {
      set idx [lsearch $setting($id,endEvents,list) $type]
      if $idx==-1 {
         puts "Don't know how to handle events yet"
         # DrawEvent $id $type $procnum $time
	 return
      } else {
	 set stateName $setting($id,endEvents,$type)
	 set isStartEvent 0
      }
   } else {
      set stateName $setting($id,startEvents,$type)
      set isStartEvent 1
   }
   # get the state name by checking start and end event lists
   if $isStartEvent {
      GatherAddState $procnum $stateName $time

#      puts "State added:"
#      for {set i 0} {$i<$gatherEvt($procnum,nstates)} {incr i} {
#	 puts "slot $i: $gatherEvt($procnum,states,$i)\
#	       $gatherEvt($procnum,times,$i)"
#      }

      # add to the list of states that need to be finished
   } else {
      set idx [GatherRemoveState $procnum $stateName]
      # state name is removed, but the time remains in $procnum,times,$idx
      if $idx==-1 {
	 puts "End state $stateName without start at $time"
      } else {
	 set startTime $gatherEvt($procnum,times,$idx)
         DrawTimeBar $id $stateName $startTime $time $procnum $idx
      }
   }
}


proc GatherAddState {procnum name time} {
   global gatherEvt

   if ![info exists gatherEvt($procnum,nstates)] {
      lappend gatherEvt(proclist) $procnum
      set gatherEvt($procnum,nstates) 1
      set gatherEvt($procnum,states,0) $name
      set gatherEvt($procnum,times,0) $time
   } else {
      for {set i 0} {$i<$gatherEvt($procnum,nstates)} {incr i} {
	 if ![string compare $gatherEvt($procnum,states,$i) ""] {
	    set gatherEvt($procnum,states,$i) $name
	    set gatherEvt($procnum,times,$i) $time
	    return
	 }
      }
      set i $gatherEvt($procnum,nstates)
      set gatherEvt($procnum,states,$i) $name
      set gatherEvt($procnum,times,$i) $time
      incr gatherEvt($procnum,nstates)
   }

#   puts "added $name to process $procnum:"
#   for {set i 0} {$i<$gatherEvt($procnum,nstates)} {incr i} {
#      puts "  $gatherEvt($procnum,states,$i)"
#   }

}


proc GatherRemoveState {procnum name} {
   # don't remove the time from $procnum,times,$idx
   global gatherEvt

   if ![info exists gatherEvt($procnum,nstates)] {
      return -1
   }
   set i [expr $gatherEvt($procnum,nstates)-1]
   if $i>=0 {
      if ![string compare $gatherEvt($procnum,states,$i) $name] {
	 incr gatherEvt($procnum,nstates) -1

#   puts "removed $name from process $procnum:"
#   for {set j 0} {$j<$gatherEvt($procnum,nstates)} {incr j} {
#      puts "  $gatherEvt($procnum,states,$j)"
#   }

	 return $i
      } else {
	 for {incr i -1} {$i>=0} {incr i -1} {
	    if ![string compare $gatherEvt($procnum,states,$i) $name] {
	       set gatherEvt($procnum,states,$i) ""

#   puts "removed $name from process $procnum:"
#   for {set j 0} {$j<$gatherEvt($procnum,nstates)} {incr j} {
#      puts "  $gatherEvt($procnum,states,$j)"
#   }

	       return $i
	    }
	 }
	 return -1
      }
   }
   return -1
}


proc NoMoreEvents {id} {
   # flush the events that may still be waiting to finish

   global setting gatherEvt

   # go through the list for each process
   if ![info exists gatherEvt(proclist)] return
   foreach procnum $gatherEvt(proclist) {
      # if it is in >0 states,
      if $gatherEvt($procnum,nstates) {
	 # check each of these states
	 for {set i [expr $gatherEvt($procnum,nstates)-1]} {$i>=0} \
	       {incr i -1} {
	    # if the state is non-null
	    if [string compare $gatherEvt($procnum,states,$i) ""] {
	       #DrawTimeBar $id $gatherEvt($procnum,states,$i) \
		     $gatherEvt($procnum,times,$i) \
		     $setting($id,lastTime) $procnum $i
	       WriteDataRecord $id $gatherEvt($procnum,states,$i) \
		     $gatherEvt($procnum,times,$i) \
		     $setting($id,lastTime) $procnum
	    }
	 }
      }
   }

   unset gatherEvt
}


proc DrawTimeBar {id stateName startTime endTime procNum idx} {
   global setting datafile

   set startTime [expr $startTime / 1000.0]
   set endTime [expr $endTime / 1000.0]
   set length [expr $endTime - $startTime]
   set color [FindDeviseColor $setting($id,states,$stateName,color)]

   puts $datafile "$procNum\t$startTime\t$length\t$stateName\t$color"
}


proc DrawSend {id sender receiver tag size time} {
   global recvWait sendWait

   #puts "DrawSend from $sender to $receiver tag $tag at $time"

   if [info exists recvWait($receiver,$sender,$tag)] {
      # if a queue exists on the receiver
      if [llength $recvWait($receiver,$sender,$tag)]>0 {
	 #puts "filledRightAway recvWait($receiver,$sender,$tag)=\
	       #$recvWait($receiver,$sender,$tag)"
	 # and the queue is not empty
	 set receiveTime [lindex $recvWait($receiver,$sender,$tag) 0]
	 # fill the first request
	 DrawArrow $id $sender $receiver $time $receiveTime $tag
	 set recvWait($receiver,$sender,$tag) \
	       [lrange $recvWait($receiver,$sender,$tag) 1 end]
	 return
      }
   }

   lappend sendWait($sender,$receiver,$tag) $time
   #puts "queued sendWait($sender,$receiver,$tag) =\
	# $sendWait($sender,$receiver,$tag)"
}


proc DrawRecv {id receiver sender tag size time} {
   global recvWait sendWait

   #puts "DrawRecv by $receiver from $sender tag $tag at $time"

   if [info exists sendWait($sender,$receiver,$tag)] {
      # if a queue exists on the sender
      if [llength $sendWait($sender,$receiver,$tag)]>0 {
	 #puts "filledRightAway sendWait($sender,$receiver,$tag) =\
	       #$sendWait($sender,$receiver,$tag)"
	 # and the queue is not empty
	 set sendTime [lindex $sendWait($sender,$receiver,$tag) 0]
	 # fill the first request
	 DrawArrow $id $sender $receiver $sendTime $time $tag
	 set sendWait($sender,$receiver,$tag) \
	       [lrange $sendWait($sender,$receiver,$tag) 1 end]
	 return
      }
   }

   lappend recvWait($receiver,$sender,$tag) $time
   #puts "queued recvWait($receiver,$sender,$tag) =\
	# $recvWait($receiver,$sender,$tag)"
}


proc DrawArrow {id sender receiver sendTime recvTime tag} {
   puts "Don't know how to handle this yet"
}


proc GuessFormat {filename} {

   if [regexp {.log$} $filename] {
      return alog
   } elseif [regexp {.trf$} $filename] {
      return picl
   } else {
      return alog
   }
}


proc LogFormatError {filename line lineNo} {
   puts "Logfile format error in line $lineNo of $filename:\n$line\n\n"
}


if {[llength $argv] < 2} {
    puts "Usage: $argv0 logfile datafile"
    exit 1
}

set logfilename [lindex $argv 0]
set logfileformat [GuessFormat $logfilename]

set datafile [open [lindex $argv 1] w]

SetGlobalVars
puts "Preprocessing..."
PreprocessLog($logfileformat) 1 $logfilename
puts "Processing..."
ProcessLog($logfileformat) 1 $logfilename

close $datafile
