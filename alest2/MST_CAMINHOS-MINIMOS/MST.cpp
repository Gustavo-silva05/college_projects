#include <iostream>
#include "edgeweightedgraph.h"
#include "minheap.h"
#include <iterator>
#include <vector>
#include <queue>

using namespace std;
class PRIM_MST
{
private:
    vector<bool> marked;
    queue<Edge> mst;
    MinHeap<Edge> pq;

public:
    PRIM_MST(EdgeWeightedGraph &G);
    void visit(EdgeWeightedGraph &G, int v);
};

PRIM_MST::PRIM_MST(EdgeWeightedGraph &G)
{
    visit(G, 0);
    while( !pq.isEmpty() && mst.size() < G.getVerts().size()-1){
        Edge e = pq.delMin();
        int v = stoi(e.v), w = stoi (e.w);
        if (marked[v] && marked[w])continue;
        mst.push(e);
        if(!marked[v]) visit(G,v);
        if(!marked[w]) visit(G,w);
    }
}

void PRIM_MST::visit(EdgeWeightedGraph &G, int v)
{
    string s = to_string(v);
    marked[v] = true;
    for (Edge e : G.getAdj(s))
    {
        if (e.w == s)
            pq.put(e);
    }
}