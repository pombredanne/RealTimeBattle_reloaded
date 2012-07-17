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


#include <string>
#include <fstream>

#include "ArenaBase.h"
#include "ArenaController.h"
#include "ArenaParser.h"
#include "Options.h"
#include "Various.h"
#include "Wall.h"

using namespace std;

void
ArenaParser::parse( const string& filename ) {


  {
    ifstream file(filename.c_str());

    if( !file ) Error(true, "Couldn't open arena file" + filename, "ArenaBase::parse_arena_file");

    int succession = 1;
    double scale = the_opts.get_d(OPTION_ARENA_SCALE);
    double angle_factor = 1.0;
    do
      {
	parse_arena_line(file, scale, succession, angle_factor);

      } while( !file.eof() );

    file.close();
  }

  {
    char buffer[500];
    ifstream file( filename.c_str() );

    if( !file ) Error(true, "Couldn't open arena file for log file " + filename,
                      "ArenaParser::parse_arena_file");

    do
      {
	file >> ws;
	file.get(buffer, 500, '\n');
	if( buffer[0] != '\0' ) {
	  Arena_Line_Event evt( buffer );
	  the_arena_controller.notify_event( & evt );
	}
      }
    while( buffer[0] != '\0' );

    file.close();
  }
};

void
ArenaParser::parse_arena_line(ifstream& file, double& scale, int& succession, double& angle_factor)
{
  double radius, radius2, bounce_c, hardn, thickness;
  int vertices;

  Vector2D vec1, vec2, vec0, center;

  Wall* wall_p;

  string keyword;
  file >> keyword;

  //cout << keyword.substr( 0, 2 ) << endl;

  if( keyword.substr(0, 2) == "//" )
    {
      char buf[ 256 ];
      file.get(buf, 256, '\n');
      //cout << "skip " << buf << endl;
    }
  else if( keyword == "scale" )
    {
      if( succession != 1 ) Error(true, "Error in arenafile: 'scale' not first",
                                  "ArenaBase::parse_arena_line");
      succession = 2;
      double scl;
      file >> scl;
      scale *= scl;
    }
  else if( keyword == "angle_unit" )
    {
      char unit[20];
      file >> ws;
      file.get(unit, 20, '\n');
      if( strcmp(unit, "radians" ) == 0)
        angle_factor = 1.0;
      else if( strcmp(unit, "degrees" ) == 0 )
        angle_factor = M_PI / 180.0;
      else
        Error(true, "Error in arenafile: Unknown angle unit: " + string(unit),
              "ArenaBase::parse_arena_line");
    }
  else if( keyword == "boundary" )
    {
      Vector2D boundary[2];
      if( succession > 2 )
        Error(true, "Error in arenafile: 'boundary' after wallpieces or duplicate",
              "ArenaBase::parse_arena_line");
      succession = 3;

      double b1, b2;
      Vector2D v;
      file >> boundary[0] >> boundary[1];
      boundary[0] = boundary[0] * scale;
      boundary[1] = boundary[1] * scale;

      cerr << boundary[0]<<boundary[1]<<endl;
      cerr <<boundary[1][0] << "  " << boundary[0][0]<<endl;
      cerr <<boundary[1][1] << "  " <<  boundary[0][1]<<endl;
      if( boundary[1][0] - boundary[0][0] <= 0 ||
          boundary[1][1] - boundary[0][1] <= 0 )
        Error(true, "Error in arenafile: 'boundary' negative",
              "ArenaBase::parse_arena_line");
      else {
        the_arena.boundary = new StartSquare( boundary[0], boundary[1] );
	//the_arena.landing_zones[0]->push_back( the_arena.boundary );
      }
    }
  else if( keyword == "exclusion_point" )
    {
      if( succession < 3 )
        Error(true, "Error in arenafile: 'boundary' after wallpieces or duplicate",
              "ArenaBase::parse_arena_line");
      file >> vec1;
      the_arena.exclusion_points.push_back(Vector2D(scale*vec1));
    }
  else if( keyword == "inner_circle" )
    {
      if( succession < 3 )
        Error(true, "Error in arenafile: 'inner_circle' before boundary",
              "ArenaBase::parse_arena_line");
      succession = 4;
      file >> bounce_c >> hardn >> vec1 >> radius;

      wall_p = new Wall( bounce_c, hardn, new InnerCircle(scale*vec1, scale*radius) );
      the_arena.walls.push_front( wall_p );
    }
  else if( keyword == "circle" )
    {
      if( succession < 3 )
        Error(true, "Error in arenafile: 'circle' before 'boundary'",
              "ArenaBase::parse_arena_line");
      succession = 4;
      file >> bounce_c >> hardn >> vec1 >> radius;
      wall_p = new Wall( bounce_c, hardn, new Circle(scale*vec1, scale*radius) );
      the_arena.walls.push_back(wall_p);
    }
  else if( keyword == "arc" )
    {
      if( succession < 3 )
        Error(true, "Error in arenafile: 'arc' before 'boundary'",
              "ArenaBase::parse_arena_line");
      succession = 4;
      double angle1, angle2;
      file >> bounce_c >> hardn >> center
	   >> radius >> radius2
	   >> angle1 >> angle2;

      wall_p = new Wall( bounce_c, hardn, new Arc( scale*center, scale*radius, scale*radius2,
                                                       angle1 * angle_factor, angle2 * angle_factor));
      the_arena.walls.push_back(wall_p);
    }
  else if( keyword == "line" )
    {
      if( succession < 3 ) Error(true, "Error in arenafile: 'line' before 'boundary'",
                                 "ArenaBase::parse_arena_line");
      succession = 4;
      file >> bounce_c >> hardn >> thickness;
      thickness *= 0.5;
      file >> vec1      // start_point
	   >> vec2;     // end_point


      if( length(vec2-vec1) == 0.0 )
        Error(true, "Error in arenafile: Zero length line",
              "ArenaBase::parse_arena_line");

      wall_p = new Wall( bounce_c, hardn,
                          new Line( scale*vec1, unit(vec2-vec1),
				     scale*length(vec2-vec1),
				     scale*thickness));
      the_arena.walls.push_back( wall_p );
    }
  else if( keyword == "polygon" )
    {
      if( succession < 3 )
        Error(true, "Error in arenafile: 'polygon' before 'boundary'",
              "ArenaBase::parse_arena_line");
      succession = 4;
      file >> bounce_c >> hardn >> thickness;
      thickness *= 0.5;
      file >> vertices   // number of vertices
	   >> vec1;      // first point
      wall_p = new Wall(bounce_c, hardn,
                        new Circle(scale*vec1, scale*thickness));
      the_arena.walls.push_back( wall_p );

      for(int i=1; i<vertices; i++)
        {
          vec2 = vec1;
          file >> vec1;      // next point

          if( length(vec2-vec1) == 0.0 )
            Error(true, "Error in arenafile: Zero length line in polygon",
                  "ArenaBase::parse_arena_line");

          wall_p = new Wall( bounce_c, hardn,
	                      new Line(scale*vec2, unit(vec1-vec2),
                                       scale*length(vec1-vec2),
                                       scale*thickness) );
          the_arena.walls.push_back( wall_p );
          wall_p = new Wall( new Circle(scale*vec1, scale*thickness), bounce_c, hardn);
          the_arena.walls.push_back( wall_p );  //NOTE: two circles at start point if close shape
        }
    }
  else if( keyword == "closed_polygon" )
    {
      if( succession < 3 )
        Error(true, "Error in arenafile: 'closed_polygon' before 'boundary'",
              "ArenaBase::parse_arena_line");
      succession = 4;
      file >> bounce_c >> hardn >> thickness;
      thickness *= 0.5;
      file >> vertices   // number of vertices
	   >> vec1;      // first point
      wall_p = new Wall( new Circle(scale*vec1, scale*thickness), bounce_c, hardn);
      the_arena.walls.push_back( wall_p );
      vec0 = vec1;

      for(int i=1; i<vertices; i++)
        {
          vec2 = vec1;
          file >> vec1;      // next point

          if( length(vec2-vec1) == 0.0 )
            Error(true, "Error in arenafile: Line in closed_polygon of zero length",
                  "ArenaBase::parse_arena_line");

          wall_p = new Wall( new Line(scale*vec2, unit(vec1-vec2),
                                       scale*length(vec1-vec2),
                                       scale*thickness),
			     bounce_c , hardn);
          the_arena.walls.push_back( wall_p );

          wall_p = new Wall( new Circle(scale*vec1, scale*thickness), bounce_c, hardn);
          the_arena.walls.push_back( wall_p );
        }

      if( length(vec0-vec1) == 0.0 )
        Error(true, "Error in arenafile: Last line in closed_polygon of zero length",
              "ArenaBase::parse_arena_line");

      wall_p = new Wall( new Line(scale*vec1, unit(vec0-vec1), scale*length(vec0-vec1),
                                   scale*thickness),
			 bounce_c , hardn);
      the_arena.walls.push_back( wall_p );
    }
  else if( keyword == "poly_curve" )
    {
      if( succession < 3 )
        Error(true, "Error in arenafile: 'closed_polygon' before 'boundary'",
              "ArenaBase::parse_arena_line");
      succession = 4;
      file >> bounce_c >> hardn >> thickness;
      thickness *= 0.5;

      Vector2D current_pos, direction;
      file >> current_pos;      // first point
      wall_p = new Wall( new Circle(scale*current_pos, scale*thickness), bounce_c, hardn);
      the_arena.walls.push_back( wall_p );

      file >> direction;      // start direction

      direction.normalize();

      if( lengthsqr(direction) < 0.01 )
        Error(true, "Error in arenafile: directions must not be zero",
              "ArenaBase::parse_arena_line");

      vec0 = current_pos;

      char c;
      double len, angle, start_angle, end_angle, tmp;
      bool finish = false;
      do
        {
          file >> c;

          switch( c )
            {
            case 'L':  // line
              file >> len;
              if( len <= 0.0 )
                Error(true, "Error in arenafile: Line in poly_curve must be positive",
                      "ArenaParser::parse_arena_line");

              wall_p = new Wall( new Line(scale*current_pos, direction,
                                           scale*len,
                                           scale*thickness),
				 bounce_c , hardn);
              the_arena.walls.push_back( wall_p );

              current_pos += len * direction;
              wall_p = new Wall( new Circle(scale*current_pos, scale*thickness),
                                  bounce_c, hardn);
              the_arena.walls.push_back( wall_p );

              break;
            case 'A':  // Arc
              file >> angle >> radius;

              angle *= angle_factor;
              center = current_pos - rotate90( direction ) * radius * sgn( angle );
              start_angle = vec2angle( current_pos - center );
              current_pos = center + radius * angle2vec( start_angle - angle );
              end_angle = vec2angle( current_pos - center );

              if( angle > 0.0 )
                { tmp = start_angle; start_angle = end_angle; end_angle = tmp; }

              wall_p = new Wall( new Arc(scale*center, scale*(radius - thickness),
                                      scale*(radius + thickness),
                                      start_angle, end_angle),
                                      bounce_c, hardn);

              the_arena.walls.push_back(wall_p);


              direction = rotate(direction, -angle);

              wall_p = new Wall( new Circle(scale*current_pos, scale*thickness),
                                 bounce_c, hardn);
              the_arena.walls.push_back( wall_p );
              break;

            case 'T':  // Turn
              file >> angle;
              direction = rotate( direction, -angle*M_PI/180.0 );

              break;

            case 'C':   // connect to start point and quit
              if( length(vec0 - current_pos ) == 0.0 )
                Error(true, "Error in arenafile: Last line in poly_curve of zero length",
                      "ArenaParser::parse_arena_line");

              wall_p = new Wall( new Line(scale*current_pos, unit(vec0-current_pos),
				           scale*length(vec0-current_pos),
					   scale*thickness),
				 bounce_c , hardn);
              the_arena.walls.push_back( wall_p );

              finish = true;
              break;
            case 'Q':   // quit
              finish = true;
              break;

            default:
              Error(true, string("Incorrect arenafile, unknown command in poly_curve: ") + c,
		    string("ArenaParser::parse_arena_line"));
              break;
            }
        }
      while( !finish );
    }
  else if( keyword == "startpos" )
    {
      char buffer[1024];
      Vector2D pos;
      LandingZone* lz;
      int dir;
      int team;

      //Get the team
      file >> ws;
      if ( file.peek() == '*' ) {
        file.get() ;
	team = 0;
      } else {
	file >> team ;
	if ( ! file ) file.get(buffer, 1024, '\n');
      }

      //Get the start position
      file >> pos >> ws;

      //Get the robot direction
      if ( file.peek() == '*' ) {
        file.get() ;
	lz = new StartPosition( pos );
      } else {
	file >> dir ;
	if ( ! file ) file.get(buffer, 1024, '\n');
	else lz = new StartPosition( pos, dir * M_PI / 180 );
      }

      if( the_arena.landing_zones[team] == NULL )
	the_arena.landing_zones[team] = new list<LandingZone*>;

      the_arena.landing_zones[team]->push_back( lz );
    }
  else if( keyword == "startsqr" )
    {
    }
  else if( keyword == "blackhole" )
    {
      Vector2D pos;
      file >> pos ;
      if( !file ) { char buffer[1024]; file.get(buffer, 1024, '\n'); }
      else the_arena.floors.push_front( new BlackHole( pos ) );
    }
  else if( keyword == "teleport" )
    {
      Vector2D start, end;
      file >> start >> end;
      if( !file ) { char buffer[1024]; file.get(buffer, 1024, '\n'); }
      else the_arena.floors.push_front( new Teleport( start, end ) );
    }
  else if( keyword == "flow" )
    {
      Vector2D start, end, acc;
      file >> start >> end >> acc;
      if( !file ) { char buffer[1024]; file.get(buffer, 1024, '\n'); }
      else the_arena.floors.push_front( new Flow( start, end, acc ) );
    	/* Parse a vector (flow) and a double (resistance ?) */
    }
  else if( keyword == "define")
    {
      //get_definition( file );
    }
  else if( keyword[0] != '\0' )
    Error(true, "Incorrect arenafile, unknown keyword: " + keyword,
          "ArenaBase::parse_arena_line");

}


void ArenaParser::get_definition( ifstream& file ) {
  //string s, name, derive_from;
  //file >> name >> derive_from;
  //cerr << name << "  " << derive_from << endl;
  //do {
  //  file >> s;
  //  cerr << s << endl;
  //} while( s != "enddef" );
}


