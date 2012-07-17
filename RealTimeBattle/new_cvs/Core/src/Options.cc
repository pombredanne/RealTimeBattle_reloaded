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

#include <fstream>
#include <math.h>
#include <stdlib.h>

#ifdef HAVE_LOCALE_H
# include <locale.h>
#endif

#include <sstream>
#include <string>

#include "IntlDefs.h"

#include "Options.h"
#include "IntlDefs.h"
#include "ArenaController.h"
#include "Messagetypes.h"
#include "Various.h"
#include "ArenaObserver.h"
#include "RobotController.h"

//Warning!!! NO COLONS IN LABELS
Options::Options()
{
#ifdef HAVE_LOCALE_H
  setlocale( LC_MESSAGES, "" );
  setlocale( LC_NUMERIC, "POSIX" );
#endif
#ifdef RTB_LOCALEDIR
  bindtextdomain( "RealTimeBattle", RTB_LOCALEDIR );
#else
  bindtextdomain( "RealTimeBattle", "/usr/local/share/locale" );
#endif
  textdomain( "RealTimeBattle" );

  // Entries are: entry_datatype, page in option, 
  //              default value, max value, min value, 
  //              max letters in entry,
  //              broadcast option?, log option?, description

  all_double_options.resize( LAST_DOUBLE_OPTION );
  all_long_options.resize( LAST_LONG_OPTION );
  all_string_options.resize( LAST_STRING_OPTION );

  all_double_options[OPTION_GRAV_CONST] = 
    new DoubleOption( 9.82, 0.2, 20.0, 12,
                          false, true, "Gravitational Constant",
                          _("Gravitational Constant") );

  all_double_options[OPTION_AIR_RESISTANCE] = 
    new DoubleOption( 0.005, 0.0, 1.0, 12,
                          false, true, "Air resistance", _("Air resistance") );

  all_double_options[OPTION_ROLL_FRICTION] = 
    new DoubleOption( 0.002, 0.0, 1.0, 12,
                          false, true, "Roll friction", _("Roll friction") );

  all_double_options[OPTION_SLIDE_FRICTION] = 
    new DoubleOption( 0.1, 0.0, 5.0, 12,
                          false, true, "Slide/brake friction",
                          _("Slide/brake friction") );

  all_long_options[OPTION_SEND_ROBOT_COORDINATES] =
    new LongOption(ENTRY_INT, 0, 0, 2, 4,
		   true, true, "Send robot coordinates", 
		   _("Send robot coordinates") );
  
  all_double_options[OPTION_ROBOT_MAX_ACCELERATION] = 
    new DoubleOption( 2.0, 0.1, 10.0, 12,
                          true, true, "Robot max acceleration",
                          _("Robot max acceleration") );

  all_double_options[OPTION_ROBOT_MIN_ACCELERATION] = 
    new DoubleOption( -0.5, -10.0, 0.0, 12,
                          true, true, "Robot min acceleration",
                          _("Robot min acceleration") );
    
  all_double_options[OPTION_ROBOT_RADIUS] = 
    new DoubleOption( 0.5, 0.1, 10.0, 12, 
                          false, true, "Robot radius", _("Robot radius") );

  all_double_options[OPTION_ROBOT_MASS] = 
    new DoubleOption( 1.0, 0.01, 100.0, 12,
                          false, true, "Robot mass", _("Robot mass") );

  all_double_options[OPTION_ROBOT_BOUNCE_COEFF] = 
    new DoubleOption( 0.7, 0.0, 1.0, 12,
                          false, true, "Robot bounce coefficient",
                          _("Robot bounce coefficient") );

  all_double_options[OPTION_ROBOT_HARDNESS] = 
    new DoubleOption( 0.5, 0.0, 1.0, 12,
                          false, true, "Robot hardness coefficient",
                          _("Robot hardness coefficient") );

  all_double_options[OPTION_ROBOT_PROTECTION] = 
    new DoubleOption( 0.5, 0.0, 1.0, 12,
                          false, true, "Robot protection coefficient",
                          _("Robot protection coefficient") );

  all_double_options[OPTION_ROBOT_FRONTSIZE] = 
    new DoubleOption( M_PI/3.0, 0.0, M_PI*2.0, 12,
                          false, true, "Robot frontsize [radians]", 
                          _("Robot frontsize [radians]") );

  all_double_options[OPTION_ROBOT_FRONT_BOUNCE_COEFF] = 
    new DoubleOption( 0.7, 0.0, 1.0, 12,
                          false, true, "Robot front bounce coefficient",
                          _("Robot front bounce coefficient") );

  all_double_options[OPTION_ROBOT_FRONT_HARDNESS] = 
    new DoubleOption( 0.9, 0.0, 1.0, 12,
                          false, true, "Robot front hardness coefficient",
                          _("Robot front hardness coefficient") );

  all_double_options[OPTION_ROBOT_FRONT_PROTECTION] = 
    new DoubleOption( 0.9, 0.0, 1.0, 12,
                          false, true, "Robot front protection coefficient",
                          _("Robot front protection coefficient") );

  all_double_options[OPTION_ROBOT_START_ENERGY] = 
    new DoubleOption( 100.0, 0.01, 10000.0, 12,
                          true, true, "Robot start energy",
                          _("Robot start energy") );

  all_double_options[OPTION_ROBOT_MAX_ENERGY] = 
    new DoubleOption( 120.0, 0.01, 10000.0, 12,
                          true, true, "Robot max energy",
                          _("Robot max energy") );

  all_double_options[OPTION_ROBOT_MAX_ROTATE] = 
    new DoubleOption( 2.0*M_PI / 8.0, 2.0*M_PI / 50.0,
                          2.0*M_PI * 5.0, 12,
                          true, true, "Robot max rotate speed [rad/s]",
                          _("Robot max rotate speed [rad/s]") );

  all_double_options[OPTION_ROBOT_CANNON_MAX_ROTATE] = 
    new DoubleOption( 2.0*M_PI / 4.0, 2.0*M_PI / 50.0,
                          2.0*M_PI * 5.0, 12,
                          true, true, "Robot cannon max rotate speed [rad/s]",
                          _("Robot cannon max rotate speed [rad/s]") );

  all_double_options[OPTION_ROBOT_RADAR_MAX_ROTATE] = 
    new DoubleOption( 2.0*M_PI / 3.0, 2.0*M_PI / 50.0,
                          2.0*M_PI * 5.0, 12,
                          true, true, "Robot radar max rotate speed [rad/s]",
                          _("Robot radar max rotate speed [rad/s]") );

  all_double_options[OPTION_CHECK_INTERVAL] = 
    new DoubleOption( 1.0, 0.01, 1000000, 12,
                          false, false, "Process check interval",
                          _("Process check interval") );
    
  all_long_options[OPTION_ROBOT_ENERGY_LEVELS] = 
    new LongOption(ENTRY_INT, 10, 1, 100, 4,
                        true, true, "Robot energy levels", _("Robot energy levels") );

  all_double_options[OPTION_SHOT_RADIUS] = 
    new DoubleOption( 0.1, 0.01, 1.0, 12,
                          false, true, "Shot radius", _("Shot radius") );

  all_double_options[OPTION_SHOT_SPEED] = 
    new DoubleOption( 10.0, 0.1, 100.0, 12,
                          true, true, "Shot speed", _("Shot speed") );

  all_double_options[OPTION_SHOOTING_PENALTY] = 
    new DoubleOption( 0.075, 0.0, 1.0, 12,
                          false, true, "Shooting penalty (times shot energy)",
                          _("Shooting penalty (times shot energy)") );

  all_double_options[OPTION_SHOT_MIN_ENERGY] = 
    new DoubleOption( 0.5, 0.0, 100.0, 12,
                          true, true, "Shot min energy", _("Shot min energy") );

  all_double_options[OPTION_SHOT_MAX_ENERGY] = 
    new DoubleOption( 30.0, 0.0, 100000000.0, 12,
                          true, true, "Shot max energy", _("Shot max energy") );

  all_double_options[OPTION_SHOT_ENERGY_INCREASE_SPEED] = 
    new DoubleOption( 10.0, 0.0, 100000000.0, 12,
                          true, true, "Shot energy increase speed [energy/s]",
                          _("Shot energy increase speed [energy/s]") );

  all_long_options[OPTION_BACKGROUND_COLOUR] = 
    new LongOption(ENTRY_HEX, 0xfaf0e6, 0x000000, 0xffffff, 6,
                        false, false, "Background colour", _("Background colour") );

  all_long_options[OPTION_FOREGROUND_COLOUR] = 
    new LongOption(ENTRY_HEX, 0x000000, 0x000000, 0xffffff, 6,
                        false, false, "Foreground colour", _("Foreground colour") );

  /*  all_long_options[OPTION_RTB_MESSAGE_COLOUR] = 
    new LongOption(ENTRY_HEX, 0x1111ee, 0x000000, 0xffffff, 6,
                        false, false, "Colour for RTB messages",
                        _("Colour for RTB messages") );
  */
  all_double_options[OPTION_COOKIE_MAX_ENERGY] = 
    new DoubleOption( 15.0, 0.0, 100000000.0, 12,
                          false, true, "Cookie max energy", _("Cookie max energy") );

  all_double_options[OPTION_COOKIE_MIN_ENERGY] = 
    new DoubleOption( 10.0, 0.0, 100000000.0, 12,
                          false, true, "Cookie min energy", _("Cookie min energy") );

  all_double_options[OPTION_COOKIE_FREQUENCY] = 
    new DoubleOption( 0.03, 0.0, 100000000.0, 12,
                          false, true, "Cookie frequency [cookies per second]",
                          _("Cookie frequency [cookies per second]") );

  all_double_options[OPTION_COOKIE_RADIUS] = 
    new DoubleOption( 0.3, 0.01, 1.0, 12,
                          false, true, "Cookie radius", _("Cookie radius") );

  all_long_options[OPTION_COOKIE_COLOUR] = 
    new LongOption(ENTRY_HEX, 0x35d715, 0x000000, 0xffffff, 6,
                        false, false, "Cookie colour", _("Cookie colour") );

  all_double_options[OPTION_MINE_MAX_ENERGY] = 
    new DoubleOption( 25.0, 0.0, 100000000.0, 12,
                          false, true, "Mine max energy", _("Mine max energy") );

  all_double_options[OPTION_MINE_MIN_ENERGY] = 
    new DoubleOption( 15.0, 0.0, 100000000.0, 12,
                          false, true, "Mine min energy", _("Mine min energy") );

  all_double_options[OPTION_MINE_FREQUENCY] = 
    new DoubleOption( 0.06, 0.0, 100000000.0, 12,
                          false, true, "Mine frequency [mines per second]",
                          _("Mine frequency [mines per second]") );

  all_double_options[OPTION_MINE_RADIUS] = 
    new DoubleOption( 0.3, 0.01, 1.0, 12,
                          false, true, "Mine radius", _("Mine radius") );

  all_long_options[OPTION_MINE_COLOUR] = 
    new LongOption(ENTRY_HEX, 0xff0000, 0x000000, 0xffffff, 6,
                        false, false, "Mine colour", _("Mine colour") );

  all_double_options[OPTION_ARENA_SCALE] = 
    new DoubleOption( 1.0, 0.001, 1000, 12,
                          false, true, "Arena scale", _("Arena scale") );
  /*
  all_double_options[OPTION_FAST_FORWARD_FACTOR] = 
    new DoubleOption( 5.0, 0.001, 1000, 12,
                          false, false, "Fast forward factor",
                          _("Fast forward factor") );
  */
  all_double_options[OPTION_TIMEOUT] = 
    new DoubleOption( 120.0, 1.0, 100000000.0, 12,
                          false, true, "Timeout [s]", _("Timeout [s]") );

  all_double_options[OPTION_MAX_TIMESTEP] = 
    new DoubleOption( 0.1, 0.001, 1.0, 12,
                          false, false, "Max timestep", _("Max timestep") );

  all_double_options[OPTION_TIMESCALE] = 
    new DoubleOption( 1.0, 0.01, 100.0, 12,
                          false, false, "Timescale", _("Timescale") );

  all_double_options[OPTION_UPDATE_INTERVAL] = 
    new DoubleOption( 0.050, 0.010, 1.0, 12,
                          false, false, "Update interval [s]",
                          _("Update interval [s]") );

  all_double_options[OPTION_ROBOT_STARTUP_TIME] = 
    new DoubleOption( 1.0, 0.050, 1000000.0, 12,
                          false, false, "Robot startup time [s]",
                          _("Robot startup time [s]") );
  /*
  all_double_options[OPTION_CPU_START_LIMIT] = 
    new DoubleOption( 5.0, 0.01, 100000000.0, 12,
                          false, false, "Start CPU time [s]", _("Start CPU time [s]") );

  all_double_options[OPTION_CPU_EXTRA] = 
    new DoubleOption( 2.5, 0.01, 100000000.0, 12,
                          false, false, "Extra CPU time [s]", _("Extra CPU time [s]") );

  all_double_options[OPTION_CPU_PERIOD] = 
    new DoubleOption( 60.0, 1.0, 100000000.0, 12,
                          false, false, "Extra CPU period [s]",
                          _("Extra CPU period [s]") );

  all_double_options[OPTION_CPU_WARNING_PERCENT] = 
    new DoubleOption( 0.9, 0.1, 1.0, 12,
                          false, false, "CPU time warning percentage",
                          _("CPU time warning percentage") );
  */
  all_long_options[OPTION_LOG_EVERY_NTH_UPDATE_INTERVAL] =
    new LongOption(ENTRY_INT, 1, 1, 1000, 5,
                        false, false, "Logging frequency [Each nth update interval]",
                        _("Logging frequency [Each n:th update interval]") );

  all_long_options[OPTION_MAX_ROBOTS_ALLOWED] =
    new LongOption(ENTRY_INT, 120, 1, 10000, 6, false, false,
                        "Max robots allowed simultaneously",
                        _("Max robots allowed simultaneously") );
  
  all_string_options[OPTION_ROBOT_SEARCH_PATH] =
    new StringOption("", 1000, false, false, 
			"Robot search path", _("Robot search path") );

  all_string_options[OPTION_ARENA_SEARCH_PATH] =
    new StringOption("", 1000, false, false, 
			"Arena search path", _("Arena search path") );

  all_string_options[OPTION_TMP_RTB_DIR] =
    new StringOption("/tmp/rtb", 1000, false, false, 
			"Directory for temporary files", _("Directory for temporary files") );  
}

