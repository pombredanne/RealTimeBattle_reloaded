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

#include <fcntl.h>
#include <iostream>
#include <math.h>

#ifdef TIME_WITH_SYS_TIME 
# include <sys/time.h>
# include <time.h>
#else
# if HAVE_SYS_TIME_H
#  include <sys/time.h>
# else
#  include <time.h>
# endif
#endif

#include <stdarg.h>
#include <stdio.h>
#include <string>
#include <sstream>
#include <set>
#include <signal.h>

using namespace std;

#include "Robot.h"
#include "ArenaController.h"
#include "Various.h"
#include "Options.h"
#include "Wall.h"
#include "Shot.h"
#include "Extras.h"
#include "ShapeVisitor.h"
#include "ArenaObserver.h"
#include "Colour.h"
#include "RobotController.h"
#include "RobotProcess.h"

Robot::Robot(const int r_id, const string& filename, const int team, RobotIO* his_io )
{
  velocity = Vector2D(0.0, 0.0);
  acceleration = 0.0;
  robot_filename = filename;
  
  my_team = team;

  my_io = his_io;
  id = r_id;

  plain_robot_name = "";
  robot_name = "";
  robot_name_uniqueness_number = 0;

  extra_air_resistance = 0.0;
  process_running = false;

  send_rotation_reached = 0;
  alive = false;
  dead_but_stats_not_set = false;

  tools.push_back( RobotTool( this ) );  //Engine
  tools.push_back( RobotTool( this ) );  //Cannon
  tools.push_back( RobotTool( this ) );  //Radar

  tools[0].set_max_rotate( the_opts.get_d(OPTION_ROBOT_CANNON_MAX_ROTATE) );
  tools[1].set_max_rotate( the_opts.get_d(OPTION_ROBOT_CANNON_MAX_ROTATE) );
  tools[2].set_max_rotate( the_opts.get_d(OPTION_ROBOT_RADAR_MAX_ROTATE) );

  compatible_with_this_version = false;

  mass = the_opts.get_d(OPTION_ROBOT_MASS);
}


Robot::~Robot()
{
  if( the_arena_controller.is_realtime() )
    {
      if( is_process_running() ) kill_process_forcefully();
      delete_pipes();
    }
} 

void
Robot::send_init_message () {
  send_message( INITIALIZE, 1 );
} 

void
Robot::live()
{
  alive = true;
  dead_but_stats_not_set = false;
}

void
Robot::die()
{
  cerr<<"DIE!!!"<< energy <<endl;
  if( alive )
    {
      alive = false;
      dead_but_stats_not_set = true;
      the_arena.remove_moveable( this );
    }
}

void
Robot::change_position( const double x, const double y,
                        const double robot_a, const double cannon_a, 
                        const double radar_a, const double en )
{
  get_shape()->set_center( Vector2D(x, y) );
  tools[0].set_pos( robot_a );
  tools[1].set_pos( cannon_a );
  tools[2].set_pos( radar_a );
  energy = en;
}

double
Robot::max_rotate_speed( double rot_speed, const set<int>& to_rotate ) {
  set<int>::const_iterator si;
  rot_speed = abs( rot_speed );
  for( si = to_rotate.begin(); si != to_rotate.end(); si ++ ) {
    rot_speed = various::min( rot_speed, tools[(*si)].get_max_rotate() );
  }
  return rot_speed;
}


