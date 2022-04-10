#include <algorithm>
#include <array>
#include <iostream>
#include <limits>
#include <stdlib.h>
#include <vector>

using namespace std;

struct Food {
  int num;
  int x;
  int y;
  float amount;
  float replacement;
  float amount_max;
  int limit;
  int consumers;

  Food(const int num_, const int x_, const int y_, const float amount_,
       const float replacement_, const int limit_) {
    num = num_;
    x = x_;
    y = y_;
    amount = amount_;
    replacement = replacement_;
    amount_max = amount_;
    limit = limit_;
    consumers = 0;
  }

  void play() {
    amount = min(amount + replacement, amount_max);
    consumers = 0;
  }

  bool serve() {
    if (amount != 0 and consumers < limit) {
      amount -= 1;
      consumers += 1;
      return true;
    } else {
      return false;
    }
  }
};

struct Pheromone {
  int clan;
  int x;
  int y;
  int from_x;
  int from_y;
  int lifetime;

  Pheromone(const int clan_, const int x_, const int y_, const int from_x_,
            const int from_y_, const int lifetime_) {
    clan = clan_;
    x = x_;
    y = y_;
    from_x = from_x_;
    from_y = from_y_;
    lifetime = lifetime_;
  }

  bool play() {
    lifetime -= 1;
    return lifetime != 0;
  }
};

struct Ant {
  int clan;
  int x;
  int y;
  int vision;

  int max_x;
  int max_y;
  int colony_x;
  int colony_y;

  int goal_type; // 0 -> colony; 1 -> food; 2 -> pheromone; 3 -> enemy ant;
  int goal_num;
  int goal_x;
  int goal_y;

  Ant(const int clan_, const int colony_x_, const int colony_y_, const int vision_,
      const int world_x_, const int world_y_) {
    clan = clan_;
    x = colony_x_;
    y = colony_y_;
    vision = vision_;

    max_x = world_x_;
    max_y = world_y_;
    colony_x = colony_x_;
    colony_y = colony_y_;

    goal_type = -1;
    goal_num = -1;
    goal_x = -1;
    goal_y = -1;
  }

  bool has_goal() { return goal_type != -1; }

  bool on_goal() { return goal_x == x and goal_y == y; }

  void walk_to_goal() {
    int diff_x = goal_x - x;
    int diff_y = goal_y - y;

    if (abs(diff_x) >= abs(diff_y))
      x += diff_x > 0 ? 1 : -1;
    else
      y += diff_y > 0 ? 1 : -1;
  }

  void walk_randomly() {
    vector<int> directions;

    if (x != 0)
      directions.push_back(0);
    if (x != max_x)
      directions.push_back(1);
    if (y != 0)
      directions.push_back(2);
    if (y != max_y)
      directions.push_back(3);

    int direction = directions[rand() % directions.size()];

    if (direction == 0)
      x -= 1;
    else if (direction == 1)
      x += 1;
    else if (direction == 2)
      y -= 1;
    else if (direction == 3)
      y += 1;
  }

  void walk() {
    if (goal_type != -1)
      walk_to_goal();
    else
      walk_randomly();
  }

  bool search_food(const vector<Food *> foods) {
    int distance;
    int min_distance = vision + 1;
    for (auto food : foods) {
      distance = abs(food->x - x) + abs(food->y - y);
      if (distance < min_distance) {
        min_distance = distance;

        goal_type = 1;
        goal_num = food->num;
        goal_x = food->x;
        goal_y = food->y;
      }
    }

    return goal_type == 1;
  }

  bool search_next_pheromone(const vector<Pheromone *> pheromones) {
    for (auto pheromone : pheromones) {
      if (pheromone->x == x and pheromone->y == y) {
        goal_type = 2;
        goal_num = -1;
        goal_x = pheromone->from_x;
        goal_y = pheromone->from_y;

        return true;
      }
    }

    return false;
  }

  bool not_same_direction(const int x, const int y, const int p_x,
                          const int p_y, const int c_x, const int c_y) {
    bool dir1_x = p_x - x > 0;
    bool dir1_y = p_y - y > 0;
    bool dir2_x = c_x - x > 0;
    bool dir2_y = c_y - y > 0;

    return dir1_x != dir2_x or dir1_y != dir2_y;
  }

