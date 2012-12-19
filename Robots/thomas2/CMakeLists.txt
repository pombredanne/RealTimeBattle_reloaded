#RealTimeBattle, a robot programming game for Unix
#Copyright (C) 1998-2000  Erik Ouchterlony and Ragnar Ouchterlony
#
#This program is free software; you can redistribute it and/or modify
#it under the terms of the GNU General Public License as published by
#the Free Software Foundation; either version 2 of the License, or
#(at your option) any later version.
#
#This program is distributed in the hope that it will be useful,
#but WITHOUT ANY WARRANTY; without even the implied warranty of
#MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#GNU General Public License for more details.
#
#You should have received a copy of the GNU General Public License
#along with this program; if not, write to the Free Software Foundation,
#Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

cmake_minimum_required(VERSION 2.4)

PROJECT (thomas2)

#FIND_PATH(GLIB_INCLUDE_DIR NAMES glib.h PATH_SUFFIXES glib-2.0)
#message(${GLIB_INCLUDE_DIR})
#INCLUDE_DIRECTORIES(${GLIB_INCLUDE_DIR})

# Not it is good beause the Thommas2 robot only needs glib
#---------INIT MAGICAL CODE FOR CHECK GTK-------------------------------
#Magic code (I unknown how to run) for gtk check.
FIND_PACKAGE (PkgConfig REQUIRED)
PKG_CHECK_MODULES (GTK2 REQUIRED gtk+-2.0)
INCLUDE_DIRECTORIES (${GTK2_INCLUDE_DIRS})
LINK_DIRECTORIES (${GTK2_LIBRARY_DIRS})
ADD_DEFINITIONS (${GTK2_CFLAGS_OTHER})
#---------END MAGICAL CODE FOR CHECK GTK--------------------------------

INCLUDE_DIRECTORIES(../../RealtimeBattle/include/)

SET (SRC
	thomas2.cc)

ADD_EXECUTABLE(thomas2.robot ${SRC})
