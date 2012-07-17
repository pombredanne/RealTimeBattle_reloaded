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

#include "MovingObject.h"
#include "Options.h"
#include "ArenaController.h"
#include "ArenaBase.h"

void
MovingObject::move( const double timestep ) {
	move( timestep, 1, timestep / 50.0 );
}


void
MovingObject::move(const double timestep, int iterstep, const double eps)
{
	object_type closest_shape = WALL;
	ArenaObject* colliding_object;
	Vector2D center = get_shape()->get_center();
	double time_to_collision =
	the_arena.get_shortest_distance(center, velocity,
	                                get_shape()->get_radius(),
	                                colliding_object, this);


	//cerr << "Time to collide : " << time_to_collision << " " << closest_shape << endl;
	if( time_to_collision > timestep ) {
	  get_shape()->set_center( center + timestep*velocity );
	} else {
	  if( iterstep > 10 ) time_to_collision = max( time_to_collision , eps );
          double time_remaining = timestep - time_to_collision;

	  cerr << typeid( *this ).name() << "  " << typeid(colliding_object).name() << endl;

	  if( closest_shape == WALL ) {
	    Vector2D normal = colliding_object->get_normal( center );
            //bounce_on_wall(colliding_object->get_bounce_coeff(),
            //               colliding_object->get_hardness_coeff(), normal);

	    center += normal*eps;
	  } else {
	    Vector2D normal = (colliding_object)->get_normal(center);
	    //bounce_on_object( this, (MovingObject*) colliding_object, normal );
            time_remaining = 0.0;
	  }
	  if( iterstep > 65 ) Error(true, "Too many bounces, must be a bug!", "Robot::move");
          if( alive && time_remaining > 0.0 ) move( time_remaining, iterstep + 1, eps );
	}
}

/*
void
MovingObject::bounce_on_wall(const double bounce_c, const double hardness_c, const Vector2D& normal)
{
  double h, p, b;
  h = the_opts.get_d(OPTION_ROBOT_FRONT_HARDNESS);
  b = the_opts.get_d(OPTION_ROBOT_FRONT_BOUNCE_COEFF);
  p = the_opts.get_d(OPTION_ROBOT_FRONT_PROTECTION);

  double e = b * bounce_c;
  Vector2D start_vel = velocity;
  velocity -= (1.0 + e) * dot(normal, velocity) * normal;

  double en_diff = 0.5 * the_opts.get_d(OPTION_ROBOT_MASS) * lengthsqr(start_vel - velocity);
  double injury = en_diff * 0.5 * (h + hardness_c ) * (1.0-e) * (1.0-p);
  change_energy(-injury);

  //send_message(COLLISION, WALL, vec2angle(-normal)-robot_angle.pos);
}

void
bounce_on_object(MovingObject* obj1, MovingObject* obj2, const Vector2D& normal)
{
  double h1, h2, p1, p2, b1, b2;
  Vector2D dir1_2 = unit(obj1->center - obj2->center);

  h1 = the_opts.get_d(OPTION_ROBOT_FRONT_HARDNESS);
  b1 = the_opts.get_d(OPTION_ROBOT_FRONT_BOUNCE_COEFF);
  p1 = the_opts.get_d(OPTION_ROBOT_FRONT_PROTECTION);

  h2 = the_opts.get_d(OPTION_ROBOT_FRONT_HARDNESS);
  b2 = the_opts.get_d(OPTION_ROBOT_FRONT_BOUNCE_COEFF);
  p2 = the_opts.get_d(OPTION_ROBOT_FRONT_PROTECTION);

  double e = b1*b2;
  Vector2D start_vel1 = obj1->velocity;
  Vector2D start_vel2 = obj2->velocity;
  double m1 = obj1->mass;
  double m2 = obj2->mass;
  double mass_sum = m1 + m2;

  Vector2D tmp = (1.0 + e) * dot(obj2->velocity - obj1->velocity, normal) * normal;
  obj1->velocity += m2 /(mass_sum) * tmp;
  obj2->velocity -= m1 /(mass_sum) * tmp;


  //double an = vec2angle(-normal);
  //double en_diff = 0.5 * mass * lengthsqr(start_vel1 - robot1.velocity);
  //double injury = en_diff * 0.5 * (h1 + h2) * (1.0-e) * (1.0-p1);
  //robot1.change_energy(-injury);
  //robot1.send_message(COLLISION, ROBOT, an-robot1.robot_angle.pos);

  //an = vec2angle(normal);
  //en_diff = 0.5 * mass * lengthsqr(start_vel2 - robot2.velocity);
  //injury = en_diff * 0.5 * (h1 + h2) * (1.0-e) * (1.0-p2);
  //robot2.change_energy(-injury);
  //robot2.send_message(COLLISION, ROBOT, an-robot2.robot_angle.pos);

  //mo1->interact( mo2 );
  //mo2->interact( mo1 );

}
*/

bool
MovingObject::change_energy(const double energy_diff)
{
  energy = min(energy+energy_diff, the_opts.get_d(OPTION_ROBOT_MAX_ENERGY));
  if( energy <= 0.0 ) {
    die();
    return false;
  }
  return true;
}


void
MovingObject::change_velocity( const double timestep ) {

  velocity = -velocity* min(the_opts.get_d(OPTION_AIR_RESISTANCE) * timestep, 0.5) +
    own_acceleration( timestep ) + acceleration * timestep;
  acceleration = Vector2D( 0, 0 );
}

//NOTE : velocity1 +=
/*
void
bounce_on_moving_object( MovingObject* mo1, MovingObject* mo2, const Vector2D& normal ) {
  double h1, h2, p1, p2, b1, b2;
  Vector2D dir1_2 = unit(mo1->center - mo2->center)
  h1 = mo1->get_hardness( dir1_2 );
  p1 = mo1->get_protection( dir1_2 );
  b1 = mo1->get_bounce_coeff( dir1_2 );

  dir1_2 = -dir1_2;
  h2 = mo2->get_hardness( dir1_2 );
  p2 = mo2->get_protection( dir1_2 );
  b2 = mo2->get_bounce_coeff( dir1_2 );

  double e = b1*b2;
  Vector2D start_vel1 = mo1->velocity;
  Vector2D start_vel2 = mo2->velocity;

  double mass_sum = m1 + m2
  Vector2D tmp = (1.0 + e ) * dot( start_val2 - start_vel1, normal ) * normal;
  mo1->velocity += m2 /(mass_sum) * tmp;
  mo2->velocity -= m1 /(mass_sum) * tmp;

  Vector2D tmp = ((1.0 + e) / 2.0) * dot( start_vel2 - start_vel1, normal ) * normal;
  mo1->velocity += tmp;
  mo2->velocity -= tmp;

  mo1->interact( mo2 );
  mo2->interact( mo1 );

}
*/
