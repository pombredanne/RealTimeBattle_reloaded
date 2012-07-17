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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Robot.h"
#include "Shot.h"
#include "ArenaController.h"
#include "Extras.h"
#include "Various.h"
#include "Options.h"
#include "ArenaObserver.h"

using namespace std;

Shot::Shot(const Vector2D& c, //const double r,
           const Vector2D& vel, const double en, const int shot_id )
  : MovingObject(c, the_opts.get_d(OPTION_SHOT_RADIUS), vel)
{
  alive = true;
  energy = en;

  if( shot_id == -1 )
    id = the_arena.increase_shot_count();
  else
    id = shot_id;
}

void
Shot::move(const double timestep)
{
/*  object_type closest_shape = WALL;
  ArenaObject* colliding_object;

  Vector2D center = get_shape()->get_center();

  double time_to_collision = the_arena.
    get_shortest_distance(center, velocity, get_shape()->get_radius(), colliding_object);

  if( time_to_collision > timestep )
    {
      get_shape()->set_center( center += timestep*velocity );
    }
  else
    {
      switch( closest_shape )
        {
        case WALL:
          die();
          break;
        case ROBOT:
          {
            Robot* robotp = (Robot*)colliding_object;
            robotp->change_energy(-energy);
            robotp->send_message(COLLISION, SHOT,
                                 vec2angle(center-robotp->get_shape()->get_center()) -
				 robotp->get_robot_angle());

            die();
          }
          break;
        case SHOT:
          {
            Shot* shotp = (Shot*)colliding_object;
            shot_collision(this, shotp); 
          }
          break;
        case COOKIE:
          {
            Cookie* cookiep =(Cookie*)colliding_object;
            cookiep->die();
            die();
          }
          break;
        case MINE:
          {
            Mine* minep =(Mine*)colliding_object;
            minep->die();
            die();
          }
          break;
        default:
          Error(true, "Collided with unknown object", "Robot::move");
          break;
        }
    }
*/
}

void
Shot::die( bool remove_me )
{
  if( alive )
    {
      alive = false;
      the_arena.remove_moveable( this );

      Object_Die_Event evt( 'S', id );
      the_arena_controller.notify_event( &evt );
    }
}

void
shot_collision(Shot* shot1p, Shot* shot2p)
{
  shot_collision(shot1p, shot2p->velocity, shot2p->energy);
  shot2p->die( false );
}

void
shot_collision(Shot* shot1p, const Vector2D& shot2_vel, const double shot2_en)
{
  Vector2D vel = ( shot1p->energy * shot1p->velocity + 
                   shot2_en * shot2_vel ) / ( shot1p->energy + shot2_en );

  double en = dot( shot1p->energy * unit(shot1p->velocity) + shot2_en * unit(shot2_vel),  
                   unit(vel));

  if( en < the_opts.get_d(OPTION_SHOT_MIN_ENERGY) || 
      length(vel) < the_opts.get_d(OPTION_SHOT_SPEED) * 0.5 )
    {
      shot1p->die();
    }
  else
    {
      shot1p->velocity = vel;
      shot1p->energy = en;

      Object_Die_Event d_evt( 'S', shot1p->id );
      the_arena_controller.notify_event( &d_evt );

      shot1p->id = the_arena.increase_shot_count();

      Vector2D his_center = shot1p->get_shape()->get_center();

      Shot_Event s_evt( shot1p->id, his_center[0], his_center[1],
			shot1p->velocity[0], shot1p->velocity[1]);
      the_arena_controller.notify_event( &s_evt );
    }
}
