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

#ifndef __COLOUR_SERVER_H__
#define __COLOUR_SERVER_H__

#include <set>

  using namespace std;

//!<<Singleton>>ColourServer : used to handle colors.
/*!
  ColourServer handle the robot colors (and possibly other colors...)
  It stores colors as long int in a list and can find free colors for new robots
*/

class ColourServer {
  static ColourServer cs;
  ColourServer( )  {};
  void operator=(ColourServer&);
  ColourServer(const ColourServer& );
  set<long int> used_colour;

  bool is_colour_allowed( const long int, double );
  double colour_dist(const long col1, const long int col2);
 

 public:
  static ColourServer& getHandle() {
    return cs;
  }

  //!find a random color if both home_colour and away_colour are used. store the returned colour
  long int find_free_colour( const long int home_colour, const long int away_colour, 
                             const bool random_colours = false) ;
  long int add_colour( const long int );

  //!remove all colours from the list
  void free_all_colours();
};



#endif