  bool search_pheromone(const vector<Pheromone *> pheromones) {
    int lifetime;
    int min_lifetime = numeric_limits<int>::max();
    for (auto pheromone : pheromones) {
      lifetime = pheromone->lifetime;
      if (not_same_direction(x, y, pheromone->x, pheromone->y, colony_x,
                             colony_y) and
          abs(pheromone->x - x) + abs(pheromone->y - y) <= vision and
          lifetime < min_lifetime) {
        min_lifetime = lifetime;

        goal_type = 2;
        goal_num = -1;
        goal_x = pheromone->x;
        goal_y = pheromone->y;
      }
    }

    return goal_type == 2;
  }

  // return:
  // {0, -1, -1} -> in colony
  // {1, goal_num, -1} -> in food
  // {2, x, y} -> going to colony
  // {-1, -1, -1} -> none of the alternatives
  array<int, 3> play(const vector<Food *> foods,
                     const vector<Pheromone *> pheromones) {
    if (has_goal()) {
      if (on_goal()) {
        if (goal_type == 0) // colony
          return {0, -1, -1};
        else if (goal_type == 1) // food
          return {1, goal_num, -1};

      } else {
        if (goal_type == 0) { // colony
          int old_x = x;
          int old_y = y;
          walk_to_goal();
          return {2, old_x, old_y};
        } else if (goal_type == 1) { // food
          walk_to_goal();
          return {-1, -1, -1};
        }
      }
    }

    reset_goal();
    if (search_food(foods))
      ;
    else if (search_next_pheromone(pheromones))
      ;
    else if (search_pheromone(pheromones))
      ;
    walk();

    return {-1, -1, -1};
  }

  void reset_goal() {
    goal_type = -1;
    goal_num = -1;
    goal_x = -1;
    goal_y = -1;
  }

  void get_food() {
    goal_type = 0;
    goal_num = -1;
    goal_x = colony_x;
    goal_y = colony_y;
  }

  void store_food() { reset_goal(); }
};

struct Colony {
  int clan;
  int x;
  int y;
  int amount;
  int consumers;
  int limit;
  vector<Ant *> ants;

  Colony(const int clan_, const int x_, const int y_, const int limit_,
         const int n_ants, const int ant_vision, const int world_x,
         const int world_y) {
    clan = clan_;
    x = x_;
    y = y_;
    amount = 0;
    consumers = 0;
    limit = limit_;
    for (int i = 0; i != n_ants; i++)
      ants.push_back(new Ant(clan_, x_, y_, ant_vision, world_x, world_y));
  }

  void play() { consumers = 0; }

  bool store() {
    if (consumers < limit) {
      amount += 1;
      consumers += 1;
      return true;
    } else {
      return false;
    }
  }
};

struct World {
  int x;
  int y;
  int ph_timelife;
  int ant_vision;
  vector<Colony *> colonies;
  vector<Food *> foods;
  vector<Pheromone *> pheromones;

  World(const int x_, const int y_, const int ph_timelife_,
        const int ant_vision_, const vector<Colony *> colonies_,
        const vector<Food *> foods_) {
    x = x_;
    y = y_;
    ph_timelife = ph_timelife_;
    ant_vision = ant_vision_;
    colonies = colonies_;
    foods = foods_;
  }

  vector<int> play() {
    for (auto colony : colonies)
      colony->play();

    for (auto food : foods)
      food->play();

    vector<Pheromone *> new_pheromens;
    for (auto pheromone : pheromones)
      if (pheromone->play())
        new_pheromens.push_back(pheromone);

    pheromones = new_pheromens;

    array<int, 3> result;
    vector<int> amounts;
    for (auto colony : colonies) {
      for (auto ant : colony->ants) {
        result = ant->play(foods, pheromones);

        if (result[0] == 0 and colony->store())
          ant->store_food();
        else if (result[0] == 1 and foods[result[1]]->serve())
          ant->get_food();
        else if (result[0] == 2)
          pheromones.push_back(new Pheromone(colony->clan, ant->x, ant->y, result[1],
                                             result[2], ph_timelife));
      }
      amounts.push_back(colony->amount);
    }

    return amounts;
  }
};
