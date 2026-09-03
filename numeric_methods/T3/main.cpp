#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>

using namespace std;

class Ponto
{
public:
    double x, y;
    Ponto(double x, double y)
    {
        this->x = x;
        this->y = y;
    }
};

vector<double> intepolacao(const vector<Ponto> &pontos)
{
    int n = pontos.size();
    vector<double> coeficientes(n);
    vector<vector<double>> tabela(n, vector<double>(n));
    for (int i = 0; i < n; i++)
    {
        tabela[i][0] = pontos[i].y;
    }
    for (int j = 1; j < n; ++j)
    {
        for (int i = 0; i < n - j; ++i)
        {
            tabela[i][j] = (tabela[i + 1][j - 1] - tabela[i][j - 1]) / (pontos[i + j].x - pontos[i].x);
        }
    }
    for (int i = 0; i < n; i++)
    {
        printf ("%2.5lf;", pontos[i].x);
        for (int j = 0; j < n - i; j++)
        {
            printf("%2.5lf;", tabela[i][j]);
        }
        cout << endl;
    }

    for (int i = 0; i < n; ++i)
    {
        coeficientes[i] = tabela[0][i];
    }
    return coeficientes;
}

double f_x (const vector<double> &coef, const vector<Ponto> &pontos, double x)
{
    double result = coef[0];
    double term = 1.0;

    for (int i = 1; i < pontos.size(); ++i)
    {
        term *= (x - pontos[i - 1].x);
        result += coef[i] * term;
    }

    return result;
}

int main()
{
    vector<Ponto> pontos;
    double x, y;
    int n;
    char opcao;
    cout << "MENU  \n(M OR m) manual  \nAutomatico \n";
    cin >> opcao;
    if (opcao == 'm' || opcao == 'M')
    {
        cout << "Digite numero de Pontos: ";
        cin >> n;
        for (int i = 0; i < n; i++)
        {
            printf("Digite x[%d] e y[%d] = ", i, i);
            scanf("%lf %lf", &x, &y);
            Ponto novo(x, y);
            pontos.push_back(novo);
        }
        int i = 0;
        for (auto c : pontos)
        {
            cout << "Ponto [" << i << "] = " << c.x << " e " << c.y << endl;
            i++;
        }
        cout << endl;
        vector<double> interpol = intepolacao(pontos);
        cout << endl;
        i = 0;
        for (auto c : interpol)
        {
            cout << "Coeficiente [" << i << "] = " << c << endl;
            i++;
        }
        cout << "Digite x: ";
        cin >> x;
        cout << "f(" << x << ") = " << f_x(interpol, pontos, x) << endl;
    }
    else
    {
        ifstream arq;
        arq.open("ex.txt");
        if (arq.is_open())
        {
            string line;
            getline(arq, line);
            n = stoi(line);
            for (int i = 0; i < n; i++)
            {
                getline(arq, line);
                stringstream ss(line);
                getline(ss, line, ' ');
                x = stod(line);
                getline(ss, line, ' ');
                y = stod(line);
                Ponto novo(x, y);
                pontos.push_back(novo);
            }
            int i = 0;
            for (auto c : pontos)
            {
                cout << "Ponto [" << i << "] = " << c.x << " e " << c.y << endl;
                i++;
            }
            cout << endl;
            vector<double> interpol = intepolacao(pontos);
            cout << endl;
            i = 0;
            for (auto c : interpol)
            {
                cout << "Coeficiente [" << i << "] = " << c << endl;
                i++;
            }
            getline(arq, line);
            x = stod(line);
            cout << "f(" << x << ") = " << f_x(interpol, pontos, x) << endl;
        }
        arq.close();
    }
}