/*
RealTimeBattle, a robot programming game for Unix
Copyright (C) 1998-2002  Erik Ouchterlony and Ragnar Ouchterlony
Copyright (C) 2003       Benoit Rousseau

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software Foundation,
Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/


#ifndef __TEXT_LOG__
#define __TEXT_LOG__

#include "fdstream.h"
#include <string>

#include "ArenaObserver.h"

//!This is the observer called when we want to log the game out
/*!
  Instances of this class are added to the ArenaController observers list
  when the user ask for a log file
*/
class TextLog : public ArenaObserver {
 public:
  TextLog( std::string filename );
  virtual ~TextLog();

  void notify_event( Observable_Event * ) ; 
  void tic( double );

 protected:
  boost::fdostream *LOG_FILE;

  int prec;
};

#endif
