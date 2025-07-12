#include "snake.h"

extern game_globals_struct game_globals;

void Snake::draw() {
  for (int i = 0; i < m_body.size(); i++) {
    float x = m_body[i].x * game_globals.cell_size;
    float y = m_body[i].y * game_globals.cell_size;
    // DrawRectangle(x, y, game_globals.cell_size, game_globals.cell_size,
    //               game_globals.dark_green);
    Rectangle rec{x + game_globals.offset, y + game_globals.offset,
                  (float)game_globals.cell_size, (float)game_globals.cell_size};
    DrawRectangleRounded(rec, 0.5, 6, game_globals.dark_green);
  }
}

void Snake::update() {
  m_body.push_front(Vector2Add(m_body[0], m_direction));
  if (m_add_segment) {
    mark_segment_added();
  } else {
    m_body.pop_back();
  }
}

void Snake::reset() {
  m_body = {Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9}};
  m_direction = {1, 0};
}