void
Robot::update_radar_and_cannon(const double timestep)
{
  ostringstream rot_reached_str;
  if( compatible_with_this_version )
    {
      set<int> reached_s;

      for( int i = 0 ; i < tools.size() ; i ++ ) {
	if( tools[i].update( timestep ) ) reached_s.insert( i+1 );
      }

      if( ! reached_s.empty() ) {
	set<int>::iterator si = reached_s.begin();
	rot_reached_str << (*si);
	while( (++si) != reached_s.end() )
	  rot_reached_str<<","<<(*si);
	rot_reached_str<<".";

	send_message(ROTATION_REACHED, rot_reached_str.str().c_str());
      }
    }
  else
    {
      int rot_reached = 0; int power = 1;
      for( int i = 0 ; i < tools.size() ; i ++ ) {
	if( tools[i].update( timestep ) ) rot_reached += power;
	power *= 2;
      }

      if( rot_reached != 0 ) {
	rot_reached_str<<rot_reached;
	send_message(ROTATION_REACHED, rot_reached_str.str().c_str());
      }
    }

  shot_energy = min( the_opts.get_d(OPTION_SHOT_MAX_ENERGY),
                     shot_energy+timestep*the_opts.get_d(OPTION_SHOT_ENERGY_INCREASE_SPEED) );

  object_type closest_arenaobject;
  ArenaObject* col_obj;
  double dist = the_arena.
    get_shortest_distance(get_shape()->get_center(),
                          angle2vec(tools[0].get_pos()+tools[2].get_pos()),
                          0.0, col_obj, this);

  send_message(RADAR, dist, closest_arenaobject, tools[2].get_pos());
  if( closest_arenaobject == ROBOT )
    {
      double lvls = (double)the_opts.get_l(OPTION_ROBOT_ENERGY_LEVELS);
      double en = ((Robot*)col_obj)->get_energy();
      if( ! compatible_with_this_version ) {
	send_message(ROBOT_INFO, rint( en / lvls ) * lvls, 0);
      } else { /* ( compatible_with_this_version ) */
	send_message(EXTRA_ROBOT_INFO, rint( en / lvls ) * lvls,
		     ((Robot*)col_obj)->get_id(), ((Robot*)col_obj)->my_team);
      }
    }

  Vector2D center = get_shape()->get_center();

  if( the_opts.get_l(OPTION_SEND_ROBOT_COORDINATES) == 1 ) // Relative starting pos
    send_message(COORDINATES, center[0] - start_pos[0], center[1] - start_pos[1],
                 tools[0].get_pos() - start_angle);

  if( the_opts.get_l(OPTION_SEND_ROBOT_COORDINATES) == 2 ) // Absolute coordinates
    send_message(COORDINATES, center[0], center[1], tools[0].get_pos());

  send_message(INFO, the_arena_controller.get_total_time(), length(velocity), tools[1].get_pos() );
}


void
Robot::reset_values()
{
  start_pos = get_shape()->get_center();
  start_angle = tools[0].get_pos();

  for( int i = 1; i < tools.size() ; i++ ) {
    tools[i].set_rot( 0.0,   0.0, -infinity, infinity, NORMAL_ROT );
  }

  shot_energy = 0.0;
  get_shape()->set_radius( the_opts.get_d(OPTION_ROBOT_RADIUS) );
  energy = the_opts.get_d(OPTION_ROBOT_START_ENERGY);
  velocity = Vector2D(0.0, 0.0);
  position_this_game = 0;
  //  points_this_game = 0.0;
  brake_percent = 0.0;
  acceleration = 0.0;
}

void
Robot::set_pos( const Vector2D& pos, const double angle) {
  get_shape()->set_center( pos );
  tools[0].set_rot( angle, 0.0, -infinity, infinity, NORMAL_ROT );
}

void
Robot::set_values_at_process_start_up()
{
  process_running = true;
  time_survived_in_sequence = 0.0;
  send_message(INITIALIZE, 1); //Compatibility with robot of RTB 1.0.5
  colour_given = false;
  name_given = false;
}


Vector2D
Robot::own_acceleration ( const double timestep ) {
  Vector2D dir = tools[0].dir();

  double gt = the_opts.get_d(OPTION_GRAV_CONST) * timestep;
  double fric = the_opts.get_d(OPTION_ROLL_FRICTION) * (1.0 - brake_percent) +
    the_opts.get_d(OPTION_SLIDE_FRICTION) * brake_percent;

  Vector2D acc = timestep*acceleration*dir +
    dot(velocity, dir) * max(0.0, 1.0-gt*fric) * dir +
    vedge(dir, velocity) * max(0.0, 1.0-gt*the_opts.get_d(OPTION_SLIDE_FRICTION)) * rotate90(dir);

  return acc ;
}


