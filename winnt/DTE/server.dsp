# Microsoft Developer Studio Project File - Name="server" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=server - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "server.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "server.mak" CFG="server - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "server - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "server - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "server - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "server - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "C:\Program Files\DevStudio\MyProjects\DTE\server"
# PROP Intermediate_Dir "C:\Program Files\DevStudio\MyProjects\DTE\server"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /I "P:\devise\okan\DTE\Core" /I "P:\devise\okan\dte\StandAlone" /I "P:\devise\okan\unidata" /I "." /I "C:\Program Files\DevStudio\MyProjects\DTE" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /TP /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ws2_32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "server - Win32 Release"
# Name "server - Win32 Debug"
# Begin Group "windows"

# PROP Default_Filter ""
# Begin Source File

SOURCE="C:\Program Files\DevStudio\MyProjects\DTE\windows\config.h"
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\DTE\Core\Aggregates.c
# End Source File
# Begin Source File

SOURCE=..\..\DTE\Core\Aggregates.h
# End Source File
# Begin Source File

SOURCE=..\..\DTE\Core\Array.h
# End Source File
# Begin Source File

SOURCE=..\..\DTE\Core\catalog.c
# End Source File
# Begin Source File

SOURCE=..\..\DTE\Core\catalog.h
# End Source File
# Begin Source File

SOURCE=..\..\DTE\StandAlone\Common.cpp
# End Source File
# Begin Source File

SOURCE=..\..\DTE\StandAlone\Common.h
# End Source File
# Begin Source File

SOURCE=..\..\unidata\DateTime.C
# End Source File
# Begin Source File

SOURCE=..\..\unidata\DateTime.h
# End Source File
# Begin Source File

SOURCE=..\..\DTE\Core\DeleteParse.c
# End Source File
# Begin Source File

SOURCE=..\..\DTE\Core\DevRead.dummy.c
# End Source File
# Begin Source File

SOURCE=..\..\DTE\Core\DevRead.h
# End Source File
# Begin Source File

SOURCE="C:\Program Files\DevStudio\MyProjects\DTE\djtypes.h"
# End Source File
# Begin Source File

SOURCE=..\..\DTE\Core\DropIndexParse.c
# End Source File
# Begin Source File

SOURCE=..\..\DTE\Core\Engine.c
# End Source File
# Begin Source File

SOURCE=..\..\DTE\Core\Engine.h
# End Source File
# Begin Source File

SOURCE=..\..\DTE\Core\exception.h
# End Source File
# Begin Source File

SOURCE=..\..\DTE\Core\ExecExpr.h
# End Source File
# Begin Source File

SOURCE=..\..\DTE\Core\ExecOp.c
# End Source File
# Begin Source File

SOURCE=..\..\DTE\Core\ExecOp.h
# End Source File
# Begin Source File

SOURCE=..\..\DTE\ODBC\GetHandle.cpp
# End Source File
# Begin Source File

SOURCE=..\..\DTE\ODBC\GetHandle.h
# End Source File
# Begin Source File

SOURCE=..\..\DTE\Core\IndexParse.dummy.c
# End Source File
# Begin Source File

SOURCE=..\..\DTE\StandAlone\InitShut.h
# End Source File
# Begin Source File

SOURCE=..\..\DTE\Core\Inserter.c
# End Source File
# Begin Source File

SOURCE=..\..\DTE\Core\Inserter.h
# End Source File
# Begin Source File

SOURCE=..\..\DTE\Core\InsertParse.c
# End Source File
# Begin Source File

SOURCE=..\..\DTE\Core\Interface.c
# End Source File
# Begin Source File

SOURCE=..\..\DTE\Core\Interface.h
# End Source File
# Begin Source File

SOURCE=..\..\DTE\Core\Iterator.h
# End Source File
# Begin Source File

SOURCE=..\..\DTE\Core\joins.c
# End Source File
# Begin Source File

SOURCE=..\..\DTE\Core\joins.h
# End Source File
# Begin Source File

SOURCE=..\..\DTE\Core\lex.yy.c
# End Source File
# Begin Source File

SOURCE=..\..\DTE\Core\listop.c
# End Source File
# Begin Source File

SOURCE=..\..\DTE\Core\listop.h
# End Source File
# Begin Source File

SOURCE=..\..\DTE\Core\MaterializeParse.c
# End Source File
# Begin Source File

SOURCE=..\..\DTE\Core\MemoryMgr.c
# End Source File
# Begin Source File

