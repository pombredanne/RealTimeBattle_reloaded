/*
RealTimeBattle, a robot programming game for Unix
Copyright (C) 1998-2002  Erik Ouchterlony and Ragnar Ouchterlony

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



#ifndef __TOURN_PACKET_H__
#define __TOURN_PACKET_H__

#include "Packets.h"
#include "Structs.h"

#include "Tournament.h"

class TournamentRobotPacketFactory: public PacketFactory {
 public:
  TournamentRobotPacketFactory( Tournament* T ) 
    : my_tournament( T ) {};
  string Protocol() {return RTB_PROTOCOL_TOURNAMENT_ROBOT ;};
  bool add_connection( NetConnection*, string = string("") );
  //void set_start(NetConnection*, bool);
  //void reset_start();
  //void remove_connection( NetConnection* );
  //void start_tournament( bool = false );

  Packet* MakePacket(string &, NetConnection*); 
 protected:
  Tournament* my_tournament;
  //tourn_info_t my_tournament;
  map<NetConnection*, Robot*> robot_connection;
};

class TournamentViewPacketFactory: public PacketFactory {
 public:
  TournamentViewPacketFactory( Tournament* T ):
    my_tournament( T ) {};
  string Protocol() {return RTB_PROTOCOL_TOURNAMENT_VIEW ;};
  //void add_connection( NetConnection* );
  //void set_start(NetConnection*, bool);
  //void reset_start();
  //void remove_connection( NetConnection* );
  //void start_tournament( bool = false );

  Packet* MakePacket(string &, NetConnection*); 
 protected:
  Tournament* my_tournament;
  //tourn_info_t my_tournament;
  //map<NetConnection*, bool> ready_to_start;
};

class OldRTBMessagePacket : /* virtual */ public Packet {
public:
  OldRTBMessagePacket( ) {};
  OldRTBMessagePacket( string & message ) 
    : message( message ) {};

  OldRTBMessagePacket( const string& data, NetConnection* nc ) 
     : Packet(data, nc) {} ;

  string make_netstring() const;
  int handle_packet( );
  packet_t packet_type() { return PACKET_OLD_RTB_MESSAGE; };

protected:
  string message;
};

class RobotCommandPacket : /* virtual */ public Packet {
public:
  RobotCommandPacket( ) {};
  RobotCommandPacket( string & message ) 
    : message( message ) {};

  RobotCommandPacket( const string& data, NetConnection* nc ) 
     : Packet(data, nc) {} ;

  string make_netstring() const;
  int handle_packet( );
  packet_t packet_type() { return PACKET_ROBOT_COMMAND; };

protected:
  string message;
};

#endif
