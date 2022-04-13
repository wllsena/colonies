#include "src/game.cpp"
#include <iostream>

using namespace std;

int main() {
  int n_threads = 2;
  int x = 50;
  int y = 20;
  int n_iters = 1000;
  int sleep_time = 250000;

  vector<Food *> foods;
  foods.push_back(new Food(0, 30, 15, 10, 1, 5));
  foods.push_back(new Food(1, 40, 17, 7, 0.1, 2));
  foods.push_back(new Food(2, 25, 10, 20, 2, 2));
  foods.push_back(new Food(3, 10, 12, 2, 0.25, 3));

  vector<Colony *> colonies;
  colonies.push_back(new Colony(0, 5, 5, 2, 8, 20, 10, x, y));
  colonies.push_back(new Colony(1, 48, 0, 2, 10, 15, 15, x, y));
  colonies.push_back(new Colony(2, 5, 18, 2, 12, 30, 5, x, y));
  colonies.push_back(new Colony(3, 20, 3, 2, 5, 20, 10, x, y));
  colonies.push_back(new Colony(4, 25, 18, 1, 2, 20, 2, x, y));

  World *world = new World(n_threads, x, y, foods, colonies);

  play(world, n_iters, sleep_time);
}
