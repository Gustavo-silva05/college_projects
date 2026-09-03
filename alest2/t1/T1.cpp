#include <iostream>
#include <fstream>
#include <stack>
#include <string>
#include <chrono>

using namespace std;
using namespace std::chrono;
int operacoes = 0;

int main(){
    auto start = steady_clock::now();
    
    ifstream arq1;  
    int col,lin;
    string linha;               
    arq1.open("teste.txt");
    if(arq1.is_open()) {                
        arq1 >> lin;
        getline(arq1,linha);
        string matriz [lin];
        for(int l=0; l < lin; l++){
            operacoes+=2;
            getline(arq1,linha);   
            matriz[l] = linha;
        }
        
        int linhaAtual=0, colunaAtual=0; 
        int valor_total=0;
        string direcao = "direita";
        for(linhaAtual =0; matriz[linhaAtual][0] != '-'; linhaAtual++);
        
        while (matriz[linhaAtual][colunaAtual]!='#'  && matriz[linhaAtual][colunaAtual]!=' ') {
            if (matriz[linhaAtual][colunaAtual]=='-'){ 
                if(direcao == "direita"){colunaAtual++;continue;}
                if(direcao == "esquerda"){colunaAtual--;continue;}
                if(direcao == "cima"){linhaAtual--;continue;}
                if(direcao == "baixo"){linhaAtual++;continue;}
                operacoes+=3;
            }
            if (matriz[linhaAtual][colunaAtual]=='|'){
                if (direcao == "baixo"){linhaAtual++;continue;}
                if (direcao == "cima"){linhaAtual--;continue;}
                if (direcao == "direita"){colunaAtual++;continue;}
                if (direcao == "esquerda"){colunaAtual--;continue;}
                operacoes+=3;
            }
            if (matriz[linhaAtual][colunaAtual]=='/' ){
                if (direcao == "direita"){direcao = "cima";linhaAtual--;continue;}
                if (direcao == "baixo"){direcao = "esquerda";colunaAtual--;continue;}
                if (direcao == "esquerda"){direcao = "baixo";linhaAtual++;continue;}
                if (direcao == "cima"){direcao = "direita";colunaAtual++;continue;}
                operacoes+=4;
            }
            if (matriz[linhaAtual][colunaAtual]=='\\'){
                if (direcao == "direita"){direcao = "baixo";linhaAtual++;continue;}
                if (direcao == "baixo"){direcao = "direita";colunaAtual++;continue;}
                if (direcao == "esquerda"){direcao = "cima";linhaAtual--;continue;}
                if (direcao == "cima"){direcao = "esquerda";colunaAtual--;continue;}
                operacoes+=4;
            }
            if(isdigit(matriz[linhaAtual][colunaAtual])){
                int valor_parcial=0;
                stack <int> pilha;
                while(isdigit(matriz[linhaAtual][colunaAtual])){
                    operacoes+=3;
                    pilha.push(matriz[linhaAtual][colunaAtual]-'0');
                    if(direcao=="direita"){colunaAtual++;continue;}
                    if(direcao=="esquerda"){colunaAtual--;continue;}
                    if(direcao=="cima"){linhaAtual--;continue;}
                    if(direcao=="baixo"){linhaAtual++;continue;}
                }
                int tam= pilha.size();
                for(int i=0; i< tam; i++){
                    operacoes+=3;
                    int aux = pilha.top();
                    for(int j=0; j<i; j++){aux*=10;operacoes++;}
                    pilha.pop();
                    valor_parcial+=aux;
                }
                valor_total+=valor_parcial;
            }
        }
        auto end = steady_clock::now();
        auto timer = end - start;

        cout << "\ntempo de execucao: " << duration<double>{timer}.count();
        cout << "\ntotal ($):  " << valor_total << endl;
        cout << "operacoes: " << operacoes << endl;
        arq1.close();
    }
    else{
        cout << "ERRO ARQUIVO NAO ABRIU OU NAO EXISTE" << endl;
    }
}
