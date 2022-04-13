#include <array>
#include <iostream>
#include <map>
#include <random>
#include <thread>
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
  vector<mutex *> sticks;

  Food(const int num_, const int x_, const int y_, const float amount_,
       const float replacement_, const int limit_) {
    num = num_;
    x = x_;
    y = y_;
    amount = amount_;
    replacement = replacement_;
    limit = limit_;

    amount_max = amount;
    sticks.reserve(limit_);
    for (int i = 0; i != limit_; i++)
      sticks.push_back(new mutex);
  }

  void update() {
    amount = min(amount + replacement, amount_max);
    for (auto &stick : sticks)
      stick->unlock();
  }

  bool try_stick() {
    for (auto &stick : sticks)
      if (stick->try_lock())
        return true;

    return false;
  }

  int count_sticks() {
    int count = 0;

    for (auto &stick : sticks)
      if (not stick->try_lock())
        count += 1;

    return count;
  }

  bool serve() {
    if (amount != 0 and try_stick()) {
      amount -= 1;
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

  bool update() {
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
  int old_x;
  int old_y;

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
    old_x = -1;
    old_y = -1;

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
    int old_x = x;
    int old_y = y;
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
  // 0 -> in colony
  // 1 -> in food
  // -1 -> none of the alternatives
  array<int, 2> update(const vector<Food *> foods,
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
      return {-1, -1};
    }

    if (on_goal()) {
      if (goal_type == 0) // colony
        return {0, -1};
      else if (goal_type == 1) // food
        return {1, goal_num};
      else {
        walk();
        return {-1, -1};
      }

    } else {
      walk();
      return {-1, -1};
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
  vector<mutex *> sticks;

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
    sticks.reserve(limit_);
    for (int i = 0; i != limit_; i++)
      sticks.push_back(new mutex);

    ants.reserve(n_ants);
    for (int i = 0; i != n_ants; i++)
      ants.push_back(new Ant(num_, world_x, world_y, x_, y_, ant_vision));
  }

  bool try_stick() {
    for (auto &stick : sticks)
      if (stick->try_lock())
        return true;

    return false;
  }

  int count_sticks() {
    int count = 0;

    for (auto &stick : sticks)
      if (not stick->try_lock())
        count += 1;

    return count;
  }

  bool store() {
    if (try_stick()) {
      amount += 1;
      return true;
    } else {
      return false;
    }
  }

  void update() {
    for (auto &stick : sticks)
      stick->unlock();

    vector<Pheromone *> new_pheromones;
    for (const auto &ant : ants)
      if (ant->goal_type == 0)
        new_pheromones.push_back(new Pheromone(num, ant->x, ant->y, ant->old_x,
                                           ant->old_y, ph_timelife));

    for (const auto &pheromone : pheromones)
      if (pheromone->update())
        new_pheromones.push_back(pheromone);

    pheromones = new_pheromones;
  }

  void update_ant(const int ant_index, const vector<Food *> foods) {
    Ant *ant = ants[ant_index];
    array<int, 2> result = ant->update(foods, pheromones);

    if (result[0] == 0 and store())
      ant->store_food();
    else if (result[0] == 1 and foods[result[1]]->serve())
      ant->get_food();
  }
};

struct World {
  int x;
  int y;

  vector<Food *> foods;
  vector<Colony *> colonies;
  vector<array<int, 2> > fix_ant_index;

  World(const int x_, const int y_, const vector<Food *> foods_,
        const vector<Colony *> colonies_) {
    x = x_;
    y = y_;

    foods = foods_;
    colonies = colonies_;

    array<int, 2> index = {0, 0};
    for (const auto &colony : colonies) {
      for (const auto &ant : colony->ants) {
        fix_ant_index.push_back(index);
        index[1] += 1;
      }
      index[0] += 1;
      index[1] = 0;
    }
  }

  vector<vector<array<int, 2> > *> get_ant_index(const int n_threads) {
    unsigned seed = chrono::system_clock::now().time_since_epoch().count();
    shuffle(fix_ant_index.begin(), fix_ant_index.end(),
            default_random_engine(seed));

    vector<vector<array<int, 2> > *> ant_index;
    ant_index.reserve(n_threads);
    for (int i = 0; i != n_threads; i++)
      ant_index.push_back(new vector<array<int, 2> >);

    int i = 0;
    for (const auto index : fix_ant_index) {
      ant_index[i]->push_back(index);
      if (i == n_threads - 1)
        i = 0;
      else
        i += 1;
    }

    return ant_index;
  }

  void update_ants(vector<array<int, 2> > *ant_index) {
    for (const auto &index : *ant_index) {
      colonies[index[0]]->update_ant(index[1], foods);
    }
  }

  void update_threads(const int n_threads) {
    vector<vector<array<int, 2> > *> ant_index = get_ant_index(n_threads);

    vector<thread> threads;
    threads.reserve(n_threads);
    for (const auto &ant_index_ : ant_index) {
      threads.emplace_back(&World::update_ants, this, ant_index_);
    }

    for (auto &thr : threads) {
      thr.join();
    }
  }

  void update(const int n_threads) {
    for (const auto &food : foods)
      food->update();

    for (const auto &colony : colonies)
      colony->update();

    update_threads(n_threads);
  }
};
