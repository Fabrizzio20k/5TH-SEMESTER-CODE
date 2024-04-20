#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cstring>

using namespace std;
struct Record
{
    long cod;
    char nombre[16];
    int ciclo;

    long left, right, height;
    Record() : cod(-1), ciclo(-1), left(-1), right(-1), height(1)
    {
        strcpy(this->nombre, "");
    }
    Record(int cod, const char *nombre, int ciclo) : cod(cod), ciclo(ciclo), left(-1), right(-1), height(1)
    {
        strcpy(this->nombre, nombre);
    }

    void showData() const
    {
        cout << "Codigo: " << cod << " | " << "Nombre: " << nombre << " | " << "Ciclo: " << ciclo << " | " << "Left: " << left << " | " << "Right: " << right << " | " << "Height: " << height << endl;
    }
};

fstream &operator>>(fstream &file, Record &record1)
{
    file.read((char *)&record1, sizeof(Record));
    return file;
}

fstream &operator<<(fstream &file, const Record &record)
{
    file.write((char *)&record, sizeof(Record));
    return file;
}

template <typename TK>
class AVLFile
{

private:
    string file_name;
    long pos_root;
    void updateRoot(fstream &file, long index_root)
    {
        file.seekp(0, ios::beg);
        file.write((char *)&index_root, sizeof(long));
        file.flush();
    }
    void getAndUpdateRoot()
    {
        fstream file(this->file_name, ios::in | ios::out | ios::binary);
        file.seekg(0);
        file.read((char *)&this->pos_root, sizeof(long));
        file.close();
    }

public:
    static void clearFile(string filename)
    {
        fstream file(filename, ios::out | ios::binary);
        file.close();
    }
    static void deleteFile(string filename)
    {
        remove(filename.c_str());
    }
    AVLFile(string file_name)
    {
        this->file_name = file_name;
        this->pos_root = -1;
        fstream file(file_name, ios::binary | ios::in | ios::out);
        if (!file)
        {
            cout << "File not found, creating new file\n\n";
            file.close();
            file.open(file_name, ios::out | ios::binary);
            file.write((char *)&pos_root, sizeof(long));
            file.close();
        }
        else
        {
            file.close();
            getAndUpdateRoot();
            cout << "File found, root at pos: " << pos_root << " (starting from 0)\n"
                 << endl;
        }
    }
    Record find(TK key)
    {
        fstream file(file_name, ios::in | ios::binary);
        Record record = find(pos_root, key, file);
        file.close();
        return record;
    }
    void insert(Record record)
    {
        fstream file(file_name, ios::in | ios::out | ios::binary);
        file.seekg(0, ios::beg);
        file.read((char *)&pos_root, sizeof(long));
        insert(record, pos_root, -1, file, false, false);
        file.close();
    }
    vector<Record> inorder()
    {
        fstream file_explorer(file_name, ios::in | ios::out | ios::binary);
        vector<Record> res;
        inorder(pos_root, file_explorer, res);
        file_explorer.close();

        return res;
    }

private:
    long read_root(fstream &filename)
    {
        filename.seekg(0, ios::beg);
        long index_root;
        filename.read((char *)&index_root, sizeof(long));
        return index_root;
    }

    void file_creator(string filename)
    {

        fstream comprobation_file(filename, ios::in | ios::binary);
        if (comprobation_file)
        {

            comprobation_file.close();
        }
        else
        {
            cout << "Not found creating new file..." << endl;
            comprobation_file.close();
            fstream create(filename, ios::out | ios::binary);
            create.close();
        }
    }

    void inorder(int pos_node, fstream &file_explorer, vector<Record> &records)
    {
        long repos = sizeof(long) + pos_node * sizeof(Record);
        if (pos_node == -1)
            return;

        file_explorer.seekg(repos);
        Record record;
        file_explorer >> record;
        inorder(record.left, file_explorer, records);
        records.push_back(record);
        inorder(record.right, file_explorer, records);
    }
    Record find(long pos_node, TK key, fstream &file)
    {
        long repos = sizeof(long) + pos_node * sizeof(Record);
        file.seekg(repos);
        Record record;
        file >> record;
        if (key > record.cod)
            find(record.right, key, file);
        else if (key < record.right)
            find(record.left, key, file);
        return record;
    }

