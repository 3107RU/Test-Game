/**
 * @file text.cpp
 * @author Alex Light (dev@3107.ru)
 * @brief реализация класса Text
 * @version 0.1
 * @date 2021-11-28
 */
#include "text.h"

// программа для вертексов символа
static const char *vscode = R"(
    attribute vec2 pos;
    attribute vec2 tex;
    varying vec2 texpos;
    void main() {
      gl_Position = vec4(pos, 0.0, 1.0);
      texpos = tex;
    }      
)";

// программа для фрагментов символа
static const char *fscode = R"(
    precision mediump float;
    uniform sampler2D tex;
    uniform vec4 color;
    varying vec2 texpos;
    void main() {    
      gl_FragColor = color * vec4(1.0, 1.0, 1.0, texture2D(tex, texpos).a);
    }      
)";

// проверка компиляции программы
static void check(GLuint shader) {
  GLint isCompiled = 0;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
  if (isCompiled == GL_FALSE) {
    GLint maxLength = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
    std::vector<GLchar> errorLog(maxLength);
    glGetShaderInfoLog(shader, maxLength, &maxLength, errorLog.data());
    for (GLint i = 0; i < maxLength; i++) std::cout << errorLog[i];
    std::cout << std::endl;
  }
}

Text::Program::Program() {
  auto vs = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vs, 1, &vscode, NULL);
  glCompileShader(vs);
  // check(vs);
  auto fs = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fs, 1, &fscode, NULL);
  glCompileShader(fs);
  // check(fs);
  id = glCreateProgram();
  glAttachShader(id, vs);
  glAttachShader(id, fs);
  glLinkProgram(id);
  glDetachShader(id, vs);
  glDetachShader(id, fs);
  glDeleteShader(vs);
  glDeleteShader(fs);
  color = glGetUniformLocation(id, "color");
  pos = glGetAttribLocation(id, "pos");
  tex = glGetAttribLocation(id, "tex");
  createSymbols();
}

Text::Program::~Program() { glDeleteProgram(id); }

Text::Program &Text::Program::get() {
  static Program prog;
  return prog;
}

namespace res {
/**
 * @brief размер ресурса шрифта
 *
 */
extern unsigned long font_ttf_len;
/**
 * @brief указатель на данные ресурса шрифта
 *
 */
extern unsigned char font_ttf_data[];
};  // namespace res

void Text::Program::createSymbols() {
  FT_Library ft;
  if (!FT_Init_FreeType(&ft)) {
    FT_Face face;
    if (!FT_New_Memory_Face(ft, (FT_Byte *)res::font_ttf_data,
                            (FT_Long)res::font_ttf_len, 0, &face)) {
      FT_Set_Pixel_Sizes(face, 0, textBitmapSize);
      glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
      for (unsigned char c = 0; c < 128; c++) {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) continue;
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, face->glyph->bitmap.width,
                     face->glyph->bitmap.rows, 0, GL_ALPHA, GL_UNSIGNED_BYTE,
                     face->glyph->bitmap.buffer);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        chars.emplace(
            c, Symbol{texture,
                      {face->glyph->bitmap.width, face->glyph->bitmap.rows},
                      {face->glyph->bitmap_left, face->glyph->bitmap_top},
                      face->glyph->advance.x});
      }
      FT_Done_Face(face);
    }
    FT_Done_FreeType(ft);
  }
}

Text::Text() : prog(Program::get()) {
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);
  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
  glVertexAttribPointer(prog.pos, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat),
                        0);
  glEnableVertexAttribArray(prog.pos);
  glVertexAttribPointer(prog.tex, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat),
                        (GLvoid *)(2 * sizeof(GLfloat)));
  glEnableVertexAttribArray(prog.tex);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

Text::~Text() {
  glDeleteBuffers(1, &vbo);
  glDeleteVertexArrays(1, &vao);
}

void Text::draw(const std::string &text, const Color &color, const Point &pt,
                GLfloat height) {
  glUseProgram(prog.id);
  glUniform4fv(prog.color, 1, color.data());
  glActiveTexture(GL_TEXTURE0);
  glBindVertexArray(vao);

  auto scale = height / textBitmapSize;
  auto x = pt[0];
  for (auto c : text) {
    auto it = prog.chars.find(c);
    if (it == prog.chars.end()) continue;
    auto ch = it->second;
    auto xpos = x + ch.bearing[0] * scale;
    auto ypos = pt[1] - (ch.size[1] - ch.bearing[1]) * scale;
    auto w = ch.size[0] * scale;
    auto h = ch.size[1] * scale;
    GLfloat vertices[6][4] = {
        {xpos, ypos + h, 0.0f, 0.0f}, {xpos, ypos, 0.0f, 1.0f},
        {xpos + w, ypos, 1.0f, 1.0f}, {xpos, ypos + h, 0.0f, 0.0f},
        {xpos + w, ypos, 1.0f, 1.0f}, {xpos + w, ypos + h, 1.0f, 0.0f}};
    glBindTexture(GL_TEXTURE_2D, ch.texture);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    x += (ch.advance >> 6) *
         scale;  // bitshift by 6 to get value in pixels (2^6 = 64)
  }
  glBindTexture(GL_TEXTURE_2D, 0);

  glBindVertexArray(0);
  glUseProgram(0);
}
