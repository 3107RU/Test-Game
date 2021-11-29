/**
 * @file common.h
 * @author Alex Light (dev@3107.ru)
 * @brief Общие определения для игры
 * @version 0.1
 * @date 2021-11-28
 */
#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#include <array>
#include <cmath>
#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <random>
#include <regex>
#include <set>
#include <sstream>
#include <vector>

/**
 * @brief количество осей игры (в реальности в геометрии используются алгоритмы 2d)
 * 
 */
constexpr size_t AXES = 2;

/**
 * @brief Цвет
 * 
 */
using Color = std::array<GLfloat, 4>;
/**
 * @brief Точка (вектор)
 * 
 */
using Point = std::array<GLfloat, AXES>;
/**
 * @brief Размер
 * 
 */
using Size = std::array<GLfloat, AXES>;
/**
 * @brief Скорость
 * 
 */
using Speed = Size;
/**
 * @brief Сила
 * 
 */
using Force = Speed;
/**
 * @brief Отрезок
 * 
 */
using Segment = std::array<Point, 2>;
/**
 * @brief Треугольник
 * 
 */
using Triangle = std::array<Point, 3>;
/**
 * @brief Прямоугольник
 * 
 */
using Rect = std::pair<Point, Size>;
/**
 * @brief круг
 * 
 */
using Circle = std::pair<Point, GLfloat>;
/**
 * @brief углы сцены
 * 
 */
using Corners = std::array<Point, 4>;

/**
 * @brief размер поля игры
 * 
 */
constexpr GLfloat gameSize = 1.f;
/**
 * @brief допустимая ошибка при сравнении float
 * 
 */
constexpr GLfloat gameError = 1e-6f;
/**
 * @brief ошибка полигона для окружности
 * 
 */
constexpr GLfloat circleError = 1e-3f;
/**
 * @brief углы сцены
 * 
 */
constexpr Corners corners = {
    Point{-gameSize, -gameSize}, Point{gameSize, -gameSize},
    Point{gameSize, gameSize}, Point{-gameSize, gameSize}};
/**
 * @brief число пи
 * 
 */
constexpr GLfloat PI = 3.14159265358979323846f;
/**
 * @brief задержка между обработкой движений в сек.
 * 
 */
constexpr double processDelay = .02;
/**
 * @brief размер символа текста при создании текстур
 * 
 */
constexpr FT_UInt textBitmapSize = 64;
/**
 * @brief цвет текста 
 * 
 */
constexpr Color scoreColor = {1.f, 1.f, 1.f, 1.f};
/**
 * @brief высота текста
 * 
 */
constexpr GLfloat scoreHeight = gameSize * .07f;
/**
 * @brief позиция текста
 * 
 */
constexpr Point scorePosition = {gameSize * -.98f, gameSize * .93f};
/**
 * @brief ускорение спрайта при появлении вектора движения
 * 
 */
constexpr GLfloat accelerateForce = gameSize * .01f;
/**
 * @brief замедление (трение)
 * 
 */
constexpr GLfloat decelerateForce = gameSize * .001f;
/**
 * @brief цвет игрового поля
 * 
 */
constexpr Color backColor = {1.f, .75f, .5f, 1.0f};
/**
 * @brief цвет невидимой части
 * 
 */
constexpr Color darknessColor = {.5f, .5f, .5f, 1.0f};
/**
 * @brief цвет препятствий
 * 
 */
constexpr Color figureColor = {0.f, 0.f, 1.f, 1.f};
/**
 * @brief лимит скорости игрока
 * 
 */
constexpr GLfloat gamerSpeedLimit = gameSize * .02f;
/**
 * @brief псевдо вес игрока (инерционность)
 * 
 */
constexpr GLfloat gamerWeight = 2.f;
/**
 * @brief цвет игрока
 * 
 */
constexpr Color gamerColor = {1.f, 1.f, 0.f, 1.f};
/**
 * @brief радиус игрока
 * 
 */
constexpr GLfloat gamerRadius = gameSize * .05f;
/**
 * @brief цвет приза
 * 
 */
constexpr Color prizeColor = {0.f, 1.f, 0.f, 1.f};
/**
 * @brief размер приза
 * 
 */
constexpr Size prizeSize = {gameSize * .05f, gameSize * .05f};
/**
 * @brief цвет активного зомби
 * 
 */
constexpr Color zombyActiveColor = {1.f, 0.f, 0.f, 1.f};
/**
 * @brief цвет пассивного зомби
 * 
 */
constexpr Color zombyInactiveColor = {1.f, 0.f, 0.f, .5f};
/**
 * @brief ограничение скорости зомби
 * 
 */
constexpr GLfloat zombySpeedLimit = gameSize * .002f;
/**
 * @brief инерция зомби
 * 
 */
constexpr GLfloat zombyWeight = 20.f;
/**
 * @brief размер зомби
 * 
 */
constexpr Size zombySize = {gameSize * .04f, gameSize * .04f};
/**
 * @brief количество зомби
 * 
 */
constexpr int zombyCount = 3;
/**
 * @brief время неактивности зоби после столкновения с игроком в сек.
 * 
 */
constexpr double sombyInactiveTime = 1.;
/**
 * @brief размер области в которой зомби выбирает случайную точку куда идти когда не видит игрока
 * 
 */
constexpr GLfloat zombyMemoryError = gameSize * .1f;

/**
 * @brief на сколько ускоряем зомби в зависимости от очков
 * 
 */
constexpr GLfloat zombySpeedFromScoreKoef = .001f;

/**
 * @brief приблизительное равно для float
 * 
 * @param a float
 * @param b float
 * @return true 
 * @return false 
 */
inline bool equal(GLfloat a, GLfloat b) {
  return std::abs(a - b) <= std::max(std::abs(a), std::abs(b)) * gameError;
}
/**
 * @brief Гарантированное меньше для float
 * 
 * @param a float
 * @param b float
 * @return true 
 * @return false 
 */
inline bool less(GLfloat a, GLfloat b) {
  return (b - a) > std::max(std::abs(a), std::abs(b)) * gameError;
}
/**
 * @brief примерное равно для вектора
 * 
 * @param a вектор
 * @param b вектор
 * @return true 
 * @return false 
 */
template <typename T>
bool equal(const T &a, const T &b) {
  return equal(a[0], b[0]) && equal(a[1], b[1]);
}
/**
 * @brief гарантированное меньше для вектора
 * 
 * @param a вектор
 * @param b вектор
 * @return true 
 * @return false 
 */
template <typename T>
bool less(const T &a, const T &b) {
  return less(a.x, b.x) && less(a.y, b.y);
}
/**
 * @brief оператор вычитания векторов
 * 
 * @param u вектор
 * @param v вектор
 * @return T 
 */
template <class T>
T operator-(const T &u, const T &v) {
  return T{u[0] - v[0], u[1] - v[1]};
}
/**
 * @brief оператор сложения векторов
 * 
 * @param u вектор
 * @param v вектор
 * @return T 
 */
template <class T>
T operator+(const T &u, const T &v) {
  return T{u[0] + v[0], u[1] + v[1]};
}
/**
 * @brief Оператор умножения вектора на скаляр
 * 
 * @param u вектор
 * @param m вектор
 * @return T 
 */
template <class T>
T operator*(const T &u, GLfloat m) {
  return T{u[0] * m, u[1] * m};
}
/**
 * @brief оператор умножения векторов
 * 
 * @param u вектор
 * @param v вектор
 * @return T 
 */
template <typename T>
T operator*(const T &u, const T &v) {
  return {u[0] * v[0], u[1] * v[1]};
}
