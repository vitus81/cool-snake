#ifndef __SNAKE_H__
#define __SNAKE_H__

#include "constants.h"
#include "raylib.h"
#include "raymath.h"

#include <deque>

class Snake {
public:
  void draw();
  void update();
  void reset();
  inline void try_move_up() {
    if (m_direction.y != 1) {
      m_direction.x = 0;
      m_direction.y = -1;
    }
  }
  inline void try_move_down() {
    if (m_direction.y != -1) {
      m_direction.x = 0;
      m_direction.y = 1;
    }
  }
  inline void try_move_left() {
    if (m_direction.x != 1) {
      m_direction.x = -1;
      m_direction.y = 0;
    }
  }
  inline void try_move_right() {
    if (m_direction.x != -1) {
      m_direction.x = 1;
      m_direction.y = 0;
    }
  }
  inline Vector2 get_head() { return m_body[0]; }
  inline std::deque<Vector2> get_body() { return m_body; }
  inline void mark_to_add_segment() { m_add_segment = true; };
  inline void mark_segment_added() { m_add_segment = false; };

private:
  bool m_add_segment = false;
  std::deque<Vector2> m_body = {Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9}};
  Vector2 m_direction = {1, 0};
};

#endif