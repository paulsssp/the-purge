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
  //VP barricades_to_build; // q no vayan a por las mismas barricadas a construir

  // retorna si la celda es valida para pasar sin obstaculos (barricadas de otros, edificios, ni jugadores)
  bool valid_cell(Pos p) {
    return (cell(p).type == Street and cell(p).id == -1 and (cell(p).b_owner == -1 or cell(p).b_owner == me()));
  }

  bool valid_cell_for_attacking(Pos p) {
    return (cell(p).type == Street and (cell(p).b_owner == -1 or cell(p).b_owner == me()));
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

  // hacer que los constructores eviten pasar por armas?
  Pos bfs_path_bonus (Pos pos_ini, VVP& prev_path, BonusType bonus) {
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
          if (cell(npos).bonus == bonus) {
            return npos;
          }
        }
      }
    } 
    return Pos(-1,-1);
  }

  Pos bfs_path_weapon (Pos pos_ini, VVP& prev_path, WeaponType weapon) {
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
          if (cell(npos).weapon == weapon) {
            return npos;
          }
        }
      }
    } 
    return Pos(-1,-1);
  }


  bool is_enemy(Pos p) {
    vector<int> w = warriors(me());
    vector<int> b = builders(me());
    for (int id : w) {
      if (citizen(id).pos == p) return false;
    }
    for (int id : b) {
      if (citizen(id).pos == p) return false;
    }
    return true;
  }

  bool has_worse_weapon(Pos p, Pos m) {
    Citizen enemy = citizen(cell(p).id);
    Citizen me = citizen(cell(m).id);
    return (enemy.weapon < me.weapon);
  }

  Pos bfs_path_atack (Pos pos_ini, VVP& prev_path) {
    VVB visited(board_rows(), VB(board_cols(), false));
    visited[pos_ini.i][pos_ini.j] = true;
    queue<Pos> q;
    q.push(pos_ini);

    while (not q.empty()) {
      Pos pos = q.front();
      q.pop();

      for (Dir d : dirs) {
        Pos npos = pos + d;
        if (pos_ok(npos) and not visited[npos.i][npos.j] and valid_cell_for_attacking(npos)) {
          q.push(npos);
          visited[npos.i][npos.j] = true;
          prev_path[npos.i][npos.j] = pos;
          if (is_enemy(npos) and has_worse_weapon(npos, pos_ini)) {
            return npos;
          }
        }
      }
    } 
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


  // falta que mo vayan todos a por la misma casilla tanto para dinero como para barricadas
  // si mis dos constructorres tienen bazocas, que se carguen los q estan en el mapa
  // van a x comida ?
  void move_builders() {
    //barricades_to_build = {}; 
    vector<vector<Pos> > prev_path(board_rows(), vector<Pos>(board_cols()));
    vector<int> b = builders(me());
    for (int i = 0; i < 4; ++i) {
      if (is_day()) {
        // los constructores que hagan sus 3 barricadas
        if (barricades(me()).size() < 3) {
          // matriz de las pos previas para construir la barricada
          Pos future_barricade = bfs_path_barricades(citizen(b[i]).pos, prev_path);
          if (future_barricade.i != -1) {
            //barricades_to_build.push_back(future_barricade);
            Pos reverse = reverse_path(future_barricade, prev_path, b[i]);
            Dir d = set_movement(reverse, b[i]);
            if (citizen(b[i]).pos + d == future_barricade) build(citizen(b[i]).id, d);
            else move(citizen(b[i]).id, d);
          }
        } 
        // si ya están todas que vayan a por dinero
        else {
          Pos money = bfs_path_bonus(citizen(b[i]).pos, prev_path, Money);
          if (money.i != -1) {
            Pos reverse = reverse_path(money, prev_path, b[i]);
            Dir d = set_movement(reverse, b[i]);
            move(citizen(b[i]).id, d);
          }
        }          
      }

      else { // is_night
        Pos money = bfs_path_bonus(citizen(b[i]).pos, prev_path, Money);
        if (money.i != -1) {
          Pos reverse = reverse_path(money, prev_path, b[i]);
          Dir d = set_movement(reverse, b[i]);
          move(citizen(b[i]).id, d);
        }
      }
    } 
  }

  void move_warriors() {
    vector<vector<Pos> > prev_path(board_rows(), vector<Pos>(board_cols()));
    vector<int> w = warriors(me());
    for (int id : w) {
      // si no tiene bazoca, que vaya a buscar el arma más potente que la que posee más cercana (primero miramos si hay bazoca)
      if (citizen(id).weapon != Bazooka) {
        Pos bazooka = bfs_path_weapon(citizen(id).pos, prev_path, Bazooka);
        if (bazooka.i != -1) {
          Pos reverse = reverse_path(bazooka, prev_path, id);
          Dir d = set_movement(reverse, id);
          move(citizen(id).id, d);
        }
        else {
          if (citizen(id).weapon != Gun) {
            Pos gun = bfs_path_weapon(citizen(id).pos, prev_path, Gun);
            if (gun.i != -1) {
              Pos reverse = reverse_path(gun, prev_path, id);
              Dir d = set_movement(reverse, id);
              move(citizen(id).id, d);
            }
          }
        } 
      } 
      
      if (is_day()){
        // acabar de pulir cuando va a por comida, si siempre o cuando le queda x vida
        // si no tiene la vida maxima, que vaya a por comida 
        if (citizen(id).life < warrior_ini_life()) {
          Pos food = bfs_path_bonus(citizen(id).pos, prev_path, Food);
          if (food.i != -1) {
            Pos reverse = reverse_path(food, prev_path, id);
            Dir d = set_movement(reverse, id);
            move(citizen(id).id, d);
          }
          // si no hay comida, que vaya a por dinero
          else {
            Pos money = bfs_path_bonus(citizen(id).pos, prev_path, Money);
            if (money.i != -1) {
              Pos reverse = reverse_path(money, prev_path, id);
              Dir d = set_movement(reverse, id);
              move(citizen(id).id, d);
            }
          }
        }
        else {
          Pos money = bfs_path_bonus(citizen(id).pos, prev_path, Money);
          if (money.i != -1) {
            Pos reverse = reverse_path(money, prev_path, id);
            Dir d = set_movement(reverse, id);
            move(citizen(id).id, d);
          }
        }
      }

      // tener en cuenta a la hora de matar, que no solo tenga un arma más baja sino que tenga menos vida que nosotros
      // que mate
      else { // is_night
        Pos enemy = bfs_path_atack(citizen(id).pos, prev_path);
        if (enemy.i != -1) {
          Pos reverse = reverse_path(enemy, prev_path, id);
          Dir d = set_movement(reverse, id);
          move(citizen(id).id, d);
        }
        else cerr << "no enemy to atack" << endl;
      }
    }
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
