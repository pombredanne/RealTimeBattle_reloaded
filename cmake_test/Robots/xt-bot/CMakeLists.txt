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

PROJECT (xt-bot)

SET (SRC_XT-BOT-A
	xt-bot-a.cpp)

SET (SRC_XT-BOT-B
	xt-bot-b.cpp)

INCLUDE_DIRECTORIES(../../RealtimeBattle/include/)

#Generate the two robots
ADD_EXECUTABLE(xt-bot-a.robot ${SRC_XT-BOT-A})

ADD_EXECUTABLE(xt-bot-b.robot ${SRC_XT-BOT-B})