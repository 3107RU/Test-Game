/**
 * @file sprites.h
 * @author Alex Light (dev@3107.ru)
 * @brief Классы нестатичных объектов игры
 * @version 0.1
 * @date 2021-11-28
 */
#pragma once

#include "objects.h"

/**
 * @brief Обобщенный класс перемещаемого объекта
 *
 */
template <typename T>
struct Sprite : public Objects {
  /**
   * @brief примитив
   *
   */
  T sprite;
  /**
   * @brief дельта скорости
   *
   */
  Force force = {0.f, 0.f};
  /**
   * @brief дельта смещения
   *
   */
  Speed speed = {0.f, 0.f};
  /**
   * @brief после удара о препятствие остается остаток смещения - добавляем его
   * на следующем шаге
   *
   */
  Speed speedDelta = {0.f, 0.f};
  /**
   * @brief типа вес - инерционность
   *
   */
  GLfloat weight;
  /**
   * @brief ограничение скоорости
   *
   */
  GLfloat speedLimit;
  /**
   * @brief указатель на полигон
   *
   */
  ObjectPtr object;
  /**
   * @brief Construct a new Sprite object
   *
   * @param sprite примитив
   * @param color цвет
   * @param weight вес
   * @param speedLimit ограничение скорости
   */
  Sprite(const T &sprite, const Color &color, GLfloat weight = 0.f,
         GLfloat speedLimit = 0.f)
      : Objects(color), sprite(sprite), weight(weight), speedLimit(speedLimit) {
    auto pts = g::points(sprite);
    object = Objects::add({pts});
  }
  /**
   * @brief
   *
   * @param figures объекты в которых движение запрещено
   * @return true сместились
   * @return false нет
   */
  bool move(const Objects &figures) {
    // Прибавим дельту приращения смещения спрайта (условная скорость)
    // Прибавляем тем меньше чем больше условный вес спрайта
    speed = speed + force * (accelerateForce / weight);
    // Замедлим с учетом веса (трение против инерции)
    speed = g::shorten(speed, decelerateForce / weight);
    // Ограничим
    speed = g::limit(speed, speedLimit);
    // Вычислим новую точку с учетом остатка отражения
    auto pt = sprite.first + speed + speedDelta;
    speedDelta = {0.f, 0.f};  // Остаток сбросим
    if (pt != sprite.first) {
      auto obj = figures.intersect(sprite.first, pt);
      if (obj) {
        // Найдем грани фигуры, пересекающиеся с линией движения
        auto v = g::intersect(obj->points, sprite.first, pt);
        if (!v.empty()) {
          // Выберем ближайшую к старой точке грань
          auto t = v.front();
          auto dst = g::norm(t[2] - sprite.first);
          for (auto const &f : v) {
            auto d = g::norm(f[2] - sprite.first);
            if (d < dst) {
              dst = d;
              t = f;
            }
          }
          // Мы не должны пересечь границу фигуры
          dst -= circleError;
          // Переместим точку поближе к грани не пересекая
          if (dst > 0.f) {
            pt = sprite.first + g::vec(speed, dst);
            if (figures.inside(pt)) {
              pt = sprite
                       .first;  // Видимо есть еще грань или фигура рядом, стоим
              // std::cout << "Pt will be inside figure after any move - stop!"
              //          << std::endl;
            }
          } else {
            // std::cout << "Pt to close to figure - stop!" << std::endl;
            pt = sprite.first;  // Мы уже вплотную, стоим
          }
          // Развернем вектор скорости в соответствии с углом отражения
          speed = g::reflect(t[1], t[0], speed);
          // Запомним остаток скорости на следующий шаг
          speedDelta = g::vec(speed, g::norm(speed) - dst);
        } else {
          pt = sprite.first;  // Это странно, линия целиком внутри?
          // std::cout << "Line inside figure!" << std::endl;
        }
      }
      // Обновим позицию и вертексы
      if (sprite.first != pt) {
        sprite.first = g::ensureInScene(pt);
        Objects::update(object, g::points(sprite));
        return true;
      }
    }
    return false;
  }
};

/**
 * @brief класс игрока
 *
 */
struct Gamer : public Sprite<Circle> {
  /**
   * @brief Construct a new Gamer object
   *
   * @param c примитив
   */
  Gamer(const Circle &c)
      : Sprite(c, gamerColor, gamerWeight, gamerSpeedLimit) {}
};

/**
 * @brief класс приза
 *
 */
struct Prize : public Sprite<Rect> {
  /**
   * @brief Construct a new Prize object
   *
   * @param rc примитив
   */
  Prize(const Rect &rc) : Sprite(rc, prizeColor) {}
  /**
   * @brief пересекается ли с игроком
   *
   * @param gamer указатель на игрока
   * @return true пересекается
   * @return false не пересекается
   */
  bool intersect(std::shared_ptr<Gamer> gamer) {
    return !!gamer->intersect(object);
  }
};

/**
 * @brief класс зомби
 *
 */
struct Zomby : public Sprite<Rect> {
  /**
   * @brief генератор смещения точки куда идти когда не видим игрока
   *
   */
  Random rnd;
  /**
   * @brief точка где видели игрока последний раз
   *
   */
  std::shared_ptr<Point> dest;
  /**
   * @brief время когда последний раз пересекались с игроком
   *
   */
  double contactTime = 0.;
  /**
   * @brief Construct a new Zomby object
   *
   * @param rc примитив
   */
  Zomby(const Rect &rc)
      : Sprite(rc, zombyActiveColor, zombyWeight, zombySpeedLimit),
        rnd(zombyMemoryError) {}
  /**
   * @brief Обработчие действий зомби
   *
   * @param figures препятствия
   * @param gamer указатель на игрока
   * @param time время игры
   * @param score набранные очки
   * @return true пересекаемся с игроком
   * @return false не пересекаемся с игроком
   */
  bool process(const Objects &figures, std::shared_ptr<Gamer> gamer,
               double time, int score) {
    // Определим мы сейчас активны или нет
    bool active = contactTime + sombyInactiveTime < time;
    // Пересекаемся с игроком?
    auto obj = gamer->intersect(object);
    if (obj) contactTime = time;  // Обновим время контакта
    // Установим цвет
    setColor(active ? zombyActiveColor : zombyInactiveColor);
    // Видим ли мы игрока?
    Point pt;
    if (!figures.intersect(sprite.first,
                           gamer->sprite.first)) {  // Если видим запомним точку
                                                    // и обновим лимит скорости
      if (!dest) dest = std::make_shared<Point>();
      *dest = gamer->sprite.first;
      pt = *dest;
      speedLimit =
          score ? zombySpeedLimit + score * zombySpeedFromScoreKoef : 0.f;
    } else {  // Если не видим снизим скорость и поплетемся к случайной точке
              // рядом с местом где видели игрока последний раз
      if (dest) pt = *dest + rnd.point2d();
      speedLimit = score ? zombySpeedLimit : 0.f;
    }
    if (dest) {  // Есть координаты игрока - идем туда
      force = g::vec(active ? pt - sprite.first : sprite.first - pt, 1.f);
      move(figures);
    }
    // Сообщим о пересечении с игроком
    return active && obj;
  }
};
