/*
 * xafile - Xavier File Manager
 * Copyright (C) 2026 Fitrian Musya
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <cstddef>
#include <cstdlib>
#include <filesystem>
#include <pwd.h>
#include <sstream>
#include <string>
#include <unistd.h>
#include <vector>

class Utility {
private:
  inline static const char *homedir{};
  inline static std::string curDir{};

public:
  static auto getHome() {
    homedir = getpwuid(getuid())->pw_dir;
    // Avoid null while launching app
    if (curDir == "")
      curDir = std::string(homedir);
    return homedir;
  }

  static auto getHomePath() {
    std::string path = getHome();
    std::stringstream ss(path);
    std::string segment;
    std::vector<std::string> result;

    while (std::getline(ss, segment, '/')) {
      if (!segment.empty()) {
        result.push_back(segment);
      }
    }
    return result;
  }

  std::vector<std::string> scan_folder(const std::string &path) {
    std::vector<std::string> result;
    for (const auto &entry : std::filesystem::directory_iterator(path)) {
      if (entry.is_directory()) {
        result.push_back(entry.path().filename().string());
      }
    }
    return result;
  }

  auto setCurDir(const char *path) { return curDir = std::string(path); }
  auto getCurDir() { return curDir; }
};
