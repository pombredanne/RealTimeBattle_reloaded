/*
RealTimeBattle, a robot programming game for Unix
Copyright (C) 1998-2000  Erik Ouchterlony and Ragnar Ouchterlony

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

#include <iostream>
using namespace std;

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "Various.h"
#include "Timer.h"
#include "Options.h"


Timer::Timer()
{
  gettimeofday(&start, NULL);
  total_time = 0.0;
  current_timer = 0.0;
  max_timestep = the_opts.get_d(OPTION_MAX_TIMESTEP);
}

void
Timer::reset()
{
  gettimeofday(&start, NULL);
  current_timer = 0.0;
  //update( );
}

double
Timer::get()
{
  timeval current_time;
  gettimeofday(&current_time, NULL);

  if (start.tv_usec > current_time.tv_usec)
    {
      current_time.tv_usec += 1000000;
      current_time.tv_sec--;
    }

  double elapsed;

  elapsed = ((double)( current_time.tv_usec - start.tv_usec )) * 1e-6;
  elapsed += (double)( current_time.tv_sec  - start.tv_sec );

  return elapsed;
}

double
Timer::get_total_time() {
  return total_time;
}

unsigned int 
Timer::t_randomize( unsigned int rseed ) {
  if( rseed == 0 ) 
    rseed = get_random_seed();

  srand( rseed );

  return rseed;
}

unsigned int
Timer::t_rand( unsigned int nb_try ) {
  while( (--nb_try) != 0 ) 
    rand();

  return rand();
}

unsigned int
Timer::nb_rand( ) {
  return 0;
}

double
Timer::update( double factor ) {
  static double last_timestep = 0;
  double last_timer = current_timer;

  current_timer = get();

  double timestep = various::min( (current_timer - last_timer), max_timestep );

  total_time += timestep ;
  total_time = various::max(total_time, 0.0);

  last_timestep = current_timer;

  return timestep;

  /*
  double last_timer = current_timer;

  current_timer = timer.get();
  double timescale = 1.0;
  if( state == GAME_IN_PROGRESS ) timescale = the_opts.get_d(OPTION_TIMESCALE);
  timestep = min( (current_timer - last_timer) * timescale, 
                  the_opts.get_d(OPTION_MAX_TIMESTEP) );
  total_time += timestep*factor;

  total_time = max(total_time, 0.0);
  */
}

void
Timer::next_check( double check_time ) {
  next_check_time = total_time + check_time;
}

bool
Timer::check() {
  return (total_time > next_check_time);
}
