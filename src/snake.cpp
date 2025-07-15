#include "snake.h"
#include "constants.h"

extern game_globals_struct game_globals;

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
      m_head_color = game_globals.dark_green;
    }
    DrawRectangleRounded(rec, 0.5, 6, (i==0)?m_head_color:game_globals.dark_green);
  }
}

void Snake::update() {
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
  m_body = {Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9}};
  m_direction = {1, 0};
  m_head_color = game_globals.dark_green;
}