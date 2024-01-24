#pragma once

#include <string>
#include <vector>

std::vector<std::string> SplitString(const std::string& str,
                                     const std::string& delimiter);

std::string& LTrim(std::string& str);
std::string& RTrim(std::string& str);
std::string& Trim(std::string& str);
