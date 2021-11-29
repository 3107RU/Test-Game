/**
 * @file scene.cpp
 * @author Alex Light (dev@3107.ru)
 * @brief Реализация класса Scene
 * @version 0.1
 * @date 2021-11-28
 */
#include "scene.h"

Scene::Scene(GLFWwindow *window, const std::vector<std::vector<Point>> &cfg)
    : rnd(gameSize),
      window(window),
      figures(figureColor),
      darkness(darknessColor) {
  // Нужен блендинг так как текстуры для текста с альфой
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  // Загрузим прпятствия
  figures.set(cfg);
}

void Scene::onKey(Keys key, bool down) {
  if (!gamer) return;
  // Зададим вектор скорости движения игрока
  auto &f = gamer->force;
  switch (key) {
    case Keys::Left:
      f[0] = down ? -1.f : .0f;
      break;
    case Keys::Up:
      f[1] = down ? 1.f : .0f;
      break;
    case Keys::Right:
      f[0] = down ? 1.f : .0f;
      break;
    case Keys::Down:
      f[1] = down ? -1.f : .0f;
      break;
  }
}

void Scene::onClick(double mx, double my) {
  GLint vp[4];
  glGetIntegerv(GL_VIEWPORT, vp);
  GLfloat x = GLfloat(mx - vp[0]) / (vp[2] - vp[0]) * 2.f - 1.f;
  GLfloat y = GLfloat(vp[3] - my) / (vp[3] - vp[1]) * 2.f - 1.f;
  std::cout << "click: " << x << "," << y << std::endl;
  updateDarkness({x, y});  // Так сделано в примере
}

void Scene::processGamepad() {
  if (!gamer) return;
  int count;
  auto axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &count);
  if (axes && count > 1) {
    // Зададим вектор скорости движения игрока
    auto &f = gamer->force;
    f[0] = axes[0];
    f[1] = -axes[1];
  }
}

void Scene::loop() {
  // Обработаем действия
  process();

  // Очистим фон
  glClearColor(backColor[0], backColor[1], backColor[2], backColor[3]);
  glClear(GL_COLOR_BUFFER_BIT);

  // Нарисуем все
  for (auto z : zombies) z->draw();  // Зобмби под темнотой
  darkness.draw();
  figures.draw();
  if (prize) prize->draw();
  if (gamer) gamer->draw();

  // Выведем текст
  std::stringstream ss;
  ss << "Score: " << score << ", BestScore: " << bestScore;
  text.draw(ss.str(), scoreColor, scorePosition, scoreHeight);

  // На экран
  glfwSwapBuffers(window);
}

bool Scene::createRandomRect(const Size &sz, Rect &rc) {
  // 100 раз попробуем случайную точку, вдруг повезет ))
  for (int i = 0; i < 100; i++) {
    auto r = Rect{rnd.point2d(), sz};
    if (r.first[0] > -1.f + sz[0] && r.first[0] < 1.f - sz[0] &&
        r.first[1] > -1.f + sz[1] && r.first[1] < 1.f - sz[1]) {
      if (!figures.intersect(g::points(r))) {
        rc = r;
        return true;
      }
    }
  }
  std::cout << "Unable to select place for random rect!" << std::endl;
  return false;
}

void Scene::createGamer() {
  // Круг по центру
  auto circle = Circle{{0.f, 0.f}, gamerRadius};
  // Функция смещает круг во все стороны и проверяет не пересекается ли он с
  // препятствиями
  auto probe = [&](GLfloat step) {
    for (GLfloat x = -step; x <= step; x += step) {
      for (GLfloat y = -step; y <= step; y += step) {
        auto c = circle;
        c.first[0] += x;
        c.first[1] += y;
        if (!figures.intersect(g::points(c))) return c;
      }
    }
    return circle;
  };
  // Начнем с нуля и будем увеличивать смещение
  GLfloat step = .0f;
  while (figures.intersect(
      g::points(circle))) {  // Можем и застрять тут если не найдем места
    step += .01f;
    circle = probe(step);
  }
  gamer = std::make_shared<Gamer>(circle);
}

void Scene::updateDarkness(const Point &pt) {
  darkness.clear();
  int k = 0, m = 0;
  // Переберем препятствия и добавим невидимые полигоны для каждой стороны
  // препятствия
  for (auto o : figures.objects) {
    for (size_t i = 0; i < o->points.size(); i++) {
      auto &A = o->points[i];
      auto &B = o->points[(i + 1) % o->points.size()];
      std::vector<Point> pts;
      g::invisiblePoligon(pt, A, B, pts);
      if (pts.size() > 2) {
        // По идее здесь можно попробовать объединить полигоны
        // С другой стороны наверное gpu быстрее отбросит ненужное, чем это
        // делать на cpu
        darkness.add(pts);
      }
    }
  }
}

void Scene::processGamer(double time) {
  // Создадим игрока
  if (!gamer) createGamer();
  // Сдвинем и если сдвинулись пересчитаем темноту
  if (gamer->move(figures)) updateDarkness(gamer->sprite.first);
}

void Scene::processPrize(double time) {
  // Проверим на пересечение с игроком, увеличим счет и удалим
  if (prize) {
    if (prize->intersect(gamer)) {
      bestScore = std::max(++score, bestScore);
      prize.reset();
    }
  }
  // Создадим приз в случайном месте
  if (!prize) {
    Rect rc;
    if (createRandomRect(prizeSize, rc)) prize = std::make_shared<Prize>(rc);
  }
}

void Scene::processZombies(double time) {
  // Создадим зомби
  if (zombies.size() < zombyCount) {
    Rect rc;
    if (createRandomRect(zombySize, rc))
      zombies.push_back(std::make_shared<Zomby>(rc));
  }
  // Обработаем действия
  for (auto z : zombies) {
    // Если пересеклись с игроком делим очки на 2
    if (z->process(figures, gamer, time, score)) score /= 2;
  }
}

void Scene::process() {
  // Действия обрабатываем не на каждой отрисовке
  auto tick = glfwGetTime();
  if (lastTick + processDelay < tick) {
    lastTick = tick;

    // Получим события мыши и клавиатуры
    glfwPollEvents();

    // Проверим gamepad
    processGamepad();

    // Выполним действия
    processGamer(tick);
    processPrize(tick);
    processZombies(tick);
  }
}
