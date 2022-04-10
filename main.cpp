#include "src/print.cpp"
#include <array>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <unistd.h>
#include <vector>

using namespace std;

int main() {
  int x = 50;
  int y = 20;

  vector<Food *> foods;
  foods.push_back(new Food(0, 30, 15, 10, 0, 1));

  vector<Colony *> colonies;
  colonies.push_back(new Colony(0, 5, 5, 1, 10, 10, 15, x, y));

  World *world = new World(x, y, foods, colonies);

  vector<int> amounts;

  clear_console();
  print_iter(0);
  print_world(world);
  print_amounts(world);
  usleep(250000);

  for (int i = 1; i != 1000; i++) {
    world->play();
    clear_console();
    print_iter(i);
    print_world(world);
    print_amounts(world);
    usleep(250000);
  }
}
