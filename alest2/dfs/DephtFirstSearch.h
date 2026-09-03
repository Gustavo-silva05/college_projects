#include "graph.h"
#include <iostream>
#include <set>
#include <vector>

using namespace std;

class DFS{
    private:
        string s;
        map<string,string> edgeTo;
        set<string> marked;
        void dfs(Graph &g, string v);
    public:
        DFS (Graph &g, string s);
        bool hasPathTo(Graph &g,string v);
        vector<string> pathTo(string v);
};

