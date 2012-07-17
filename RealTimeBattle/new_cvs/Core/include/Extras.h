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

#ifndef __EXTRAS__
#define __EXTRAS__


#include "MovingObject.h"
#include "Functors.h"

class ExtraDef;


class Extras : public MovingObject
{
public:
  Extras(const Vector2D& c, const double r, const double e, const long int col);
  Extras( const ExtraDef& ed );
  ~Extras() {}

  void set_id( int i ) { id = i; }
  virtual void die();

  void move( double timestep ) {};

  char log_file_char;
  int type_int;
};

class ExtraDef {
public:
  ExtraDef( char tc, int ti, double min, double max,
            double r, long int c ) :
    type_char( tc ), type_int( ti ) ,
    min_energy( min ), max_energy( max ),
    radius( r ), colour( c ) {}
  ExtraDef( const ExtraDef& );
  char type_char;
  int type_int;
  double min_energy, max_energy;
  double radius;
  long int colour;
};

class ExtraCreator : public RandomFunction {
public:
  ExtraCreator( double time, ExtraDef ed ) :
    RandomFunction( time ) , my_def( ed ) {}
  void do_exec() ;

protected:
  ExtraDef my_def;
};

// ---------------------  Cookie : Extras  ---------------------

class Cookie : public Extras
{
public:
  Cookie(const Vector2D& c, const double e, const int c_id = -1);
  ~Cookie() {}
  void die();
  //  arenaobject_t get_arenaobject_t() { return COOKIE; }
};

// ---------------------  Mine : Extras  ---------------------

class Mine : public Extras
{
public:
  Mine(const Vector2D& c, const double e, const int m_id = -1);
  ~Mine() {}
  void die();
  //  arenaobject_t get_arenaobject_t() { return MINE; }
};

#endif /* __EXTRAS__ */

