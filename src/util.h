#ifndef UTIL_H
#define UTIL_H

#include <vector>
#include <list>

class FillSpace {
public:
  FillSpace(); 
  std::vector<int> next(); 
private:
  std::list<std::vector<int> > points_; 
  int shell_; 
}; 


#endif 
