#ifndef CONFIG_HELPER_H
#define CONFIG_HELPER_H

#include <cmath>

//pi
const float PI = std::acos(-1);

//0值判断精度
const float EPSILON = 1e-7f;
//伽马校正
const float GAMMA = 2.2f;

//分层采样控制层数
const int STRATIFY_SIZE = 10;

//BV节点容纳的三角形最小极限数量
const int BVH_LIMIT = 1;

//俄罗斯轮盘赌的上限和概率
const int RUSSIAN_ROULETTE_THRESHOLD = 3;
const float RUSSIAN_ROULETTE_PROBABILITY = 0.9f;

//ssp
static int SAMPLE_PER_PIXEL = 64;

#endif