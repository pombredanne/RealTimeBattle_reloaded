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


#include "RobotMessageIO.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;

RobotMessageIO::RobotMessageIO( string in_filename ) {
  outstreamp = NULL;
  instreamp = NULL;
  filename = in_filename;
  nb_tic = 1;
}

bool
RobotMessageIO::start_process( Robot* ) {
  infile.open( filename.c_str() );
  if( !infile ) return false;
  outstreamp = new ofstream();
  instreamp = new stringstream;
  return true;
}

void RobotMessageIO::notify_tic( ) {
  instreamp->clear();

  if( (--nb_tic) != 0 ) return;
  char msg[256];

  instreamp->clear();
  while( 1 ) {
    infile >> ws;
    if( infile.eof() ) return;

    infile.get( msg, 256, '\n');

    if( msg[0] == 't' ) {
    	istringstream iss( msg );
	char c;
	iss >> c >> nb_tic;
	if( !iss ) {
	  nb_tic = 1;
	}
    	return;
    } 

    if( msg[0] != '-' )
      (* (stringstream*)instreamp) << msg << endl;
  }
}

RobotMessageIO::~RobotMessageIO() {
  delete outstreamp;
  delete instreamp;
}


