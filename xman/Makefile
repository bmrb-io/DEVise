#***************************************************************************
#
# Master Makefile
#
# (C) 1999 Jacob Dreyer - Geotechnical Software Services
# jacob.dreyer@geosoft.no - http://geosoft.no
#
# Modified by Hao Wang - May 14, 2000
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
#
#***************************************************************************

ROOT_DIR   = .

SOURCE_DIR = $(ROOT_DIR)/src
CLASS_DIR  = $(ROOT_DIR)/classes
DOC_DIR    = $(ROOT_DIR)/docs



#***************************************************************************
#
# Section 2: Packages
# 
#***************************************************************************

PACKAGEDIR     = xman
PACKAGE_PREFIX = xman 

PACKAGES = xvis      \
	   shared    \
	   importers \
	   wei       \
	   xnav      \
	   xmanGUI   \
	   xmanMain 


#***************************************************************************
#
# Section 3. Javadoc
#
# These variables define some of the layout of the javadoc HTML
# output. See javadoc manual for details.
#
#***************************************************************************

WINDOWTITLE = 'Experiment Management - XMan'
DOCTITLE    = 'XMan Java API'
HEADER      = 'XMan Java API'
BOTTOM      = 'Copyright 2000 - University of Wisconsin'

JAVADOC     = javadoc

JAVADOC_OPTIONS = \
      -d $(DOC_DIR) \
      -sourcepath $(SOURCE_DIR) \
      -use \
      -splitIndex \
      -windowtitle $(WINDOWTITLE) \
      -doctitle $(DOCTITLE) \
      -header $(HEADER) \
      -bottom $(BOTTOM)



#***************************************************************************
#
# Section 5. Rules and dependencies.
#
# The following commands are valid:
# 
# 1. make classes - Make all class files for all packages
# 2. make         - Same as (1)
# 3. make jni     - Make C/C++ header files for all packages
# 4. make rmi     - Make stub and skeletons for all packages
# 5. make jar     - Make JAR archives for all packages
# 6. make doc     - Make common documentation for all packages
# 7. make clean   - Clean all class directories
#
#***************************************************************************

PACKS = $(PACKAGES:%=$(PACKAGE_PREFIX).%)

CLASS = $(PACKAGES:%=$(SOURCE_DIR)/$(PACKAGEDIR)/%.class)
JNI   = $(PACKAGES:%=$(SOURCE_DIR)/$(PACKAGEDIR)/%.jni)
RMI   = $(PACKAGES:%=$(SOURCE_DIR)/$(PACKAGEDIR)/%.rmi)
JAR   = $(PACKAGES:%=$(SOURCE_DIR)/$(PACKAGEDIR)/%.jar)
DOC   = $(PACKAGES:%=$(SOURCE_DIR)/$(PACKAGEDIR)/%.doc)
CLEAN = $(PACKAGES:%=$(SOURCE_DIR)/$(PACKAGEDIR)/%.clean)


default : classes

classes : $(CLASS)
%.class :
	cd $*; make classes

jni     : $(JNI)
%.jni   :
	cd $*; make jni

jar     : $(JAR)
%.jar   :
	cd $*; make jar

clean   : $(CLEAN)
%.clean :
	cd $*; make clean

all     : classes jni rmi jar

doc     :
	javadoc $(JAVADOC_OPTIONS) $(PACKS)
