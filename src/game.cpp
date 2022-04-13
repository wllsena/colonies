#include "structure_and_print.cpp"
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

void play_(World *world, int n_iters, int sleep_time) {
  clear_console();
  cout << "Press ENTER to start, pause and resume." << endl;

  for (int i = 0; i != n_iters;) {
    if (paused) {
      world->update();
      clear_console();
      print_iter(i);
      print_world(world);
      print_values(world);
      usleep(sleep_time);
      i += 1;
    }
  }
}

void play(World *world, int n_iters, int sleep_time) {
  thread t_control(control);
  thread t_play(play_, world, n_iters, sleep_time);

  t_control.join();
  t_play.join();
}
