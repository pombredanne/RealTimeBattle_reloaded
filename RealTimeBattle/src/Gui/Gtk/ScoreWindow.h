/*
RealTimeBattle, a robot programming game for Unix
Copyright (C) 1998-2001  Erik Ouchterlony and Ragnar Ouchterlony

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

#ifndef RTB_GTKGUI__SCORE_WINDOW__
#define RTB_GTKGUI__SCORE_WINDOW__

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <gtk/gtk.h>

#include <list>
#include <string>

#include "Window.h"

class ScoreWindow : public Window
{
public:
  ScoreWindow                         ();
  ~ScoreWindow                        ();

  void create                         ( const int&           default_width  = -1,
                                        const int&           default_height = -1,
                                        const int&           default_x_pos  = -1,
                                        const int&           default_y_pos  = -1 );

  void destroy                        ();

  void set_window_title               ();

private:

  // Data

  list<string>                        list_of_team_stats;
  list<string>                        list_of_robot_stats;

private:

  // Graphics helper functions
  // Note: the parents in these must be a GTK_CONTAINER!
  void create_team_view               ( GtkWidget*           parent );
  void create_robot_view              ( GtkWidget*           parent );

  // Graphics callbacks
  static gint close_window            ( GtkWidget*           widget,
                                        GdkEvent*            event,
                                        ScoreWindow*         object_p );

  // Graphics data

  GtkWidget*                          team_scrolled_window;
  GtkWidget*                          robot_scrolled_window;

  GtkWidget*                          team_view;
  GtkWidget*                          robot_view;
};

#endif // RTB_GTKGUI__SCORE_WINDOW__
