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


/* A brut force dispatcher for collision... to be improved */

#include "ArenaObject.h"
#include "MovingObject.h"
#include "Robot.h"
#include "Shot.h"
#include "Wall.h"


#include <iostream>



namespace collision {

void collide( Robot&, Robot& );
void collide( Robot&, Shot& );
void collide( Robot&, Wall& );
void collide( Shot&, Shot& );
void collide( Shot&, Wall& );

void
dispatch( MovingObject& lhs, ArenaObject& rhs ) {
  if( Robot* r1 = dynamic_cast<Robot*>(&lhs) ) {
    std::cerr << "Robot against something" << std::endl;

  } else if( Shot* s1 = dynamic_cast<Shot*>( &lhs ) ) {
    std::cerr << "Shot against something" << std::endl;
  }

}




}