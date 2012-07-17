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


#ifndef __START_ZONE_H__
#define __START_ZONE_H__

#include "Vector2D.h"

class InteractivZone {
  public:
    virtual bool interact( MovingObject* ) = 0;
};


class StartZone {
 public:
  virtual bool place_robot( Robot* ) = 0;
  int team;  //Is there a specific team for this zone (0 if not)
};

class StartPosition : StartZone {
 public:
  StartPosition( int team, Vector2D& position, double dir ) :
    team( team ), position( position ), direction( dir ), any_dir( false ) {}
  StartPosition( int team, Vector2D& position ) :
    team( team ), position( position ), any_dir( true ) {}

  bool place_robot( Robot* );

  Vector2D position;
  double direction;
  bool any_dir;
};

class StartSquare : public StartZone {
};




#endif 
