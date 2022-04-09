#include <algorithm>
#include <array>
#include <iostream>
#include <limits>
#include <stdlib.h>
#include <vector>

using namespace std;

struct Food {
  int x;
  int y;
  float amount;
  float replacement_fee;
  int consumer_limit;
  int consumers;

  Food(const int x_, const int y_, const float amount_,
       const float replacement_fee_, const int consumer_limit_) {
    x = x_;
    y = y_;
    amount = amount_;
    replacement_fee = replacement_fee_;
    consumer_limit = consumer_limit_;
    consumers = 0;
  }
};

struct Pheromone {
  int x;
  int y;
  int lifetime;

  Pheromone(const int x_, const int y_, const int lifetime_) {
    x = x_;
    y = y_;
    lifetime = lifetime_;
  }

  const void decrease() { lifetime -= 1; }
};

struct Ant {
  int max_x;
  int max_y;
  int x;
  int y;
  int vision;

  int colony_x;
  int colony_y;

  int goal_type; // 0 -> colony; 1 -> food; 2 -> pheromone; 3 -> enemy ant;
  int goal_num;
  int goal_x;
  int goal_y;

  Ant(const int map_x, const int map_y, const int colony_x_,
      const int colony_y_, const int vision_) {
    max_x = map_x;
    max_y = map_y;
    x = colony_x_;
    y = colony_y_;
    vision = vision_;

    colony_x = colony_x_;
    colony_y = colony_y_;

    goal_type = -1;
    goal_num = -1;
    goal_x = -1;
    goal_y = -1;
  }

  const bool has_goal() { return goal_type != -1; }

  const bool on_goal() { return goal_x == x and goal_y == y; }

  const void walk_to_goal() {
    int diff_x = goal_x - x;
    int diff_y = goal_y - y;

    if (abs(diff_x) >= abs(diff_y))
      x += diff_x > 0 ? 1 : -1;
    else
      y += diff_y > 0 ? 1 : -1;
  }

  const void walk_randomly() {
    vector<int> directions; // 0 = up; 1 = right; 2 = down; 3 = left
    int direction;

    if (y != max_y)
      directions.push_back(0);
    if (x != max_x)
      directions.push_back(1);
    if (y != 0)
      directions.push_back(2);
    if (x != 0)
      directions.push_back(3);

    direction = directions[rand() % directions.size()];

    if (direction == 0)
      y += 1;
    else if (direction == 1)
      x += 1;
    else if (direction == 2)
      y -= 1;
    else if (direction == 3)
      x -= 1;
  }

  const bool search_food(const vector<Food *> foods) {
    int num = 0;
    int distance;
    int min_distance = vision + 1;
    for (auto food : foods) {
      distance = abs(food->x - x) + abs(food->y - y);
      if (distance < min_distance) {
        min_distance = distance;

        goal_type = 1;
        goal_num = num;
        goal_x = food->x;
        goal_y = food->y;
      }
      num += 1;
    }

    return goal_type == 1;
  }

  const bool search_pheromone(const vector<Pheromone *> pheromones) {
    int num = 0;
    int distance;
    int min_distance = vision + 1;
    for (auto pheromone : pheromones) {
      distance = abs(pheromone->x - x) + abs(pheromone->y - y);
      if (distance < min_distance) {
        min_distance = distance;

        goal_type = 2;
        goal_num = num;
        goal_x = pheromone->x;
        goal_y = pheromone->y;
      }
      num += 1;
    }

    return goal_type == 2;
  }

  const bool search_next_pheromone(const vector<Pheromone *> pheromones) {
    int num = 0;
    int lifetime;
    int min_lifetime = numeric_limits<int>::max();
    for (auto pheromone : pheromones) {
      lifetime = pheromone->lifetime;
      if (abs(pheromone->x - x) + abs(pheromone->y - y) == 1 &
          lifetime < min_lifetime) {
        min_lifetime = lifetime;

        goal_type = 2;
        goal_num = num;
        goal_x = pheromone->x;
        goal_y = pheromone->y;
      }
      num += 1;
    }

    return goal_type == 2;
  }

  const array<int, 3> play(const vector<Food *> foods,
                           const vector<Pheromone *> pheromones) {
    if (has_goal()) {
      if (on_goal()) {
        if (goal_type == 2) { // pheromone
          if (search_food(foods))
            ;
          else if (search_next_pheromone(pheromones))
            ;
        }
        return {goal_type, goal_num, -1};
      } else {
        walk_to_goal();
        if (goal_type == 0) // colony
          return {0, x, y};
      }

    } else {
      if (search_food(foods))
        ;
      else if (search_pheromone(pheromones))
        ;

      if (goal_type != -1)
        walk_to_goal();
      else
        walk_randomly();
    }

    return {goal_type, -1, -1};
  }

  const void leave_food() { // after 0
    goal_type = -1;
    goal_num = -1;
    goal_x = -1;
    goal_y = -1;
  }

  const void get_food() { // after 1
    goal_type = 0;
    goal_num = -1;
    goal_x = colony_x;
    goal_y = colony_y;
  }
};

struct Colony {
  int x;
  int y;
  vector<Ant *> ants;

  Colony(const int map_x, const int map_y, const int x_, const int y_,
         const int n_ants, const int vision) {
    x = x_;
    y = y_;
    for (int i = 0; i != n_ants; i++)
      ants.push_back(new Ant(map_x, map_y, x, y, vision));
  }
};
