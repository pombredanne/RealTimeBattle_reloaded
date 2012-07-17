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


#include <math.h>


#include "Colour.h"
#include "Various.h"

//initialize the static singleton
ColourServer ColourServer::cs;


long int 
ColourServer::find_free_colour( const long int home_colour, const long int away_colour, 
			   const bool random_colours )
{
  long int tmp_colour;

  //Do it 10 times, reducing the min distance
  for(double min_dist = 0.1; min_dist > 0.01 ; min_dist *= 0.8)
    {
      if( !random_colours )
        {
          if( is_colour_allowed(home_colour, min_dist) ) return add_colour( home_colour );
          if( is_colour_allowed(away_colour, min_dist) ) return add_colour( away_colour );
        }
      for(int i=0; i<25; i++)
        {
          tmp_colour = rand() & 0xffffff;
          if( is_colour_allowed(tmp_colour, min_dist*2) ) return add_colour( tmp_colour );
        }                  
    }
   Error(true, "Impossible to find colour", "ArenaRealTime::find_free_colour");

   return 0;
}

long int
ColourServer::add_colour( const long int colour ) {
  used_colour.insert( colour );
  return colour;
}


bool 
ColourServer::is_colour_allowed( const long int colour, double min_dist ) 
{
  double d = 1.0;
  set<long int>::iterator li;

  for( li = used_colour.begin();
       li != used_colour.end(); li ++ )
    {
      d = colour_dist( colour, (*li) );
      if( d < min_dist ) return false;          
    }
  
  if( d < min_dist ) return false;

  return true;  
}

// coulor_dist is supposed to be a subjective distance between two colours, normalized
// to be between 0.0 (same colour) and 1.0 (furthest away).
double
ColourServer::colour_dist(const long col1, const long int col2)
{
  double red1   = (col1 & 0x0000ff);
  double green1 = (col1 & 0x00ff00) >> 8;
  double blue1  = (col1 & 0xff0000) >> 16;

  double red2   = (col2 & 0x0000ff);
  double green2 = (col2 & 0x00ff00) >> 8;
  double blue2  = (col2 & 0xff0000) >> 16;

  return  ( fabs(red1  -red2  ) * log(1.0 + sqrt(red1  +red2  )) + 
            fabs(green1-green2) * log(1.0 + sqrt(green1+green2)) +
            fabs(blue1 -blue2 ) * log(1.0 + sqrt(blue1 +blue2 )) ) / 2417.8;
}



