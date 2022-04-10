#include "../include/colors.hpp"
#include "structure.cpp"
#include <iostream>
#include <stdlib.h>
#include <string>
#include <vector>

using namespace std;

void print_color(const string value, const int color) {
  if (color == -1)
    cout << value;
  else if (color == 0)
    cout << FBLU(value);
};

void clear_console() { printf("\033c"); }

void print_iter(const int iter) {
  print_color("Iter: ", -1);
  print_color(to_string(iter), -1);
  cout << endl;
};

void print_food(const int color) { print_color("#", color); };

void print_colony(const int color) { print_color("O", color); };

void print_ant(const int color) { print_color("x", color); };

void print_pheromone(const int color) { print_color(".", color); };

void print_void(const int color) { print_color(" ", color); };

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

  for (auto pheromone : world->pheromones) {
    types[pheromone->y][pheromone->x] = 3;
    colors[pheromone->y][pheromone->x] = pheromone->clan;
  }

  for (auto colony : world->colonies) {
    for (auto ant : colony->ants) {
      types[ant->y][ant->x] = 2;
      colors[ant->y][ant->x] = colony->clan;
    }
    types[colony->y][colony->x] = 1;
    colors[colony->y][colony->x] = colony->clan;
  }

  for (auto food : world->foods) {
    types[food->y][food->x] = 0;
    colors[food->y][food->x] = -1;
  }

  int type;
  int color;
  cout << string(world->x + 2, '_') << endl;
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
  cout << string(world->x + 2, '_') << endl;
};

void print_amounts(const vector<int> amounts) {
  for (int clan = 0; clan != amounts.size(); clan++) {
    print_color("Colony; ", clan);
    print_color(to_string(amounts[clan]), clan);
    print_color("; ", clan);
  }
  cout << endl;
};
