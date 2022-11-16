#include "cave.h"

void Cave::InitCave() {
  for (int h = 0; h < MAP_HEIGHT; h++) {
    for (int w = 0; w < MAP_WIDTH; w++) {
        if ( h == 0 || w == 0 || h == MAP_HEIGHT-1 || w == MAP_WIDTH-1 ) {
            map[h][w] = ALIVE;
        } else {
          if (GetRandomValue(0, 100) > aliveProb) {
            map[h][w] = DEAD;
          } else {
            map[h][w] = ALIVE;
          }
        }
    }
  }
}

int Cave::Near(int x, int y) {
  int count = 0;
  for (int h = y - 1; h < 2 + y; h++) {
    for (int w = x - 1; w < 2 + x; w++) {
      if (!(w == x && h == y)) {
        count += map[h][w];
      }
    }
  }
  return count;
}

void Cave::Draw() {
  for (int h = 0; h < MAP_HEIGHT; h++) {
    for (int w = 0; w < MAP_WIDTH; w++) {
        if (map[h][w] == ALIVE) DrawRectangle(w * size, h * size, size, size, BLACK);
    }
  }
}

Cave::Cave() {
  InitCave();

  for (int gen = 0; gen < generations; gen++) {
    for (int h = 1; h < MAP_HEIGHT - 1; h++) {
      for (int w = 1; w < MAP_WIDTH - 1; w++) {
        int count = Near(w, h);
        if (count > 4) {
            map[h][w] = ALIVE;
        } else if (count < 4) {
            map[h][w] = DEAD;
        }
      }
    }
  }
}
