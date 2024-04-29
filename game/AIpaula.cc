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
  typedef queue<Pos> Posicions;

  // realmente lo necesito? 
  const vector<Dir> dirs = {Up,Down,Left,Right};


  // Identifying a street cell between two building cells
  // hago un bfs y retorno el camino para que lo siga el constructor
  // tener en cuenta de que nadie más está yendo a esa misma celda
  void pos_barricades(Pos pos_ini) {
    
  }

  int minim_tresor(const Graf& mat, Distancies& dis, Posicions& pos, int f, int c) {
    while (not pos.empty()) {
        int dis_fila = pos.front().first; //distancias anteriores
        int dis_col = pos.front().second;
        pos.pop();
        for (int i = 0; i < dir; ++i) {
            int nf = dis_fila + df[i];
            int nc = dis_col + dc[i];
            if (pos_valida(mat, dis, nf, nc)) {
                pos.push(make_pair(nf, nc));
                dis[nf][nc] = dis[dis_fila][dis_col] + 1;
                if (mat[nf][nc] == 't') return dis[nf][nc];
            } 
        }
    } 
    return -1;
}

  void move_builders() {
    if (is_day()) {
      // los constructores que hagan sus 3 barricadas
      int nro_barricadas = barricades(me()).size();
      vector<int> b = builders(me());
      int i = 0;
      while (nro_barricadas < 3) {
        b[i].build_barricades();

        ++i;
      }

      /*
      if (barricades(me()).size() < 3) {
        vector<int> b = builders(me());
        for (int id : b) {
          if (barricades(me()).size() < 3) {
            for (Dir d : dirs) {
              if (cell(me()).pos + d != cell(me()).pos) {
                build(id, d);
              }
            }
          }
        }
      }*/


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
