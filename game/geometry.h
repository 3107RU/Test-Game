/**
 * @file geometry.h
 * @author Alex Light (dev@3107.ru)
 * @brief Геометрия для игры
 * @version 0.1
 * @date 2021-11-28
 */
#pragma once

#include "common.h"

namespace Geometry {

/**
 * @brief Проверка находтся ли точка внутри треугольника
 *
 * @tparam T 2d точка
 * @param A первая точка треугольника
 * @param B вторая точка треугольника
 * @param C третья точка треугольника
 * @param P проверяемая точка
 * @return true если точка внутри
 * @return false если точка снаружи
 */
template <typename T>
bool ptInTriangle(const T &A, const T &B, const T &C, const T &P) {
  auto side = [&](const T &a, const T &b) {
      return (b[0] - a[0]) * (P[1] - a[1]) - (b[1] - a[1]) * (P[0] - a[0]) >= 0.f;
  };  
  return side(A, B) && side(B, C) && side(C, A);
};

/**
 * @brief Триангуляция фигур по точкам полигона
 *
 * @param contour Набор точек полигона
 * @param result Коллекция треугольников
 * @return true Разбиение успешно
 * @return false Некорректный полигон
 */
inline bool triangulate2d(const std::vector<Point> &contour,
                          std::vector<Triangle> &result) {
  auto n = contour.size();
  if (n < 3) return false;

  float a = .0f;
  for (size_t p = n - 1, q = 0; q < n; p = q++)
    a += contour[p][0] * contour[q][1] - contour[q][0] * contour[p][1];

  std::vector<size_t> indexes(n);
  for (size_t v = 0; v < n; v++) indexes[v] = a > .0f ? v : (n - 1) - v;

  auto select = [&](int u, int v, int w, int n) {
    auto A = contour[indexes[u]];
    auto B = contour[indexes[v]];
    auto C = contour[indexes[w]];
    if (gameError >
        ((B[0] - A[0]) * (C[1] - A[1])) - ((B[1] - A[1]) * (C[0] - A[0])))
      return false;
    for (int p = 0; p < n; p++) {
      if ((p == u) || (p == v) || (p == w)) continue;
      if (ptInTriangle(A, B, C, contour[indexes[p]])) return false;
    }
    return true;
  };

  auto nv = n;
  auto count = 2 * nv;
  for (size_t m = 0, v = nv - 1; nv > 2;) {
    if (0 >= (count--)) {
      return false;
    }
    auto u = v;
    if (nv <= u) u = 0;
    v = u + 1;
    if (nv <= v) v = 0;
    auto w = v + 1;
    if (nv <= w) w = 0;
    if (select(u, v, w, nv)) {
      result.push_back(Triangle{contour[indexes[u]], contour[indexes[v]],
                                contour[indexes[w]]});
      m++;
      for (auto s = v, t = v + 1; t < nv; s++, t++) indexes[s] = indexes[t];
      nv--;
      count = 2 * nv;
    }
  }

  return true;
}

/**
 * @brief Проверка нахождения точки в полигоне
 *
 * @tparam T 2d точка
 * @param pts Набор точек полигона
 * @param P Точка для проверки
 * @return true Точка внутри
 * @return false Точка снаружи
 */
template <typename T>
bool ptInPoligon(const std::vector<T> &pts, const T &P) {
  std::vector<Triangle> v;
  if (triangulate2d(pts, v)) {
    for (auto const &t : v) {
      if (ptInTriangle(t[0], t[1], t[2], P)) return true;
    }
  }
  return false;
}

/**
 * @brief Пересечение двух отрезков
 *
 * @tparam T 2d точка
 * @param p0 1ая точка 1ого отрезка
 * @param p1 2ая точка 1ого отрезка
 * @param p2 1ая точка 2ого отрезка
 * @param p3 2ая точка 2ого отрезка
 * @param i опциональный указатель куда поместить точку пересечения
 * @return true отрезки пересекаются
 * @return false отрезки не пересекаются
 */
template <typename T>
bool intersect(const T &p0, const T &p1, const T &p2, const T &p3,
               T *i = nullptr) {
  auto s1x = p1[0] - p0[0], s1y = p1[1] - p0[1];
  auto s2x = p3[0] - p2[0], s2y = p3[1] - p2[1];

  auto s = (-s1y * (p0[0] - p2[0]) + s1x * (p0[1] - p2[1])) /
           (-s2x * s1y + s1x * s2y);
  auto t = (s2x * (p0[1] - p2[1]) - s2y * (p0[0] - p2[0])) /
           (-s2x * s1y + s1x * s2y);

  if (s >= 0 && s <= 1 && t >= 0 && t <= 1) {
    if (i) {
      (*i)[0] = p0[0] + (t * s1x);
      (*i)[1] = p0[1] + (t * s1y);
    }
    return true;
  }

  return false;
}

/**
 * @brief Пересечение отрезка и треугольника
 *
 * @param t треугольник
 * @param pt1 1ая точка отрезка
 * @param pt2 2ая точка отрезка
 * @return true пересекаются
 * @return false не пересекаются
 */
inline bool intersect(const Triangle &t, const Point &pt1, const Point &pt2) {
  if (ptInTriangle(t[0], t[1], t[2], pt1)) return true;
  if (ptInTriangle(t[0], t[1], t[2], pt2)) return true;
  for (size_t i = 0; i < t.size(); i++) {
    if (intersect(t[i], t[i < t.size() - 1 ? i + 1 : 0], pt1, pt2)) return true;
  }
  return false;
}

/**
 * @brief Пересечение двух треугольников
 *
 * @param t1 треугольник
 * @param t2 треугольник
 * @return true пересекаются
 * @return false
 */
inline bool intersect(const Triangle &t1, const Triangle &t2) {
  for (size_t i = 0; i < t1.size(); i++)
    if (ptInTriangle(t2[0], t2[1], t2[2], t1[i])) return true;
  for (size_t i = 0; i < t2.size(); i++)
    if (ptInTriangle(t1[0], t1[1], t1[2], t2[i])) return true;
  for (size_t i = 0; i < t1.size(); i++) {
    for (size_t j = 0; j < t2.size(); j++) {
      if (intersect(t1[i], t1[i < t1.size() - 1 ? i + 1 : 0], t2[j],
                    t2[j < t2.size() - 1 ? j + 1 : 0]))
        return true;
    }
  }
  return false;
}

/**
 * @brief Точки контура треугольника
 *
 * @param t треугольник
 * @return точки полигона
 */
inline auto points(const Triangle &t) {
  return std::vector<Point>(t.begin(), t.end());
}

/**
 * @brief Точки контура прямоугольника
 *
 * @param rc примитив прямоугольника
 * @return точки полигона
 */
inline auto points(const Rect &rc) {
  std::vector<Point> pts;
  pts.push_back(Point{rc.first[0] - rc.second[0], rc.first[1] - rc.second[1]});
  pts.push_back(Point{rc.first[0] + rc.second[0], rc.first[1] - rc.second[1]});
  pts.push_back(Point{rc.first[0] + rc.second[0], rc.first[1] + rc.second[1]});
  pts.push_back(Point{rc.first[0] - rc.second[0], rc.first[1] + rc.second[1]});
  return pts;
}

/**
 * @brief Точки контура окружности
 *
 * @param c примитив круга
 * @return точки полигона
 */
inline auto points(const Circle &c) {
  std::vector<Point> pts;
  const auto angle =
      std::acos(2.f * std::powf(1.f - circleError / c.second, 2.f) - 1.f);
  auto phi = 0.f;
  while (phi < 2 * PI) {
    pts.push_back(Point{c.first[0] + c.second * std::cos(phi),
                        c.first[1] + c.second * std::sin(phi)});
    phi += angle;
  }
  return pts;
}

/**
 * @brief Скалярное произведение векторов
 *
 * @tparam T
 * @param u вектор
 * @param v вектор
 * @return произведение
 */
template <typename T>
auto dot(const T &u, const T &v) {
  return u[0] * v[0] + u[1] * v[1];
}

//
/**
 * @brief Длина вектора
 *
 * @tparam T
 * @param v вектор
 * @return длина
 */
template <typename T>
auto norm(const T &v) {
  return std::sqrt(dot(v, v));
}

/**
 * @brief Угол вектора
 *
 * @tparam T
 * @param v вектор
 * @return угол
 */
template <typename T>
auto angle(const T &v) {
  return std::atan2(v[1], v[0]);
}

/**
 * @brief Поворот вектора
 *
 * @tparam T
 * @param v вектор
 * @param ang угол
 * @return вектор
 */
template <typename T>
auto rot(const T &v, GLfloat ang) {
  auto len = norm(v);
  return Point{len * std::cos(ang), len * std::sin(ang)};
}

/**
 * @brief Расстояние от точки до отрезка
 *
 * @tparam T
 * @param a 1ая точка отрезка
 * @param b 2ая точка отрезка
 * @param pt точка отсчета
 * @return расстояние
 */
template <typename T>
auto dist(const T &a, const T &b, const T &pt) {
  auto v = b - a;
  auto w = pt - a;
  auto c1 = dot(w, v);
  if (c1 <= 0) return norm(pt - a);
  auto c2 = dot(v, v);
  if (c2 <= c1) return norm(pt - b);
  auto d = c1 / c2;
  return norm(pt - (a + T{d * v[0], d * v[1]}));
}

/**
 * @brief Найти все грани пересекающиеся с отрезком
 *
 * @tparam T
 * @param pts точки полигона
 * @param a 1ая точка отрезка
 * @param b 2ая точка отрезка
 * @return список пересекающихся сторон полигона и точек пересечения
 */
template <typename T>
auto intersect(const std::vector<T> &pts, const T &a, const T &b) {
  std::vector<std::array<T, 3>> result;
  for (size_t i = 0; i < pts.size(); i++) {
    auto &c = pts[i];
    auto &d = pts[i < pts.size() - 1 ? i + 1 : 0];
    T p;
    if (intersect(a, b, c, d, &p)) result.push_back({c, d, p});
  }
  return result;
}

/**
 * @brief Разворот вектора по углу отражения от отрезка
 *
 * @tparam T
 * @param A 1ая точка отрезка
 * @param B 2ая точка отрезка
 * @param S вектор
 * @return вектор
 */
template <typename T>
auto reflect(const T &A, const T &B, const T &S) {
  auto speedAngle = angle(S);
  auto rotateang =
      PI - (speedAngle - angle(B - A)) * 2.f;  // Получим угол отражения
  rotateang += PI;  // Развернем на 180 градусов
  return rot(S, speedAngle + rotateang);  // Развернем вектор
}

/**
 * @brief Делает вектор заданной длины
 *
 * @tparam T
 * @param v вектор
 * @param len длина
 * @return вектор
 */
template <typename T>
auto vec(const T &v, GLfloat len) {
  auto l = norm(v);
  if (equal(l, 0.f)) return v;
  auto q = len / l;
  return T{v[0] * q, v[1] * q};
}

/**
 * @brief Ограничивает вектор по модулю
 *
 * @tparam T
 * @param v вектор
 * @param len макс. длина
 * @return вектор
 */
template <typename T>
auto limit(const T &v, GLfloat len) {
  return norm(v) > len ? vec(v, len) : v;
}

/**
 * @brief Уменьшает длину вектора
 *
 * @tparam T
 * @param v вектор
 * @param len длина на которую уменьшаем
 * @return новый вектор
 */
template <typename T>
auto shorten(const T &v, GLfloat len) {
  return vec(v, std::max(norm(v) - len, 0.f));
}

/**
 * @brief Построение невидимого из точки полигона сзади отрезка с дополнением
 * углами границ игрового поля
 *
 * @tparam T
 * @param P точка откуда смотрим
 * @param A 1ая точка отрезка
 * @param B 2ая точка отрезка
 * @param pts сюда помещаем точки полигона
 */
template <typename T>
void invisiblePoligon(const T &P, const T &A, const T &B, std::vector<T> &pts) {
  // Сделаем лучи из точки через края отрезка длиннее чем область игры
  struct Ray {
    T origin;  // точка на отрезке через которую идет луч
    T ray;
    T border;        // пересечение с границей
    int index = -1;  // индекс границы
    Ray(const T &P, const T &A)
        : origin(A), ray(P - vec(P - A, gameSize * 4.f)) {}
  };
  std::array<Ray, 2> rays{Ray{P, A}, Ray{P, B}};

  // Найдем точки пересечения лучей
  for (size_t i = 0; i < corners.size(); i++) {
    // Берем текущую границу
    auto &c = corners[i];
    auto &d = corners[(i + 1) % corners.size()];
    // Ищем пересечение с лучем
    for (auto &r : rays) {
      if (r.index == -1) {
        if (intersect(P, r.ray, c, d, &r.border)) r.index = i;
      }
    }
    if (rays[0].index >= 0 && rays[1].index >= 0) break;
  }

  // Точки могут совпадать, отбрасываем
  auto pushUnique = [&](const T &pt) {
    for (auto const &p : pts)
      if (equal(pt, p)) return false;
    pts.push_back(pt);
    return true;
  };

  if (rays[0].index != rays[1].index) {  // Если пересечения на разных границах
    auto addCorners = [&](const Ray &r1, const Ray &r2) {
      pushUnique(r1.origin);
      pushUnique(r1.border);
      // Если 2ой идекс меньше первого увеличим на кол-во углов
      int j = r1.index > r2.index ? r2.index + 4 : r2.index;
      for (auto i = r1.index; i < j; i++)
        pushUnique(
            corners[(i + 1) % corners.size()]);  // Берем вторую точку границы
      pushUnique(r2.border);
      pushUnique(r2.origin);
      if (ptInPoligon(pts, P))  // Если смотрящий внутри - не туда идем
        pts.clear();
    };
    // Сначала попробуем добавлять углы от первого луча ко второму, потом
    // наоборот
    addCorners(rays[0], rays[1]);
    if (pts.empty()) addCorners(rays[1], rays[0]);
  } else {  // Пересечения на одной границе
    pushUnique(rays[0].origin);
    pushUnique(rays[0].border);
    pushUnique(rays[1].border);
    pushUnique(rays[1].origin);
  }
}

}  // namespace Geometry

namespace g = Geometry;