Options::~Options() {
  for(int i=0;i<LAST_DOUBLE_OPTION;i++)
    if( all_double_options[i]->log_option )
      {
	delete all_double_options[i];
      } 
                                           
  for(int i=0;i<LAST_LONG_OPTION;i++)
    if( all_long_options[i]->log_option )
      {
	delete all_long_options[i];
      }

  for(int i=0;i<LAST_STRING_OPTION;i++)
    if( all_string_options[i]->log_option )
      {
	delete all_string_options[i];
      }
}

string
Options::make_option_string( game_option_type opt, double value ) {
  string to_return = string(message_to_robot[GAME_OPTION].msg) + " ";

  ostringstream o;
  o << opt << " " << value ;
  to_return = to_return + o.str();
  return to_return;
}

string
Options::make_double_option_string( int i ) {
  ostringstream o;
  o << "D" 
    << all_double_options[i]->label << ":"
    << all_double_options[i]->value ;
  return o.str();
}

string
Options::make_long_option_string( int i ) {
  ostringstream o;
  entry_datatype_t ent_tp = all_long_options[i]->datatype;

  if( ent_tp == ENTRY_INT ) {
    o << "L" 
      << all_long_options[i]->label << ":"
      << all_long_options[i]->value ;
  }
  else if (ent_tp == ENTRY_HEX ) {
    o << "H"
      << all_long_options[i]->label << ":"
      << hex << all_long_options[i]->value << dec;
  }
  return o.str();
}