/*void
Robot::bounce_on_wall(Shape* w, const Vector2D& normal)
{
/*
  double h, p, b;
  double bounce_c = w->get_bounce_coeff()  ;
  double hardness_c = w->get_hardness_coeff() ;


  if( -dot(normal,angle2vec(tools[0].get_pos())) > cos(the_opts.get_d(OPTION_ROBOT_FRONTSIZE)*0.5) )
    {
      h = the_opts.get_d(OPTION_ROBOT_FRONT_HARDNESS);
      b = the_opts.get_d(OPTION_ROBOT_FRONT_BOUNCE_COEFF);
      p = the_opts.get_d(OPTION_ROBOT_FRONT_PROTECTION);
    }
  else
    {
      h = the_opts.get_d(OPTION_ROBOT_HARDNESS);
      b = the_opts.get_d(OPTION_ROBOT_BOUNCE_COEFF);
      p = the_opts.get_d(OPTION_ROBOT_PROTECTION);
    }

  double e = b * bounce_c;
  Vector2D start_vel = velocity;
  velocity -= (1.0 + e) * dot(normal, velocity) * normal;

  double en_diff = 0.5 * the_opts.get_d(OPTION_ROBOT_MASS) * lengthsqr(start_vel - velocity);
  double injury = en_diff * 0.5 * (h + hardness_c ) * (1.0-e) * (1.0-p);
  change_energy(-injury);

  send_message(COLLISION, WALL, vec2angle(-normal)-tools[0].get_pos());
*/
//}

void
bounce_on_robot(Robot& robot1, Robot& robot2, const Vector2D& normal)
{
/*
  double h1, h2, p1, p2, b1, b2;
  Vector2D dir1_2 = unit(robot2.center - robot1.center);

  if( dot(dir1_2, angle2vec(robot1.tools[0].get_pos())) >
       cos(the_opts.get_d(OPTION_ROBOT_FRONTSIZE)*0.5) )
    {
      h1 = the_opts.get_d(OPTION_ROBOT_FRONT_HARDNESS);
      b1 = the_opts.get_d(OPTION_ROBOT_FRONT_BOUNCE_COEFF);
      p1 = the_opts.get_d(OPTION_ROBOT_FRONT_PROTECTION);
    }
  else
    {
      h1 = the_opts.get_d(OPTION_ROBOT_HARDNESS);
      b1 = the_opts.get_d(OPTION_ROBOT_BOUNCE_COEFF);
      p1 = the_opts.get_d(OPTION_ROBOT_PROTECTION);
    }

  if( -dot(dir1_2, angle2vec(robot2.tools[0].get_pos() )) >
      cos(the_opts.get_d(OPTION_ROBOT_FRONTSIZE)*0.5) )
    {
      h2 = the_opts.get_d(OPTION_ROBOT_FRONT_HARDNESS);
      b2 = the_opts.get_d(OPTION_ROBOT_FRONT_BOUNCE_COEFF);
      p2 = the_opts.get_d(OPTION_ROBOT_FRONT_PROTECTION);
    }
  else
    {
      h2 = the_opts.get_d(OPTION_ROBOT_HARDNESS);
      b2 = the_opts.get_d(OPTION_ROBOT_BOUNCE_COEFF);
      p2 = the_opts.get_d(OPTION_ROBOT_PROTECTION);
    }

  double e = b1*b2;
  Vector2D start_vel1 = robot1.velocity;
  Vector2D start_vel2 = robot2.velocity;
  double mass = the_opts.get_d(OPTION_ROBOT_MASS);
  Vector2D tmp = ((1.0 + e) / 2.0) * dot(robot2.velocity - robot1.velocity, normal) * normal;
  robot1.velocity += tmp;
  robot2.velocity -= tmp;

  double an = vec2angle(-normal);
  double en_diff = 0.5 * mass * lengthsqr(start_vel1 - robot1.velocity);
  double injury = en_diff * 0.5 * (h1 + h2) * (1.0-e) * (1.0-p1);
  robot1.change_energy(-injury);
  robot1.send_message(COLLISION, ROBOT, an-robot1.tools[0].get_pos());

  an = vec2angle(normal);
  en_diff = 0.5 * mass * lengthsqr(start_vel2 - robot2.velocity);
  injury = en_diff * 0.5 * (h1 + h2) * (1.0-e) * (1.0-p2);
  robot2.change_energy(-injury);
  robot2.send_message(COLLISION, ROBOT, an-robot2.tools[0].get_pos());
*/
}



