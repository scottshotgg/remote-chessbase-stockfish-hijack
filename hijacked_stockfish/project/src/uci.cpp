/*
  Stockfish, a UCI chess playing engine derived from Glaurung 2.1
  Copyright (C) 2004-2008 Tord Romstad (Glaurung author)
  Copyright (C) 2008-2015 Marco Costalba, Joona Kiiski, Tord Romstad
  Copyright (C) 2015-2016 Marco Costalba, Joona Kiiski, Gary Linscott, Tord Romstad

  Stockfish is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Stockfish is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <iostream>
#include <sstream>
#include <string>
//#include <stdio.h>

#include "evaluate.h"
#include "movegen.h"
#include "position.h"
#include "search.h"
#include "thread.h"
#include "timeman.h"
#include "uci.h"

// User added includes and defines
#include <string>
// #include "happyhttp.h"
// #include "json/json.h"
#include "client.h"
#include <thread>

#ifdef WIN32
  #include <winsock2.h>
  #define vsnprintf _vsnprintf
  WSADATA wsaData;
#endif

  // End of user added includes and defines

// blehhhhhh
using namespace std;

extern void benchmark(const Position& pos, istream& is);

namespace {

  // FEN string of the initial position, normal chess
  const char* StartFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

  // A list to keep track of the position states along the setup moves (from the
  // start position to the position just before the search starts). Needed by
  // 'draw by repetition' detection.
  StateListPtr States(new std::deque<StateInfo>(1));


  // position() is called when engine receives the "position" UCI command.
  // The function sets up the position described in the given FEN string ("fen")
  // or the starting position ("startpos") and then makes the moves given in the
  // following move list ("moves").

            // void position(Position& pos, istringstream& is) {

            //   Move m;
            //   string token, fen;

            //   is >> token;

            //   if (token == "startpos")
            //   {
            //       fen = StartFEN;
            //       is >> token; // Consume "moves" token if any
            //   }
            //   else if (token == "fen")
            //       while (is >> token && token != "moves")
            //           fen += token + " ";
            //   else
            //       return;

            //   States = StateListPtr(new std::deque<StateInfo>(1));
            //   pos.set(fen, Options["UCI_Chess960"], &States->back(), Threads.main());

            //   // Parse move list (if any)
            //   while (is >> token && (m = UCI::to_move(pos, token)) != MOVE_NONE)
            //   {
            //       States->push_back(StateInfo());
            //       pos.do_move(m, States->back(), pos.gives_check(m));
            //   }
            // }


  // setoption() is called when engine receives the "setoption" UCI command. The
  // function updates the UCI option ("name") to the given value ("value").

        // void setoption(istringstream& is) {

        //   string token, name, value;

        //   is >> token; // Consume "name" token

        //   // Read option name (can contain spaces)
        //   while (is >> token && token != "value")
        //       name += string(" ", name.empty() ? 0 : 1) + token;

        //   // Read option value (can contain spaces)
        //   while (is >> token)
        //       value += string(" ", value.empty() ? 0 : 1) + token;

        //   if (Options.count(name))
        //       Options[name] = value;
        //   else
        //       sync_cout << "No such option: " << name << sync_endl;
        // }


  // go() is called when engine receives the "go" UCI command. The function sets
  // the thinking time and other parameters from the input string, then starts
  // the search.

        //   void go(Position& pos, istringstream& is) {

        //     Search::LimitsType limits;
        //     string token;

        //     limits.startTime = now(); // As early as possible!

        //     while (is >> token)
        //         if (token == "searchmoves")
        //             while (is >> token)
        //                 limits.searchmoves.push_back(UCI::to_move(pos, token));

        //         else if (token == "wtime")     is >> limits.time[WHITE];
        //         else if (token == "btime")     is >> limits.time[BLACK];
        //         else if (token == "winc")      is >> limits.inc[WHITE];
        //         else if (token == "binc")      is >> limits.inc[BLACK];
        //         else if (token == "movestogo") is >> limits.movestogo;
        //         else if (token == "depth")     is >> limits.depth;
        //         else if (token == "nodes")     is >> limits.nodes;
        //         else if (token == "movetime")  is >> limits.movetime;
        //         else if (token == "mate")      is >> limits.mate;
        //         else if (token == "infinite")  limits.infinite = 1;
        //         else if (token == "ponder")    limits.ponder = 1;

        //     Threads.start_thinking(pos, States, limits);
        //   }

} // namespace


//int counted = 0;

// ==========================================================

int quit = 0;
int tid = 1;

  boost::asio::io_service io_service;
  // mine
  std::string ip = "10.201.40.97";
  
  // remote
  //std::string ip = "10.201.40.25";

  //local
  //std::string ip = "127.0.0.1";
  //std::string ip = "192.168.80.63";
  //UDPClient client;
  //UDPClient tempClient;

  // sync_cout << client.socket_ << sync_endl;
  // sync_cout << client.endpoint_ << sync_endl;
  // sync_cout << client.sender_endpoint << sync_endl;

  //printf("Connecting to %s...", ip.c_str());
  //UDPClient client(io_service, ip.c_str(), "6000");

//void quitThread(std::string cmd, int id) {
void quitThread() {

  UDPClient tempClient(io_service, ip.c_str(), "6000");

  //tempClient.Send(std::string(cmd + "\n"));
  //printf("this is the command: %s", cmd.c_str());

  std::string returnstring;
  while(quit == 0) {
    returnstring = tempClient.Recv();

    if(returnstring[0] == 'd' && returnstring[1] == 'o' && returnstring[2] == 'n' && returnstring[3] == 'e') {
      break;
    }
    //printf("this is the return string: %s", returnstring.c_str());
    sync_cout << returnstring << sync_endl;
  }
  //sync_cout << "Thread" << id << " done!" << sync_endl;
}


//void beginExport(std::string cmd, std::string token, int id, UDPClient client) {
void beginExport(std::string cmd, std::string token, UDPClient* client) {

  client->Send(std::string(cmd + "\n"));
  //printf("this is the command: %s", cmd.c_str());

  std::string returnstring;
  while(quit == 0) {
    returnstring = client->Recv();

    if(returnstring[0] == 'd' && returnstring[1] == 'o' && returnstring[2] == 'n' && returnstring[3] == 'e' && token != "bench") {
      break;
    } else if(returnstring[0] == 'b' && returnstring[1] == 'd' && returnstring[2] == 'o' && returnstring[3] == 'n' && returnstring[4] == 'e' && token == "bench") {
      break;
    }
    //printf("this is the return string: %s", returnstring.c_str());
    sync_cout << returnstring << sync_endl;
  }

  //sync_cout << "Thread" << id << " done!" << sync_endl;
}


void UCI::loop(int argc, char* argv[]) {

  #ifdef WIN32
    WSAStartup(MAKEWORD(2, 2), &wsaData);
  #endif

  UDPClient client(io_service, ip.c_str(), "6000");
  
  Position pos;
  string token, cmd;

  pos.set(StartFEN, false, &States->back(), Threads.main());

  for (int i = 1; i < argc; ++i)
      cmd += std::string(argv[i]) + " ";

  do {  
      // Block here waiting for input or EOF
      if (argc == 1 && !getline(cin, cmd))
        cmd = "quit";

      istringstream is(cmd);
      token.clear(); // getline() could return empty or blank line
      is >> skipws >> token;

      if (token == "quit") {
        //std::thread t1(quitThread, "stop", tid);
        std::thread t1(quitThread);
        quit = 1;
        t1.join();
        Search::Signals.stop = true;
        Threads.main()->start_searching(true);

      } else {
        //std::thread t1(beginExport, cmd, token, tid, client);
        std::thread t1(beginExport, cmd, token, &client);
        //sync_cout << "Thread " << tid <<  " waiting..." << sync_endl;
        tid++;
        t1.detach();
      } 
  } while (token != "quit" && argc == 1);

  // may want to make a function like this that waits for everything
  //waitForJoin()

  quit = 1;
  Threads.main()->wait_for_search_finished();
}


/// UCI::value() converts a Value to a string suitable for use with the UCI
/// protocol specification:
///
/// cp <x>    The score from the engine's point of view in centipawns.
/// mate <y>  Mate in y moves, not plies. If the engine is getting mated
///           use negative values for y.

string UCI::value(Value v) {

  stringstream ss;

  if (abs(v) < VALUE_MATE - MAX_PLY)
      ss << "cp " << v * 100 / PawnValueEg;
  else
      ss << "mate " << (v > 0 ? VALUE_MATE - v + 1 : -VALUE_MATE - v) / 2;

  return ss.str();
}


/// UCI::square() converts a Square to a string in algebraic notation (g1, a7, etc.)

std::string UCI::square(Square s) {
  return std::string{ char('a' + file_of(s)), char('1' + rank_of(s)) };
}


/// UCI::move() converts a Move to a string in coordinate notation (g1f3, a7a8q).
/// The only special case is castling, where we print in the e1g1 notation in
/// normal chess mode, and in e1h1 notation in chess960 mode. Internally all
/// castling moves are always encoded as 'king captures rook'.

string UCI::move(Move m, bool chess960) {

  Square from = from_sq(m);
  Square to = to_sq(m);

  if (m == MOVE_NONE)
      return "(none)";

  if (m == MOVE_NULL)
      return "0000";

  if (type_of(m) == CASTLING && !chess960)
      to = make_square(to > from ? FILE_G : FILE_C, rank_of(from));

  string move = UCI::square(from) + UCI::square(to);

  if (type_of(m) == PROMOTION)
      move += " pnbrqk"[promotion_type(m)];

  return move;
}


/// UCI::to_move() converts a string representing a move in coordinate notation
/// (g1f3, a7a8q) to the corresponding legal Move, if any.

Move UCI::to_move(const Position& pos, string& str) {

  if (str.length() == 5) // Junior could send promotion piece in uppercase
      str[4] = char(tolower(str[4]));

  for (const auto& m : MoveList<LEGAL>(pos))
      if (str == UCI::move(m, pos.is_chess960()))
          return m;

  return MOVE_NONE;
}