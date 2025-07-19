#include "snake.h"
#include "astar.hpp"
#include "constants.h"

#include <iostream>
#include <vector>

extern game_globals_struct game_globals;

std::vector<Vector2> filter_food_by_type(std::vector<Food> &food, int cat,
                                         int type, bool accept_bonus) {
  std::vector<Vector2> tmp;
  for (auto &item : food) {
    if (item.get_category() == cat && item.get_type() == type) {
      tmp.push_back(item.get_position());
    }
  }
  if (accept_bonus) {
    for (auto &item : food) {
      if (item.get_category() == FOOD_BONUS) {
        tmp.push_back(item.get_position());
      }
    }
  }
  return tmp;
}

bool is_vec2_in_vec(Vector2 t, std::vector<Vector2> v) {
  for (auto &vv : v) {
    if (Vector2Equals(t, vv))
      return true;
  }
  return false;
}

Vector2 Snake::find_path(Game_grid_t &grid, Vector2 target, bool *fail) {
  AStar::AStarFast pathFinder;
  pathFinder.setHeuristic(AStar::Heuristic::manhattan);
  pathFinder.setDiagonalMovement(false);
  *fail = false;

  std::vector<uint32_t> world(game_globals.cell_count * game_globals.cell_count,
                              0);

  for (int y = 0; y < game_globals.cell_count; y++) {
    for (int x = 0; x < game_globals.cell_count; x++) {
      world[game_globals.cell_count * y + x] = grid[y][x];
    }
  }

  // target can't be an obstacle!
  world[game_globals.cell_count * target.y + target.x] = 0;

  // set lambda function to check if is an obstacle (value == 1)
  auto isObstacle = [](uint32_t value) -> bool { return value == 1; };
  pathFinder.setObstacle(isObstacle);

  auto path = pathFinder.findPath(
      {get_head().x, get_head().y}, {target.x, target.y}, world,
      {game_globals.cell_count, game_globals.cell_count});

  Vector2 first_move;
  Vector2 direction, old_direction;

  if (path.size() >= 2) {
    first_move.x = path[path.size() - 2].x;
    first_move.y = path[path.size() - 2].y;

    old_direction = m_direction;
    direction = Vector2Subtract(first_move, get_head());
    direction.x = (float)(int)(direction.x);
    direction.y = (float)(int)(direction.y);

    // direction.x = (direction.x > 0) ? 1 : ((direction.x < 0) ? -1 : 0);
    // direction.y = (direction.y > 0) ? 1 : ((direction.y < 0) ? -1 : 0);
    // direction.x = (direction.x < -1) ? -1 : direction.x;
    // direction.y = (direction.y < -1) ? -1 : direction.y;
    // direction.x = (direction.x > 1) ? 1 : direction.x;
    // direction.y = (direction.y > 1) ? 1 : direction.y;

    if (Vector2Equals(Vector2Add(direction, old_direction), Vector2{0, 0})) {
      // std::cout << "Direction conflict! Forced direction" << std::endl;
      direction = m_direction;
    }

  } else {
    // std::cout << "No pathfinding! Forced direction" << std::endl;
    direction = m_direction;
    *fail = true;
  }

  // std::cout << "Head is at " << get_head().x << "," << get_head().y
  //           << std::endl;
  // std::cout << "Target is at " << target.x << "," << target.y << std::endl;
  // std::cout << "1st move is at " << first_move.x << "," << first_move.y
  //           << std::endl;
  // std::cout << "first dir: " << Vector2Subtract(first_move, get_head()).x <<
  // " "
  //           << Vector2Subtract(first_move, get_head()).y << std::endl;
  // std::cout << "actual: " << direction.x << " " << direction.y << std::endl;

  // // Print the path
  // for (auto &p : path) {
  //   std::cout << p.x << " " << p.y << std::endl;
  // }
  return direction;
}

Snake::Snake(Snake_ctrl_t ctrl, std::deque<Vector2> init_body) {
  m_initial_body = init_body;
  m_body = init_body;
  m_controller = ctrl;
  set_color();
}