void
Robot::move(const double timestep)
{

  the_arena.ground_interaction( this );

  //TODO : Update all the tools...
  if( timestep != 0 ) {
    update_radar_and_cannon(timestep);
    change_velocity(timestep);
  }

  MovingObject::move( timestep );
  //move( timestep, 1, timestep / 50.0 );

  Vector2D center = get_shape()->get_center();
  // if( is_alive() )
  Robot_Position_Info_Event evt(id, center[0], center[1],
				tools[0].get_pos(),
				tools[1].get_pos(), tools[2].get_pos(),
				energy );
  the_arena_controller.notify_event( &evt );
}

/* TODO : Remove this function as soon as possible */
/*void
Robot::move( const double timestep, const int iterstep, const double eps ) {
  object_type closest_shape = ROBOT;
  ArenaObject* colliding_object;
  Vector2D center = get_shape()->get_center();
  double time_to_collision =
    the_arena.get_shortest_distance(center, velocity, get_shape()->get_radius(),
                                    colliding_object, this);
  if( time_to_collision > timestep )
    {
      get_shape()->set_center( center + timestep*velocity );
    }
  else
    {
      if( iterstep > 10 ) time_to_collision = max( time_to_collision , eps );
      double time_remaining = timestep - time_to_collision;
      get_shape()->set_center( center+= time_to_collision*velocity );

      //      Vector2D new_center = center - min(eps, time_to_collision)*velocity;


      switch( closest_shape )
        {
        case WALL:
          {
            Vector2D normal = colliding_object->get_normal(center);
            //bounce_on_wall(colliding_object, normal);
	    get_shape()->set_center( center+=normal*eps );
          }
          break;
        case ROBOT:
          {
            Vector2D normal = colliding_object->get_normal(center);
            bounce_on_robot(*this, *(Robot*)colliding_object, normal);
            time_remaining = 0.0;
          }
          break;
        case SHOT:
          {
            Shot* shotp =(Shot*)colliding_object;

            double en =  -shotp->get_energy();
            change_energy( en );
            send_message(COLLISION, SHOT, vec2angle(shotp->get_shape()->get_center()-center)
	                                  -tools[0].get_pos());
            shotp->die();
            //the_arena.get_object_lists()[SHOT].remove( shotp );
          }
          break;
        case COOKIE:
	case MINE:
          {
            Extras* extra = (Extras*)colliding_object;
            double en =  extra->get_energy();
            change_energy( en );
            send_message(COLLISION, COOKIE,
	                 vec2angle(extra->get_shape()->get_center()-center)-tools[0].get_pos());
            extra->die();
          }
          break;
        default:
          Error(true, "Collided with unknown object", "Robot::move");
          break;
        }

      if( iterstep > 65 ) Error(true, "Too many bounces, must be a bug!", "Robot::move");
      if( alive && time_remaining > 0.0 ) move( time_remaining, iterstep + 1, eps );
    }

}
*/


