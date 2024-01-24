#pragma once

#include <string>
#include <vector>

class Parser {
 public:
  virtual bool Parse(const std::vector<std::string>& str) const = 0;
  virtual ~Parser() = default;

 private:
  virtual void PrepareGrammar() = 0;
};
