#include <iostream>
#include <cmath>
#include <string>
using namespace std;

typedef struct Block
{
    string process_name; // ID do processo que alocou o bloco
    int is_free;         // 1 = livre, 0 = ocupado
    struct Block *left;
    struct Block *right;
    struct Block *parent;
    int size;      // tamanho do bloco
    int used_size; // tamanho usado do bloco
    string str()
    {
        return "Block(size: " + to_string(size) + ", used_size: " + to_string(used_size) + ", is_free: " + to_string(is_free) +
               ", process: " + process_name + ")";
    }
} Block;

bool check_pow_of_2(int main_size)
{
    return (main_size <= 0 || (main_size & (main_size - 1)) != 0) ? false : true;
}

class buddy
{
private:
    Block *root;

    //  Recursivamente encontra o primeiro bloco livre com tamanho >= necessário
    Block *find_and_split(Block *node, int size)
    {
        if (!node || node->size < size)
            return nullptr;

        // Se for folha e livre
        if (node->is_free && node->left == nullptr && node->right == nullptr)
        {
            if (node->size == size)
                return node;

            // Se maior, divide e continua
            if (node->size > size)
            {
                int half_size = node->size / 2;

                node->left = new Block();
                node->left->size = half_size;
                node->left->is_free = 1;
                node->left->parent = node;
                node->left->left = nullptr;
                node->left->right = nullptr;
                node->left->used_size = 0;
                node->left->process_name = "";

                node->right = new Block();
                node->right->size = half_size;
                node->right->is_free = 1;
                node->right->parent = node;
                node->right->left = nullptr;
                node->right->right = nullptr;
                node->right->used_size = 0;
                node->right->process_name = "";

                node->is_free = 0; // Agora é interno, não pode ser alocado
            }
        }

        // Continua busca nos filhos
        Block *left_result = find_and_split(node->left, size);
        if (left_result)
            return left_result;

        return find_and_split(node->right, size);
    }

    void print_tree(Block *node, int depth)
    {
        if (node == nullptr)
            return;

        // Imprime o bloco atual
        cout << string(depth, '\t') << node->str() << endl;

        // Imprime os blocos filhos
        print_tree(node->left, depth + 1);
        print_tree(node->right, depth + 1);
    }

    void print_free_blocks_fragments(Block *node)
    {
        if (node == nullptr)
            return;

        if (node->left == nullptr && node->right == nullptr)
        {
            if (node->is_free)
                cout << node->size << " | ";
            else if (node->used_size > 0 && node->size != node->used_size)
                cout << node->size << "(" << node->size - node->used_size << ") | ";
        }

        print_free_blocks_fragments(node->left);
        print_free_blocks_fragments(node->right);
    }

    void destroy_tree(Block *node)
    {
        if (node == nullptr)
            return;

        if (node->left)
            destroy_tree(node->left);
        if (node->right)
            destroy_tree(node->right);

        delete node;
    }
    Block *find_process(Block *node, const string &name)
    {
        if (!node)
            return nullptr;

        if (node->process_name == name && node->is_free == 0)
            return node;

        Block *found_left = find_process(node->left, name);
        if (found_left)
            return found_left;

        return find_process(node->right, name);
    }

    void try_merge(Block *node)
    {
        if (!node || !node->parent)
            return;

        Block *parent = node->parent;
        Block *sibling = (parent->left == node) ? parent->right : parent->left;

        if (!sibling || sibling->is_free == 0 || sibling->left || sibling->right)
            return; // Não pode mesclar

        // Pode mesclar: remove filhos, libera pai
        delete parent->left;
        delete parent->right;
        parent->left = nullptr;
        parent->right = nullptr;
        parent->is_free = 1;
        parent->used_size = 0;
        parent->process_name = "";

        try_merge(parent); // Tenta subir o merge
    }

public:
    buddy(int main_size)
    {
        if (check_pow_of_2(main_size))
        {
            root = new Block();
            root->is_free = 1;
            root->left = nullptr;
            root->right = nullptr;
            root->parent = nullptr;
            root->size = main_size;
            root->used_size = 0;
            root->process_name = "";
        }
        else
        {
            root = nullptr;
            cout << "Erro: tamanho da memória não é potência de 2" << endl;
        }
    }

    ~buddy()
    {
        destroy_tree(root);
    }

    void str()
    {
        if (root == nullptr)
        {
            cout << "Memória vazia." << endl;
            return;
        }
        print_free_blocks_fragments(root);
        cout << endl;
    }

    void in(string process_name, int size)
    {
        if (!root)
        {
            cout << "Sistema não inicializado corretamente." << endl;
            return;
        }
        int alloc_size = pow(2, ceil(log2(size)));
        Block *target = find_and_split(root, alloc_size);
        if (target == nullptr)
        {
            cout << "Erro: memória insuficiente para alocar " << size << " bytes." << endl;
            return;
        }

        target->is_free = 0;
        target->used_size = size;
        target->process_name = process_name;

        cout << "Alocado bloco de " << size << " bytes em no de tamanho " << target->size << " para o processo " << target->process_name << endl;
        // str();
    }

    void out(const string &name)
    {
        Block *target = find_process(root, name);
        if (!target)
        {
            cout << "Processo '" << name << "' não encontrado." << endl;
            return;
        }

        target->is_free = 1;
        target->used_size = 0;
        target->process_name = "";

        cout << "Memória liberada para processo '" << name << "'" << endl;

        try_merge(target); // Tenta fundir com o buddy

        // str();
    }
};
