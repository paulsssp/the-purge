#include "Player.hh"


/**
 * Write the name of your player and save this file
 * with the same name and .cc extension.
 */
#define PLAYER_NAME paula


struct PLAYER_NAME : public Player {

  /**
   * Factory: returns a new instance of this class.
   * Do not modify this function.
   */
  static Player* factory () {
    return new PLAYER_NAME;
  }

  /**
   * Types and attributes for your player can be defined here.
   */

  typedef vector<bool> VB;
  typedef vector<VB> VVB;
  typedef vector<Pos> VP;
  typedef vector<VP> VVP;

  const vector<Dir> dirs = {Up,Down,Left,Right};
  //vector<Pos> barricades_to_build(3); q no vayan a por las mismas barricadas a construir

  // retorna si la celda es valida para pasar sin obstaculos (barricadas de otros, edificios, ni jugadores)
  bool valid_cell(Pos p) {
    return (cell(p).type == Street and cell(p).id == -1 and (cell(p).b_owner == -1 or cell(p).b_owner == me()));
  }


  bool between_buildings(Pos p) {
    return ((cell(p+Up).type == Building and cell(p+Down).type == Building) or (cell(p+Right).type == Building and cell(p+Left).type == Building));
  }

  bool between_building_limit(Pos p) {
    return ((cell(p+Up).type == Building and not pos_ok(p+Down)) or ((cell(p+Down).type == Building and not pos_ok(p+Up))) or ((cell(p+Right).type == Building and not pos_ok(p+Left))) or ((cell(p+Left).type == Building and not pos_ok(p+Right))));
  }

  bool can_place_barricade(Pos p) {
    return cell(p).type == Street and cell(p).id == -1 and cell(p).b_owner == -1 and (between_buildings(p) or between_building_limit(p));
  }

  // hago un bfs y retorno el camino para que lo siga el constructor
  // tener en cuenta de que nadie más está yendo a esa misma celda
  Pos bfs_path_barricades (Pos pos_ini, VVP& prev_path) {
    VVB visited(board_rows(), VB(board_cols(), false));
    visited[pos_ini.i][pos_ini.j] = true;
    queue<Pos> q;
    q.push(pos_ini);

    while (not q.empty()) {
      Pos pos = q.front();
      q.pop();

      for (Dir d : dirs) {
        Pos npos = pos + d;
        if (pos_ok(npos) and not visited[npos.i][npos.j] and valid_cell(npos)) {
          q.push(npos);
          visited[npos.i][npos.j] = true;
          prev_path[npos.i][npos.j] = pos;
          if (can_place_barricade(npos)) {
            return npos;
          }
        }
      }
    } 
    return Pos(-1,-1);
  }

  Pos bfs_path_money (Pos pos_ini, VVP& prev_path) {
    VVB visited(board_rows(), VB(board_cols(), false));
    visited[pos_ini.i][pos_ini.j] = true;
    queue<Pos> q;
    q.push(pos_ini);

    while (not q.empty()) {
      Pos pos = q.front();
      q.pop();

      for (Dir d : dirs) {
        Pos npos = pos + d;
        if (pos_ok(npos) and not visited[npos.i][npos.j] and valid_cell(npos)) {
          q.push(npos);
          visited[npos.i][npos.j] = true;
          prev_path[npos.i][npos.j] = pos;
          if (cell(npos).bonus == Money) {
            //cerr << "Money found" << endl;
            return npos;
          }
        }
      }
    } 
    //cerr << "No path found for money" << endl;
    return Pos(-1,-1);
  }

  Pos reverse_path(Pos pos, const VVP& prev_path, int id) {
    stack<Pos> s;
    s.push(pos);
    while (s.top() != citizen(id).pos) {
      s.push(prev_path[s.top().i][s.top().j]);
    }
    s.pop();
    return s.top();
  }

  Dir set_movement(Pos movement, int id) {
      Dir d;
      if (movement.i == citizen(id).pos.i + 1) d = Down; 
      else if (movement.i == citizen(id).pos.i - 1) d = Up;
      else if (movement.j == citizen(id).pos.j + 1) d = Right;
      else d = Left;
      return d;
  }


  // bug al llegar a x ronda sólo se mueve un constructor
  void move_builders() {
    vector<vector<Pos> > prev_path(board_rows(), vector<Pos>(board_cols()));
    vector<int> b = builders(me());
    for (int i = 0; i < 4; ++i) {
      if (is_day()) {
        // los constructores que hagan sus 3 barricadas
        if (barricades(me()).size() < 3) {
          // matriz de las pos previas para construir la barricada
          Pos future_barricade = bfs_path_barricades(citizen(b[i]).pos, prev_path);
          if (future_barricade.i == -1) {
            //cerr << "No path found for barricade" << endl;
            //break;
          } 
          else {
            Pos reverse = reverse_path(future_barricade, prev_path, b[i]);
            Dir d = set_movement(reverse, b[i]);
            if (citizen(b[i]).pos + d == future_barricade) build(citizen(b[i]).id, d);
            else move(citizen(b[i]).id, d);
          }
        } 
        // si ya están todas que vayan a por dinero
        else {
          Pos money = bfs_path_money(citizen(b[i]).pos, prev_path);
          if (money.i == -1) {
            //cerr << "No path found for money" << endl;
            ///break;
          } 
          else {
            Pos reverse = reverse_path(money, prev_path, b[i]);
            Dir d = set_movement(reverse, b[i]);
            move(citizen(b[i]).id, d);
            //cerr << "Moved" << endl;
          }
        }
      }

      else {
        Pos money = bfs_path_money(citizen(b[i]).pos, prev_path);
        if (money.i == -1) {
          //cerr << "No path found for money " << money << endl;
          //break;
        } 
        else {
          Pos reverse = reverse_path(money, prev_path, b[i]);
          cout << reverse << endl;
          Dir d = set_movement(reverse, b[i]);
          move(citizen(b[i]).id, d);
          //cerr << "Moved" << endl;
        }
      }
    } 
  }

  void move_warriors() {

  }


  /**
   * Play method, invoked once per each round.
   */
  virtual void play () {
    move_builders();
    move_warriors();
  }

};


/**
 * Do not modify the following line.
 */
RegisterPlayer(PLAYER_NAME);
