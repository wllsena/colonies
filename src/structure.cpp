#include <array>
#include <map>
#include <vector>

using namespace std;

struct Food {
  int num;
  int x;
  int y;
  float amount;
  float replacement;
  int limit;

  float amount_max;
  int consumers;

  Food(const int num_, const int x_, const int y_, const float amount_,
       const float replacement_, const int limit_) {
    num = num_;
    x = x_;
    y = y_;
    amount = amount_;
    replacement = replacement_;
    limit = limit_;

    amount_max = amount_;
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
    } else
      return false;
  }
};

struct Pheromone {
  int num;
  int x;
  int y;
  int from_x;
  int from_y;
  int lifetime;

  Pheromone(const int num_, const int x_, const int y_, const int from_x_,
            const int from_y_, const int lifetime_) {
    num = num_;
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
  int num;
  int world_x;
  int world_y;
  int colony_x;
  int colony_y;
  int vision;

  int x;
  int y;

  int goal_type; // 0 -> colony; 1 -> food; 2 -> pheromone; 3 -> enemy ant;
  int goal_num;
  int goal_x;
  int goal_y;

  Ant(const int num_, const int world_x_, const int world_y_,
      const int colony_x_, const int colony_y_, const int vision_) {
    num = num_;
    world_x = world_x_;
    world_y = world_y_;
    colony_x = colony_x_;
    colony_y = colony_y_;
    vision = vision_;

    x = colony_x_;
    y = colony_y_;

    goal_type = -1;
    goal_num = -1;
    goal_x = -1;
    goal_y = -1;
  }

  void reset_goal() {
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
    if (x != world_x - 1)
      directions.push_back(1);
    if (y != 0)
      directions.push_back(2);
    if (y != world_y - 1)
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
    if (has_goal())
      walk_to_goal();
    else
      walk_randomly();
  }

  bool search_colony() { return goal_type == 0; }

  bool search_food(const vector<Food *> foods) {
    reset_goal();

    int distance;
    int min_distance = vision + 1;
    for (const auto &food : foods) {
      distance = abs(food->x - x) + abs(food->y - y);
      if (distance < min_distance and food->amount > 0) {
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
    reset_goal();

    int max_lifetime = 0;
    for (const auto &pheromone : pheromones) {
      if (pheromone->x == x and pheromone->y == y and
          pheromone->lifetime > max_lifetime) {
        max_lifetime = pheromone->lifetime;

        goal_type = 2;
        goal_num = -1;
        goal_x = pheromone->from_x;
        goal_y = pheromone->from_y;
      }
    }

    return goal_type == 2;
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
    reset_goal();

    array<int, 2> key;
    map<array<int, 2>, int> counter;
    for (const auto &pheromone : pheromones) {
      if (abs(pheromone->x - x) + abs(pheromone->y - y) <= vision and
          not_same_direction(x, y, pheromone->x, pheromone->y, colony_x,
                             colony_y)) {
        key[0] = pheromone->x;
        key[1] = pheromone->y;

        if (counter.count(key))
          counter[key] += 1;
        else
          counter[key] = 1;
      }
    }

    int max_count = 0;
    for (const auto &count : counter) {
      if (count.second > max_count) {
        max_count = count.second;

        goal_type = 2;
        goal_num = -1;
        goal_x = count.first[0];
        goal_y = count.first[1];
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
    if (search_colony())
      ;
    else if (search_food(foods))
      ;
    else if (search_next_pheromone(pheromones))
      ;
    else if (search_pheromone(pheromones))
      ;
    else {
      reset_goal();
      walk();
      return {-1, -1, -1};
    }

    if (on_goal()) {
      if (goal_type == 0) // colony
        return {0, -1, -1};
      else if (goal_type == 1) // food
        return {1, goal_num, -1};
      else {
        walk();
        return {-1, -1, -1};
      }

    } else if (goal_type == 0) { // colony
      int old_x = x;
      int old_y = y;
      walk();
      return {2, old_x, old_y};
    } else {
      walk();
      return {-1, -1, -1};
    }
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
  int num;
  int x;
  int y;
  int limit;
  int ph_timelife;

  int amount;
  int consumers;

  vector<Ant *> ants;
  vector<Pheromone *> pheromones;

  Colony(const int num_, const int x_, const int y_, const int limit_,
         const int ph_timelife_, const int n_ants, const int ant_vision,
         const int world_x, const int world_y) {
    num = num_;
    x = x_;
    y = y_;
    limit = limit_;
    ph_timelife = ph_timelife_;

    amount = 0;
    consumers = 0;

    for (int i = 0; i != n_ants; i++)
      ants.push_back(new Ant(num_, world_x, world_y, x_, y_, ant_vision));
  }

  bool store() {
    if (consumers < limit) {
      amount += 1;
      consumers += 1;
      return true;
    } else {
      return false;
    }
  }

  void play(vector<Food *> foods) {
    consumers = 0;

    vector<Pheromone *> new_pheromens;
    for (const auto &pheromone : pheromones)
      if (pheromone->play())
        new_pheromens.push_back(pheromone);

    pheromones = new_pheromens;

    array<int, 3> result;
    for (const auto &ant : ants) {
      result = ant->play(foods, pheromones);

      if (result[0] == 0 and store())
        ant->store_food();
      else if (result[0] == 1 and foods[result[1]]->serve())
        ant->get_food();
      else if (result[0] == 2)
        pheromones.push_back(new Pheromone(num, ant->x, ant->y, result[1],
                                           result[2], ph_timelife));
    }
  }
};

struct World {
  int x;
  int y;

  vector<Food *> foods;
  vector<Colony *> colonies;

  World(const int x_, const int y_, const vector<Food *> foods_,
        const vector<Colony *> colonies_) {
    x = x_;
    y = y_;

    foods = foods_;
    colonies = colonies_;
  }

  void play() {
    for (const auto &food : foods)
      food->play();

    for (const auto &colony : colonies)
      colony->play(foods);
  }
};
