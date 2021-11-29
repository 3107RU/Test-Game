#include <map>
#include <regex>
#include <fstream>
#include <iostream>

static const std::string base64_chars =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";

bool copyBase64(const char *file, std::ofstream &ofs) {
  std::ifstream ifs(file, std::ios::binary);

  int i = 0;
  int j = 0;
  unsigned char char_array_3[3];
  unsigned char char_array_4[4];

  char c;
  while (ofs && ifs.get(c)) {
    char_array_3[i++] = c;
    if (i == 3) {
      char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
      char_array_4[1] =
          ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
      char_array_4[2] =
          ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
      char_array_4[3] = char_array_3[2] & 0x3f;

      for (i = 0; (i < 4); i++) ofs << base64_chars[char_array_4[i]];
      i = 0;
    }
  }

  if (i) {
    for (j = i; j < 3; j++) char_array_3[j] = '\0';

    char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
    char_array_4[1] =
        ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
    char_array_4[2] =
        ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
    char_array_4[3] = char_array_3[2] & 0x3f;

    for (j = 0; (j < i + 1); j++) ofs << base64_chars[char_array_4[j]];

    while ((i++ < 3)) ofs << '=';
  }
  return !ifs.fail() && !ofs.fail();
}

bool copy(const char *file, std::ofstream &ofs) {
  std::ifstream ifs(file);
  ofs << ifs.rdbuf();
  return !ifs.fail() && !ofs.fail();
}

std::string read(const char *file) {
  std::ifstream ifs(file);
  return std::string((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
}

int main(int argc, char **argv) {
  std::cout << "packer - combine html+cfg+js+wasm files in one html" << std::endl;

  if (argc != 6) {
    std::cout << "Usage: packer in.html in.cfg in.wasm in.js out.html" << std::endl;
    return 1;
  }

  auto html = read(argv[1]);
  std::ofstream ofs(argv[5]);

  auto begin = html.begin();
  std::match_results<std::string::iterator> m;
  std::regex r(R"(\{\s*\{\s*\{\s*(CONFIG|WASM|SCRIPT)\s*\}\s*\}\s*\})");
  while(std::regex_search(begin, html.end(), m, r)) {
    ofs.write(&*m.prefix().first, m.prefix().second - m.prefix().first);
    if (m[1] == "CONFIG")
      copyBase64(argv[2], ofs);
    else if (m[1] == "WASM")
      copyBase64(argv[3], ofs);
    else if (m[1] == "SCRIPT")
      copy(argv[4], ofs);
    begin = m.suffix().first;
  }
  ofs.write(&*begin, html.end() - begin);
  return 0;
}