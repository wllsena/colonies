#include "../include/colors.hpp"
#include "structure.cpp"
#include <iostream>
#include <stdlib.h>
#include <string>
#include <vector>

using namespace std;

void clear_console() { printf("\033c"); }

void print_color(const string value, const int color) {
  if (color == -1)
    cout << value;
  else if (color == 0)
    cout << FBLU(value);
  else if (color == 1)
    cout << FRED(value);
};

void print_iter(const int iter) {
  print_color("Iter: ", -1);
  print_color(to_string(iter), -1);
  cout << endl;
};

void print_food(const int color) { print_color("#", -1); };

void print_colony(const int color) { print_color("O", color); };

void print_ant(const int color) { print_color("x", color); };

void print_pheromone(const int color) { print_color(".", color); };

void print_void(const int color) { print_color(" ", -1); };

void print_world(const World *world) {
  // food -> 0; colony -> 1; ant -> 2; pheromone -> 3
  int types[world->y][world->x];
  int colors[world->y][world->x];
  for (int i = 0; i != world->y; i++) {
    for (int j = 0; j != world->x; j++) {
      types[i][j] = -1;
      colors[i][j] = -1;
    }
  }

  for (const auto &colony : world->colonies) {
    for (const auto &pheromone : colony->pheromones) {
      types[pheromone->y][pheromone->x] = 3;
      colors[pheromone->y][pheromone->x] = pheromone->clan;
    }

    for (const auto &ant : colony->ants) {
      types[ant->y][ant->x] = 2;
      colors[ant->y][ant->x] = colony->clan;
    }

    types[colony->y][colony->x] = 1;
    colors[colony->y][colony->x] = colony->clan;
  }

  for (const auto &food : world->foods) {
    types[food->y][food->x] = 0;
    colors[food->y][food->x] = food->num;
  }

  int type;
  int color;
  cout << string(world->x + 2, '-') << endl;
  for (int i = 0; i != world->y; i++) {
    cout << "|";
    for (int j = 0; j != world->x; j++) {
      type = types[i][j];
      color = colors[i][j];

      if (type == -1)
        print_void(color);
      else if (type == 0)
        print_food(color);
      else if (type == 1)
        print_colony(color);
      else if (type == 2)
        print_ant(color);
      else if (type == 3)
        print_pheromone(color);
    }
    cout << "|" << endl;
  }
  cout << string(world->x + 2, '-') << endl;
};

void print_amounts(const World *world) {
  for (const auto &colony : world->colonies) {
    print_color("Colony; ", colony->clan);
    print_color(to_string(colony->amount), colony->clan);
    print_color("; ", colony->clan);
  }
  cout << endl;
};
