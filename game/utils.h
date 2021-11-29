/**
 * @file utils.h
 * @author Alex Light (dev@3107.ru)
 * @brief вспомогательные функции
 * @version 0.1
 * @date 2021-11-28
 */
#pragma once

#include "common.h"

/**
 * @brief генератор случайных чисел
 * 
 */
class Random {
  std::default_random_engine rng;
  std::uniform_real_distribution<GLfloat> dt;

 public:
 /**
  * @brief Construct a new Random object
  * 
  * @param sz диапазон
  */
  Random(GLfloat sz) : dt(-sz, sz) {
    std::random_device dev;
    rng.seed(dev());
  }
  /**
   * @brief 
   * 
   * @return GLfloat случайное число в диапазоне
   */
  GLfloat operator()() { return dt(rng); }
  /**
   * @brief случайная точка в диапазоне
   * 
   * @return Point 
   */
  Point point2d() { return {operator()(), operator()()}; }
};

/**
 * @brief Парсит строку конфига в набор полигонов
 * 
 * @param cfg строка
 * @return std::vector<std::vector<Point>> 
 */
inline std::vector<std::vector<Point>> parseConfig(const std::string &cfg) {
  std::vector<std::vector<Point>> res;
  GLfloat min[AXES], max[AXES];
  auto begin0 = cfg.begin();
  std::regex r0(R"(\[(.+)\])");
  std::match_results<std::string::const_iterator> m0;
  while (std::regex_search(begin0, cfg.end(), m0, r0)) {
    std::vector<Point> pts;
    std::regex r(R"(\(\s*([+-]?\d+\.?\d*)\s*,\s*([+-]?\d+\.?\d*)\s*\))");
    auto begin = m0[1].first;
    std::match_results<std::string::const_iterator> m;
    while (std::regex_search(begin, m0[1].second, m, r)) {
      Point pt;
      for (int i = 0; i < AXES; i++) {
        pt[i] = std::stof(m[i + 1].str());
        min[i] = res.empty() && pts.empty() ? pt[i] : std::min(pt[i], min[i]);
        max[i] = res.empty() && pts.empty() ? pt[i] : std::max(pt[i], max[i]);
      }
      pts.push_back(pt);
      begin = m.suffix().first;
    }
    res.push_back(pts);
    begin0 = m0.suffix().first;
  }
  for (int i = 0; i < AXES; i++) {
    max[i] -= min[i];
    if (max[i] < 1e-10f) return {};
  }
  for (auto &pts : res) {
    for (auto &pt : pts) {
      for (int i = 0; i < AXES; i++) {
        pt[i] = (pt[i] - min[i]) / max[i] * 2.f - 1.f;
      }
    }
  }
  return res;
}
