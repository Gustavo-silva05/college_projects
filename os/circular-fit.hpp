#include <iostream>
#include <vector>
#include <map>
using namespace std;

class CircularFit
{
private:
    vector<int> memory;
    int pointer = 0;
    int nextId = 1;
    map<string, int> nameToId;
    map<int, string> idToName;

    int getId(string name)
    {
        if (nameToId.count(name))
            return nameToId[name];
        nameToId[name] = nextId;
        idToName[nextId] = name;
        return nextId++;
    }

public:
    CircularFit(int size)
    {
        memory = vector<int>(size, 0);
    }

    void addMemory(string name, int size)
    {
        int id = getId(name);
        int memSize = memory.size();
        int start = -1;
        int count = 0;
        int scanned = 0;
        int i = pointer;

        while (scanned < memSize)
        {
            if (memory[i] == 0)
            {
                if (count == 0)
                    start = i;
                count++;
                if (count == size)
                {
                    for (int j = start; j < start + size; j++)
                    {
                        memory[j % memSize] = id;
                    }
                    pointer = (start + size) % memSize;
                    cout << "Processo " << name << " alocado a partir da posicao " << start << endl;
                    return;
                }
            }
            else
            {
                count = 0;
                start = -1;
            }
            i = (i + 1) % memSize;
            scanned++;
        }

        cout << "Nao ha espaco suficiente para o processo " << name << endl;
    }

    void removeMemory(string name)
    {
        if (!nameToId.count(name))
        {
            cout << "Processo " << name << " nao encontrado na memoria." << endl;
            return;
        }
        int id = nameToId[name];
        for (int i = 0; i < memory.size(); i++)
        {
            if (memory[i] == id)
            {
                memory[i] = 0;
            }
        }
        cout << "Processo " << name << " removido da memoria." << endl;
        nameToId.erase(name);
        idToName.erase(id);
    }

    void printMemory() 
    {
        cout << "Estado atual da memoria: ";

        int i = 0;
        while (i < memory.size()) 
        {
            int id = memory[i];
            int count = 1;

            // conta quantos bytes consecutivos tem o mesmo ID
            while (i + count < memory.size() && memory[i + count] == id) 
            {
                count++;
            }

            if (id == 0) {
                cout << count << " | ";
            } 
            else {
                cout << count << "(" << idToName[id] << ") | ";
            }

            i += count;
        }

        cout << endl;
    }
};