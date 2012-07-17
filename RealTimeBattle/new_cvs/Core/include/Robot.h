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

#ifndef __ROBOT__
#define __ROBOT__

#include <sys/types.h>
#include <unistd.h>

#include "Messagetypes.h"

#include "Structs.h"
#include "MovingObject.h"
#include "RobotTool.h"
#include <list>
#include <set>
#include <vector>
#include <ios>
#include <string>


//! A virtual class to define Interactions with the robot
/*!
  In a first time, RobotProcess and RobotNetwork will herite from this class
*/
class RobotIO {
 public:
  RobotIO() {};
  virtual void notify_tic( ) {} ;
  
  //Imperative functions
  virtual bool start_process( Robot* ) = 0;
  virtual void send_signal() = 0;
  virtual bool is_process_running() {return true;};
  virtual void end_process() = 0;
  virtual void kill_process_forcefully() = 0;


  //Optionnal functions
  virtual void delete_pipes() {};

  std::istream* get_instreamp() { return instreamp; }
  std::ostream* get_outstreamp() { return outstreamp; }
 protected:
  std::istream* instreamp;
  std::ostream* outstreamp;
};


//! The robot class : not much to say in brief
/*!
  Arena state : Function run (in this order ? )
*/
class Robot : public MovingObject
{
public:
  Robot(const int r_id, const string& filename, const int team, RobotIO* his_io );
  ~Robot();

  friend class RobotController;
  friend class RobotTool;
  //friend class RobotIO;
  
  void send_init_message();

  void move(const double timestep);
  //void move( const double timestep, const int iterstep, const double eps );
  //void change_velocity(const double timestep);
  void update_radar_and_cannon(const double timestep);
  bool update_rotation(rotation_t& angle, const double timestep);
  //void bounce_on_wall( Shape* w, const Vector2D& normal );
  //friend void bounce_on_robot(Robot& robot1, Robot& robot2, const Vector2D& normal);
  void change_position( const double x, const double y,
                        const double robot_a, const double cannon_a, 
                        const double radar_a, const double en );

  void get_messages();
  void send_message(enum message_to_robot_type ...);
  void reset_values();
  void set_pos( const Vector2D& pos, double angle = 0 );

  void set_values_at_process_start_up();

  bool is_dead_but_stats_not_set() { return dead_but_stats_not_set; }

  void start_process();
  bool is_process_running();

  void send_signal();
  void end_process();
  void delete_pipes();
  void kill_process_forcefully();

  void live();
  void die();

  std::string get_robot_name() { return robot_name; }
  std::string get_robot_filename() { return robot_filename; }

  int get_position_this_game() { return position_this_game; }
  double get_total_points();
  int get_last_position();

  bool is_colour_given() { return colour_given; }
  void set_colour_given( const bool c ) { colour_given = c; }
  bool is_name_given() { return name_given; }

  double get_robot_angle() { return tools[0].get_pos(); }

  bool set_and_get_has_competed()
    { if( has_competed ) return true; else { has_competed = true; return false; } }


  //TODO : move it to protected !!!
  int my_team;
  bool compatible_with_this_version;

  Vector2D own_acceleration( const double );
  std::ostream* get_outstreamp() { return my_io->get_outstreamp(); }

private:
  //! Return the min of rotate speed of arg1 and the speed of tools in the set
  double max_rotate_speed( double, const set<int>& );

  message_from_robot_type name2msg_from_robot_type(char*);
  bool check_state_for_message(const message_from_robot_type msg_t, const enum state_t state1,
                               const enum state_t state2 = NO_STATE);

  std::istream* get_instreamp() { return my_io->get_instreamp(); }

  //! get which object to move when receive ROTATE or something else (to come later)
  /*! When you want to do some stuff with some gadgets, it is possible to ask for the
    same action for a set of gadgets (ie rotate cannon and radar at the same speed)
    Passing the set of gadgets you want to move with this notation :
    4 or {4}      just to move the gadget that have id == 4 (more formal using '{' and '}'
    {1, 2, 3, 5}  ej !! easy to get !!!
    {0-4, 6}      move gadgets from 0 to 4 (included) and object 6
    {7-2} is interpreted as {2-7} even if it is not so readable for the robot coder !
    \return bool is false if ok, true if there as been an error
    0 usualy refers to the engine, 1 to the cannon, 2 to the radar
  */
  bool get_gadget_id_set( set<int>& gad_set, bool accept_engine = false );

  bool process_running;

  int  send_rotation_reached;
  bool has_competed;

  double extra_air_resistance;
  double brake_percent;

  std::vector<RobotTool> tools;

  double acceleration;
  double shot_energy;

  Vector2D start_pos;
  double start_angle;

  int robot_name_uniqueness_number;
  std::string plain_robot_name;      // Name given by robot
  std::string robot_name;            // plain_robot_name + uniqueness number

  std::string robot_filename;        // Filename with path  
  std::string robot_plain_filename;  // Filename without path

  bool colour_given;
  bool name_given;
  
  int position_this_game;
  double time_survived_in_sequence;
  bool dead_but_stats_not_set;

  RobotIO* my_io;
};

#endif /* __ROBOT__ */
