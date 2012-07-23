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

PROJECT (RealtimeBattle_reloaded)

SET (SRC
	src/Broadcast.cc
	src/Dialog.cc
	src/MessageWindow.cc
	src/Vector2D.cc
	src/Structs.cc
	src/Various.cc
	src/RealTimeBattle.cc
	src/ArenaWindow.cc
	src/getopt.c
	src/ArenaBase.cc
	src/ArenaReplay.cc
	src/Robot.cc
	src/ArenaController.cc
	src/GeometricalObjects.cc
	src/ControlWindow.cc
	src/ArenaRealTime.cc
	src/ClientInterface.cc
	src/StatisticsWindow.cc
	src/Timer.cc
	src/OptionsWindow.cc
	src/StartTournamentWindow.cc
	src/Shape.cc
	src/ScoreWindow.cc
	src/Options.cc
	src/Extras.cc
	src/Gui.cc
	src/Shot.cc)

INCLUDE_DIRECTORIES(include)

#Magic code (I unknown how to run) for gtk check.
FIND_PACKAGE (PkgConfig REQUIRED)
PKG_CHECK_MODULES (GTK2 REQUIRED gtk+-2.0)
INCLUDE_DIRECTORIES (${GTK2_INCLUDE_DIRS})
LINK_DIRECTORIES (${GTK2_LIBRARY_DIRS})
ADD_DEFINITIONS (${GTK2_CFLAGS_OTHER})

ADD_DEFINITIONS(-D VERSION=666 -D NDEBUG -g)

ADD_EXECUTABLE(RealtimeBattle_reloaded ${SRC})

TARGET_LINK_LIBRARIES(RealtimeBattle_reloaded ${GTK2_LIBRARIES})