string
Options::make_string_option_string( int i ) {
  ostringstream o;
  o << "S" 
    << all_string_options[i]->label << ":"
    << all_string_options[i]->value ;
  return o.str();
}

void
Options::broadcast_opts()
{
  the_robot_controller.broadcast( make_option_string( ROBOT_CANNON_MAX_ROTATE, 
						get_d(OPTION_ROBOT_CANNON_MAX_ROTATE)));
  the_robot_controller.broadcast( make_option_string( ROBOT_RADAR_MAX_ROTATE, 
						get_d(OPTION_ROBOT_RADAR_MAX_ROTATE)));
  
  the_robot_controller.broadcast( make_option_string( ROBOT_MAX_ACCELERATION, 
						get_d(OPTION_ROBOT_MAX_ACCELERATION)));
  the_robot_controller.broadcast( make_option_string( ROBOT_MIN_ACCELERATION, 
						get_d(OPTION_ROBOT_MIN_ACCELERATION)));
  
  the_robot_controller.broadcast( make_option_string( ROBOT_START_ENERGY, 
						get_d(OPTION_ROBOT_START_ENERGY)));
  the_robot_controller.broadcast( make_option_string( ROBOT_MAX_ENERGY, 
						get_d(OPTION_ROBOT_MAX_ENERGY)));
  the_robot_controller.broadcast( make_option_string( ROBOT_ENERGY_LEVELS, 
						(double)get_l(OPTION_ROBOT_ENERGY_LEVELS)));
  
  the_robot_controller.broadcast( make_option_string( SHOT_SPEED, 
						get_d(OPTION_SHOT_SPEED)));
  the_robot_controller.broadcast( make_option_string( SHOT_MIN_ENERGY, 
						get_d(OPTION_SHOT_MIN_ENERGY)));
  the_robot_controller.broadcast( make_option_string( SHOT_MAX_ENERGY, 
						get_d(OPTION_SHOT_MAX_ENERGY)));
  the_robot_controller.broadcast( make_option_string( SHOT_ENERGY_INCREASE_SPEED, 
						get_d(OPTION_SHOT_ENERGY_INCREASE_SPEED)));
  
  the_robot_controller.broadcast( make_option_string( TIMEOUT, 
						get_d(OPTION_TIMEOUT)));  
  
  // the_robot_controller.broadcast( make_option_string( DEBUG_LEVEL, 
  // (double)the_robot_controller.get_debug_level()));  
  the_robot_controller.broadcast( make_option_string( SEND_ROBOT_COORDINATES, 
						(double)get_l(OPTION_SEND_ROBOT_COORDINATES)));  
}

