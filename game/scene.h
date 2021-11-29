/**
 * @file scene.h
 * @author Alex Light (dev@3107.ru)
 * @brief Сцена игры
 * @version 0.1
 * @date 2021-11-28
 */
#pragma once

#include "objects.h"
#include "sprites.h"
#include "text.h"

/**
 * @brief Класс сцены где вся игра и происходит
 *
 */
class Scene {
  Random rnd;
  GLFWwindow *window;
  Text text;
  Objects figures;
  Objects darkness;
  std::shared_ptr<Gamer> gamer;
  std::shared_ptr<Prize> prize;
  std::list<std::shared_ptr<Zomby>> zombies;
  int score = 0, bestScore = 0;
  double lastTick = 0.;

  /**
   * @brief Create a Gamer object
   *
   */
  void createGamer();
  /**
   * @brief Create a Random Rect object
   *
   * @param sz размер прямоугольника
   * @param rc результат
   * @return true
   * @return false
   */
  bool createRandomRect(const Size &sz, Rect &rc);
  /**
   * @brief Расчет невидимых полигонов
   *
   * @param pt точка откуда смотрим
   */
  void updateDarkness(const Point &pt);
  /**
   * @brief Обработка поведения игрока
   *
   * @param time
   */
  void processGamer(double time);
  /**
   * @brief Обработка поведения приза
   *
   * @param time
   */
  void processPrize(double time);
  /**
   * @brief Обработка поведения зомби
   *
   * @param time
   */
  void processZombies(double time);
  /**
   * @brief Обработка действий
   *
   */
  void process();

  /**
   * @brief Обработка gamepad
   *
   */
  void processGamepad();

 public:
  /**
   * @brief Construct a new Scene object
   *
   * @param window окно
   * @param cfg конфигурация
   */
  Scene(GLFWwindow *window, const std::vector<std::vector<Point>> &cfg);
  /**
   * @brief возможные клавиши управления
   *
   */
  enum class Keys { Left, Up, Right, Down };
  /**
   * @brief Вызывается при нажатии клавиш
   *
   * @param key клавиша
   * @param down нажата или отжата
   */
  void onKey(Keys key, bool down);
  /**
   * @brief Вызывается при клике мышкой
   *
   * @param mx x
   * @param my y
   */
  void onClick(double mx, double my);
  /**
   * @brief Главный цикл игры
   *
   */
  void loop();
};