void
Robot::send_message(const message_to_robot_type msg_type ...)
{
  va_list args;
  va_start(args, msg_type);
  (*get_outstreamp()) << message_to_robot[msg_type].msg << " ";
  for(int i=0; i<message_to_robot[msg_type].number_of_args; i++)
    {
      switch(message_to_robot[msg_type].arg_type[i])
        {
        case NONE:
          Error(true, "Couldn't send message, no arg_type", "Robot::send_message");
          break;
        case INT:
          (*get_outstreamp()) << va_arg(args, int) << " ";
          break;
        case DOUBLE:
          (*get_outstreamp()) << va_arg(args, double) << " ";
          break;
        case STRING:
          (*get_outstreamp()) << va_arg(args, char*) << " ";
          break;
        case HEX:
          (*get_outstreamp()) << hex << va_arg(args, int) << " ";
          break;
        default:
          Error(true, "Couldn't send message, unknown arg_type", "Robot::send_message");
        }
    }
  (*get_outstreamp()) << endl;
}


// This is the function which gets messages from the robot,
// parses them and take appropriate action. It is certainly
// too long, but who cares :-)
void
Robot::get_messages()
{
  char buffer[81];
  char text[161];
  char msg_name[81];
  message_from_robot_type msg_t;

  my_io->notify_tic();

  (*get_instreamp()) >> ws;
  get_instreamp()->clear();
  get_instreamp()->peek();

  while( !((*get_instreamp()) >> msg_name).eof() )
    {
      msg_t = name2msg_from_robot_type(msg_name);

      (*get_instreamp()) >> ws;
      switch(msg_t)
        {
        case UNKNOWN_MESSAGE_FROM_ROBOT:
          send_message(WARNING, UNKNOWN_MESSAGE, msg_name);
          get_instreamp()->get(buffer, 80, '\n');
          break;
        case ROBOT_OPTION:
          if( check_state_for_message(msg_t, STARTING_ROBOTS) )
            {
	      int opt_nr, val;
	      (*get_instreamp()) >> opt_nr;
	      switch(opt_nr)
		{
		case SEND_SIGNAL:
		  (*get_instreamp()) >> val;
		  if( typeid( *my_io ) == typeid( RobotProcess ) )
		    ((RobotProcess*)my_io)->set_signal( val==true, SIGUSR1 );
		  send_signal();
		  break;
		case SIGNAL:
		  (*get_instreamp()) >> val;
		  if( val > 0 && val < NSIG )
		    {
		      if( typeid( *my_io ) == typeid( RobotProcess ) )
			((RobotProcess*)my_io)->set_signal( true, val );
		      send_signal();
		    }
		  else
		    {
		      if( val >= NSIG ) send_message(WARNING, UNKNOWN_OPTION, msg_name);
		      if( typeid( *my_io ) == typeid( RobotProcess ) )
			((RobotProcess*)my_io)->set_signal( false, 0 );
		    }
		  break;
		case SEND_ROTATION_REACHED:
                  (*get_instreamp()) >> val;
                  if( val < 0 ) val = 0;
                  if( val > 2 ) val = 2;
                  send_rotation_reached = val;
                  break;
                case USE_NON_BLOCKING:
                  (*get_instreamp()) >> val;
		  if( typeid( *my_io ) == typeid( RobotProcess ) )
		    ((RobotProcess*)my_io)->set_non_blocking_state( val );
                  break;
		case RTB_1_1_X_COMPATIBLE:
		  ((*get_instreamp())) >> compatible_with_this_version;

		  if( compatible_with_this_version ) //Compatible with RTB 1.1.0 
		    {
		      send_message( TEAM_INFO, get_id(), my_team );
		      send_message( ROBOT_EQUIPMENT, 1, "Engine", "{}" );
		      send_message( ROBOT_EQUIPMENT, 2, "Cannon", "{}" );
		      send_message( ROBOT_EQUIPMENT, 3, "Radar", "{}" );
		    }
		  break;
		  
                default:
                  send_message(WARNING, UNKNOWN_OPTION, msg_name);
                  break;
                }
            }
          break;
        case NAME:
          if( check_state_for_message(msg_t, STARTING_ROBOTS) )
            {
              get_instreamp()->get(text, 80, '\n');
              plain_robot_name = text;
              robot_name = the_robot_controller.check_name_uniqueness( this );
              name_given = true;
            }
          break;
        case COLOUR:
          if( check_state_for_message(msg_t, STARTING_ROBOTS) )
            {
              long home_colour, away_colour;
              (*get_instreamp()) >> hex >> home_colour >> away_colour >> dec;
              // TODO: check if colour is already allocated! 
              set_colour( ColourServer::getHandle().find_free_colour(home_colour, away_colour) );
              colour_given = true;
            }
          break;
        case ROTATE:
          if( check_state_for_message(msg_t, GAME_IN_PROGRESS) )
            { 
	      set<int> to_rotate;
	      if( !get_gadget_id_set( to_rotate, true ) ) {
		send_message(WARNING, UNKNOWN_MESSAGE, "rotate : bad first arg");
		get_instreamp()->get(buffer, 80, '\n');
	      } else {
		double rot_speed;
		(*get_instreamp()) >> rot_speed;

		double rot_sign = sgn(rot_speed);
		rot_speed = rot_sign * max_rotate_speed( rot_speed, to_rotate );

		for( set<int>::iterator si = to_rotate.begin(); si !=to_rotate.end(); si++ ) {
		  tools[(*si)].rotate( rot_speed );
		}
	      }
	    }
          break;

        case ROTATE_TO:
          if( check_state_for_message(msg_t, GAME_IN_PROGRESS) )
            {
	      set<int> to_rotate;

	      if( !get_gadget_id_set( to_rotate ) ) {
		send_message(WARNING, UNKNOWN_MESSAGE, "rotate : bad first arg");
		get_instreamp()->get(buffer, 80, '\n');
	      } else {
		double rot_speed, rot_end_angle;
		(*get_instreamp()) >> rot_speed >> rot_end_angle;
		rot_end_angle = various::max( various::min(rot_end_angle, infinity), -infinity);

		rot_speed = max_rotate_speed( rot_speed, to_rotate );
		
		for( set<int>::iterator si = to_rotate.begin(); si != to_rotate.end() ; si ++ )
		  tools[(*si)].rotate_to( rot_speed, rot_end_angle );
	      } //end if (entry_not_ok) ... else ...
	    }
          break;

        case ROTATE_AMOUNT:
          if( check_state_for_message(msg_t, GAME_IN_PROGRESS) )
            {
	      set<int> to_rotate;

	      if( ! get_gadget_id_set( to_rotate ) ) {
		send_message(WARNING, UNKNOWN_MESSAGE, "rotate : bad first arg");
		get_instreamp()->get(buffer, 80, '\n');
	      } else {
		double rot_speed, rot_amount;
		(*get_instreamp()) >> rot_speed >> rot_amount;
		
		rot_speed = max_rotate_speed( rot_speed, to_rotate );

		for( set<int>::iterator si = to_rotate.begin(); si != to_rotate.end(); si++ ) 
		  tools[(*si)].rotate_amount( rot_speed, rot_amount );
	      }
            }
          break;
        case SWEEP:
          if( check_state_for_message(msg_t, GAME_IN_PROGRESS) )
            {
	      set<int> to_rotate;

	      if( !get_gadget_id_set( to_rotate ) ) {
		send_message(WARNING, UNKNOWN_MESSAGE, "rotate : bad first arg");
		get_instreamp()->get(buffer, 80, '\n');
	      } else {
		double rot_speed, sweep_left, sweep_right;
		(*get_instreamp()) >> rot_speed >> sweep_left >> sweep_right;

		rot_speed = max_rotate_speed( rot_speed, to_rotate );
		sweep_left = max(min(sweep_left, infinity), -infinity);
		sweep_right = max(min(sweep_right, infinity), -infinity);

		for( set<int>::iterator si = to_rotate.begin(); si != to_rotate.end(); si++ )
		  tools[(*si)].sweep( rot_speed, sweep_left, sweep_right );
	      }
	    }
          break;
        case PRINT:
          {
            get_instreamp()->get(text, 160, '\n');

	    Robot_Print_Message_Event evt( id, text );
            the_arena_controller.notify_event( & evt );
          }
          break;
	case TO_TEAM_MESSAGE:
	  {
	    get_instreamp()->get(text, 160, '\n');
	    the_robot_controller.broadcast_team( this , text );
	  }
        case DEBUG:
        case DEBUG_LINE:
        case DEBUG_CIRCLE:
	  get_instreamp()->get(text, 160, '\n');
	  break;

        case SHOOT:
          if( check_state_for_message(msg_t, GAME_IN_PROGRESS) )
            {
              double en;
              (*get_instreamp()) >> en;
              en = min(en, shot_energy);
              if( en < the_opts.get_d(OPTION_SHOT_MIN_ENERGY) ) break;
              shot_energy -= en;

              Vector2D dir = angle2vec(tools[1].get_pos()+tools[0].get_pos());
              double shot_radius = the_opts.get_d(OPTION_SHOT_RADIUS);
              Vector2D shot_center = get_shape()->get_center() +
	                             (get_shape()->get_radius()+1.5*shot_radius)*dir;
              Vector2D shot_vel = velocity + dir * the_opts.get_d(OPTION_SHOT_SPEED);

              if( the_arena.space_available( shot_center, shot_radius*1.00001 ) )
                {
                  Shot* shotp = new Shot( shot_center, shot_vel, en );
		  the_arena.add_moveable( shotp );

		  Shot_Event evt ( shotp->get_id(),
				   shot_center[0], shot_center[1],
				   shot_vel[0], shot_vel[1]);
                  the_arena_controller.notify_event( &evt );
                }
              else  // No space for shot, direct hit!!
                {
                  ArenaObject* col_obj;
                  object_type cl_shape;
                  double dist;
                  if( (dist = the_arena.get_shortest_distance
                       ( get_shape()->get_center(), dir, shot_radius*1.00001,
                         col_obj, this))    >   get_shape()->get_radius()+1.5*shot_radius )
                    {
                      Error(false, "Shot has space available after all?", "Robot::get_messages");
                    }
                  switch(cl_shape)
                    {
                    case WALL:
                      break;
                    case ROBOT:
                      {
                        Robot* robotp = (Robot*)col_obj;
                        robotp->change_energy(-en);
			double a = vec2angle(get_shape()->get_center()+
			                    dir*get_shape()->get_radius()-
					    robotp->get_shape()->get_center()) -
				   robotp->get_robot_angle();

                        robotp->send_message( COLLISION, SHOT, a );
                      }
                      break;
                    case SHOT:
                      shot_collision((Shot*)col_obj, shot_vel, en);
                      break;
                    case COOKIE:
		    case MINE:
                      {
                        Extras* extrap = (Extras*)col_obj;
                        extrap->die();
                      }
                      break;
                    default:
                      Error(true, "Shot on unknown object", "Robot::get_messages");
                    }
                }
              change_energy(-en * the_arena.get_shooting_penalty() );
            }
          break;
        case ACCELERATE:
          if( check_state_for_message(msg_t, GAME_IN_PROGRESS) )
            {
              double acc;
              (*get_instreamp()) >> acc;
              acc = max( acc, the_opts.get_d(OPTION_ROBOT_MIN_ACCELERATION) );
              acc = min( acc, the_opts.get_d(OPTION_ROBOT_MAX_ACCELERATION) );
              acceleration = acc;
            }
          break;
        case BREAK:  // Included only for compatibility reasons
          send_message(WARNING, OBSOLETE_KEYWORD, msg_name);
        case BRAKE:
          if( check_state_for_message(msg_t, GAME_IN_PROGRESS) )
            {
              double brk;
              (*get_instreamp()) >> brk;
              brk = max( brk, 0.0);
              brk = min( brk, 1.0);
              brake_percent = brk;
            } 
          break;

        default:
          Error(true, "Message_type not implemented, " + (string)msg_name, "Robot::get_messages");
        }

      (*get_instreamp()) >> ws;
      get_instreamp()->clear();
      get_instreamp()->peek();
    }
}


