#include <iostream>
#include <fstream>
#include <sstream>
#include <set>
#include "dijkstra.h"
#include <chrono>
#include "Caixa.h"
#include <unordered_map>
#include <algorithm>
#include <stdlib.h>

int op = 0;

using namespace std;
using namespace std::chrono;

int main()
{
    auto start = steady_clock::now();
    vector<Caixa> index;
    set<string> vert;
    unordered_map<string, vector<Edge>> graf;
    unordered_map<string, int> entradas;

    // leitura de arquivo de texto

    ifstream reader;
    string line;
    reader.open("teste100.txt");
    if (reader.is_open())
    {
        while (!reader.eof())
        {
            getline(reader, line);
            if (line != "")
            {
                Caixa n(line);
                index.push_back(n);
                op++;
                vert.insert(line);
            }
        }
    }

    else
    {
        std::cout << "FALHA NA LEITURA" << endl;
    }
    reader.close();

    /*
            laço que verifica se cada caixa cabe em todas e gurada
            as saidas e entradas de cada string em maps;
    */
    std::sort(index.begin(), index.end());
    

    for (int i = 0; i < index.size() - 1; i++)
    {
        vector<Edge> &list = graf[index[i].caixa];
        for (int j = i + 1; j < index.size(); j++)
        {
            op++;
            if (index[i].compara(index[j]))
            {
                Edge e(index[i].caixa, index[j].caixa);
                list.push_back(e);

                entradas[index[j].caixa]++;
            }
        }
    }
    /*
           Verifica quais os pontos (vertices) são raizes e quais são folhas;
    */
    vector<Caixa> raiz;
    for (int i = 0; i < index.size() - 1; i++)
    {
        op++;
        if (entradas[index[i].caixa] == 0)
        {
            raiz.push_back(index[i]);
        }
    }
    
    /*
            cria o grafo e faz o caminhamentos da raiz para as folhas
            e guarda o maior
    */
    Grafo_Dirigido g(graf, vert);
    cout << endl << endl;
    int max = 0;
    for (auto a : raiz)
    {
        dijktra d(g, a.caixa);
        if (d.MAX() > max)
        {
            max = d.MAX();
            cout << a.caixa << "\t" << max << endl;
        }
    }
    auto end = steady_clock::now();
    auto timer = end - start;
    std::cout << "\ntempo de execucao: " << duration<double>{timer}.count() << " seg" << endl;
    std::cout << "Maior ramo = " << max << endl;
    std::cout << "operacoes = " << op;
}