/*
RealTimeBattle, a robot programming game for Unix
Copyright (C) 1998-2002  Erik Ouchterlony and Ragnar Ouchterlony
copyright (C) 2003       Benoit Rousseau

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

#ifndef __OPTIONS__
#define __OPTIONS__

#include <string>
#include <vector>
#include "Various.h"
#include "Messagetypes.h"

  using namespace std;

enum option_double_t
{
  OPTION_GRAV_CONST,
  OPTION_AIR_RESISTANCE,
  OPTION_ROLL_FRICTION,
  OPTION_SLIDE_FRICTION,
  OPTION_ROBOT_MAX_ACCELERATION,
  OPTION_ROBOT_MIN_ACCELERATION,

  OPTION_ROBOT_RADIUS,
  OPTION_ROBOT_MASS,

  OPTION_ROBOT_BOUNCE_COEFF,
  OPTION_ROBOT_HARDNESS,
  OPTION_ROBOT_PROTECTION,

  OPTION_ROBOT_FRONT_BOUNCE_COEFF,
  OPTION_ROBOT_FRONT_HARDNESS,
  OPTION_ROBOT_FRONT_PROTECTION,

  OPTION_ROBOT_FRONTSIZE,

  OPTION_ROBOT_START_ENERGY,
  OPTION_ROBOT_MAX_ENERGY,

  OPTION_ROBOT_MAX_ROTATE,
  OPTION_ROBOT_CANNON_MAX_ROTATE,
  OPTION_ROBOT_RADAR_MAX_ROTATE,

  OPTION_SHOT_RADIUS,
  OPTION_SHOT_SPEED,
  OPTION_SHOOTING_PENALTY,
  OPTION_SHOT_MIN_ENERGY,
  OPTION_SHOT_MAX_ENERGY,
  OPTION_SHOT_ENERGY_INCREASE_SPEED,

  OPTION_COOKIE_MAX_ENERGY,
  OPTION_COOKIE_MIN_ENERGY,
  OPTION_COOKIE_FREQUENCY,
  OPTION_COOKIE_RADIUS,

  OPTION_MINE_MAX_ENERGY,
  OPTION_MINE_MIN_ENERGY,
  OPTION_MINE_FREQUENCY,
  OPTION_MINE_RADIUS,

  OPTION_ARENA_SCALE,
  //  OPTION_FAST_FORWARD_FACTOR,

  OPTION_TIMEOUT,
  OPTION_MAX_TIMESTEP,
  OPTION_TIMESCALE,
  OPTION_UPDATE_INTERVAL,
  OPTION_ROBOT_STARTUP_TIME,

  //  OPTION_CPU_START_LIMIT,
  //  OPTION_CPU_EXTRA,
  //  OPTION_CPU_PERIOD,
  //  OPTION_CPU_WARNING_PERCENT,

  OPTION_CHECK_INTERVAL,

  LAST_DOUBLE_OPTION
};

enum option_long_t
{
  OPTION_SEND_ROBOT_COORDINATES,
  OPTION_MAX_ROBOTS_ALLOWED, 

  OPTION_ROBOT_ENERGY_LEVELS,

  OPTION_BACKGROUND_COLOUR,
  OPTION_FOREGROUND_COLOUR,
  //  OPTION_RTB_MESSAGE_COLOUR,
  OPTION_COOKIE_COLOUR,
  OPTION_MINE_COLOUR,

  OPTION_LOG_EVERY_NTH_UPDATE_INTERVAL,

  LAST_LONG_OPTION
};

enum option_string_t
{
  OPTION_ROBOT_SEARCH_PATH,
  OPTION_ARENA_SEARCH_PATH,
  OPTION_TMP_RTB_DIR,
  LAST_STRING_OPTION
};

enum option_bool_t
{
  LAST_BOOL_OPTION
};


class OptionBase {
 public:
  OptionBase(const entry_datatype_t d, const int l,
	     const bool bo, const bool lo,
	     const string & s, const string& t ) :
    datatype( d ), max_letters_in_entry( l ), broadcast_option( bo ),
    log_option( lo ), label( s ), translated_label( t ) 
    {}

  entry_datatype_t datatype;
  int max_letters_in_entry;
  bool broadcast_option;
  bool log_option;
  string label;
  string translated_label;    
};

class DoubleOption : public OptionBase {
 public:
  DoubleOption( const double v, const double mn, const double mx, const int l,
		const bool bo, const bool lo, const string& s,
		const string& t ) :
    OptionBase( ENTRY_DOUBLE, l, bo, lo, s, t ), value( v ), default_value( v ), min_value(mn),
    max_value(mx) {}

  double value;
  double default_value;
  double min_value;
  double max_value;
};

class LongOption : public OptionBase {
 public:
  LongOption( entry_datatype_t d, 
	      const long v, const long mn, const long mx, const int l,
	      const bool bo, const bool lo, const string& s,
	      const string& t ) :
    OptionBase( d, l, bo, lo, s, t ), value( v ), default_value( v ), min_value(mn),
    max_value(mx)
    {}

  long value;
  long default_value;
  long min_value;
  long max_value;
};

class StringOption : public OptionBase {
 public:
  StringOption(const string v, const int l,
	       const bool bo, const bool lo, 
	       const string& s, const string& t) :
    OptionBase(ENTRY_CHAR, l, bo, lo, s, t) ,
    value(v), default_value(v)
    {};

  string value;
  string default_value;
};



struct option_return_t
{
  option_return_t( const entry_datatype_t d, const int n ) :
    datatype(d), option_number(n) {}

  option_return_t& operator=( const option_return_t& n )
    { datatype = n.datatype; option_number = n.option_number; return *this; }

  entry_datatype_t datatype;
  int option_number;
};



class Options
{
 public:
  Options                                      ();
  ~Options                                     ();
  //static void set_options();

  inline double get_d                          ( option_double_t  option )
    { return all_double_options[option]->value; }
  inline long   get_l                          ( option_long_t    option )
    { return all_long_options[option]->value; }
  inline const string get_s                    ( option_string_t  option )
    { return all_string_options[option]->value; }

  void broadcast_opts                          ();
  void log_all_options                         ();

  void save_all_options_to_file                ( string filename,
                                                 const bool as_default );
  void get_options_from_rtbrc                  ();
  void read_options_file                       ( string file_string,
                                                 const bool as_default );

  option_return_t get_option_from_string       ( const string& option_name );

 protected:
  string make_option_string( game_option_type opt, double value ) ;
  string make_double_option_string( int i ) ;
  string make_long_option_string( int i ) ;
  string make_string_option_string( int i ) ;

 private:
  vector<DoubleOption*> all_double_options;
  vector<LongOption*>   all_long_options;
  vector<StringOption*> all_string_options;

  //  option_info_t<bool> all_bool_options[LAST_BOOL_OPTION];

};

extern Options the_opts;
 
#endif 
