#ifndef CONFIG_HELPER_H
#define CONFIG_HELPER_H

#include <cmath>

const float PI = std::acos(-1);

//0值判断精度
const float EPSILON = 1e-6f;

const float GAMMA = 2.2f;
//分层采样控制层数
const int STRATIFY_SIZE = 10;
//BV节点容纳的三角形最小极限数量
const int BVH_LIMIT = 1;

const int RUSSIAN_ROULETTE_THRESHOLD = 10;
const float RUSSIAN_ROULETTE_PROBABILITY = 0.9f;

static int SAMPLE_PER_PIXEL = 128;
//对光源的采样数量
static int SAMPLE_PER_LIGHT = 1;
static int ITERATION_PER_REFRESH = 10;

#endif