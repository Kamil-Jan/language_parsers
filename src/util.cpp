#include "util.hpp"

#include <algorithm>

std::vector<std::string> SplitString(const std::string& str,
                                     const std::string& delimiter) {
  size_t pos = 0;
  size_t search_start = 0;
  std::vector<std::string> result;
  while ((pos = str.find(delimiter, search_start)) != std::string::npos) {
    result.push_back(str.substr(search_start, pos - search_start));
    search_start = pos + delimiter.size();
  }
  result.push_back(str.substr(search_start));
  return result;
}

std::string& LTrim(std::string& str) {
  if (str.empty()) {
    return str;
  }
  str.erase(str.begin(), str.begin() + str.find_first_not_of(" \t"));
  return str;
}

std::string& RTrim(std::string& str) {
  if (str.empty()) {
    return str;
  }
  str.erase(str.begin() + str.find_last_not_of(" \t") + 1, str.end());
  return str;
}

std::string& Trim(std::string& str) { return LTrim(RTrim(str)); }
