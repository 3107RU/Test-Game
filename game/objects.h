/**
 * @file objects.h
 * @author Alex Light (dev@3107.ru)
 * @brief Класс коллекции объектов (полигонов) игры
 * @version 0.1
 * @date 2021-11-28
 */
#pragma once

#include "geometry.h"
#include "utils.h"

/**
 * @brief класс объекта полигона
 *
 */
struct Object {
  /**
   * @brief точки полигона
   *
   */
  std::vector<Point> points;
  /**
   * @brief треугольники полигона
   *
   */
  std::vector<Triangle> triangles;
};
/**
 * @brief указатель на объект
 *
 */
using ObjectPtr = std::shared_ptr<Object>;

/**
 * @brief класс коллекции объектов
 *
 */
class Objects {
  /**
   * @brief ид буфера
   *
   */
  GLuint vbo;
  /**
   * @brief ид контекста
   *
   */
  GLuint vao;
  /**
   * @brief размер буфера
   *
   */
  GLsizei cnt;
  /**
   * @brief цвет закраски
   *
   */
  Color color;
  /**
   * @brief Треугольники и буфер opengl не совпадают, надо обновить
   *
   */
  bool changed = false;
  /**
   * @brief класс программы отрисовки
   *
   */
  struct Program {
    /**
     * @brief ид программы
     *
     */
    GLuint id;
    /**
     * @brief адрес позиции вертекса в программе
     *
     */
    GLuint pos;
    /**
     * @brief адрес цвета в программе
     *
     */
    GLuint color;
    /**
     * @brief Construct a new Program object
     *
     */
    Program();
    /**
     * @brief Destroy the Program object
     *
     */
    ~Program();
    /**
     * @brief Возвращает ссылку на синглетон программы
     *
     * @return Program&
     */
    static Program &get();
  };
  /**
   * @brief ссылка на программу отрисовки
   *
   */
  Program &prog;

 public:
  /**
   * @brief список объектов
   *
   */
  std::list<ObjectPtr> objects;
  /**
   * @brief Construct a new Objects object
   *
   * @param color цвет объектов
   */
  Objects(const Color &color);
  /**
   * @brief Destroy the Objects object
   *
   */
  ~Objects();
  /**
   * @brief Set the Color object
   *
   * @param color новый цвет объектов
   */
  void setColor(const Color &color);
  /**
   * @brief Находит объекто с точкой внутри
   *
   * @param pt Точка для проверки
   * @return ObjectPtr указатель на объект или nullptr
   */
  ObjectPtr inside(const Point &pt) const;
  /**
   * @brief Находит объект пересекающийся с полигоном
   *
   * @param pts точки полигона
   * @return ObjectPtr указатель на объект или nullptr
   */
  ObjectPtr intersect(const std::vector<Point> &pts) const;
  /**
   * @brief Находит объект пересекающийся с отрезком
   *
   * @param pt1 1ая точка отрезка
   * @param pt2 2ая точка отрезка
   * @return ObjectPtr указатель на объект или nullptr
   */
  ObjectPtr intersect(const Point &pt1, const Point &pt2) const;
  /**
   * @brief Находит объект пересекающийся с объектом
   *
   * @param o указатель на объект
   * @return ObjectPtr указатель на объект или nullptr
   */
  ObjectPtr intersect(ObjectPtr o) const;
  /**
   * @brief Добавляет объект в коллекцию
   *
   * @param val точки объекта
   * @return ObjectPtr указатель на объект
   */
  ObjectPtr add(const std::vector<Point> &val);
  /**
   * @brief Устанавливает все объекты коллекции
   *
   * @param val коллекция коллекций точек объектов
   */
  void set(const std::vector<std::vector<Point>> &val);
  /**
   * @brief Обновляет точки объекта
   *
   * @param o указатель на объект
   * @param pts новые точки
   * @return true удалось заменить
   * @return false не удалось заменить
   */
  bool update(ObjectPtr o, const std::vector<Point> &pts);
  /**
   * @brief Удаляет объект из коллекции
   *
   * @param o указатель на объект
   */
  void remove(ObjectPtr o);
  /**
   * @brief Удаляет все объекты
   *
   */
  void clear();
  /**
   * @brief Возвращает количество объектов
   *
   * @return size_t
   */
  size_t size() const;
  /**
   * @brief Проверяет есть ли объекты
   *
   * @return true
   * @return false
   */
  bool empty() const;
  /**
   * @brief Отрисовывает объекты
   *
   */
  void draw();
};
