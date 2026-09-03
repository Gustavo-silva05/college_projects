#include <iostream>
#include <vector>
using namespace std;

#define TOLERANCIA 1e-5

double absoluto(double a)
{
    return (a < 0) ? -a : a;
}

bool verifica_delta(vector<vector<double>> matriz, vector<double> resp, vector<double> sol)
{
    vector<double> delta;
    for (int i = 0; i < matriz.size(); i++)
    {
        delta.push_back(0.0);
        for (int j = 0; j < matriz.size(); j++)
        {
            delta[i] += (matriz[i][j] * resp[j]);
        }
    }
    double diferenca = 0;
    for (int i = 0; i < matriz.size(); i++)
    {
        if (absoluto(delta[i] - sol[i]) > TOLERANCIA)
            return false;
        diferenca += absoluto(delta[i] - sol[i]);
    }

    return (diferenca < TOLERANCIA) ? true : false;
}

vector<double> gauss_jacob_t2(vector<vector<double>> matriz, vector<double> sol)
{
    vector<double> sol_ant, sol_nova;
    for (int i = 0; i < matriz.size(); i++)
    {
        sol_ant.push_back(1.0);
        sol_nova.push_back(0.0);
    }
    do
    {
        for (int i = 0; i < matriz.size(); i++)
        {
            sol_nova[i] = 0.0;
            sol_nova[i] += sol[i];
            if (i == 0)
            {
                sol_nova[i] += (-matriz[i][i + 1]) * sol_ant[i + 1];
            }
            else if (i == matriz.size() - 1)
            {
                sol_nova[i] += (-matriz[i][i - 1]) * sol_ant[i - 1];
            }
            else
            {
                sol_nova[i] += (-matriz[i][i + 1]) * sol_ant[i + 1];
                sol_nova[i] += (-matriz[i][i - 1]) * sol_ant[i - 1];
            }
            sol_nova[i] /= matriz[i][i];
        }
        sol_ant = sol_nova;
    } while (!verifica_delta(matriz, sol_nova, sol));

    return sol_nova;
}

vector<double> gauss_jacob_geral(vector<vector<double>> matriz, vector<double> sol)
{
    vector<double> sol_ant, sol_nova;
    for (int i = 0; i < matriz.size(); i++)
    {
        sol_ant.push_back(1.0);
        sol_nova.push_back(0.0);
    }
    do
    {
        for (int i = 0; i < matriz.size(); i++)
        {
            sol_nova[i] = 0.0;
            sol_nova[i] += sol[i];
            for (int j = 0; j < matriz.size(); j++)
            {
                if (i != j)
                {
                    sol_nova[i] += (-matriz[i][j]) * sol_ant[j];
                }
            }
            sol_nova[i] /= matriz[i][i];
        }
        sol_ant = sol_nova;
    } while (!verifica_delta(matriz, sol_nova, sol));

    return sol_nova;
}

vector<double> gauss_seidel_geral(vector<vector<double>> matriz, vector<double> sol)
{
    vector<double> sol_ant, sol_nova;
    for (int i = 0; i < matriz.size(); i++)
    {
        sol_ant.push_back(1.0);
        sol_nova.push_back(0.0);
    }
    do
    {
        for (int i = 0; i < matriz.size(); i++)
        {
            sol_nova[i] = 0.0;
            sol_nova[i] += sol[i];
            for (int j = 0; j < matriz.size(); j++)
            {
                if (i != j)
                {
                    if (j < i)
                    {
                        sol_nova[i] += (-matriz[i][j]) * sol_nova[j];
                    }
                    else
                    {
                        sol_nova[i] += (-matriz[i][j]) * sol_ant[j];
                    }
                }
            }
            sol_nova[i] /= matriz[i][i];
        }
        sol_ant = sol_nova;
    } while (!verifica_delta(matriz, sol_nova, sol));

    return sol_nova;
}

vector<double> gauss_seidel_t2(vector<vector<double>> matriz, vector<double> sol)
{
    vector<double> sol_ant, sol_nova;
    for (int i = 0; i < matriz.size(); i++)
    {
        sol_ant.push_back(1.0);
        sol_nova.push_back(0.0);
    }
    do
    {
        for (int i = 0; i < matriz.size(); i++)
        {
            sol_nova[i] = 0.0;
            sol_nova[i] += sol[i];

            if (i == 0)
            {
                sol_nova[i] += (-matriz[i][i + 1]) * sol_ant[i + 1];
            }
            else if (i == matriz.size() - 1)
            {
                sol_nova[i] += (-matriz[i][i - 1]) * sol_nova[i - 1];
            }
            else
            {
                sol_nova[i] += (-matriz[i][i + 1]) * sol_ant[i + 1];
                sol_nova[i] += (-matriz[i][i - 1]) * sol_nova[i - 1];
            }

            sol_nova[i] /= matriz[i][i];
        }
        sol_ant = sol_nova;
    } while (!verifica_delta(matriz, sol_nova, sol));

    return sol_nova;
}

vector<double> gauss_classic(vector<vector<double>> matriz, vector<double> sol)
{
    vector<vector<double>> a;
    vector<double> x;
    float ratio;
    int i, j, k;
    int n = sol.size();
    for (i = 0; i < n; i++)
    {
        x.push_back(0.0);
        a.push_back(matriz[i]);
        a[i].push_back(sol[i]);
    }

    /* Applying Gauss Elimination */
    for (i = 0; i < n - 1; i++)
    {
        if (a[i][i] == 0.0)
        {
            printf("Mathematical Error!");
            exit(0);
        }
        for (j = i + 1; j < n; j++)
        {
            ratio = a[j][i] / a[i][i];

            for (k = 0; k < n + 1; k++)
            {
                a[j][k] = a[j][k] - ratio * a[i][k];
            }
        }
    }
    /* Obtaining Solution by Back Subsitution */
    x[n - 1] = a[n - 1][n] / a[n - 1][n - 1];
    for (i = n - 2; i >= 0; i--)
    {
        x[i] = a[i][n];
        for (j = i + 1; j < n; j++)
        {
            x[i] = x[i] - a[i][j] * x[j];
        }
        x[i] = x[i] / a[i][i];
    }

    return x;
}

vector<double> gauss_classic_t2(vector<vector<double>> matriz, vector<double> sol)
{
    vector<vector<double>> a;
    vector<double> x;
    float ratio;
    int i, j, k;
    int n = sol.size();
    for (i = 0; i < n; i++)
    {
        x.push_back(0.0);
        a.push_back(matriz[i]);
        a[i].push_back(sol[i]);
    }

    /* Applying Gauss Elimination */
    for (i = 0; i < n - 1; i++)
    {
        ratio = a[i + 1][i] / a[i][i];
        a[i + 1][i] = a[i + 1][i] - ratio * a[i][i];
        a[i + 1][i + 1] = a[i + 1][i + 1] - ratio * a[i][i + 1];
        a[i + 1][n] = a[i + 1][n] - ratio * a[i][n];
    }
    /* Obtaining Solution by Back Subsitution */
   
    for (i = n-1; i >= 0; i--)
    {
        if ( i == n-1)  x[i] = a[i][n] / a[i][i];
        else            x[i] = (a[i][n] - (a[i][i+1] * x[i+1])) / a[i][i];
    }
    
    return x;
}

