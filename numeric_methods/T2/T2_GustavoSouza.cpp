#include "Gauss.h"
#include <math.h>

void print_mv(vector<vector<double>> matriz, vector<double> vetor)
{
    for (int i = 0; i < matriz.size(); i++)
    {
        for (int j = 0; j < matriz.size(); j++)
        {
            printf("%5.3g ", matriz[i][j]);
        }
        cout << " = " << vetor[i] << endl;
    }
}

void printv(vector<double> vet)
{
    for (int i = 0; i < vet.size(); i++)
        cout << vet[i] << endl;
}

double soma(vector<double> vetor)
{
    double soma = 0.0;
    for (auto i : vetor)
    {
        soma += i;
    }
    return round(soma);
}

vector<vector<double>> matriz(int n)
{
    vector<vector<double>> m;
    for (int i = 0; i < n; i++)
    {
        vector<double> newline;
        for (int j = 0; j < n; j++)
        {
            if (i == 0 || i == n - 1)
            {
                if (i == j)
                    newline.push_back(1.0 - ((n * 1.0 - 1.0) / (2 * n * 1.0)));
                else if (j == i + 1 || j == i - 1)
                    newline.push_back(-1.0 / 2.0);
                else
                    newline.push_back(0.0);
            }
            else
            {
                if (j == i - 1 || j == i + 1)
                    newline.push_back(-1.0 / 2.0);
                else if (i == j)
                    newline.push_back(1);
                else
                    newline.push_back(0.0);
            }
        }
        m.push_back(newline);
    }
    return m;
}

vector<double> solucao(int n)
{
    vector<double> sol;
    for (int i = 0; i < n; i++)
    {
        sol.push_back(0.0);
    }
    sol[((n + 1) / 2) - 1] = 1.0;
    return sol;
}

int main(int numargs, char *arg[])
{
    int n = strtol (arg[1], NULL, 10);;

    vector<vector<double>> mat = matriz(n);
    
    vector<double> sol = solucao(n);
    
    vector<double> pos_gaus = gauss_classic(mat,sol);

    cout << endl << "Populacao para " << n << " salas: " << soma(pos_gaus) << " atores"<< endl << endl;
    
     return EXIT_SUCCESS;

}