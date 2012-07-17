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

#ifndef __ROBOT_TOOL__
#define __ROBOT_TOOL__

#include "Structs.h"
#include "Vector2D.h"

class Robot;

//! RobotTool is engine, radar and cannon...
class RobotTool {
 public:
  RobotTool( Robot* rp ) : owner( rp ), pos(0), vel(0), left(0), right(0), mode(NORMAL_ROT) {}
  RobotTool( Robot* rp, const double p, const double v, const double l, const double r, const rotation_mode_t m) :
    owner( rp ), pos(p), vel(v), left(l), right(r), mode(m) {}
  void set_rot(const double p, const double v, const double l, const double r, const rotation_mode_t m) 
    { pos=p; vel=v; left=l; right=r; mode=m; }

  void set_pos( const double );
  double get_pos() { return pos; };
  Vector2D dir();

  void rotate( double v ) { vel = v ; }
  void rotate_to( double, double );
  void rotate_amount( double, double );
  void sweep( double, double, double );

  bool update( const double timestep );

  void set_max_rotate( double v ) { max_rotate = v ; }
  double get_max_rotate( ) { return max_rotate; }
double vel;
 protected:
  double pos;               //! The angle
  //double vel;               //! Rotation speed
  double max_rotate;        //! Max Rotate speed
  double left;              //! left limit for sweep
  double right;             //! Right limit for sweep
  rotation_mode_t mode;     //! Rotation mode (sweep, normal, ...)
  Robot* owner;
};


#endif
