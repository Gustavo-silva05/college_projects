#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>

using namespace std;

class Caixa
{
public:
  string caixa;
  vector<int> dimensoes;
  int vol = 1;
  Caixa(string c)
  {
    caixa = c;
    stringstream ss(c);
    string n;
    int aux;
    for (int i = 0; i < 3; i++)
    {
      getline(ss, n, ' ');
        aux = stoi(n);
        dimensoes.push_back(aux);
        vol *= (aux);
    }
    std::sort(dimensoes.begin(), dimensoes.end());
  }
  
  bool operator<(Caixa &a)
  {
    return (vol < a.vol);
  }
  
  bool operator>(Caixa &a)
  {
    return (vol > a.vol);
  }
  
  bool compara(Caixa &a)
  {
    return ((dimensoes[0] < a.dimensoes[0]) && (dimensoes[1] < a.dimensoes[1]) && (dimensoes[2] < a.dimensoes[2]));
  }
};