/**
 * @file objects.cpp
 * @author Alex Light (dev@3107.ru)
 * @brief Реализация класса Objects
 * @version 0.1
 * @date 2021-11-28
 */
#include "objects.h"

/**
 * @brief Программа для вертексов полигона
 * 
 */
static const char *vscode = R"(
      attribute vec2 pos;
      void main() {
        gl_Position = vec4(pos, 0.0, 1.0);
      }
    )";

/**
 * @brief Программа для фрагментов полигона
 * 
 */
static const char *fscode = R"(
      precision mediump float;
      uniform vec4 color;
      void main() {
        gl_FragColor = color;
      }
    )";

Objects::Program::Program() {
  auto vs = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vs, 1, &vscode, NULL);
  glCompileShader(vs);
  auto fs = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fs, 1, &fscode, NULL);
  glCompileShader(fs);
  id = glCreateProgram();
  glAttachShader(id, vs);
  glAttachShader(id, fs);
  glLinkProgram(id);
  glDetachShader(id, vs);
  glDetachShader(id, fs);
  glDeleteShader(vs);
  glDeleteShader(fs);
  pos = glGetAttribLocation(id, "pos");
  color = glGetUniformLocation(id, "color");
}

Objects::Program::~Program() { glDeleteProgram(id); }

Objects::Program &Objects::Program::get() {
  static Program prog;
  return prog;
}

Objects::Objects(const Color &color) : prog(Program::get()), color(color) {
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glVertexAttribPointer(prog.pos, AXES, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(prog.pos);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

Objects::~Objects() {
  glDeleteBuffers(1, &vbo);
  glDeleteVertexArrays(1, &vao);
}

void Objects::setColor(const Color &clr) { color = clr; }

ObjectPtr Objects::inside(const Point &pt) const {
  for (auto o : objects) {
    if (g::ptInPoligon(o->points, pt)) return o;
  }
  return nullptr;
}

ObjectPtr Objects::intersect(const std::vector<Point> &pts) const {
  std::vector<Triangle> triangles;
  g::triangulate2d(pts, triangles);
  for (auto o : objects) {
    for (auto const &t1 : o->triangles) {
      for (auto const &t2 : triangles) {
        if (g::intersect(t1, t2)) return o;
      }
    }
  }
  return nullptr;
}

ObjectPtr Objects::intersect(const Point &pt1, const Point &pt2) const {
  for (auto o : objects) {
    for (auto const &t : o->triangles) {
      if (g::intersect(t, pt1, pt2)) return o;
    }
  }
  return nullptr;
}

ObjectPtr Objects::intersect(ObjectPtr o) const {
  for (auto o1 : objects) {
    for (auto const &t1 : o1->triangles) {
      for (auto const &t2 : o->triangles) {
        if (g::intersect(t1, t2)) return o1;
      }
    }
  }
  return nullptr;
}

ObjectPtr Objects::add(const std::vector<Point> &pts) {
  auto o = std::make_shared<Object>();
  if (g::triangulate2d(pts, o->triangles)) {
    o->points = pts;
    objects.push_back(o);
    changed = true;
    return o;
  }
  return nullptr;
}

void Objects::set(const std::vector<std::vector<Point>> &val) {
  clear();
  for (auto const &v : val) add(v);
}

bool Objects::update(ObjectPtr o, const std::vector<Point> &pts) {
  std::vector<Triangle> t;
  if (g::triangulate2d(pts, t)) {
    o->points = pts;
    o->triangles.swap(t);
    changed = true;
    return true;
  }
  return false;
}

void Objects::remove(ObjectPtr o) {
  objects.remove(o);
  changed = true;
}

void Objects::clear() {
  objects.clear();
  changed = true;
}

size_t Objects::size() const { return objects.size(); }

bool Objects::empty() const { return objects.empty(); }

void Objects::draw() {
  glUseProgram(prog.id);
  glUniform4fv(prog.color, 1, color.data());
  glBindVertexArray(vao);
  if (changed) { // Нужно обновить буфер?
    changed = false;
    std::vector<GLfloat> buf;
    for (auto o : objects) {
      for (auto const &t : o->triangles) {
        for (size_t i = 0; i < t.size(); i++) {
          for (size_t j = 0; j < AXES; j++) {
            buf.push_back(t[i][j]);
          }
        }
      }
    }
    cnt = buf.size() / AXES;
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, buf.size() * sizeof(GLfloat), buf.data(),
                 GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
  }
  glDrawArrays(GL_TRIANGLES, 0, cnt);
  glBindVertexArray(0);
  glUseProgram(0);
}
