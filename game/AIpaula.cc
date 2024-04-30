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

  typedef vector<int> VI;
  typedef vector<VI> Dist;
  typedef vector<bool> VB;
  typedef vector<VB> VVB;
  typedef queue<Pos> Posicions;

  // realmente lo necesito? 
  const vector<Dir> dirs = {Up,Down,Left,Right};
  //vector<Pos> barricades_to_build(3); q no vayan a por las mismas barricadas a construir

  // retorna si la celda es valida para pasar sin obstaculos (barricadas de otros, edificios, ni jugadores)
  // si me guardo un vector de esto, puede q luego sí q hayan obstaculos ?????????????????????????
  bool barricade_valid_cell(Pos p) {
    return cell(p).type == Street and cell(p).id == -1 and (cell(p).b_owner == -1 or cell(p).b_owner == me());
  }


  bool between_buildings(Pos p) {
    if ((cell(p+Up).type == Building and cell(p+Down).type == Building) or (cell(p+Right).type == Building and cell(p+Left).type == Building)) return true;
    return false;
  }

  bool between_building_limit(Pos p) {
    if ((cell(p+Up).type == Building and not pos_ok(p+Down)) or ((cell(p+Down).type == Building and not pos_ok(p+Up))) or ((cell(p+Right).type == Building and not pos_ok(p+Left))) or ((cell(p+Left).type == Building and not pos_ok(p+Right))))  return true;
    return false;
  }

  bool can_place_barricade(Pos p) {
    return cell(p).type == Street and cell(p).id == -1 and cell(p).b_owner == -1 and (between_buildings(p) or between_building_limit(p));
  }

  // Identifying a street cell between two building cells
  // hago un bfs y retorno el camino para que lo siga el constructor
  // tener en cuenta de que nadie más está yendo a esa misma celda
  vector<Pos> path_barricades(Pos pos_ini) {
    vector<Pos> path;
    VVB visited(board_rows(), VB(board_cols(), false));
    queue<Pos> q;

    q.push(pos_ini);
    visited[pos_ini.i][pos_ini.j] = true;

    while (not q.empty()) {
      Pos pos = q.front();
      q.pop();

      for (Dir d : dirs) {
        Pos npos = pos + d;
        if (pos_ok(npos) and not visited[npos.i][npos.j] and barricade_valid_cell(npos)) {
          visited[npos.i][npos.j] = true;
          path.push_back(npos);
          if (can_place_barricade(npos)) return path; 
          q.push(npos); 
        }
      }
    }
    return vector<Pos>();
  }

  Dir set_movement(Pos movement, int id) {
      Dir d;
      if (movement.i == citizen(id).pos.i + 1) d = Down; 
      else if (movement.i == citizen(id).pos.i - 1) d = Up;
      else if (movement.j == citizen(id).pos.j + 1) d = Right;
      else d = Left;
      return d;
  }


  void move_builders() {
    if (is_day()) {
      vector<int> b = builders(me());
      for (int i = 0; i < 4; ++i) {
        // los constructores que hagan sus 3 barricadas
        if (barricades(me()).size() < 4) {
          vector<Pos> path = path_barricades(citizen(b[i]).pos);
          if (path.size() == 0) {
            cerr << "No path found for barricade" << endl;
            break;
          } 
          else {
            Dir d = set_movement(path[0], b[i]);
            if (path.size() == 1) build(citizen(b[i]).id, d);
            else move(citizen(b[i]).id, d);
          }
        } 
        // si ya están todas que vayan a por dinero
        else {

          //Dir d = set_movement();
        //  move(citizen(b[i]).id, );
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
