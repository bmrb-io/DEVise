# Microsoft Developer Studio Generated NMAKE File, Based on dte.dsp
!IF "$(CFG)" == ""
CFG=dte - Win32 Debug
!MESSAGE No configuration specified. Defaulting to dte - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "dte - Win32 Release" && "$(CFG)" != "dte - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "dte.mak" CFG="dte - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "dte - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "dte - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "dte - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\dte.exe"

!ELSE 

ALL : "$(OUTDIR)\dte.exe"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\Aggregates.obj"
	-@erase "$(INTDIR)\catalog.obj"
	-@erase "$(INTDIR)\Common.obj"
	-@erase "$(INTDIR)\DateTime.obj"
	-@erase "$(INTDIR)\DeleteParse.obj"
	-@erase "$(INTDIR)\DevRead.dummy.obj"
	-@erase "$(INTDIR)\DropIndexParse.obj"
	-@erase "$(INTDIR)\Engine.obj"
	-@erase "$(INTDIR)\ExecOp.obj"
	-@erase "$(INTDIR)\GetHandle.obj"
	-@erase "$(INTDIR)\IndexParse.dummy.obj"
	-@erase "$(INTDIR)\Inserter.obj"
	-@erase "$(INTDIR)\InsertParse.obj"
	-@erase "$(INTDIR)\Interface.obj"
	-@erase "$(INTDIR)\joins.obj"
	-@erase "$(INTDIR)\lex.yy.obj"
	-@erase "$(INTDIR)\listop.obj"
	-@erase "$(INTDIR)\main.obj"
	-@erase "$(INTDIR)\MaterializeParse.obj"
	-@erase "$(INTDIR)\MemoryMgr.obj"
	-@erase "$(INTDIR)\MinMax.obj"
	-@erase "$(INTDIR)\my.yacc.tab.obj"
	-@erase "$(INTDIR)\ODBC.obj"
	-@erase "$(INTDIR)\ODBCInterface.obj"
	-@erase "$(INTDIR)\ODBCIterator.obj"
	-@erase "$(INTDIR)\ODBCSite.obj"
	-@erase "$(INTDIR)\PQueue.obj"
	-@erase "$(INTDIR)\QueryTree.obj"
	-@erase "$(INTDIR)\resolve.obj"
	-@erase "$(INTDIR)\RTreeRead.dummy.obj"
	-@erase "$(INTDIR)\SBMInit.dummy.obj"
	-@erase "$(INTDIR)\SchemaParse.obj"
	-@erase "$(INTDIR)\site.obj"
	-@erase "$(INTDIR)\SockStream.obj"
	-@erase "$(INTDIR)\Sort.obj"
	-@erase "$(INTDIR)\StandardRead.obj"
	-@erase "$(INTDIR)\TypeCheck.obj"
	-@erase "$(INTDIR)\types.obj"
	-@erase "$(INTDIR)\UnionParse.obj"
	-@erase "$(INTDIR)\url.obj"
	-@erase "$(INTDIR)\Util.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\WSInit.obj"
	-@erase "$(OUTDIR)\dte.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D\
 "_MBCS" /Fp"$(INTDIR)\dte.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Release/
CPP_SBRS=.

