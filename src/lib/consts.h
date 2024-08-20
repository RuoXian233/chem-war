#pragma once
#include <string>

const int WINDOW_WIDTH = 1366;
const int WINDOW_HEIGHT = 768;
const int CHARACTER_FIGURES_COUNT = 5;
const int AUDIO_DEFUALT_CHUNK_SIZE = 4096;
const int MAX_FRAMERATE = 60;
const int MAX_RESOURCE_TEXTURE_CACHE_SIZE = 5e3;
const int RESOURCE_REELASE_BLOCK_SIZE = 100;
const int RESOURCE_ACCUMULATING_MINIMUM = 100;

const std::string CHARACTER_FIGURE_PATH = "assets/character";
const std::string CHARACTER_FIGURE_URI = "character.figure";
const std::string CHARACTER_FIGURE_EXT = "png";
const std::string CHRACTER_FIGURE_DATA = "character.figure.data";
const std::string BGM_PATH = "assets/";

#define WORLD_PARTICAL_INIT_NUM 100 
#define PARTICAL_SINK_INC 50
#define PI 3.1415926535897932384626433
#define DEG_TO_RAD(x) PI * x / 180.0f
