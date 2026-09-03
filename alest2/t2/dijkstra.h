#include "Grafo_dirigido.h"
#include <iostream>
#include <queue>
using namespace std;

class dijktra
{
private:
    int max;
    unordered_map<string, int> distTo;
    queue<string> pq;
    set<string> vertex;
public:
    dijktra(Grafo_Dirigido &g, string s)
    {
        max = 0;
        for (auto v : g.getVerts())
        {
            distTo[v] = -1.0;
        }
        distTo[s] = 1;
        pq.push(s);
        vertex.insert(s);
        while (!pq.empty())
        {
            string v = pq.front();
            pq.pop();
            for (Edge e : g.getAdj(v))
            {
                relax(e);
            }
        }
    }

    int DistTo(string v) { return distTo[v]; }

    bool contain(set<string> &vertex, string w) { return *vertex.find(w) == w; }

    void relax(Edge e)
    {
        string v = e.v;
        string w = e.w;
        if (distTo[w] < distTo[v] + 1.0)
        {
            distTo[w] = distTo[v] + 1.0;
            if (distTo[w] > max)
                max = distTo[w];
            if (!contain(vertex, w)){
                pq.push(w);
                vertex.insert(w);
            }
        }
    }

    int MAX() { return max; }
};