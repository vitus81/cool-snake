#include "snake.h"
#include "constants.h"

#include <iostream>
#include <vector>

extern game_globals_struct game_globals;

bool is_vec2_in_vec(Vector2 t, std::vector<Vector2> v) {
  for (auto &vv : v) {
    if (Vector2Equals(t, vv))
      return true;
  }
  return false;
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
    DrawRectangleRounded(rec, 0.5, 6, (i == 0) ? m_head_color : m_color);
  }
}

void Snake::decide_direction(Game_grid_t &grid) {

  const std::vector<Vector2> directions = {Vector2{0, -1}, Vector2{0, 1},
                                           Vector2{-1, 0}, Vector2{1, 0}};

  if (m_controller == AI_RANDOM_WALK) {

    // for (int j = 0; j < game_globals.cell_count; j++) {
    //   for (int i = 0; i < game_globals.cell_count; i++) {
    //     printf("%s", grid[j][i] ? "■" : "□");
    //   }
    //   printf("\n");
    // }

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
      std::cout << "No free cells!" << std::endl;
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
  }
}

void Snake::update(Game_grid_t &grid) {

  if (m_controller != PLAYER)
    decide_direction(grid);

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
