#ifndef __CONSTANTS_H__
#define __CONSTANTS_H__

#include "raylib.h"

#include <string>

struct game_globals_struct {
  Color green = {192, 230, 149, 255};
  Color dark_green = {74, 112, 106, 255};
  int cell_size = 30;
  int cell_count = 25;
  int offset = 75;
  std::string img_path = "../../resources/";
};

#endif