void Snake::set_color() {
  switch (m_controller) {
  case PLAYER:
    m_color = game_globals.dark_green;
    m_head_color = game_globals.head_highlight_color;
    break;
  case AI_RANDOM_WALK:
    m_color = {252, 186, 3, 255};
    m_head_color = m_color;
    break;
  case AI_OUTER:
    m_color = {130, 68, 150, 255};
    m_head_color = m_color;
    break;
  case AI_EAT_APPLES:
    m_color = {200, 0, 0, 255};
    m_head_color = m_color;
    break;
  case AI_CHASE:
    m_color = {50, 121, 168, 255};
    m_head_color = m_color;
    break;
  case AI_EAT_CLOSEST:
    m_color = {37, 190, 77, 255};
    m_head_color = m_color;
    break;
  default:
    m_color = {64, 64, 64, 255};
    m_head_color = m_color;
  }
}

void Snake::draw() {
  for (int i = 0; i < m_body.size(); i++) {
    float x = m_body[i].x * game_globals.cell_size;
    float y = m_body[i].y * game_globals.cell_size;

    Rectangle rec{x + game_globals.offset, y + game_globals.offset,
                  (float)game_globals.cell_size, (float)game_globals.cell_size};

    if (m_head_highlight) {
      m_head_color = game_globals.head_highlight_color;
      m_highlight_time = GetTime();
      reset_head();
    }
    if (ColorIsEqual(m_head_color, game_globals.head_highlight_color) &&
        (GetTime() - m_highlight_time > game_globals.head_highlight_timeout)) {
      m_head_color = m_color;
    }
    if (i == 0) {
      Rectangle rec{x + game_globals.offset, y + game_globals.offset,
                    (float)game_globals.cell_size,
                    (float)game_globals.cell_size};
      DrawRectangleRounded(rec, 0.5, 6, m_head_color);
    } else {
      Rectangle rec{x + game_globals.offset + 3, y + game_globals.offset + 3,
                    (float)game_globals.cell_size - 6,
                    (float)game_globals.cell_size - 6};
      DrawRectangleRounded(rec, 0.25, 6, m_color);
    }
  }
}

void Snake::decide_direction(Game_grid_t &grid, std::vector<Food> &food,
                             std::deque<Vector2> player_body) {

  const std::vector<Vector2> directions = {Vector2{0, -1}, Vector2{0, 1},
                                           Vector2{-1, 0}, Vector2{1, 0}};

  // for (int j = 0; j < game_globals.cell_count; j++) {
  //   for (int i = 0; i < game_globals.cell_count; i++) {
  //     printf("%s", grid[j][i] ? "■" : "□");
  //   }
  //   printf("\n");
  // }

  if (m_controller == AI_RANDOM_WALK) {

    int n = -1;
    Vector2 candidate;
    std::vector<Vector2> allowed;
    allowed.clear();
    for (auto &d : directions) {
      candidate = Vector2Add(get_head(), d);
      if (!Vector2Equals(Vector2Add(m_direction, d), Vector2{0, 0})) {
        if ((candidate.x) >= 0 && (candidate.x) < game_globals.cell_count &&
            (candidate.y) >= 0 && (candidate.y) < game_globals.cell_count) {
          if (grid[candidate.y][candidate.x] == 0) {
            allowed.push_back(d);
            // std::cout << d.x << "," << d.y << " | " << candidate.x << ","
            //           << candidate.y << " is allowed" << std::endl;
          }
        }
      }
    }
    if (allowed.size() == 0) {
      // do nothing, will die
      // std::cout << "No free cells!" << std::endl;
    } else {
      int prob_keep_old = 0;
      if (is_vec2_in_vec(m_direction, allowed)) {
        prob_keep_old = 50;
      }
      int tmp = GetRandomValue(0, 100);
      if (tmp >= prob_keep_old) {
        n = GetRandomValue(0, allowed.size() - 1);
        m_direction = allowed[n];
        // std::cout << "Chose " << m_direction.x << "," << m_direction.y
        //           << " rnd " << n << std::endl;
      } else {
        // keep olp direction
        // std::cout << "Kept old direction" << std::endl;
      }
    }
  } else if (m_controller == AI_OUTER) {
    int n = -1;
    Vector2 candidate;
    std::vector<Vector2> allowed;
    allowed.clear();
    for (auto &d : directions) {
      candidate = Vector2Add(get_head(), d);
      if (!Vector2Equals(Vector2Add(m_direction, d), Vector2{0, 0})) {
        if ((candidate.x) >= 0 && (candidate.x) < game_globals.cell_count &&
            (candidate.y) >= 0 && (candidate.y) < game_globals.cell_count) {
          if (grid[candidate.y][candidate.x] == 0) {
            allowed.push_back(d);
            // std::cout << d.x << "," << d.y << " | " << candidate.x << ","
            //           << candidate.y << " is allowed" << std::endl;
          }
        }
      }
    }
    if (allowed.size() == 0) {
      // do nothing, will die
      // std::cout << "No free cells!" << std::endl;
    } else {
      int rnd_dec = GetRandomValue(0, 100);
      const int prob_random = 10;
      if (rnd_dec <= prob_random) {
        n = GetRandomValue(0, allowed.size() - 1);
        m_direction = allowed[n];
      } else {
        int max_abs = 0;
        int curr;
        int j = 0;
        int x, y;
        int max_idx = 0;
        for (auto &d : allowed) {
          candidate = Vector2Add(get_head(), d);
          x = candidate.x - game_globals.cell_count / 2;
          y = candidate.y - game_globals.cell_count / 2;
          curr = x * x + y * y;
          if (curr > max_abs) {
            max_abs = curr;
            max_idx = j;
          }
          j++;
        }
        m_direction = allowed[max_idx];
      }
    }
  } else if (m_controller == AI_EAT_APPLES) {
    m_direction = ai_eat_apples(grid, food);
  } else if (m_controller == AI_CHASE) {
    m_direction = ai_chase(grid, player_body);
  } else if (m_controller == AI_EAT_CLOSEST) {
    m_direction = ai_eat_closest(grid, food);
  } else if (m_controller == AI_TRAIN) {
    m_direction = ai_eat_closest(grid, food);
  }
}

