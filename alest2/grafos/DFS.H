#include <iostream>
#include <set>
#include "graph.h"

using namespace std;

class DFS{
private:

set <string> marked;
map <string, string> edgeTo;
string s;

public:
    DFS(Graph &g, string v);
    void dfs(Graph &g, string v);
    vector <string> pathTo (string v);
    bool haspathTo(string v);
    void printPath(Graph &g);
    
};