void
Options::log_all_options()
{
  for(int i=0;i<LAST_DOUBLE_OPTION;i++)
    if( all_double_options[i]->log_option )
      {
 	Option_Event obs_evt( make_double_option_string( i ) );
	the_arena_controller.notify_event( &obs_evt );
      } 
                                           
  for(int i=0;i<LAST_LONG_OPTION;i++)
    if( all_long_options[i]->log_option )
      {
	Option_Event obs_evt( make_long_option_string( i ) );
	the_arena_controller.notify_event( &obs_evt );
      }

  for(int i=0;i<LAST_STRING_OPTION;i++)
    if( all_string_options[i]->log_option )
      {
	Option_Event obs_evt( make_string_option_string( i ) );
	the_arena_controller.notify_event( &obs_evt );
      }
}

void
Options::get_options_from_rtbrc()
{
  char* home_dir;
  if( NULL == ( home_dir = getenv("HOME") ) )
    return;

  string resource_file = string(home_dir) + "/.rtbrc";
  read_options_file(resource_file,true);
}

void
Options::read_options_file(string file_string, const bool as_default)
{
  ifstream file(file_string.c_str());
  if( !file )
    return;

  for(;;)
    {
      char temp;
      char buffer[1000];
      bool option_found_flag = false;

      file >> ws;
      file.get(buffer,100,':');
      file.get(temp);
      string option_name(buffer);
      if(option_name == "")
        break;

      for(int i=0;i<LAST_DOUBLE_OPTION;i++)
        if(option_name == all_double_options[i]->label )
          {
            double option_value;
            file >> option_value;
            file.get(buffer,100,'\n');
            option_value = min( option_value,
                                all_double_options[i]->max_value );
            option_value = max( option_value,
                                all_double_options[i]->min_value );
            all_double_options[i]->value = option_value;
            if(as_default)
              all_double_options[i]->default_value = option_value;
            option_found_flag = true;
          }

      for(int i=0;i<LAST_LONG_OPTION;i++)
        if( option_name == all_long_options[i]->label )
          {
            long option_value = 0;
            if( all_long_options[i]->datatype == ENTRY_INT )
              file >> option_value;
            if( all_long_options[i]->datatype == ENTRY_HEX )
              {
                file >> hex >> option_value;
              }
            file.get(buffer,100,'\n');
            option_value = min( option_value,
                                all_long_options[i]->max_value );
            option_value = max( option_value,
                                all_long_options[i]->min_value );
            all_long_options[i]->value = option_value;
            if(as_default)
              all_long_options[i]->default_value = option_value;
            option_found_flag = true;
          }

      for(int i=0;i<LAST_STRING_OPTION;i++)
        if( option_name == all_string_options[i]->label )
          {
            string option_value;
            file >> option_value;
            while( option_value[0] == ' ' )
              option_value = option_value.substr(1, option_value.size());
            file.get(buffer,100,'\n');
            all_string_options[i]->value = option_value;
            if(as_default)
              all_string_options[i]->default_value = option_value;
            option_found_flag = true;
          }

      if(!option_found_flag)
        file.get(buffer,1000,'\n');        
    }
}

