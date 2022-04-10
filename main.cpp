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
  foods.push_back(new Food(0, 30, 15, 20, 0.05, 1));
  foods.push_back(new Food(1, 40, 17, 15, 0.1, 1));
  foods.push_back(new Food(2, 25, 10, 30, 0.01, 1));
  foods.push_back(new Food(3, 10, 12, 5, 0.01, 1));

  vector<Colony *> colonies;
  colonies.push_back(new Colony(0, 5, 5, 1, 10, 10, 14, x, y));
  colonies.push_back(new Colony(1, 48, 0, 1, 10, 5, 15, x, y));
  colonies.push_back(new Colony(2, 5, 18, 1, 20, 30, 10, x, y));
  colonies.push_back(new Colony(3, 20, 3, 1, 5, 10, 5, x, y));

  World *world = new World(x, y, foods, colonies);

  vector<int> amounts;

  clear_console();
  print_iter(0);
  print_world(world);
  print_values(world);
  usleep(250000);

  for (int i = 1; i != 1000; i++) {
    world->play();
    clear_console();
    print_iter(i);
    print_world(world);
    print_values(world);
    usleep(250000);
  }
}