bool 
Robot::get_gadget_id_set( set<int>& gad_set, bool accept_engine ) {
  int c;

  if( compatible_with_this_version ) {
    (*get_instreamp()) >> ws ; c = get_instreamp()->peek();

    if( isdigit( c ) ) { //just one object to move
      int i;
      (*get_instreamp()) >> i ;
      gad_set.insert( i ) ;
    } else if ( c == '{' ) {
      get_instreamp()->get();
      
      for( bool is_parsed = false ; is_parsed != true ; ) {
	int first, last;
	
	(*get_instreamp()) >> ws ; c = get_instreamp()->peek();
	if( isdigit( c ) ) {
	  (*get_instreamp()) >> first ;
	}
	
	(*get_instreamp()) >> ws ; c = get_instreamp()->peek();
	if( c == ',' || c == '}' ) { //Just one (more) object to add 
	  gad_set.insert( first );
	} else if ( c == '-' ) {
	  get_instreamp()->get();
	
	  (*get_instreamp()) >> ws ; c = get_instreamp()->peek();
	  if( isdigit( c ) ) {
	    (*get_instreamp()) >> last;
	    if( first > last ) //sweep them
	      { int j = first; first = last; last = j; }
	    for( int i = first; i <= last; i ++ )
	      gad_set.insert( i );
	  } 
	}
	
	(*get_instreamp()) >> ws ; c = get_instreamp()->get();
	if( c == '}' ) 
	  is_parsed = false;
	else if( c != ',' )
	  {
	    return false;  //Erroneous
	  }
      } //end for( is_parsed ... )
    } else {
      return false; //Erroneous
    }
  } else {
    int bits;
    (*get_instreamp()) >> bits ;
    if( bits & 1 ) gad_set.insert( 0 );
    if( bits & 2 ) gad_set.insert( 1 );
    if( bits & 4 ) gad_set.insert( 2 );
  }

  if( !accept_engine && gad_set.find( 0 ) != gad_set.end() )
    gad_set.erase( 0 );

  return true; //Everything is ok
}


