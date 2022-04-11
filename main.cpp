#include "src/structure_and_print.cpp"
#include <iostream>
#include <thread>
#include <unistd.h>
#include <vector>

using namespace std;

bool paused = false;

void control() {
  while (true) {
    if (cin.get())
      paused = not paused;
  }
}

void play(World *world, int n_iters, int sleep_time) {
  clear_console();
  cout << "Press ENTER to start, pause and resume." << endl;

  for (int i = 0; i != n_iters;) {
    if (paused) {
      world->play();
      clear_console();
      print_iter(i);
      print_world(world);
      print_values(world);
      usleep(sleep_time);
      i += 1;
    }
  }
}

int main() {
  int threads = 1;
  int x = 50;
  int y = 20;
  int n_iters = 1000;
  int sleep_time = 250000;

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

  thread t_control(control);
  thread t_play(play, world, n_iters, sleep_time);

  t_control.join();
  t_play.join();
}
