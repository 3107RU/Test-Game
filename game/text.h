/**
 * @file text.h
 * @author Alex Light (dev@3107.ru)
 * @brief класс для отрисовки текста
 * @version 0.1
 * @date 2021-11-28
 */
#pragma once

#include "common.h"

/**
 * @brief класс для отрисовки текста
 *
 */
class Text {
  /**
   * @brief класс программы для отрисовки текста
   *
   */
  struct Program {
    /**
     * @brief идентификатор программы
     *
     */
    GLuint id;
    /**
     * @brief указатель на позицию вертекса в программе
     *
     */
    GLuint pos;
    /**
     * @brief указатель на позицию текстуры в программе
     *
     */
    GLuint tex;
    /**
     * @brief указатель на цвет в программе
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
     * @brief возвращает ссылку на синглетон программы
     *
     * @return Program&
     */
    static Program &get();
    /**
     * @brief класс символа текста
     *
     */
    struct Symbol {
      /**
       * @brief ид текстуры
       *
       */
      GLuint texture;
      /**
       * @brief размер
       *
       */
      std::array<FT_UInt, 2> size;
      /**
       * @brief смещение в строке
       *
       */
      std::array<FT_Int, 2> bearing;
      /**
       * @brief смещение до следующего символа
       *
       */
      FT_Pos advance;
    };
    /**
     * @brief список символов
     *
     */
    std::map<char, Symbol> chars;
    /**
     * @brief Загружает шрифт и создает первые 128 символов
     *
     */
    void createSymbols();
  };
  /**
   * @brief ссылка на программу отрисовки
   *
   */
  Program &prog;
  /**
   * @brief ид буфера
   *
   */
  GLuint vbo;
  /**
   * @brief ид контекста буферов
   *
   */
  GLuint vao;

 public:
  /**
   * @brief Construct a new Text object
   *
   */
  Text();
  /**
   * @brief Destroy the Text object
   *
   */
  ~Text();
  /**
   * @brief рисует текст
   *
   * @param text строка
   * @param color цвет
   * @param pt координаты
   * @param height высота
   */
  void draw(const std::string &text, const Color &color, const Point &pt,
            GLfloat height);
};