    long height(long pos_node, fstream &file)
    {
        long repos = sizeof(long) + pos_node * sizeof(Record);
        if (pos_node == -1)
            return 0;
        file.seekg(repos, ios::beg);
        Record record;
        file >> record;
        return record.height;
    }

    long getBalanceFactor(long pos_node, fstream &file)
    {
        long repos = sizeof(long) + pos_node * sizeof(Record);
        if (pos_node == -1)
            return 0;

        file.seekg(repos, ios::beg);
        Record record;
        file >> record;
        return height(record.left, file) - height(record.right, file);
    }

    void update_height(long pos_node, fstream &file)
    {
        long repos = sizeof(long) + pos_node * sizeof(Record);
        file.seekg(repos, ios::beg);
        Record record;
        file >> record;

        long new_height = 1 + max(height(record.left, file), height(record.right, file));

        file.seekp(repos + sizeof(Record) - sizeof(long));
        file.write((char *)&new_height, sizeof(long));
    }

    void insert(Record record, long pos_node, long parent_pos, fstream &file, bool isLeft, bool isRight)
    {
        long repos = sizeof(long) + pos_node * sizeof(Record);
        long repar = sizeof(long) + parent_pos * sizeof(Record);

        if (pos_node == -1)
        {
            file.seekp(0, ios::end);
            file << record;
            pos_root = 0;
            updateRoot(file, pos_root);
            return;
        }

        file.seekg(repos, ios::beg);
        Record current;
        file >> current;

        if (record.cod > current.cod)
        {
            if (current.right == -1)
            {
                file.seekp(0, ios::end);
                long right = (file.tellp() - sizeof(long)) / sizeof(Record);
                file << record;

                if (right == -1)
                    return;
                file.seekp(repos + sizeof(Record) - 2 * sizeof(long), ios::beg);
                file.write((char *)&right, sizeof(long));
            }
            else
            {
                insert(record, current.right, pos_node, file, false, true);
            }
        }
        else if (record.cod < current.cod)
        {
            if (current.left == -1)
            {
                file.seekp(0, ios::end);
                long left = (file.tellp() - sizeof(long)) / sizeof(Record);
                file << record;

                if (left == -1)
                    return;
                file.seekp(repos + sizeof(Record) - 3 * sizeof(long), ios::beg);
                file.write((char *)&left, sizeof(long));
            }
            else
            {
                insert(record, current.left, pos_node, file, true, false);
            }
        }

        update_height(pos_node, file);

        if (getBalanceFactor(pos_node, file) <= -2 && getBalanceFactor(current.right, file) < 0)
        {
            if (parent_pos == -1)
            {
                pos_root = leftRotation(pos_node, file);
                updateRoot(file, pos_root);
            }
            else
            {
                long newNode = leftRotation(pos_node, file);

                if (isRight)
                {

                    file.seekp(repar + sizeof(Record) - 2 * sizeof(long));
                    file.write((char *)&newNode, sizeof(long));
                }
                else if (isLeft)
                {
                    file.seekp(repos + sizeof(Record) - 3 * sizeof(long));
                    file.write((char *)&newNode, sizeof(long));
                }
            }
        }
        if (getBalanceFactor(pos_node, file) >= 2 && getBalanceFactor(current.left, file) > 0)
        {
            if (parent_pos == -1)
            {
                pos_root = rightRotation(pos_node, file);
                updateRoot(file, pos_root);
            }
            else
            {
                long newNode = rightRotation(pos_node, file);
                if (isRight)
                {

                    file.seekp(repar + sizeof(Record) - 2 * sizeof(long));
                    file.write((char *)&newNode, sizeof(long));
                }
                else if (isLeft)
                {
                    file.seekp(repar + sizeof(Record) - 3 * sizeof(long));
                    file.write((char *)&newNode, sizeof(long));
                }
            }
        }
        if (getBalanceFactor(pos_node, file) >= 2 && getBalanceFactor(current.left, file) < 0)
        {
            long prevNode = leftRotation(current.left, file);
            current.left = prevNode;
            file.seekp(repos, ios::beg);
            file << current;

            long newNode = rightRotation(pos_node, file);

            if (parent_pos == -1)
            {
                pos_root = newNode;
                updateRoot(file, pos_root);
                update_height(pos_root, file);
            }
            else
            {
                Record parent_node;
                file.seekg(repar, ios::beg);
                file >> parent_node;

                if (isRight)
                {

                    file.seekp(repar, ios::beg);
                    parent_node.right = newNode;
                    file << parent_node;
                }
                else if (isLeft)
                {
                    file.seekp(repar, ios::beg);
                    parent_node.left = newNode;
                    file << parent_node;
                }
            }
        }
        if (getBalanceFactor(pos_node, file) <= -2 && getBalanceFactor(current.right, file) > 0)
        {
            long prevNode = rightRotation(current.right, file);
            current.right = prevNode;
            file.seekp(repos, ios::beg);
            file << current;

            long newNode = leftRotation(pos_node, file);

            if (parent_pos != -1)
            {
                pos_root = newNode;
                updateRoot(file, pos_root);
                update_height(pos_root, file);
            }
            else
            {
                Record parent_node;
                file.seekg(repar, ios::beg);
                file >> parent_node;

                if (isRight)
                {

                    file.seekp(repar, ios::beg);
                    parent_node.right = newNode;
                    file << parent_node;
                }
                else if (isLeft)
                {
                    file.seekp(repar, ios::beg);
                    parent_node.left = newNode;
                    file << parent_node;
                }
            }
        }
    }

