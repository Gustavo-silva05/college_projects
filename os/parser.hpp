#include <iostream>
#include <fstream>
#include <vector>
#include <string>

using namespace std;

struct Operacao {
    string tipo;  
    string id;    
    int tamanho;      
};

string removeBlank(string input) { // tira os espacos em branco
    string output;
    for (char c : input) {
        if (c != ' '  && c != '\t')
            output += c;
    }

    return output;
}

vector<Operacao> parseArquivoOperacoes(string nome_arquivo) {
    vector<Operacao> operacoes;
    ifstream arquivo(nome_arquivo);

    if (!arquivo.is_open()) {
        cerr << "Erro ao abrir o arquivo: " << nome_arquivo << endl;
        return operacoes;
    }

    string linha;
    while (getline(arquivo, linha)) {
        linha = removeBlank(linha);

        if (linha.substr(0, 2) == "IN") { // verificacao pra IN, adiciona id e tamanho no vetor
            int abrePar = linha.find('(');
            int virgula = linha.find(',');
            int fechaPar = linha.find(')');

            if (abrePar != string::npos && virgula != string::npos && fechaPar != string::npos) {
                string id = linha.substr(abrePar + 1, virgula - abrePar - 1);
                int tamanho = stoi(linha.substr(virgula + 1, fechaPar - virgula - 1));
                operacoes.push_back({"IN", id, tamanho});
            }
        } else if (linha.substr(0, 3) == "OUT") { // verificacao pra OUT, so ve o id do processo
            int abrePar = linha.find('(');
            int fechaPar = linha.find(')');
            if (abrePar != string::npos && fechaPar != string::npos) {
                string id = linha.substr(abrePar + 1, fechaPar - abrePar - 1);
                operacoes.push_back({"OUT", id, 0});
            }
        }
    }

    arquivo.close();
    return operacoes;
}