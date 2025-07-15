#include "snake.h"
#include "constants.h"

#include <vector>

extern game_globals_struct game_globals;

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
  case AI_RANDOM:
    m_color = {252, 186, 3, 255};
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

void Snake::decide_direction() {
  if (m_controller == AI_RANDOM) {
    bool valid_sum = true;
    bool valid_range = false;
    bool valid = false;
    Vector2 new_direction;
    Vector2 sum_direction;
    do {
      valid_sum = true;
      valid_range = false;
      valid = false;
      int change_dir = GetRandomValue(0, 1);
      if (change_dir) {
        int rnd_val = GetRandomValue(0, 3);
        switch (rnd_val) {
        case 0:
          new_direction.x = 0;
          new_direction.y = 1;
          break;
        case 1:
          new_direction.x = 1;
          new_direction.y = 0;
          break;
        case 2:
          new_direction.x = 0;
          new_direction.y = -1;
          break;
        default:
          new_direction.x = -1;
          new_direction.y = 0;
        }
      } else {
        new_direction = m_direction;
      };
      sum_direction = Vector2Add(m_direction, new_direction);
      if (sum_direction.x == 0 && sum_direction.y == 0)
        valid_sum = false;
      if ((get_head().x + new_direction.x) >= 0 &&
          (get_head().x + new_direction.x) < game_globals.cell_count &&
          (get_head().y + new_direction.y) >= 0 &&
          (get_head().y + new_direction.y) < game_globals.cell_count)
        valid_range = true;
      valid = valid_sum && valid_range;
    } while (!valid);
    m_direction = new_direction;
  }
}

void Snake::update() {

  if (m_controller != PLAYER)
    decide_direction();

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
  m_direction = {1, 0};
  m_head_color = m_color;
}
