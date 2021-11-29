/**
 * @file main.cpp
 * @author Alex Light (dev@3107.ru)
 * @brief Основной файл игры
 * @version 0.1
 * @date 2021-11-28
 * \mainpage Документация на игру
 * \section intro_sec Информация
 * Игра разработана как тест. Автор Alex Light (dev@3107.ru).
 */
#include "scene.h"

/**
 * @brief Загрузка конфигурации или из файла или из javascript
 *
 */
#ifdef EMSCRIPTEN
#include <emscripten.h>
std::string readConfig() {
  return emscripten_run_script_string(
      "(function(){return atob(Module.gameConfig);})()");
}
#else
/**
 * @brief Загрузка конфигурации из файла
 *
 * @param file Путь к файлу
 * @return std::string строка конфигурации
 */
std::string readConfig(const char *file) {
  std::ifstream fs(file);
  return std::string((std::istreambuf_iterator<char>(fs)),
                     std::istreambuf_iterator<char>());
}
#endif
/**
 * @brief Стартовый метод, принимает параметр путь к файлу конфигурации
 *
 * @param argc
 * @param argv
 * @return int
 */
int main(int argc, char **argv) {
  std::cout << "Starting game..." << std::endl;

#ifdef EMSCRIPTEN
  std::string cfg = readConfig();
#else
  if (argc != 2) {
    std::cout << "Usage: game path_to_cfg" << std::endl;
    return EXIT_FAILURE;
  }
  std::string cfg = readConfig(argv[1]);
#endif

  if (!glfwInit()) {
    std::cout << "glfwInit init failed!" << std::endl;
    return EXIT_FAILURE;
  }

  const int WNDSIZE = 600;

  auto window = glfwCreateWindow(WNDSIZE, WNDSIZE, "Game", nullptr, nullptr);
  if (!window) {
    glfwTerminate();
    return EXIT_FAILURE;
  }

  glfwMakeContextCurrent(window);

  glewExperimental = true;
  if (glewInit() != GLEW_OK) {
    std::cout << "Failed to initialize GLEW" << std::endl;
    return EXIT_FAILURE;
  }

  glViewport(0, 0, WNDSIZE, WNDSIZE);

  Scene scene(window, parseConfig(cfg));

  glfwSetWindowUserPointer(window, &scene);

  glfwSetFramebufferSizeCallback(
      window, [](GLFWwindow *window, int width, int height) {
        auto sz = std::min(width, height);
        glViewport((width - sz) / 2, (height - sz) / 2, sz, sz);
      });

  glfwSetMouseButtonCallback(window, [](GLFWwindow *window, int button,
                                        int action, int mods) {
    Scene *scene = reinterpret_cast<Scene *>(glfwGetWindowUserPointer(window));
    switch (button) {
      case GLFW_MOUSE_BUTTON_LEFT:
        if (action == GLFW_PRESS) {
          double x, y;
          glfwGetCursorPos(window, &x, &y);
          scene->onClick(x, y);
        }
    }
  });

  glfwSetKeyCallback(window, [](GLFWwindow *window, int key, int scancode,
                                int action, int mods) {
    Scene *scene = reinterpret_cast<Scene *>(glfwGetWindowUserPointer(window));
    if (action != GLFW_REPEAT) {
      switch (key) {
        case GLFW_KEY_A:
        case GLFW_KEY_LEFT:
          scene->onKey(Scene::Keys::Left, action == GLFW_PRESS);
          break;
        case GLFW_KEY_W:
        case GLFW_KEY_UP:
          scene->onKey(Scene::Keys::Up, action == GLFW_PRESS);
          break;
        case GLFW_KEY_D:
        case GLFW_KEY_RIGHT:
          scene->onKey(Scene::Keys::Right, action == GLFW_PRESS);
          break;
        case GLFW_KEY_S:
        case GLFW_KEY_DOWN:
          scene->onKey(Scene::Keys::Down, action == GLFW_PRESS);
          break;
      }
    }
  });

  auto loop = [](void *scene) { reinterpret_cast<Scene *>(scene)->loop(); };

#ifdef EMSCRIPTEN
  emscripten_set_main_loop_arg(loop, &scene, 0, 1);
#else
  while (!glfwWindowShouldClose(window)) loop(&scene);
#endif

  std::cout << "Closing..." << std::endl;
  glfwDestroyWindow(window);
  glfwTerminate();

  return EXIT_SUCCESS;
}
