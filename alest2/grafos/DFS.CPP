#include "DFS.H"

DFS::DFS(Graph &g, string v){
    this->s = s;
    dfs(g,v);
}

void DFS::dfs (Graph &g, string v){
    marked.insert(v);
    for(auto const &w : g.getAdj(v)){
        if (marked.find(w) == marked.end()){
            edgeTo[w] = v;
            dfs(g,w);
        }
    }
}

vector <string> DFS::pathTo(string v){
    vector <string> path;
    while(v!=s){
        path.insert(path.begin(), v);
        v = edgeTo[v];
    }
    path.insert(path.begin(),s);
    return path;

}

bool DFS::haspathTo(string v){
    return marked.find(v) == marked.end();
}

void DFS::printPath(Graph &g){
    for (auto const &w: g.getVerts()){
        cout << w << ": "; 
        int n = pathTo(w).size()-1;
        for(string &k : pathTo(w)){
            if(k == pathTo(w)[n]) cout << k ;
            else cout << k << ", ";
        }
        cout << endl;
    }
}