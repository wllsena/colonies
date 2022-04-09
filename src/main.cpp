#include "structure.cpp"
#include <array>
#include <iostream>
#include <stdlib.h>
#include <vector>

using namespace std;

int main() {
  vector<Food *> foods = *new vector<Food *>;
  Colony colony = Colony(100, 100, 50, 50, 1, 10);

  cout << ant->x << " " << ant->y << endl;
  ant.walk_randomly();
}
