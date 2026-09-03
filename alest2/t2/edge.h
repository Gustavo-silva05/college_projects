#include <string>
#include <sstream>

#ifndef EWGRAPH_EDGE_H
#define EWGRAPH_EDGE_H

struct Edge
{
  std::string v;
  std::string w;
  Edge(std::string v, std::string w) {
    this->v = v;
    this->w = w;
  }
  friend std::ostream& operator<<(std::ostream& os, const Edge& obj) {
    return os << obj.v << "-" << obj.w ;
  }
};

#endif

