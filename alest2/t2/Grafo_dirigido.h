#include <unordered_map>
#include <set>
#include <string>
#include <vector>
#include "edge.h"
#include <iostream>
#include <sstream>

using namespace std;
#ifndef EWGRAPH_H
#define EWGRAPH_H

class Grafo_Dirigido
{
private:
  unordered_map<string, vector<Edge>> graph;

protected:
  set<string> vertices;

public:
  Grafo_Dirigido(unordered_map<string, vector<Edge>> &graf, set<string> &vert)
  {
    graph = graf;
    vertices = vert;
  }
  vector<Edge> getAdj(string v){ return graph[v]; }
  set<string> getVerts() { return vertices; }
};
#endif
