#include <iostream>
#include <vector>
#include <map>

using namespace std;

class WorstFit
{
private:
    vector<int> memory;
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
    WorstFit(int size)
    {
        memory = vector<int>(size, 0);
    }

    void addMemory(string name, int size)
    {
        int id = getId(name);
        int maxBlockStart = -1;
        int maxBlockSize = 0;

        int i = 0;
        while (i < memory.size())
        {
            if (memory[i] == 0)
            {
                int start = i;
                int length = 0;

                while (i < memory.size() && memory[i] == 0)
                {
                    length++;
                    i++;
                }

                if (length >= size && length > maxBlockSize)
                {
                    maxBlockSize = length;
                    maxBlockStart = start;
                }
            }
            else
            {
                i++;
            }
        }

        if (maxBlockStart == -1)
        {
            cout << "Não ha espaço suficiente para o processo " << id << endl;
        }
        else
        {
            for (int j = maxBlockStart; j < maxBlockStart + size; j++)
            {
                memory[j] = id;
            }
            cout << "Processo " << name << " alocado a partir da posicao " << maxBlockStart << endl;
        }
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