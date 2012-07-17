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

#ifndef __TIMER__
#define __TIMER__

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
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
#  include <sys/time.h>
#include <unistd.h>

//! Deal with time...
class Timer
{
public:
  
  Timer();
  ~Timer() {}

  //! return the nb of usec elapsed since last reset
  double get();
  //! return total game time
  double get_total_time();
  //! reset the start time
  void reset();

  //! seconds to be elapsed before check return true
  void next_check( double );
  //! Return true if time set by set(double) is elapsed.
  bool check( );

  //! Update total time, return the next timestep
  double update( double = 1.0 );

  //! get a random seed based on start time
  unsigned int get_random_seed() { return (unsigned int)start.tv_usec; }
  //! set a random seed. is seed == 0, the function will do it for you
  unsigned int t_randomize( unsigned int = 0 );
  //! try a number of random numbers and return the last rand nb
  unsigned int t_rand( unsigned int = 1 );

  //! Return the number of random since last time it was called
  unsigned int nb_rand( );

private:
  
  struct timeval start;
  
  double current_timer;

  //! Amount of time to "wait" [in s]
  double next_check_time;

  //! Total time since begining of the game [in s]
  double total_time;

  //! Time step [in s], stored not to access to the option during the game
  double max_timestep; 
};

#endif /* __TIMER__ */
