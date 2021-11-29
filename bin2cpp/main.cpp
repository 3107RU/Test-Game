#include <fstream>
#include <iostream>
#include <regex>
#include <vector>

std::string getVarName(std::string path) {
  auto filename = path.substr(path.find_last_of("/\\") + 1);
  auto str = std::regex_replace(filename, std::regex("\\W"), "_");
  std::cout << "Generated var name: " << str << std::endl;
  return str;
}

int main(int argc, char **argv) {
  std::cout << "bin2cpp - convert binaryfile to cpp code" << std::endl;

  if (argc != 3) {
    std::cout << "Error wrong arg count: " << argc << std::endl;
    return 1;
  }

  std::ifstream fsi(argv[1], std::ios::in | std::ios::binary);
  std::vector<char> data((std::istreambuf_iterator<char>(fsi)),
                         (std::istreambuf_iterator<char>()));
  if (!fsi) {
    std::cout << "Error reading file: " << argv[1] << std::endl;
    return 1;
  }

  std::ofstream fso(argv[2], std::ios::out);
  auto var = getVarName(argv[1]);
  fso << "namespace res {unsigned long " << var << "_len=" << data.size()
      << "; unsigned char " << var << "_data[]={";
  unsigned line_len = 0;
  for (auto c : data) {
    unsigned int d = (unsigned char)c;
    fso << "0x" << std::hex << d << ',';
    if (++line_len > 100) {
      line_len = 0;
      fso << std::endl;
    }
  }
  fso << "};}" << std::endl;
  if (!fso) {
    std::cout << "Error writing file: " << argv[2] << std::endl;
    return 1;
  }
  std::cout << "Written file: " << argv[2] << std::endl;
  return 0;
}
