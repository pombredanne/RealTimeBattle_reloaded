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

#ifndef __SHAPE_VISITOR__
#define __SHAPE_VISITOR__

#include "Shape.h"

class InnerCircle;
class Vector2D;

//! The Shape Visitors Base (see the definition of a Visitor in a Design Partern document)
/*!
  A visitor goes in each shape and the shape decide which function is to run
  The visitor is attached to a LocalObserver which will send it to the ArenaRealTime at every tic.
  Then the arena will send it to all the shape referenced in object_lists.
  As for Gui:s the interface is not fixed and is waiting for you to be improved

  Of course, to be a real visitor, we should add the accept_ShapeName(Shape*) functions but it
  is simplier right now to do like this.
  \sa LocalObserver
*/

class ShapeVisitor {
 public:
  virtual void draw_arc( const Vector2D& center, 
		 const double inner_radius, const double outer_radius,
		 const double angle1, const double angle2,
		 long int c ){}
  virtual void draw_line( const Vector2D& start, const Vector2D& direction,
		  const double length, const double thickness,
		  long int c ){}
  virtual void draw_line( const Vector2D& start, const Vector2D& direction,
		  const double length, long int c ){}
  virtual void draw_rectangle( const Vector2D& start, const Vector2D& end,
		       long int c, const bool filled ){}
  virtual void draw_circle( const Vector2D& center, const double radius,
		    long int c, const bool filled ){}


  double get_drawing_area_scale() { return drawing_area_scale; }
  void set_drawing_area_scale( double s ) { drawing_area_scale = s ; }

 protected:
  double drawing_area_scale;

};

#endif /* __SHAPE_VISITOR__ */