void
Options::save_all_options_to_file(string filename, const bool as_default)
{
  if(as_default)
    {
      char* home_dir;
      if( NULL == ( home_dir = getenv("HOME") ) )
        return;

      filename = string(home_dir) + "/.rtbrc";
    }

  ofstream file(filename.c_str(), ios::out);

  if( !file )
    {
      cerr << "Couldn't open save file" << endl;
      return;
    }

  for(int i=0;i<LAST_DOUBLE_OPTION;i++)
    file << all_double_options[i]->label << ": " << all_double_options[i]->value << endl;

  for(int i=0;i<LAST_LONG_OPTION;i++)
    {
      if(all_long_options[i]->datatype == ENTRY_INT)
        file << all_long_options[i]->label << ": " << all_long_options[i]->value << endl;
      if(all_long_options[i]->datatype == ENTRY_HEX)
        file << all_long_options[i]->label << ": " 
	     << hex << all_long_options[i]->value << dec
	     << endl;
    }

  for(int i=0;i<LAST_STRING_OPTION;i++)
    file << all_string_options[i]->label << ": " << all_string_options[i]->value << endl;

}

option_return_t
Options::get_option_from_string( const string& option_name )
{
  option_return_t result( ENTRY_INT, -1 );

  for( int i=0; i<LAST_DOUBLE_OPTION; i++ )
    if( option_name == all_double_options[i]->label )
      result = option_return_t( ENTRY_DOUBLE, i );
  for( int i=0; i<LAST_LONG_OPTION; i++ )
    if( option_name == all_long_options[i]->label )
      result = option_return_t( all_long_options[i]->datatype, i );
  for( int i=0; i<LAST_STRING_OPTION; i++ )
    if( option_name == all_string_options[i]->label )
      result = option_return_t( ENTRY_CHAR, i );

  return result;
}