    long rightRotation(long pos_node, fstream &file)
    {

        Record currentNode, leftNode;
        file.seekg(sizeof(long) + pos_node * sizeof(Record), ios::beg);
        file.read((char *)&currentNode, sizeof(Record));

        long left = currentNode.left;

        file.seekg(sizeof(long) + left * sizeof(Record), ios::beg);
        file.read((char *)&leftNode, sizeof(Record));

        currentNode.left = leftNode.right;
        leftNode.right = pos_node;

        currentNode.height = 1 + max(height(currentNode.left, file), height(currentNode.right, file));
        leftNode.height = 1 + max(height(leftNode.left, file), height(pos_node, file));

        file.seekp(sizeof(long) + pos_node * sizeof(Record), ios::beg);
        file << currentNode;
        file.seekp(sizeof(long) + left * sizeof(Record), ios::beg);
        file << leftNode;

        return left;
    }
    long leftRotation(long pos_node, fstream &file)
    {
        Record currentNode, rightNode;

        file.seekg(sizeof(long) + pos_node * sizeof(Record), ios::beg);
        file >> currentNode;
        long right = currentNode.right;

        file.seekg(sizeof(long) + right * sizeof(Record), ios::beg);
        file >> rightNode;

        currentNode.right = rightNode.left;
        rightNode.left = pos_node;

        currentNode.height = 1 + max(height(currentNode.left, file), height(currentNode.right, file));
        rightNode.height = 1 + max(height(pos_node, file), height(rightNode.right, file));

        file.seekp(sizeof(long) + pos_node * sizeof(Record), ios::beg);
        file << currentNode;
        file.seekp(sizeof(long) + right * sizeof(Record), ios::beg);
        file << rightNode;
        return right;
    }
};

void writeFile(string filename)
{
    AVLFile<long>::deleteFile(filename);
    AVLFile<long> file(filename);

    vector<Record> records = {
        Record(1, "Juan", 1),
        Record(2, "Pedro", 2),
        Record(3, "Maria", 3),
        Record(4, "Jose", 4),
        Record(5, "Carlos", 5),
        Record(6, "Luis", 6),
        Record(7, "Ana", 7),
        Record(8, "Rosa", 8),
        Record(9, "Luisa", 9),
        Record(10, "Luz", 10),
    };
    for (const auto &record : records)
    {
        file.insert(record);
    }
}

void readFile(string filename)
{
    AVLFile<long> file(filename);

    cout << "--------- show along sorted data -----------\n";
    vector<Record> result = file.inorder();
    for (Record re : result)
    {
        re.showData();
    }
    cout << "--------- find data -----------\n";
    Record record = file.find(5);
    record.showData();
}

int main()
{

    writeFile("data.dat");
    readFile("data.dat");
}