.c{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\dte.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:console /incremental:no\
 /pdb:"$(OUTDIR)\dte.pdb" /machine:I386 /out:"$(OUTDIR)\dte.exe" 
LINK32_OBJS= \
	"$(INTDIR)\Aggregates.obj" \
	"$(INTDIR)\catalog.obj" \
	"$(INTDIR)\Common.obj" \
	"$(INTDIR)\DateTime.obj" \
	"$(INTDIR)\DeleteParse.obj" \
	"$(INTDIR)\DevRead.dummy.obj" \
	"$(INTDIR)\DropIndexParse.obj" \
	"$(INTDIR)\Engine.obj" \
	"$(INTDIR)\ExecOp.obj" \
	"$(INTDIR)\GetHandle.obj" \
	"$(INTDIR)\IndexParse.dummy.obj" \
	"$(INTDIR)\Inserter.obj" \
	"$(INTDIR)\InsertParse.obj" \
	"$(INTDIR)\Interface.obj" \
	"$(INTDIR)\joins.obj" \
	"$(INTDIR)\lex.yy.obj" \
	"$(INTDIR)\listop.obj" \
	"$(INTDIR)\main.obj" \
	"$(INTDIR)\MaterializeParse.obj" \
	"$(INTDIR)\MemoryMgr.obj" \
	"$(INTDIR)\MinMax.obj" \
	"$(INTDIR)\my.yacc.tab.obj" \
	"$(INTDIR)\ODBC.obj" \
	"$(INTDIR)\ODBCInterface.obj" \
	"$(INTDIR)\ODBCIterator.obj" \
	"$(INTDIR)\ODBCSite.obj" \
	"$(INTDIR)\PQueue.obj" \
	"$(INTDIR)\QueryTree.obj" \
	"$(INTDIR)\resolve.obj" \
	"$(INTDIR)\RTreeRead.dummy.obj" \
	"$(INTDIR)\SBMInit.dummy.obj" \
	"$(INTDIR)\SchemaParse.obj" \
	"$(INTDIR)\site.obj" \
	"$(INTDIR)\SockStream.obj" \
	"$(INTDIR)\Sort.obj" \
	"$(INTDIR)\StandardRead.obj" \
	"$(INTDIR)\TypeCheck.obj" \
	"$(INTDIR)\types.obj" \
	"$(INTDIR)\UnionParse.obj" \
	"$(INTDIR)\url.obj" \
	"$(INTDIR)\Util.obj" \
	"$(INTDIR)\WSInit.obj"

"$(OUTDIR)\dte.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "dte - Win32 Debug"

OUTDIR=C:\Program Files\DevStudio\MyProjects\DTE\normal
INTDIR=C:\Program Files\DevStudio\MyProjects\DTE\normal
# Begin Custom Macros
OutDir=C:\Program Files\DevStudio\MyProjects\DTE\normal
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\dte.exe"

!ELSE 

ALL : "$(OUTDIR)\dte.exe"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\Aggregates.obj"
	-@erase "$(INTDIR)\catalog.obj"
	-@erase "$(INTDIR)\Common.obj"
	-@erase "$(INTDIR)\DateTime.obj"
	-@erase "$(INTDIR)\DeleteParse.obj"
	-@erase "$(INTDIR)\DevRead.dummy.obj"
	-@erase "$(INTDIR)\DropIndexParse.obj"
	-@erase "$(INTDIR)\Engine.obj"
	-@erase "$(INTDIR)\ExecOp.obj"
	-@erase "$(INTDIR)\GetHandle.obj"
	-@erase "$(INTDIR)\IndexParse.dummy.obj"
	-@erase "$(INTDIR)\Inserter.obj"
	-@erase "$(INTDIR)\InsertParse.obj"
	-@erase "$(INTDIR)\Interface.obj"
	-@erase "$(INTDIR)\joins.obj"
	-@erase "$(INTDIR)\lex.yy.obj"
	-@erase "$(INTDIR)\listop.obj"
	-@erase "$(INTDIR)\main.obj"
	-@erase "$(INTDIR)\MaterializeParse.obj"
	-@erase "$(INTDIR)\MemoryMgr.obj"
	-@erase "$(INTDIR)\MinMax.obj"
	-@erase "$(INTDIR)\my.yacc.tab.obj"
	-@erase "$(INTDIR)\ODBC.obj"
	-@erase "$(INTDIR)\ODBCInterface.obj"
	-@erase "$(INTDIR)\ODBCIterator.obj"
	-@erase "$(INTDIR)\ODBCSite.obj"
	-@erase "$(INTDIR)\PQueue.obj"
	-@erase "$(INTDIR)\QueryTree.obj"
	-@erase "$(INTDIR)\resolve.obj"
	-@erase "$(INTDIR)\RTreeRead.dummy.obj"
	-@erase "$(INTDIR)\SBMInit.dummy.obj"
	-@erase "$(INTDIR)\SchemaParse.obj"
	-@erase "$(INTDIR)\site.obj"
	-@erase "$(INTDIR)\SockStream.obj"
	-@erase "$(INTDIR)\Sort.obj"
	-@erase "$(INTDIR)\StandardRead.obj"
	-@erase "$(INTDIR)\TypeCheck.obj"
	-@erase "$(INTDIR)\types.obj"
	-@erase "$(INTDIR)\UnionParse.obj"
	-@erase "$(INTDIR)\url.obj"
	-@erase "$(INTDIR)\Util.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\WSInit.obj"
	-@erase "$(OUTDIR)\dte.exe"
	-@erase "$(OUTDIR)\dte.ilk"
	-@erase "$(OUTDIR)\dte.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MLd /W3 /GX /Od /I "P:\devise\okan\DTE\Core" /I\
 "P:\devise\okan\dte\StandAlone" /I "P:\devise\okan\unidata" /I "." /I\
 "C:\Program Files\DevStudio\MyProjects\DTE" /D "WIN32" /D "_DEBUG" /D\
 "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\dte.pch" /YX /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /TP /pdb /c 
CPP_OBJS="C:\Program Files\DevStudio\MyProjects\DTE\normal/"
CPP_SBRS=.

.c{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"dte.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib ws2_32.lib /nologo /subsystem:console /incremental:yes\
 /pdb:"$(OUTDIR)\dte.pdb" /debug /machine:I386 /out:"$(OUTDIR)\dte.exe"\
 /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\Aggregates.obj" \
	"$(INTDIR)\catalog.obj" \
	"$(INTDIR)\Common.obj" \
	"$(INTDIR)\DateTime.obj" \
	"$(INTDIR)\DeleteParse.obj" \
	"$(INTDIR)\DevRead.dummy.obj" \
	"$(INTDIR)\DropIndexParse.obj" \
	"$(INTDIR)\Engine.obj" \
	"$(INTDIR)\ExecOp.obj" \
	"$(INTDIR)\GetHandle.obj" \
	"$(INTDIR)\IndexParse.dummy.obj" \
	"$(INTDIR)\Inserter.obj" \
	"$(INTDIR)\InsertParse.obj" \
	"$(INTDIR)\Interface.obj" \
	"$(INTDIR)\joins.obj" \
	"$(INTDIR)\lex.yy.obj" \
	"$(INTDIR)\listop.obj" \
	"$(INTDIR)\main.obj" \
	"$(INTDIR)\MaterializeParse.obj" \
	"$(INTDIR)\MemoryMgr.obj" \
	"$(INTDIR)\MinMax.obj" \
	"$(INTDIR)\my.yacc.tab.obj" \
	"$(INTDIR)\ODBC.obj" \
	"$(INTDIR)\ODBCInterface.obj" \
	"$(INTDIR)\ODBCIterator.obj" \
	"$(INTDIR)\ODBCSite.obj" \
	"$(INTDIR)\PQueue.obj" \
	"$(INTDIR)\QueryTree.obj" \
	"$(INTDIR)\resolve.obj" \
	"$(INTDIR)\RTreeRead.dummy.obj" \
	"$(INTDIR)\SBMInit.dummy.obj" \
	"$(INTDIR)\SchemaParse.obj" \
	"$(INTDIR)\site.obj" \
	"$(INTDIR)\SockStream.obj" \
	"$(INTDIR)\Sort.obj" \
	"$(INTDIR)\StandardRead.obj" \
	"$(INTDIR)\TypeCheck.obj" \
	"$(INTDIR)\types.obj" \
	"$(INTDIR)\UnionParse.obj" \
	"$(INTDIR)\url.obj" \
	"$(INTDIR)\Util.obj" \
	"$(INTDIR)\WSInit.obj"

"$(OUTDIR)\dte.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(CFG)" == "dte - Win32 Release" || "$(CFG)" == "dte - Win32 Debug"
SOURCE=..\..\DTE\Core\Aggregates.c

!IF  "$(CFG)" == "dte - Win32 Release"

DEP_CPP_AGGRE=\
	"..\..\DTE\Core\Aggregates.h"\
	"..\..\DTE\Core\Array.h"\
	"..\..\DTE\Core\exception.h"\
	"..\..\DTE\Core\Iterator.h"\
	"..\..\DTE\Core\listop.h"\
	"..\..\DTE\Core\MemoryMgr.h"\
	"..\..\DTE\Core\myopt.h"\
	"..\..\DTE\Core\queue.h"\
	"..\..\DTE\Core\site.h"\
	"..\..\DTE\Core\SockStream.h"\
	"..\..\DTE\Core\StringLess.h"\
	"..\..\DTE\Core\TypeCheck.h"\
	"..\..\DTE\Core\url.h"\
	
NODEP_CPP_AGGRE=\
	"..\..\DTE\Core\sysdep.h"\
	

"$(INTDIR)\Aggregates.obj" : $(SOURCE) $(DEP_CPP_AGGRE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "dte - Win32 Debug"

DEP_CPP_AGGRE=\
	"..\..\DTE\Core\Aggregates.h"\
	"..\..\DTE\Core\Array.h"\
	"..\..\DTE\Core\exception.h"\
	"..\..\DTE\Core\Iterator.h"\
	"..\..\DTE\Core\listop.h"\
	"..\..\DTE\Core\MemoryMgr.h"\
	"..\..\DTE\Core\myopt.h"\
	"..\..\DTE\Core\queue.h"\
	"..\..\DTE\Core\site.h"\
	"..\..\DTE\Core\SockStream.h"\
	"..\..\DTE\Core\StringLess.h"\
	"..\..\DTE\Core\TypeCheck.h"\
	"..\..\DTE\Core\url.h"\
	"..\..\unidata\sysdep.h"\
	

"$(INTDIR)\Aggregates.obj" : $(SOURCE) $(DEP_CPP_AGGRE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\DTE\Core\catalog.c

!IF  "$(CFG)" == "dte - Win32 Release"

DEP_CPP_CATAL=\
	"..\..\DTE\Core\Array.h"\
	"..\..\DTE\Core\catalog.h"\
	"..\..\DTE\Core\exception.h"\
	"..\..\DTE\Core\Inserter.h"\
	"..\..\DTE\Core\Interface.h"\
	"..\..\DTE\Core\Iterator.h"\
	"..\..\DTE\Core\listop.h"\
	"..\..\DTE\Core\myopt.h"\
	"..\..\DTE\Core\queue.h"\
	"..\..\DTE\Core\site.h"\
	"..\..\DTE\Core\SockStream.h"\
	"..\..\DTE\Core\StandardRead.h"\
	"..\..\DTE\Core\url.h"\
	"..\..\DTE\Core\Utility.h"\
	
NODEP_CPP_CATAL=\
	"..\..\DTE\Core\sysdep.h"\
	

"$(INTDIR)\catalog.obj" : $(SOURCE) $(DEP_CPP_CATAL) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "dte - Win32 Debug"

DEP_CPP_CATAL=\
	"..\..\DTE\Core\Array.h"\
	"..\..\DTE\Core\catalog.h"\
	"..\..\DTE\Core\exception.h"\
	"..\..\DTE\Core\Inserter.h"\
	"..\..\DTE\Core\Interface.h"\
	"..\..\DTE\Core\Iterator.h"\
	"..\..\DTE\Core\listop.h"\
	"..\..\DTE\Core\myopt.h"\
	"..\..\DTE\Core\queue.h"\
	"..\..\DTE\Core\site.h"\
	"..\..\DTE\Core\SockStream.h"\
	"..\..\DTE\Core\StandardRead.h"\
	"..\..\DTE\Core\url.h"\
	"..\..\DTE\Core\Utility.h"\
	"..\..\unidata\sysdep.h"\
	

"$(INTDIR)\catalog.obj" : $(SOURCE) $(DEP_CPP_CATAL) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\DTE\StandAlone\Common.cpp

!IF  "$(CFG)" == "dte - Win32 Release"

NODEP_CPP_COMMO=\
	"..\..\DTE\StandAlone\Engine.h"\
	"..\..\DTE\StandAlone\exception.h"\
	"..\..\DTE\StandAlone\sysdep.h"\
	

"$(INTDIR)\Common.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "dte - Win32 Debug"

DEP_CPP_COMMO=\
	"..\..\DTE\Core\Engine.h"\
	"..\..\DTE\Core\exception.h"\
	"..\..\unidata\sysdep.h"\
	

"$(INTDIR)\Common.obj" : $(SOURCE) $(DEP_CPP_COMMO) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\unidata\DateTime.C

!IF  "$(CFG)" == "dte - Win32 Release"

DEP_CPP_DATET=\
	"..\..\unidata\DateTime.h"\
	"..\..\unidata\sysdep.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_DATET=\
	"..\..\unidata\HTVMSUtils.h"\
	"..\..\unidata\windows\config.h"\
	

"$(INTDIR)\DateTime.obj" : $(SOURCE) $(DEP_CPP_DATET) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "dte - Win32 Debug"

DEP_CPP_DATET=\
	"..\..\unidata\config.h"\
	"..\..\unidata\DateTime.h"\
	"..\..\unidata\sysdep.h"\
	"C:\Program Files\DevStudio\MyProjects\DTE\unistd.h"\
	"C:\Program Files\DevStudio\MyProjects\DTE\windows\config.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_DATET=\
	"..\..\unidata\HTVMSUtils.h"\
	

"$(INTDIR)\DateTime.obj" : $(SOURCE) $(DEP_CPP_DATET) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\DTE\Core\DeleteParse.c

!IF  "$(CFG)" == "dte - Win32 Release"

DEP_CPP_DELET=\
	"..\..\DTE\Core\Array.h"\
	"..\..\DTE\Core\catalog.h"\
	"..\..\DTE\Core\exception.h"\
	"..\..\DTE\Core\ExecExpr.h"\
	"..\..\DTE\Core\Iterator.h"\
	"..\..\DTE\Core\listop.h"\
	"..\..\DTE\Core\MemoryMgr.h"\
	"..\..\DTE\Core\myopt.h"\
	"..\..\DTE\Core\ParseTree.h"\
	"..\..\DTE\Core\queue.h"\
	"..\..\DTE\Core\site.h"\
	"..\..\DTE\Core\SockStream.h"\
	"..\..\DTE\Core\StringLess.h"\
	"..\..\DTE\Core\TypeCheck.h"\
	"..\..\DTE\Core\url.h"\
	"..\..\DTE\Core\Utility.h"\
	
NODEP_CPP_DELET=\
	"..\..\DTE\Core\sysdep.h"\
	

"$(INTDIR)\DeleteParse.obj" : $(SOURCE) $(DEP_CPP_DELET) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "dte - Win32 Debug"

DEP_CPP_DELET=\
	"..\..\DTE\Core\Array.h"\
	"..\..\DTE\Core\catalog.h"\
	"..\..\DTE\Core\exception.h"\
	"..\..\DTE\Core\ExecExpr.h"\
	"..\..\DTE\Core\Iterator.h"\
	"..\..\DTE\Core\listop.h"\
	"..\..\DTE\Core\MemoryMgr.h"\
	"..\..\DTE\Core\myopt.h"\
	"..\..\DTE\Core\ParseTree.h"\
	"..\..\DTE\Core\queue.h"\
	"..\..\DTE\Core\site.h"\
	"..\..\DTE\Core\SockStream.h"\
	"..\..\DTE\Core\StringLess.h"\
	"..\..\DTE\Core\TypeCheck.h"\
	"..\..\DTE\Core\url.h"\
	"..\..\DTE\Core\Utility.h"\
	"..\..\unidata\sysdep.h"\
	

"$(INTDIR)\DeleteParse.obj" : $(SOURCE) $(DEP_CPP_DELET) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\DTE\Core\DevRead.dummy.c

!IF  "$(CFG)" == "dte - Win32 Release"

DEP_CPP_DEVRE=\
	"..\..\DTE\Core\DevRead.h"\
	"..\..\DTE\Core\Iterator.h"\
	
NODEP_CPP_DEVRE=\
	"..\..\DTE\Core\sysdep.h"\
	

"$(INTDIR)\DevRead.dummy.obj" : $(SOURCE) $(DEP_CPP_DEVRE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "dte - Win32 Debug"

DEP_CPP_DEVRE=\
	"..\..\DTE\Core\DevRead.h"\
	"..\..\DTE\Core\Iterator.h"\
	"..\..\unidata\sysdep.h"\
	

"$(INTDIR)\DevRead.dummy.obj" : $(SOURCE) $(DEP_CPP_DEVRE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\DTE\Core\DropIndexParse.c

!IF  "$(CFG)" == "dte - Win32 Release"

DEP_CPP_DROPI=\
	"..\..\DTE\Core\Array.h"\
	"..\..\DTE\Core\Engine.h"\
	"..\..\DTE\Core\exception.h"\
	"..\..\DTE\Core\Iterator.h"\
	"..\..\DTE\Core\listop.h"\
	"..\..\DTE\Core\myopt.h"\
	"..\..\DTE\Core\ParseTree.h"\
	"..\..\DTE\Core\queue.h"\
	"..\..\DTE\Core\site.h"\
	"..\..\DTE\Core\SockStream.h"\
	"..\..\DTE\Core\StringLess.h"\
	"..\..\DTE\Core\TypeCheck.h"\
	"..\..\DTE\Core\url.h"\
	
NODEP_CPP_DROPI=\
	"..\..\DTE\Core\sysdep.h"\
	

"$(INTDIR)\DropIndexParse.obj" : $(SOURCE) $(DEP_CPP_DROPI) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "dte - Win32 Debug"

DEP_CPP_DROPI=\
	"..\..\DTE\Core\Array.h"\
	"..\..\DTE\Core\Engine.h"\
	"..\..\DTE\Core\exception.h"\
	"..\..\DTE\Core\Iterator.h"\
	"..\..\DTE\Core\listop.h"\
	"..\..\DTE\Core\myopt.h"\
	"..\..\DTE\Core\ParseTree.h"\
	"..\..\DTE\Core\queue.h"\
	"..\..\DTE\Core\site.h"\
	"..\..\DTE\Core\SockStream.h"\
	"..\..\DTE\Core\StringLess.h"\
	"..\..\DTE\Core\TypeCheck.h"\
	"..\..\DTE\Core\url.h"\
	"..\..\unidata\sysdep.h"\
	

"$(INTDIR)\DropIndexParse.obj" : $(SOURCE) $(DEP_CPP_DROPI) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\DTE\Core\Engine.c

!IF  "$(CFG)" == "dte - Win32 Release"

DEP_CPP_ENGIN=\
	"..\..\DTE\Core\Array.h"\
	"..\..\DTE\Core\Engine.h"\
	"..\..\DTE\Core\exception.h"\
	"..\..\DTE\Core\ExecExpr.h"\
	"..\..\DTE\Core\ExecOp.h"\
	"..\..\DTE\Core\Iterator.h"\
	"..\..\DTE\Core\listop.h"\
	"..\..\DTE\Core\myopt.h"\
	"..\..\DTE\Core\ParseTree.h"\
	"..\..\DTE\Core\queue.h"\
	"..\..\DTE\Core\site.h"\
	"..\..\DTE\Core\SockStream.h"\
	"..\..\DTE\Core\StringLess.h"\
	"..\..\DTE\Core\TypeCheck.h"\
	"..\..\DTE\Core\url.h"\
	
NODEP_CPP_ENGIN=\
	"..\..\DTE\Core\sysdep.h"\
	

"$(INTDIR)\Engine.obj" : $(SOURCE) $(DEP_CPP_ENGIN) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "dte - Win32 Debug"

DEP_CPP_ENGIN=\
	"..\..\DTE\Core\Array.h"\
	"..\..\DTE\Core\Engine.h"\
	"..\..\DTE\Core\exception.h"\
	"..\..\DTE\Core\ExecExpr.h"\
	"..\..\DTE\Core\ExecOp.h"\
	"..\..\DTE\Core\Iterator.h"\
	"..\..\DTE\Core\listop.h"\
	"..\..\DTE\Core\myopt.h"\
	"..\..\DTE\Core\ParseTree.h"\
	"..\..\DTE\Core\queue.h"\
	"..\..\DTE\Core\site.h"\
	"..\..\DTE\Core\SockStream.h"\
	"..\..\DTE\Core\StringLess.h"\
	"..\..\DTE\Core\TypeCheck.h"\
	"..\..\DTE\Core\url.h"\
	"..\..\unidata\sysdep.h"\
	

"$(INTDIR)\Engine.obj" : $(SOURCE) $(DEP_CPP_ENGIN) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\DTE\Core\ExecOp.c

!IF  "$(CFG)" == "dte - Win32 Release"

DEP_CPP_EXECO=\
	"..\..\DTE\Core\Array.h"\
	"..\..\DTE\Core\exception.h"\
	"..\..\DTE\Core\ExecExpr.h"\
	"..\..\DTE\Core\ExecOp.h"\
	"..\..\DTE\Core\Iterator.h"\
	"..\..\DTE\Core\listop.h"\
	"..\..\DTE\Core\MemoryMgr.h"\
	"..\..\DTE\Core\myopt.h"\
	"..\..\DTE\Core\queue.h"\
	"..\..\DTE\Core\RTreeRead.h"\
	"..\..\DTE\Core\SockStream.h"\
	"..\..\DTE\Core\StandardRead.h"\
	"..\..\DTE\Core\url.h"\
	
NODEP_CPP_EXECO=\
	"..\..\DTE\Core\sysdep.h"\
	

"$(INTDIR)\ExecOp.obj" : $(SOURCE) $(DEP_CPP_EXECO) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "dte - Win32 Debug"

DEP_CPP_EXECO=\
	"..\..\DTE\Core\Array.h"\
	"..\..\DTE\Core\exception.h"\
	"..\..\DTE\Core\ExecExpr.h"\
	"..\..\DTE\Core\ExecOp.h"\
	"..\..\DTE\Core\Iterator.h"\
	"..\..\DTE\Core\listop.h"\
	"..\..\DTE\Core\MemoryMgr.h"\
	"..\..\DTE\Core\myopt.h"\
	"..\..\DTE\Core\queue.h"\
	"..\..\DTE\Core\RTreeRead.h"\
	"..\..\DTE\Core\SockStream.h"\
	"..\..\DTE\Core\StandardRead.h"\
	"..\..\DTE\Core\url.h"\
	"..\..\unidata\sysdep.h"\
	

"$(INTDIR)\ExecOp.obj" : $(SOURCE) $(DEP_CPP_EXECO) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\DTE\ODBC\GetHandle.cpp

!IF  "$(CFG)" == "dte - Win32 Release"

DEP_CPP_GETHA=\
	"..\..\DTE\ODBC\GetHandle.h"\
	
NODEP_CPP_GETHA=\
	"..\..\DTE\ODBC\exception.h"\
	

"$(INTDIR)\GetHandle.obj" : $(SOURCE) $(DEP_CPP_GETHA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "dte - Win32 Debug"

DEP_CPP_GETHA=\
	"..\..\DTE\Core\exception.h"\
	"..\..\DTE\ODBC\GetHandle.h"\
	

"$(INTDIR)\GetHandle.obj" : $(SOURCE) $(DEP_CPP_GETHA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\DTE\Core\IndexParse.dummy.c

!IF  "$(CFG)" == "dte - Win32 Release"

DEP_CPP_INDEX=\
	"..\..\DTE\Core\Aggregates.h"\
	"..\..\DTE\Core\Array.h"\
	"..\..\DTE\Core\catalog.h"\
	"..\..\DTE\Core\exception.h"\
	"..\..\DTE\Core\Iterator.h"\
	"..\..\DTE\Core\listop.h"\
	"..\..\DTE\Core\MemoryMgr.h"\
	"..\..\DTE\Core\myopt.h"\
	"..\..\DTE\Core\ParseTree.h"\
	"..\..\DTE\Core\queue.h"\
	"..\..\DTE\Core\site.h"\
	"..\..\DTE\Core\SockStream.h"\
	"..\..\DTE\Core\StringLess.h"\
	"..\..\DTE\Core\TypeCheck.h"\
	"..\..\DTE\Core\url.h"\
	"..\..\DTE\Core\Utility.h"\
	
NODEP_CPP_INDEX=\
	"..\..\DTE\Core\sysdep.h"\
	

"$(INTDIR)\IndexParse.dummy.obj" : $(SOURCE) $(DEP_CPP_INDEX) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "dte - Win32 Debug"

DEP_CPP_INDEX=\
	"..\..\DTE\Core\Aggregates.h"\
	"..\..\DTE\Core\Array.h"\
	"..\..\DTE\Core\catalog.h"\
	"..\..\DTE\Core\exception.h"\
	"..\..\DTE\Core\Iterator.h"\
	"..\..\DTE\Core\listop.h"\
	"..\..\DTE\Core\MemoryMgr.h"\
	"..\..\DTE\Core\myopt.h"\
	"..\..\DTE\Core\ParseTree.h"\
	"..\..\DTE\Core\queue.h"\
	"..\..\DTE\Core\site.h"\
	"..\..\DTE\Core\SockStream.h"\
	"..\..\DTE\Core\StringLess.h"\
	"..\..\DTE\Core\TypeCheck.h"\
	"..\..\DTE\Core\url.h"\
	"..\..\DTE\Core\Utility.h"\
	"..\..\unidata\sysdep.h"\
	

"$(INTDIR)\IndexParse.dummy.obj" : $(SOURCE) $(DEP_CPP_INDEX) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\DTE\Core\Inserter.c

!IF  "$(CFG)" == "dte - Win32 Release"

DEP_CPP_INSER=\
	"..\..\DTE\Core\exception.h"\
	"..\..\DTE\Core\Inserter.h"\
	"..\..\DTE\Core\Iterator.h"\
	"..\..\DTE\Core\MemoryMgr.h"\
	"..\..\DTE\Core\SockStream.h"\
	"..\..\DTE\Core\StandardRead.h"\
	"..\..\DTE\Core\url.h"\
	
NODEP_CPP_INSER=\
	"..\..\DTE\Core\sysdep.h"\
	

"$(INTDIR)\Inserter.obj" : $(SOURCE) $(DEP_CPP_INSER) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "dte - Win32 Debug"

DEP_CPP_INSER=\
	"..\..\DTE\Core\exception.h"\
	"..\..\DTE\Core\Inserter.h"\
	"..\..\DTE\Core\Iterator.h"\
	"..\..\DTE\Core\MemoryMgr.h"\
	"..\..\DTE\Core\SockStream.h"\
	"..\..\DTE\Core\StandardRead.h"\
	"..\..\DTE\Core\url.h"\
	"..\..\unidata\sysdep.h"\
	

"$(INTDIR)\Inserter.obj" : $(SOURCE) $(DEP_CPP_INSER) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\DTE\Core\InsertParse.c

!IF  "$(CFG)" == "dte - Win32 Release"

DEP_CPP_INSERT=\
	"..\..\DTE\Core\Array.h"\
	"..\..\DTE\Core\catalog.h"\
	"..\..\DTE\Core\exception.h"\
	"..\..\DTE\Core\Iterator.h"\
	"..\..\DTE\Core\listop.h"\
	"..\..\DTE\Core\myopt.h"\
	"..\..\DTE\Core\ParseTree.h"\
	"..\..\DTE\Core\queue.h"\
	"..\..\DTE\Core\site.h"\
	"..\..\DTE\Core\SockStream.h"\
	"..\..\DTE\Core\StringLess.h"\
	"..\..\DTE\Core\TypeCheck.h"\
	"..\..\DTE\Core\url.h"\
	"..\..\DTE\Core\Utility.h"\
	
NODEP_CPP_INSERT=\
	"..\..\DTE\Core\sysdep.h"\
	

"$(INTDIR)\InsertParse.obj" : $(SOURCE) $(DEP_CPP_INSERT) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "dte - Win32 Debug"

DEP_CPP_INSERT=\
	"..\..\DTE\Core\Array.h"\
	"..\..\DTE\Core\catalog.h"\
	"..\..\DTE\Core\exception.h"\
	"..\..\DTE\Core\Iterator.h"\
	"..\..\DTE\Core\listop.h"\
	"..\..\DTE\Core\myopt.h"\
	"..\..\DTE\Core\ParseTree.h"\
	"..\..\DTE\Core\queue.h"\
	"..\..\DTE\Core\site.h"\
	"..\..\DTE\Core\SockStream.h"\
	"..\..\DTE\Core\StringLess.h"\
	"..\..\DTE\Core\TypeCheck.h"\
	"..\..\DTE\Core\url.h"\
	"..\..\DTE\Core\Utility.h"\
	"..\..\unidata\sysdep.h"\
	

"$(INTDIR)\InsertParse.obj" : $(SOURCE) $(DEP_CPP_INSERT) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\DTE\Core\Interface.c

!IF  "$(CFG)" == "dte - Win32 Release"

DEP_CPP_INTER=\
	"..\..\DTE\Core\Array.h"\
	"..\..\DTE\Core\catalog.h"\
	"..\..\DTE\Core\DevRead.h"\
	"..\..\DTE\Core\Engine.h"\
	"..\..\DTE\Core\exception.h"\
	"..\..\DTE\Core\Inserter.h"\
	"..\..\DTE\Core\Interface.h"\
	"..\..\DTE\Core\Iterator.h"\
	"..\..\DTE\Core\listop.h"\
	"..\..\DTE\Core\myopt.h"\
	"..\..\DTE\Core\ParseTree.h"\
	"..\..\DTE\Core\queue.h"\
	"..\..\DTE\Core\site.h"\
	"..\..\DTE\Core\SockStream.h"\
	"..\..\DTE\Core\StandardRead.h"\
	"..\..\DTE\Core\StringLess.h"\
	"..\..\DTE\Core\TypeCheck.h"\
	"..\..\DTE\Core\url.h"\
	"..\..\DTE\Core\Utility.h"\
	
NODEP_CPP_INTER=\
	"..\..\DTE\Core\sysdep.h"\
	

"$(INTDIR)\Interface.obj" : $(SOURCE) $(DEP_CPP_INTER) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "dte - Win32 Debug"

DEP_CPP_INTER=\
	"..\..\DTE\Core\Array.h"\
	"..\..\DTE\Core\catalog.h"\
	"..\..\DTE\Core\DevRead.h"\
	"..\..\DTE\Core\Engine.h"\
	"..\..\DTE\Core\exception.h"\
	"..\..\DTE\Core\Inserter.h"\
	"..\..\DTE\Core\Interface.h"\
	"..\..\DTE\Core\Iterator.h"\
	"..\..\DTE\Core\listop.h"\
	"..\..\DTE\Core\myopt.h"\
	"..\..\DTE\Core\ParseTree.h"\
	"..\..\DTE\Core\queue.h"\
	"..\..\DTE\Core\site.h"\
	"..\..\DTE\Core\SockStream.h"\
	"..\..\DTE\Core\StandardRead.h"\
	"..\..\DTE\Core\StringLess.h"\
	"..\..\DTE\Core\TypeCheck.h"\
	"..\..\DTE\Core\url.h"\
	"..\..\DTE\Core\Utility.h"\
	"..\..\unidata\sysdep.h"\
	

"$(INTDIR)\Interface.obj" : $(SOURCE) $(DEP_CPP_INTER) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\DTE\Core\joins.c

!IF  "$(CFG)" == "dte - Win32 Release"

DEP_CPP_JOINS=\
	"..\..\DTE\Core\Array.h"\
	"..\..\DTE\Core\exception.h"\
	"..\..\DTE\Core\Iterator.h"\
	"..\..\DTE\Core\joins.h"\
	"..\..\DTE\Core\listop.h"\
	"..\..\DTE\Core\myopt.h"\
	"..\..\DTE\Core\queue.h"\
	"..\..\DTE\Core\site.h"\
	"..\..\DTE\Core\SockStream.h"\
	"..\..\DTE\Core\url.h"\
	
NODEP_CPP_JOINS=\
	"..\..\DTE\Core\sysdep.h"\
	

"$(INTDIR)\joins.obj" : $(SOURCE) $(DEP_CPP_JOINS) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "dte - Win32 Debug"

DEP_CPP_JOINS=\
	"..\..\DTE\Core\Array.h"\
	"..\..\DTE\Core\exception.h"\
	"..\..\DTE\Core\Iterator.h"\
	"..\..\DTE\Core\joins.h"\
	"..\..\DTE\Core\listop.h"\
	"..\..\DTE\Core\myopt.h"\
	"..\..\DTE\Core\queue.h"\
	"..\..\DTE\Core\site.h"\
	"..\..\DTE\Core\SockStream.h"\
	"..\..\DTE\Core\url.h"\
	"..\..\unidata\sysdep.h"\
	

"$(INTDIR)\joins.obj" : $(SOURCE) $(DEP_CPP_JOINS) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\DTE\Core\lex.yy.c

!IF  "$(CFG)" == "dte - Win32 Release"

DEP_CPP_LEX_Y=\
	"..\..\DTE\Core\Array.h"\
	"..\..\DTE\Core\exception.h"\
	"..\..\DTE\Core\listop.h"\
	"..\..\DTE\Core\my.yacc.tab.h"\
	"..\..\DTE\Core\myopt.h"\
	"..\..\DTE\Core\ParseTree.h"\
	"..\..\DTE\Core\queue.h"\
	"..\..\DTE\Core\SockStream.h"\
	"..\..\DTE\Core\StringLess.h"\
	"..\..\DTE\Core\TypeCheck.h"\
	"..\..\DTE\Core\url.h"\
	"..\..\DTE\Core\Utility.h"\
	
NODEP_CPP_LEX_Y=\
	"..\..\DTE\Core\sysdep.h"\
	

"$(INTDIR)\lex.yy.obj" : $(SOURCE) $(DEP_CPP_LEX_Y) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "dte - Win32 Debug"

DEP_CPP_LEX_Y=\
	"..\..\DTE\Core\Array.h"\
	"..\..\DTE\Core\exception.h"\
	"..\..\DTE\Core\listop.h"\
	"..\..\DTE\Core\my.yacc.tab.h"\
	"..\..\DTE\Core\myopt.h"\
	"..\..\DTE\Core\ParseTree.h"\
	"..\..\DTE\Core\queue.h"\
	"..\..\DTE\Core\SockStream.h"\
	"..\..\DTE\Core\StringLess.h"\
	"..\..\DTE\Core\TypeCheck.h"\
	"..\..\DTE\Core\url.h"\
	"..\..\DTE\Core\Utility.h"\
	"..\..\unidata\sysdep.h"\
	"C:\Program Files\DevStudio\MyProjects\DTE\unistd.h"\
	

"$(INTDIR)\lex.yy.obj" : $(SOURCE) $(DEP_CPP_LEX_Y) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\DTE\Core\listop.c

!IF  "$(CFG)" == "dte - Win32 Release"

DEP_CPP_LISTO=\
	"..\..\DTE\Core\Array.h"\
	"..\..\DTE\Core\catalog.h"\
	"..\..\DTE\Core\exception.h"\
	"..\..\DTE\Core\ExecExpr.h"\
	"..\..\DTE\Core\Iterator.h"\
	"..\..\DTE\Core\listop.h"\
	"..\..\DTE\Core\myopt.h"\
	"..\..\DTE\Core\queue.h"\
	"..\..\DTE\Core\site.h"\
	"..\..\DTE\Core\SockStream.h"\
	"..\..\DTE\Core\url.h"\
	"..\..\DTE\Core\Utility.h"\
	
NODEP_CPP_LISTO=\
	"..\..\DTE\Core\sysdep.h"\
	

"$(INTDIR)\listop.obj" : $(SOURCE) $(DEP_CPP_LISTO) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "dte - Win32 Debug"

DEP_CPP_LISTO=\
	"..\..\DTE\Core\Array.h"\
	"..\..\DTE\Core\catalog.h"\
	"..\..\DTE\Core\exception.h"\
	"..\..\DTE\Core\ExecExpr.h"\
	"..\..\DTE\Core\Iterator.h"\
	"..\..\DTE\Core\listop.h"\
	"..\..\DTE\Core\myopt.h"\
	"..\..\DTE\Core\queue.h"\
	"..\..\DTE\Core\site.h"\
	"..\..\DTE\Core\SockStream.h"\
	"..\..\DTE\Core\url.h"\
	"..\..\DTE\Core\Utility.h"\
	"..\..\unidata\sysdep.h"\
	

"$(INTDIR)\listop.obj" : $(SOURCE) $(DEP_CPP_LISTO) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\DTE\StandAlone\main.c

!IF  "$(CFG)" == "dte - Win32 Release"

DEP_CPP_MAIN_=\
	"..\..\DTE\StandAlone\Common.h"\
	"..\..\DTE\StandAlone\InitShut.h"\
	
NODEP_CPP_MAIN_=\
	"..\..\DTE\StandAlone\sysdep.h"\
	

"$(INTDIR)\main.obj" : $(SOURCE) $(DEP_CPP_MAIN_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "dte - Win32 Debug"

DEP_CPP_MAIN_=\
	"..\..\DTE\StandAlone\Common.h"\
	"..\..\DTE\StandAlone\InitShut.h"\
	"..\..\unidata\sysdep.h"\
	

"$(INTDIR)\main.obj" : $(SOURCE) $(DEP_CPP_MAIN_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\DTE\Core\MaterializeParse.c

!IF  "$(CFG)" == "dte - Win32 Release"

DEP_CPP_MATER=\
	"..\..\DTE\Core\Array.h"\
	"..\..\DTE\Core\catalog.h"\
	"..\..\DTE\Core\Engine.h"\
	"..\..\DTE\Core\exception.h"\
	"..\..\DTE\Core\Inserter.h"\
	"..\..\DTE\Core\Interface.h"\
	"..\..\DTE\Core\Iterator.h"\
	"..\..\DTE\Core\listop.h"\
	"..\..\DTE\Core\myopt.h"\
	"..\..\DTE\Core\ParseTree.h"\
	"..\..\DTE\Core\queue.h"\
	"..\..\DTE\Core\site.h"\
	"..\..\DTE\Core\SockStream.h"\
	"..\..\DTE\Core\StringLess.h"\
	"..\..\DTE\Core\TypeCheck.h"\
	"..\..\DTE\Core\url.h"\
	"..\..\DTE\Core\Utility.h"\
	
NODEP_CPP_MATER=\
	"..\..\DTE\Core\sysdep.h"\
	

"$(INTDIR)\MaterializeParse.obj" : $(SOURCE) $(DEP_CPP_MATER) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "dte - Win32 Debug"

DEP_CPP_MATER=\
	"..\..\DTE\Core\Array.h"\
	"..\..\DTE\Core\catalog.h"\
	"..\..\DTE\Core\Engine.h"\
	"..\..\DTE\Core\exception.h"\
	"..\..\DTE\Core\Inserter.h"\
	"..\..\DTE\Core\Interface.h"\
	"..\..\DTE\Core\Iterator.h"\
	"..\..\DTE\Core\listop.h"\
	"..\..\DTE\Core\myopt.h"\
	"..\..\DTE\Core\ParseTree.h"\
	"..\..\DTE\Core\queue.h"\
	"..\..\DTE\Core\site.h"\
	"..\..\DTE\Core\SockStream.h"\
	"..\..\DTE\Core\StringLess.h"\
	"..\..\DTE\Core\TypeCheck.h"\
	"..\..\DTE\Core\url.h"\
	"..\..\DTE\Core\Utility.h"\
	"..\..\unidata\sysdep.h"\
	

"$(INTDIR)\MaterializeParse.obj" : $(SOURCE) $(DEP_CPP_MATER) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\DTE\Core\MemoryMgr.c

!IF  "$(CFG)" == "dte - Win32 Release"

DEP_CPP_MEMOR=\
	"..\..\DTE\Core\Interface.h"\
	"..\..\DTE\Core\MemoryMgr.h"\
	
NODEP_CPP_MEMOR=\
	"..\..\DTE\Core\DateTime.h"\
	"..\..\DTE\Core\sysdep.h"\
	

"$(INTDIR)\MemoryMgr.obj" : $(SOURCE) $(DEP_CPP_MEMOR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "dte - Win32 Debug"

DEP_CPP_MEMOR=\
	"..\..\DTE\Core\Interface.h"\
	"..\..\DTE\Core\MemoryMgr.h"\
	"..\..\unidata\DateTime.h"\
	"..\..\unidata\sysdep.h"\
	

"$(INTDIR)\MemoryMgr.obj" : $(SOURCE) $(DEP_CPP_MEMOR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\DTE\Core\MinMax.c

!IF  "$(CFG)" == "dte - Win32 Release"

DEP_CPP_MINMA=\
	"..\..\DTE\Core\Array.h"\
	"..\..\DTE\Core\catalog.h"\
	"..\..\DTE\Core\exception.h"\
	"..\..\DTE\Core\listop.h"\
	"..\..\DTE\Core\myopt.h"\
	"..\..\DTE\Core\queue.h"\
	"..\..\DTE\Core\SockStream.h"\
	"..\..\DTE\Core\url.h"\
	"..\..\DTE\Core\Utility.h"\
	
NODEP_CPP_MINMA=\
	"..\..\DTE\Core\sysdep.h"\
	

"$(INTDIR)\MinMax.obj" : $(SOURCE) $(DEP_CPP_MINMA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "dte - Win32 Debug"

DEP_CPP_MINMA=\
	"..\..\DTE\Core\Array.h"\
	"..\..\DTE\Core\catalog.h"\
	"..\..\DTE\Core\exception.h"\
	"..\..\DTE\Core\listop.h"\
	"..\..\DTE\Core\myopt.h"\
	"..\..\DTE\Core\queue.h"\
	"..\..\DTE\Core\SockStream.h"\
	"..\..\DTE\Core\url.h"\
	"..\..\DTE\Core\Utility.h"\
	"..\..\unidata\sysdep.h"\
	

"$(INTDIR)\MinMax.obj" : $(SOURCE) $(DEP_CPP_MINMA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\DTE\Core\my.yacc.tab.c

!IF  "$(CFG)" == "dte - Win32 Release"

DEP_CPP_MY_YA=\
	"..\..\DTE\Core\Array.h"\
	"..\..\DTE\Core\exception.h"\
	"..\..\DTE\Core\listop.h"\
	"..\..\DTE\Core\myopt.h"\
	"..\..\DTE\Core\ParseTree.h"\
	"..\..\DTE\Core\queue.h"\
	"..\..\DTE\Core\SockStream.h"\
	"..\..\DTE\Core\StringLess.h"\
	"..\..\DTE\Core\TypeCheck.h"\
	"..\..\DTE\Core\url.h"\
	
NODEP_CPP_MY_YA=\
	"..\..\DTE\Core\sysdep.h"\
	

"$(INTDIR)\my.yacc.tab.obj" : $(SOURCE) $(DEP_CPP_MY_YA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "dte - Win32 Debug"

DEP_CPP_MY_YA=\
	"..\..\DTE\Core\Array.h"\
	"..\..\DTE\Core\exception.h"\
	"..\..\DTE\Core\listop.h"\
	"..\..\DTE\Core\myopt.h"\
	"..\..\DTE\Core\ParseTree.h"\
	"..\..\DTE\Core\queue.h"\
	"..\..\DTE\Core\SockStream.h"\
	"..\..\DTE\Core\StringLess.h"\
	"..\..\DTE\Core\TypeCheck.h"\
	"..\..\DTE\Core\url.h"\
	"..\..\unidata\sysdep.h"\
	

"$(INTDIR)\my.yacc.tab.obj" : $(SOURCE) $(DEP_CPP_MY_YA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\DTE\ODBC\ODBC.cpp

!IF  "$(CFG)" == "dte - Win32 Release"

DEP_CPP_ODBC_=\
	"..\..\DTE\ODBC\GetHandle.h"\
	"..\..\DTE\ODBC\ODBC.h"\
	
NODEP_CPP_ODBC_=\
	"..\..\DTE\ODBC\DateTime.h"\
	"..\..\DTE\ODBC\exception.h"\
	"..\..\DTE\ODBC\sysdep.h"\
	

"$(INTDIR)\ODBC.obj" : $(SOURCE) $(DEP_CPP_ODBC_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "dte - Win32 Debug"

DEP_CPP_ODBC_=\
	"..\..\DTE\Core\exception.h"\
	"..\..\DTE\ODBC\GetHandle.h"\
	"..\..\DTE\ODBC\ODBC.h"\
	"..\..\unidata\DateTime.h"\
	"..\..\unidata\sysdep.h"\
	

"$(INTDIR)\ODBC.obj" : $(SOURCE) $(DEP_CPP_ODBC_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\DTE\ODBC\ODBCInterface.cpp

!IF  "$(CFG)" == "dte - Win32 Release"

DEP_CPP_ODBCI=\
	"..\..\DTE\ODBC\GetHandle.h"\
	"..\..\DTE\ODBC\ODBC.h"\
	"..\..\DTE\ODBC\ODBCSite.h"\
	
NODEP_CPP_ODBCI=\
	"..\..\DTE\ODBC\catalog.h"\
	"..\..\DTE\ODBC\DevRead.h"\
	"..\..\DTE\ODBC\Engine.h"\
	"..\..\DTE\ODBC\exception.h"\
	"..\..\DTE\ODBC\Inserter.h"\
	"..\..\DTE\ODBC\Interface.h"\
	"..\..\DTE\ODBC\site.h"\
	"..\..\DTE\ODBC\StandardRead.h"\
	"..\..\DTE\ODBC\sysdep.h"\
	

"$(INTDIR)\ODBCInterface.obj" : $(SOURCE) $(DEP_CPP_ODBCI) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "dte - Win32 Debug"

DEP_CPP_ODBCI=\
	"..\..\DTE\Core\catalog.h"\
	"..\..\DTE\Core\DevRead.h"\
	"..\..\DTE\Core\Engine.h"\
	"..\..\DTE\Core\exception.h"\
	"..\..\DTE\Core\Inserter.h"\
	"..\..\DTE\Core\Interface.h"\
	"..\..\DTE\Core\site.h"\
	"..\..\DTE\Core\StandardRead.h"\
	"..\..\DTE\ODBC\GetHandle.h"\
	"..\..\DTE\ODBC\ODBC.h"\
	"..\..\DTE\ODBC\ODBCSite.h"\
	"..\..\unidata\sysdep.h"\
	

"$(INTDIR)\ODBCInterface.obj" : $(SOURCE) $(DEP_CPP_ODBCI) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\DTE\ODBC\ODBCIterator.cpp

!IF  "$(CFG)" == "dte - Win32 Release"

DEP_CPP_ODBCIT=\
	"..\..\DTE\ODBC\GetHandle.h"\
	"..\..\DTE\ODBC\ODBC.h"\
	"..\..\DTE\ODBC\ODBCIterator.h"\
	
NODEP_CPP_ODBCIT=\
	"..\..\DTE\ODBC\exception.h"\
	"..\..\DTE\ODBC\Iterator.h"\
	"..\..\DTE\ODBC\sysdep.h"\
	

"$(INTDIR)\ODBCIterator.obj" : $(SOURCE) $(DEP_CPP_ODBCIT) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "dte - Win32 Debug"

DEP_CPP_ODBCIT=\
	"..\..\DTE\Core\exception.h"\
	"..\..\DTE\Core\Iterator.h"\
	"..\..\DTE\ODBC\GetHandle.h"\
	"..\..\DTE\ODBC\ODBC.h"\
	"..\..\DTE\ODBC\ODBCIterator.h"\
	"..\..\unidata\sysdep.h"\
	

"$(INTDIR)\ODBCIterator.obj" : $(SOURCE) $(DEP_CPP_ODBCIT) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\DTE\ODBC\ODBCSite.cpp

!IF  "$(CFG)" == "dte - Win32 Release"

DEP_CPP_ODBCS=\
	"..\..\DTE\ODBC\GetHandle.h"\
	"..\..\DTE\ODBC\ODBC.h"\
	"..\..\DTE\ODBC\ODBCIterator.h"\
	"..\..\DTE\ODBC\ODBCSite.h"\
	
NODEP_CPP_ODBCS=\
	"..\..\DTE\ODBC\exception.h"\
	"..\..\DTE\ODBC\Iterator.h"\
	"..\..\DTE\ODBC\site.h"\
	"..\..\DTE\ODBC\sysdep.h"\
	

"$(INTDIR)\ODBCSite.obj" : $(SOURCE) $(DEP_CPP_ODBCS) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "dte - Win32 Debug"

DEP_CPP_ODBCS=\
	"..\..\DTE\Core\exception.h"\
	"..\..\DTE\Core\Iterator.h"\
	"..\..\DTE\Core\site.h"\
	"..\..\DTE\ODBC\GetHandle.h"\
	"..\..\DTE\ODBC\ODBC.h"\
	"..\..\DTE\ODBC\ODBCIterator.h"\
	"..\..\DTE\ODBC\ODBCSite.h"\
	"..\..\unidata\sysdep.h"\
	

"$(INTDIR)\ODBCSite.obj" : $(SOURCE) $(DEP_CPP_ODBCS) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\DTE\Core\PQueue.c

!IF  "$(CFG)" == "dte - Win32 Release"

DEP_CPP_PQUEU=\
	"..\..\DTE\Core\PQueue.h"\
	
NODEP_CPP_PQUEU=\
	"..\..\DTE\Core\sysdep.h"\
	

"$(INTDIR)\PQueue.obj" : $(SOURCE) $(DEP_CPP_PQUEU) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "dte - Win32 Debug"

DEP_CPP_PQUEU=\
	"..\..\DTE\Core\PQueue.h"\
	"..\..\unidata\sysdep.h"\
	

"$(INTDIR)\PQueue.obj" : $(SOURCE) $(DEP_CPP_PQUEU) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\DTE\Core\QueryTree.c

!IF  "$(CFG)" == "dte - Win32 Release"

DEP_CPP_QUERY=\
	"..\..\DTE\Core\Aggregates.h"\
	"..\..\DTE\Core\Array.h"\
	"..\..\DTE\Core\catalog.h"\
	"..\..\DTE\Core\DevRead.h"\
	"..\..\DTE\Core\exception.h"\
	"..\..\DTE\Core\Interface.h"\
	"..\..\DTE\Core\Iterator.h"\
	"..\..\DTE\Core\listop.h"\
	"..\..\DTE\Core\MemoryMgr.h"\
	"..\..\DTE\Core\myopt.h"\
	"..\..\DTE\Core\ParseTree.h"\
	"..\..\DTE\Core\PQueue.h"\
	"..\..\DTE\Core\queue.h"\
	"..\..\DTE\Core\site.h"\
	"..\..\DTE\Core\SockStream.h"\
	"..\..\DTE\Core\Sort.h"\
	"..\..\DTE\Core\StandardRead.h"\
	"..\..\DTE\Core\StringLess.h"\
	"..\..\DTE\Core\TypeCheck.h"\
	"..\..\DTE\Core\url.h"\
	"..\..\DTE\Core\Utility.h"\
	
NODEP_CPP_QUERY=\
	"..\..\DTE\Core\sysdep.h"\
	

"$(INTDIR)\QueryTree.obj" : $(SOURCE) $(DEP_CPP_QUERY) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "dte - Win32 Debug"

DEP_CPP_QUERY=\
	"..\..\DTE\Core\Aggregates.h"\
	"..\..\DTE\Core\Array.h"\
	"..\..\DTE\Core\catalog.h"\
	"..\..\DTE\Core\DevRead.h"\
	"..\..\DTE\Core\exception.h"\
	"..\..\DTE\Core\Interface.h"\
	"..\..\DTE\Core\Iterator.h"\
	"..\..\DTE\Core\listop.h"\
	"..\..\DTE\Core\MemoryMgr.h"\
	"..\..\DTE\Core\myopt.h"\
	"..\..\DTE\Core\ParseTree.h"\
	"..\..\DTE\Core\PQueue.h"\
	"..\..\DTE\Core\queue.h"\
	"..\..\DTE\Core\site.h"\
	"..\..\DTE\Core\SockStream.h"\
	"..\..\DTE\Core\Sort.h"\
	"..\..\DTE\Core\StandardRead.h"\
	"..\..\DTE\Core\StringLess.h"\
	"..\..\DTE\Core\TypeCheck.h"\
	"..\..\DTE\Core\url.h"\
	"..\..\DTE\Core\Utility.h"\
	"..\..\unidata\sysdep.h"\
	

"$(INTDIR)\QueryTree.obj" : $(SOURCE) $(DEP_CPP_QUERY) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\DTE\Core\resolve.c

!IF  "$(CFG)" == "dte - Win32 Release"

DEP_CPP_RESOL=\
	"..\..\DTE\Core\Array.h"\
	"..\..\DTE\Core\catalog.h"\
	"..\..\DTE\Core\exception.h"\
	"..\..\DTE\Core\ExecExpr.h"\
	"..\..\DTE\Core\Interface.h"\
	"..\..\DTE\Core\Iterator.h"\
	"..\..\DTE\Core\listop.h"\
	"..\..\DTE\Core\myopt.h"\
	"..\..\DTE\Core\queue.h"\
	"..\..\DTE\Core\site.h"\
	"..\..\DTE\Core\SockStream.h"\
	"..\..\DTE\Core\url.h"\
	"..\..\DTE\Core\Utility.h"\
	
NODEP_CPP_RESOL=\
	"..\..\DTE\Core\sysdep.h"\
	

"$(INTDIR)\resolve.obj" : $(SOURCE) $(DEP_CPP_RESOL) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "dte - Win32 Debug"

DEP_CPP_RESOL=\
	"..\..\DTE\Core\Array.h"\
	"..\..\DTE\Core\catalog.h"\
	"..\..\DTE\Core\exception.h"\
	"..\..\DTE\Core\ExecExpr.h"\
	"..\..\DTE\Core\Interface.h"\
	"..\..\DTE\Core\Iterator.h"\
	"..\..\DTE\Core\listop.h"\
	"..\..\DTE\Core\myopt.h"\
	"..\..\DTE\Core\queue.h"\
	"..\..\DTE\Core\site.h"\
	"..\..\DTE\Core\SockStream.h"\
	"..\..\DTE\Core\url.h"\
	"..\..\DTE\Core\Utility.h"\
	"..\..\unidata\sysdep.h"\
	

"$(INTDIR)\resolve.obj" : $(SOURCE) $(DEP_CPP_RESOL) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\DTE\Core\RTreeRead.dummy.c

!IF  "$(CFG)" == "dte - Win32 Release"

DEP_CPP_RTREE=\
	"..\..\DTE\Core\Array.h"\
	"..\..\DTE\Core\exception.h"\
	"..\..\DTE\Core\Iterator.h"\
	"..\..\DTE\Core\listop.h"\
	"..\..\DTE\Core\myopt.h"\
	"..\..\DTE\Core\queue.h"\
	"..\..\DTE\Core\RTreeRead.h"\
	"..\..\DTE\Core\SockStream.h"\
	"..\..\DTE\Core\StandardRead.h"\
	"..\..\DTE\Core\url.h"\
	
NODEP_CPP_RTREE=\
	"..\..\DTE\Core\sysdep.h"\
	

"$(INTDIR)\RTreeRead.dummy.obj" : $(SOURCE) $(DEP_CPP_RTREE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "dte - Win32 Debug"

DEP_CPP_RTREE=\
	"..\..\DTE\Core\Array.h"\
	"..\..\DTE\Core\exception.h"\
	"..\..\DTE\Core\Iterator.h"\
	"..\..\DTE\Core\listop.h"\
	"..\..\DTE\Core\myopt.h"\
	"..\..\DTE\Core\queue.h"\
	"..\..\DTE\Core\RTreeRead.h"\
	"..\..\DTE\Core\SockStream.h"\
	"..\..\DTE\Core\StandardRead.h"\
	"..\..\DTE\Core\url.h"\
	"..\..\unidata\sysdep.h"\
	

"$(INTDIR)\RTreeRead.dummy.obj" : $(SOURCE) $(DEP_CPP_RTREE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\DTE\StandAlone\SBMInit.dummy.c
DEP_CPP_SBMIN=\
	"..\..\DTE\StandAlone\InitShut.h"\
	

"$(INTDIR)\SBMInit.dummy.obj" : $(SOURCE) $(DEP_CPP_SBMIN) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\DTE\Core\SchemaParse.c

!IF  "$(CFG)" == "dte - Win32 Release"

DEP_CPP_SCHEM=\
	"..\..\DTE\Core\Array.h"\
	"..\..\DTE\Core\catalog.h"\
	"..\..\DTE\Core\exception.h"\
	"..\..\DTE\Core\Interface.h"\
	"..\..\DTE\Core\Iterator.h"\
	"..\..\DTE\Core\listop.h"\
	"..\..\DTE\Core\myopt.h"\
	"..\..\DTE\Core\ParseTree.h"\
	"..\..\DTE\Core\queue.h"\
	"..\..\DTE\Core\site.h"\
	"..\..\DTE\Core\SockStream.h"\
	"..\..\DTE\Core\StringLess.h"\
	"..\..\DTE\Core\TypeCheck.h"\
	"..\..\DTE\Core\url.h"\
	"..\..\DTE\Core\Utility.h"\
	
NODEP_CPP_SCHEM=\
	"..\..\DTE\Core\sysdep.h"\
	

"$(INTDIR)\SchemaParse.obj" : $(SOURCE) $(DEP_CPP_SCHEM) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "dte - Win32 Debug"

DEP_CPP_SCHEM=\
	"..\..\DTE\Core\Array.h"\
	"..\..\DTE\Core\catalog.h"\
	"..\..\DTE\Core\exception.h"\
	"..\..\DTE\Core\Interface.h"\
	"..\..\DTE\Core\Iterator.h"\
	"..\..\DTE\Core\listop.h"\
	"..\..\DTE\Core\myopt.h"\
	"..\..\DTE\Core\ParseTree.h"\
	"..\..\DTE\Core\queue.h"\
	"..\..\DTE\Core\site.h"\
	"..\..\DTE\Core\SockStream.h"\
	"..\..\DTE\Core\StringLess.h"\
	"..\..\DTE\Core\TypeCheck.h"\
	"..\..\DTE\Core\url.h"\
	"..\..\DTE\Core\Utility.h"\
	"..\..\unidata\sysdep.h"\
	

"$(INTDIR)\SchemaParse.obj" : $(SOURCE) $(DEP_CPP_SCHEM) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\DTE\Core\site.c

!IF  "$(CFG)" == "dte - Win32 Release"

DEP_CPP_SITE_=\
	"..\..\DTE\Core\Array.h"\
	"..\..\DTE\Core\exception.h"\
	"..\..\DTE\Core\ExecExpr.h"\
	"..\..\DTE\Core\ExecOp.h"\
	"..\..\DTE\Core\Interface.h"\
	"..\..\DTE\Core\Iterator.h"\
	"..\..\DTE\Core\listop.h"\
	"..\..\DTE\Core\MemoryMgr.h"\
	"..\..\DTE\Core\myopt.h"\
	"..\..\DTE\Core\queue.h"\
	"..\..\DTE\Core\RTreeRead.h"\
	"..\..\DTE\Core\site.h"\
	"..\..\DTE\Core\SockStream.h"\
	"..\..\DTE\Core\StandardRead.h"\
	"..\..\DTE\Core\url.h"\
	
NODEP_CPP_SITE_=\
	"..\..\DTE\Core\sysdep.h"\
	

"$(INTDIR)\site.obj" : $(SOURCE) $(DEP_CPP_SITE_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "dte - Win32 Debug"

DEP_CPP_SITE_=\
	"..\..\DTE\Core\Array.h"\
	"..\..\DTE\Core\exception.h"\
	"..\..\DTE\Core\ExecExpr.h"\
	"..\..\DTE\Core\ExecOp.h"\
	"..\..\DTE\Core\Interface.h"\
	"..\..\DTE\Core\Iterator.h"\
	"..\..\DTE\Core\listop.h"\
	"..\..\DTE\Core\MemoryMgr.h"\
	"..\..\DTE\Core\myopt.h"\
	"..\..\DTE\Core\queue.h"\
	"..\..\DTE\Core\RTreeRead.h"\
	"..\..\DTE\Core\site.h"\
	"..\..\DTE\Core\SockStream.h"\
	"..\..\DTE\Core\StandardRead.h"\
	"..\..\DTE\Core\url.h"\
	"..\..\unidata\sysdep.h"\
	

"$(INTDIR)\site.obj" : $(SOURCE) $(DEP_CPP_SITE_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\DTE\Core\SockStream.c

!IF  "$(CFG)" == "dte - Win32 Release"

DEP_CPP_SOCKS=\
	"..\..\DTE\Core\SockStream.h"\
	
NODEP_CPP_SOCKS=\
	"..\..\DTE\Core\sysdep.h"\
	

"$(INTDIR)\SockStream.obj" : $(SOURCE) $(DEP_CPP_SOCKS) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "dte - Win32 Debug"

DEP_CPP_SOCKS=\
	"..\..\DTE\Core\SockStream.h"\
	"..\..\unidata\sysdep.h"\
	

"$(INTDIR)\SockStream.obj" : $(SOURCE) $(DEP_CPP_SOCKS) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\DTE\Core\Sort.c

!IF  "$(CFG)" == "dte - Win32 Release"

DEP_CPP_SORT_=\
	"..\..\DTE\Core\Array.h"\
	"..\..\DTE\Core\exception.h"\
	"..\..\DTE\Core\Inserter.h"\
	"..\..\DTE\Core\Iterator.h"\
	"..\..\DTE\Core\listop.h"\
	"..\..\DTE\Core\MemoryMgr.h"\
	"..\..\DTE\Core\myopt.h"\
	"..\..\DTE\Core\PQueue.h"\
	"..\..\DTE\Core\queue.h"\
	"..\..\DTE\Core\site.h"\
	"..\..\DTE\Core\SockStream.h"\
	"..\..\DTE\Core\Sort.h"\
	"..\..\DTE\Core\StandardRead.h"\
	"..\..\DTE\Core\url.h"\
	
NODEP_CPP_SORT_=\
	"..\..\DTE\Core\sysdep.h"\
	

"$(INTDIR)\Sort.obj" : $(SOURCE) $(DEP_CPP_SORT_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "dte - Win32 Debug"

DEP_CPP_SORT_=\
	"..\..\DTE\Core\Array.h"\
	"..\..\DTE\Core\exception.h"\
	"..\..\DTE\Core\Inserter.h"\
	"..\..\DTE\Core\Iterator.h"\
	"..\..\DTE\Core\listop.h"\
	"..\..\DTE\Core\MemoryMgr.h"\
	"..\..\DTE\Core\myopt.h"\
	"..\..\DTE\Core\PQueue.h"\
	"..\..\DTE\Core\queue.h"\
	"..\..\DTE\Core\site.h"\
	"..\..\DTE\Core\SockStream.h"\
	"..\..\DTE\Core\Sort.h"\
	"..\..\DTE\Core\StandardRead.h"\
	"..\..\DTE\Core\url.h"\
	"..\..\unidata\sysdep.h"\
	

"$(INTDIR)\Sort.obj" : $(SOURCE) $(DEP_CPP_SORT_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\DTE\Core\StandardRead.c

!IF  "$(CFG)" == "dte - Win32 Release"

DEP_CPP_STAND=\
	"..\..\DTE\Core\Array.h"\
	"..\..\DTE\Core\exception.h"\
	"..\..\DTE\Core\ExecExpr.h"\
	"..\..\DTE\Core\ExecOp.h"\
	"..\..\DTE\Core\Iterator.h"\
	"..\..\DTE\Core\queue.h"\
	"..\..\DTE\Core\SockStream.h"\
	"..\..\DTE\Core\StandardRead.h"\
	"..\..\DTE\Core\url.h"\
	
NODEP_CPP_STAND=\
	"..\..\DTE\Core\sysdep.h"\
	

"$(INTDIR)\StandardRead.obj" : $(SOURCE) $(DEP_CPP_STAND) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "dte - Win32 Debug"

DEP_CPP_STAND=\
	"..\..\DTE\Core\Array.h"\
	"..\..\DTE\Core\exception.h"\
	"..\..\DTE\Core\ExecExpr.h"\
	"..\..\DTE\Core\ExecOp.h"\
	"..\..\DTE\Core\Iterator.h"\
	"..\..\DTE\Core\queue.h"\
	"..\..\DTE\Core\SockStream.h"\
	"..\..\DTE\Core\StandardRead.h"\
	"..\..\DTE\Core\url.h"\
	"..\..\unidata\sysdep.h"\
	

"$(INTDIR)\StandardRead.obj" : $(SOURCE) $(DEP_CPP_STAND) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\DTE\Core\TypeCheck.c

!IF  "$(CFG)" == "dte - Win32 Release"

DEP_CPP_TYPEC=\
	"..\..\DTE\Core\Array.h"\
	"..\..\DTE\Core\catalog.h"\
	"..\..\DTE\Core\exception.h"\
	"..\..\DTE\Core\Interface.h"\
	"..\..\DTE\Core\listop.h"\
	"..\..\DTE\Core\myopt.h"\
	"..\..\DTE\Core\queue.h"\
	"..\..\DTE\Core\SockStream.h"\
	"..\..\DTE\Core\StringLess.h"\
	"..\..\DTE\Core\TypeCheck.h"\
	"..\..\DTE\Core\url.h"\
	"..\..\DTE\Core\Utility.h"\
	
NODEP_CPP_TYPEC=\
	"..\..\DTE\Core\sysdep.h"\
	

"$(INTDIR)\TypeCheck.obj" : $(SOURCE) $(DEP_CPP_TYPEC) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "dte - Win32 Debug"

DEP_CPP_TYPEC=\
	"..\..\DTE\Core\Array.h"\
	"..\..\DTE\Core\catalog.h"\
	"..\..\DTE\Core\exception.h"\
	"..\..\DTE\Core\Interface.h"\
	"..\..\DTE\Core\listop.h"\
	"..\..\DTE\Core\myopt.h"\
	"..\..\DTE\Core\queue.h"\
	"..\..\DTE\Core\SockStream.h"\
	"..\..\DTE\Core\StringLess.h"\
	"..\..\DTE\Core\TypeCheck.h"\
	"..\..\DTE\Core\url.h"\
	"..\..\DTE\Core\Utility.h"\
	"..\..\unidata\sysdep.h"\
	

"$(INTDIR)\TypeCheck.obj" : $(SOURCE) $(DEP_CPP_TYPEC) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\DTE\Core\types.c

!IF  "$(CFG)" == "dte - Win32 Release"

DEP_CPP_TYPES=\
	"..\..\DTE\Core\catalog.h"\
	"..\..\DTE\Core\exception.h"\
	"..\..\DTE\Core\Interface.h"\
	"..\..\DTE\Core\queue.h"\
	"..\..\DTE\Core\Utility.h"\
	
NODEP_CPP_TYPES=\
	"..\..\DTE\Core\DateTime.h"\
	"..\..\DTE\Core\sysdep.h"\
	

"$(INTDIR)\types.obj" : $(SOURCE) $(DEP_CPP_TYPES) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "dte - Win32 Debug"

DEP_CPP_TYPES=\
	"..\..\DTE\Core\catalog.h"\
	"..\..\DTE\Core\exception.h"\
	"..\..\DTE\Core\Interface.h"\
	"..\..\DTE\Core\queue.h"\
	"..\..\DTE\Core\Utility.h"\
	"..\..\unidata\DateTime.h"\
	"..\..\unidata\sysdep.h"\
	

"$(INTDIR)\types.obj" : $(SOURCE) $(DEP_CPP_TYPES) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\DTE\Core\UnionParse.c

!IF  "$(CFG)" == "dte - Win32 Release"

DEP_CPP_UNION=\
	"..\..\DTE\Core\Array.h"\
	"..\..\DTE\Core\exception.h"\
	"..\..\DTE\Core\Iterator.h"\
	"..\..\DTE\Core\listop.h"\
	"..\..\DTE\Core\myopt.h"\
	"..\..\DTE\Core\ParseTree.h"\
	"..\..\DTE\Core\queue.h"\
	"..\..\DTE\Core\site.h"\
	"..\..\DTE\Core\SockStream.h"\
	"..\..\DTE\Core\StringLess.h"\
	"..\..\DTE\Core\TypeCheck.h"\
	"..\..\DTE\Core\url.h"\
	
NODEP_CPP_UNION=\
	"..\..\DTE\Core\sysdep.h"\
	

"$(INTDIR)\UnionParse.obj" : $(SOURCE) $(DEP_CPP_UNION) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "dte - Win32 Debug"

DEP_CPP_UNION=\
	"..\..\DTE\Core\Array.h"\
	"..\..\DTE\Core\exception.h"\
	"..\..\DTE\Core\Iterator.h"\
	"..\..\DTE\Core\listop.h"\
	"..\..\DTE\Core\myopt.h"\
	"..\..\DTE\Core\ParseTree.h"\
	"..\..\DTE\Core\queue.h"\
	"..\..\DTE\Core\site.h"\
	"..\..\DTE\Core\SockStream.h"\
	"..\..\DTE\Core\StringLess.h"\
	"..\..\DTE\Core\TypeCheck.h"\
	"..\..\DTE\Core\url.h"\
	"..\..\unidata\sysdep.h"\
	

"$(INTDIR)\UnionParse.obj" : $(SOURCE) $(DEP_CPP_UNION) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\DTE\Core\url.c

!IF  "$(CFG)" == "dte - Win32 Release"

DEP_CPP_URL_C=\
	"..\..\DTE\Core\exception.h"\
	"..\..\DTE\Core\SockStream.h"\
	"..\..\DTE\Core\url.h"\
	
NODEP_CPP_URL_C=\
	"..\..\DTE\Core\sysdep.h"\
	

"$(INTDIR)\url.obj" : $(SOURCE) $(DEP_CPP_URL_C) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "dte - Win32 Debug"

DEP_CPP_URL_C=\
	"..\..\DTE\Core\exception.h"\
	"..\..\DTE\Core\SockStream.h"\
	"..\..\DTE\Core\url.h"\
	"..\..\unidata\sysdep.h"\
	

"$(INTDIR)\url.obj" : $(SOURCE) $(DEP_CPP_URL_C) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\DTE\Core\Util.c

!IF  "$(CFG)" == "dte - Win32 Release"

DEP_CPP_UTIL_=\
	"..\..\DTE\Core\exception.h"\
	"..\..\DTE\Core\Inserter.h"\
	"..\..\DTE\Core\Utility.h"\
	
NODEP_CPP_UTIL_=\
	"..\..\DTE\Core\sysdep.h"\
	

"$(INTDIR)\Util.obj" : $(SOURCE) $(DEP_CPP_UTIL_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "dte - Win32 Debug"

DEP_CPP_UTIL_=\
	"..\..\DTE\Core\exception.h"\
	"..\..\DTE\Core\Inserter.h"\
	"..\..\DTE\Core\Utility.h"\
	"..\..\unidata\sysdep.h"\
	

"$(INTDIR)\Util.obj" : $(SOURCE) $(DEP_CPP_UTIL_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\DTE\ODBC\WSInit.cpp

!IF  "$(CFG)" == "dte - Win32 Release"

NODEP_CPP_WSINI=\
	"..\..\DTE\ODBC\sysdep.h"\
	

"$(INTDIR)\WSInit.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "dte - Win32 Debug"

DEP_CPP_WSINI=\
	"..\..\unidata\sysdep.h"\
	

"$(INTDIR)\WSInit.obj" : $(SOURCE) $(DEP_CPP_WSINI) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 


!ENDIF 

