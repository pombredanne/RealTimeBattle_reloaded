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

#include "RobotTool.h"
#include "Robot.h"
#include "Structs.h"
#include "ArenaBase.h"
#include "Various.h"

bool 
RobotTool::update( const double timestep ) {
  pos += timestep * vel;
  bool rot_reached = false;

  if( pos >= right && mode == ROTATE_TO_RIGHT )
    {
      set_rot( right, 0.0, -infinity, infinity, NORMAL_ROT);
      if( owner->send_rotation_reached >= 1 ) rot_reached = true;
    }

  if( pos >= right && mode == SWEEP_RIGHT )
    {
      set_rot( right, -vel, left, right, SWEEP_LEFT);
      if( owner->send_rotation_reached >= 2 ) rot_reached = true;
    }
  
  if( pos <= left && mode == ROTATE_TO_LEFT )
    {      
      set_rot( left, 0.0, -infinity, infinity, NORMAL_ROT);
      if( owner->send_rotation_reached >= 1 ) rot_reached = true;
    }

  if( pos <= left && mode == SWEEP_LEFT )
    {
      set_rot( left, -vel, left, right, SWEEP_RIGHT);
      if( owner->send_rotation_reached >= 2 ) rot_reached = true;
    }

  return rot_reached;
}

void 
RobotTool::set_pos( double p ) {
  pos = p;
}

Vector2D
RobotTool::dir() {
  return angle2vec( pos );
}


void
RobotTool::rotate_to( double rot_speed, double rot_end_angle ) {
  pos -= rint( (pos-rot_end_angle) / (2.0*M_PI) ) * 2.0 * M_PI;
  double rot_amount = rot_end_angle - pos;
  rotate_amount( rot_speed, rot_amount );
}

void
RobotTool::rotate_amount( double rot_speed, double rot_amount ) {
  if( rot_amount > 0 )
    set_rot( pos, rot_speed,
	     -infinity, pos + rot_amount, 
	     ROTATE_TO_RIGHT );
  else
    set_rot( pos, -rot_speed,
	     pos + rot_amount, infinity, 
	     ROTATE_TO_LEFT );
}

void
RobotTool::sweep( double rot_speed, double sweep_left, double sweep_right ) {
  rotation_mode_t rot_dir;
  rot_dir = ( rot_speed < 0 ? SWEEP_LEFT :  SWEEP_RIGHT );
		
  pos -= rint( (pos - 0.5*(sweep_left+sweep_right)) / (2.0*M_PI) ) * 2.0 * M_PI;
  if( various::abs(vel) > 1e-10 ) 
    rot_dir = ( vel < 0 ? SWEEP_LEFT :  SWEEP_RIGHT );
		    
  if( pos <= sweep_left  ) rot_dir = SWEEP_RIGHT;
  if( pos >= sweep_right ) rot_dir = SWEEP_LEFT;
		    
  double speed = rot_speed;
  if( rot_dir == SWEEP_LEFT ) speed = -rot_speed;    
  set_rot( pos, speed, 
	   sweep_left, sweep_right, rot_dir );
}