SOURCE=..\..\DTE\Core\MemoryMgr.h
# End Source File
# Begin Source File

SOURCE=..\..\DTE\Core\MinMax.c
# End Source File
# Begin Source File

SOURCE=..\..\DTE\Core\MinMax.h
# End Source File
# Begin Source File

SOURCE=..\..\DTE\Core\my.yacc.tab.c
# End Source File
# Begin Source File

SOURCE=..\..\DTE\Core\my.yacc.tab.h
# End Source File
# Begin Source File

SOURCE=..\..\DTE\Core\myopt.h
# End Source File
# Begin Source File

SOURCE=..\..\DTE\ODBC\ODBC.cpp
# End Source File
# Begin Source File

SOURCE=..\..\DTE\ODBC\ODBC.h
# End Source File
# Begin Source File

SOURCE=..\..\DTE\ODBC\ODBCInterface.cpp
# End Source File
# Begin Source File

SOURCE=..\..\DTE\ODBC\ODBCIterator.cpp
# End Source File
# Begin Source File

SOURCE=..\..\DTE\ODBC\ODBCIterator.h
# End Source File
# Begin Source File

SOURCE=..\..\DTE\ODBC\ODBCSite.cpp
# End Source File
# Begin Source File

SOURCE=..\..\DTE\ODBC\ODBCSite.h
# End Source File
# Begin Source File

SOURCE=..\..\DTE\Core\ParseTree.h
# End Source File
# Begin Source File

SOURCE=..\..\DTE\Core\PQueue.c
# End Source File
# Begin Source File

SOURCE=..\..\DTE\Core\PQueue.h
# End Source File
# Begin Source File

SOURCE=..\..\DTE\Core\QueryTree.c
# End Source File
# Begin Source File

SOURCE=..\..\DTE\Core\queue.h
# End Source File
# Begin Source File

SOURCE=..\..\DTE\Core\resolve.c
# End Source File
# Begin Source File

SOURCE=..\..\DTE\Core\RTreeRead.dummy.c
# End Source File
# Begin Source File

SOURCE=..\..\DTE\Core\RTreeRead.h
# End Source File
# Begin Source File

SOURCE=..\..\DTE\StandAlone\SBMInit.dummy.c
# End Source File
# Begin Source File

SOURCE=..\..\DTE\Core\SchemaParse.c
# End Source File
# Begin Source File

SOURCE=..\..\DTE\StandAlone\Server.cpp
# End Source File
# Begin Source File

SOURCE=..\..\DTE\Core\site.c
# End Source File
# Begin Source File

SOURCE=..\..\DTE\Core\site.h
# End Source File
# Begin Source File

SOURCE=..\..\DTE\Core\SockStream.c
# End Source File
# Begin Source File

SOURCE=..\..\DTE\Core\SockStream.h
# End Source File
# Begin Source File

SOURCE=..\..\DTE\Core\Sort.c
# End Source File
# Begin Source File

SOURCE=..\..\DTE\Core\Sort.h
# End Source File
# Begin Source File

SOURCE=..\..\DTE\Core\StandardRead.c
# End Source File
# Begin Source File

SOURCE=..\..\DTE\Core\StandardRead.h
# End Source File
# Begin Source File

SOURCE=..\..\DTE\Core\StringLess.h
# End Source File
# Begin Source File

SOURCE=..\..\unidata\sysdep.h
# End Source File
# Begin Source File

SOURCE=..\..\DTE\Core\TypeCheck.c
# End Source File
# Begin Source File

SOURCE=..\..\DTE\Core\TypeCheck.h
# End Source File
# Begin Source File

SOURCE=..\..\DTE\Core\types.c
# End Source File
# Begin Source File

SOURCE=..\..\DTE\Core\types.h
# End Source File
# Begin Source File

SOURCE=..\..\DTE\Core\UnionParse.c
# End Source File
# Begin Source File

SOURCE="C:\Program Files\DevStudio\MyProjects\DTE\unistd.h"
# End Source File
# Begin Source File

SOURCE=..\..\DTE\Core\url.c
# End Source File
# Begin Source File

SOURCE=..\..\DTE\Core\url.h
# End Source File
# Begin Source File

SOURCE=..\..\DTE\Core\Util.c
# End Source File
# Begin Source File

SOURCE=..\..\DTE\Core\Utility.h
# End Source File
# Begin Source File

SOURCE=..\..\DTE\ODBC\WSInit.cpp
# End Source File
# End Target
# End Project