Vector2 Snake::ai_chase(Game_grid_t &grid, std::deque<Vector2> player_body) {

  Vector2 direction;
  Vector2 target = player_body.back();
  target.x += GetRandomValue(-3, 3);
  target.y += GetRandomValue(-3, 3);
  target.x = (target.x < 0) ? 0 : target.x;
  target.y = (target.y < 0) ? 0 : target.y;
  target.x = (target.x >= game_globals.cell_count) ? game_globals.cell_count - 1
                                                   : target.x;
  target.y = (target.y >= game_globals.cell_count) ? game_globals.cell_count - 1
                                                   : target.y;
  bool fail;
  direction = find_path(grid, target, &fail);
  return direction;
}

#define FOOD_APPLE 0
Vector2 Snake::ai_eat_apples(Game_grid_t &grid, std::vector<Food> &food) {

  Vector2 target;
  bool *fail;
  std::vector<Vector2> tmp =
      filter_food_by_type(food, FOOD_REGULAR, FOOD_APPLE, true);
  if (tmp.size() == 0) {
    if (abs(get_head().x - (game_globals.cell_count - 1) / 2) < 3 &&
        abs(get_head().y - (game_globals.cell_count - 1) / 2) < 3) {
      target.x = game_globals.cell_count - 1;
      target.y = game_globals.cell_count - 1;
    } else {
      target.x = (game_globals.cell_count - 1) / 2;
      target.y = (game_globals.cell_count - 1) / 2;
    }
  } else {
    target = tmp[0]; // go to the first apple found
  }
  return find_path(grid, target, fail);
}

Vector2 Snake::ai_eat_closest(Game_grid_t &grid, std::vector<Food> &food) {

  Vector2 target;
  bool *fail;
  float shortest = 10000.0f;
  int best_idx = 0;
  int k = 0;
  for (auto &f : food) {
    float curr = Vector2Distance(get_head(), f.get_position());
    if (curr < shortest) {
      shortest = curr;
      best_idx = k;
    }
    k++;
  }
  if (best_idx < food.size()) {
    target = food[best_idx].get_position();
  } else {
    target.x = (game_globals.cell_count - 1) / 2;
    target.y = (game_globals.cell_count - 1) / 2;
  }
  return find_path(grid, target, fail);
}

void Snake::update(Game_grid_t &grid, std::vector<Food> &food,
                   std::deque<Vector2> player_body) {

  if (m_controller != PLAYER)
    decide_direction(grid, food, player_body);

  m_body.push_front(Vector2Add(m_body[0], m_direction));
  if (m_add_segment) {
    mark_segment_added();
  } else {
    m_body.pop_back();
  }
  if (m_segm_to_remove) {
    for (int i = 0; i < m_segm_to_remove; i++) {
      if (m_body.size() < 4)
        break;
      m_body.pop_back();
    }
    mark_segments_removed();
  }
}

void Snake::reset() {
  m_body = m_initial_body;
  if (get_controller() == PLAYER)
    m_direction = {1, 0};
  else
    m_direction = {0, 1};
  m_head_color = m_color;
}