message_from_robot_type
Robot::name2msg_from_robot_type(char* msg_name)
{
  for(int i=0; message_from_robot[i].msg[0] != '\0'; i++)
    {
      if( strcmp(message_from_robot[i].msg, msg_name) == 0 )
        return (message_from_robot_type)i;
    }
  return UNKNOWN_MESSAGE_FROM_ROBOT;
}

bool
Robot::check_state_for_message(const message_from_robot_type msg_t, const state_t state1, const state_t state2)
{
  return true;
  state_t state = the_arena_controller.get_state();

  if( state != state1 && state != state2 )
    {
      if( state != BEFORE_GAME_START )
	send_message(WARNING, MESSAGE_SENT_IN_ILLEGAL_STATE, 
		     message_from_robot[msg_t].msg);
      char buffer[80];
      get_instreamp()->get(buffer, 80, '\n');
      return false;
    }
  return true;
}




void Robot::start_process() {
  my_io->start_process( this );
  set_values_at_process_start_up();
  send_signal();
}

bool Robot::is_process_running() {
  return process_running;
}

void Robot::end_process() {
  send_message(EXIT_ROBOT);
  my_io->end_process();
}

void Robot::send_signal() {
  my_io->send_signal();
}

void Robot::kill_process_forcefully() {
  my_io->kill_process_forcefully();
  process_running = false;
}

void Robot::delete_pipes() {
  my_io->delete_pipes();
}




