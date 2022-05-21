#include "../include/colors.hpp"
#include "structure.cpp"
#include <iostream>
#include <string>

using namespace std;

void clear_console() { printf("\033c"); }

void print_color(const string value, const int color) {
  if (color == -1)
    cout << FWHT(value);
  else if (color == 0)
    cout << FRED(value);
  else if (color == 1)
    cout << FGRN(value);
  else if (color == 2)
    cout << FYEL(value);
  else if (color == 3)
    cout << FCYN(value);
  else if (color == 4)
    cout << FMAG(value);
  else if (color == 5)
    cout << FBLU(value);
};

void print_iter(const int iter) {
  print_color("Iter: ", -1);
  print_color(to_string(iter), -1);
  cout << endl;
};

void print_food(const int color) { print_color(to_string(color), -1); };

void print_colony(const int color) { print_color("#", color); };

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
      colors[pheromone->y][pheromone->x] = pheromone->num;
    }

    for (const auto &ant : colony->ants) {
      types[ant->y][ant->x] = 2;
      colors[ant->y][ant->x] = colony->num;
    }

    types[colony->y][colony->x] = 1;
    colors[colony->y][colony->x] = colony->num;
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

void print_values(World *world) {
  vector<vector<int> > stats_food = world->get_stats_foods();
  for (vector<int> stats : stats_food) {
    print_color("Food: ", -1);
    print_color(to_string(stats[0]), -1);
    print_color(". Amount: ", -1);
    print_color(to_string(stats[1]), -1);
    print_color(". Consumers: ", -1);
    print_color(to_string(stats[2]), -1);
    print_color(".", -1);

    cout << endl;
  }

  vector<vector<int> > stats_colonies = world->get_stats_colonies();
  for (vector<int> stats : stats_colonies) {
    print_color("Colony: ", stats[0]);
    print_color(to_string(stats[0]), stats[0]);
    print_color(". Food: ", stats[0]);
    print_color(to_string(stats[1]), stats[0]);
    print_color(". Inside: ", stats[0]);
    print_color(to_string(stats[2]), stats[0]);
    print_color(". Ants: ", stats[0]);
    print_color(to_string(stats[3]), stats[0]);
    print_color(". Pheromones: ", stats[0]);
    print_color(to_string(stats[4]), stats[0]);
    print_color(".", stats[0]);

    cout << endl;
  }
};
