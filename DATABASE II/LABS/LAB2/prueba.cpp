#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include "register.h"
using namespace std;

struct Record
{
    int cod;
    char nombre[12];
    int ciclo;

    long left, right;

    int height;

    Record() : left(-1), right(-1), height(1) {}
    Record(int cod, string nombre, int ciclo) : cod(cod), ciclo(ciclo), left(-1), right(-1), height(1)
    {
        if (nombre.size() > 12)
        {
            throw "Nombre no puede ser mayor a 12 caracteres";
        }
        for (int i = 0; i < nombre.size(); i++)
        {
            this->nombre[i] = nombre[i];
        }
        for (int i = nombre.size(); i < 12; i++)
        {
            this->nombre[i] = ' ';
        }
    }

    void setData()
    {
        cout << "Codigo:";
        cin >> cod;
        cout << "Nombre: ";
        cin >> nombre;
        cout << "Ciclo: ";
        cin >> ciclo;
    }

    void showData() const
    {
        cout << "Codigo: " << cod << " | " << "Nombre: " << nombre << " | " << "Ciclo: " << ciclo << " | " << "Left: " << left << " | " << "Right: " << right << " | " << "Height: " << height << endl;
    }
};

fstream &operator<<(fstream &file, const Record &record)
{
    file.write((char *)&record, sizeof(Record));
    return file;
}

fstream &operator>>(fstream &file, Record &record)
{
    file.read((char *)&record, sizeof(Record));
    return file;
}

class AVLFile
{
private:
    string filename;
    long pos_root;
    void updateRoot(long pos_root)
    {
        fstream file(filename, ios::in | ios::out | ios::binary);
        file.seekp(0);
        file.write((char *)&this->pos_root, sizeof(long));
        file.flush();
        file.close();

        this->pos_root = pos_root;
    }
    void getandUpdateRoot()
    {
        fstream file(filename, ios::in | ios::binary);
        file.read((char *)&this->pos_root, sizeof(long));
        file.close();
    }
    void appendNode(Record record)
    {
        fstream file(filename, ios::in | ios::out | ios::binary);
        file.seekp(0, ios::end);
        file.write((char *)&record, sizeof(Record));
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
    AVLFile(string filename)
    {
        this->filename = filename;
        fstream file(filename, ios::in | ios::binary);
        if (!file)
        {
            cout << "File not found, creating new file\n\n";
            file.close();
            file.open(filename, ios::out | ios::binary);
            this->pos_root = -1;
            file.write((char *)&this->pos_root, sizeof(long));
            file.close();
        }
        else
        {
            file.close();
            getandUpdateRoot();
            cout << "File found, root: " << pos_root << endl;
        }
    }
    Record find(int key)
    {
        fstream file(filename, ios::in | ios::binary);
        Record record;
        record = find(pos_root, key, file);
        file.close();
        return record;
    }
    void insert(Record record)
    {
        fstream file(filename, ios::in | ios::out | ios::binary);
        insert(pos_root, record, file, -1, false, false);
        file.close();
    }
    vector<Record> inorder()
    {
        fstream file(filename, ios::in | ios::out | ios::binary);
        vector<Record> result;
        file.seekg(sizeof(long), ios::beg);
        file.read((char *)&pos_root, sizeof(long));
        inorder(pos_root, file, result);
        file.close();
        return result;
    }

private:
    Record find(long pos_node, int key, fstream &file)
    {
        file.seekg(sizeof(long) + pos_node * sizeof(Record));
        Record record;
        file >> record;
        if (record.cod == key)
        {
            return record;
        }
        else if (record.cod > key)
        {
            if (record.left == -1)
            {
                throw "Record not found";
            }
            return find(record.left, key, file);
        }
        else
        {
            if (record.right == -1)
            {
                throw "Record not found";
            }
            return find(record.right, key, file);
        }
    }
    long insertAndLastPos(fstream &file, Record record)
    {
        file.seekp(0, ios::end);
        long pos = file.tellp();
        pos = (pos - sizeof(long)) / sizeof(Record);
        file << record;
        return pos;
    }

    void insert(long &pos_node, Record record, fstream &file, long parent, bool is_left, bool is_right)
    {
        if (pos_node == -1)
        {
            appendNode(record);
            updateRoot(0);
            return;
        }
        Record current;
        long repos = sizeof(long) + pos_node * sizeof(Record);
        file.seekg(repos, ios::beg);
        file >> current;

        if (record.cod > current.cod)
        {
            if (current.right == -1)
            {
                long right = insertAndLastPos(file, record);
                if (right == -1)
                {
                    throw "Error inserting record";
                }
                file.seekp(repos + sizeof(Record) - 2 * sizeof(long), ios::beg);
                file.write((char *)&right, sizeof(long));
            }
            else
            {
                insert(current.right, record, file, pos_node, false, true);
            }
        }
        else if (record.cod < current.cod)
        {
            if (current.left == -1)
            {
                long left = insertAndLastPos(file, record);
                if (left == -1)
                {
                    throw "Error inserting record";
                }
                file.seekp(repos + sizeof(Record) - 3 * sizeof(long), ios::beg);
                file.write((char *)&left, sizeof(long));
            }
            else
            {
                insert(current.left, record, file, pos_node, true, false);
            }
        }
        else
        {
            throw "Record already exists";
        }
        updateHeight(pos_node, file);
        if (getBalanceFactor(pos_node, file) <= -2 && getBalanceFactor(current.right, file) < 0)
        {
            if (parent != -1)
            {
                long newTree = leftRotation(pos_node, file);
                if (is_right)
                {
                    file.seekp(sizeof(long) + parent * sizeof(Record) + sizeof(Record) - 2 * sizeof(long), ios::beg);
                    file.write((char *)&newTree, sizeof(long));
                }
                else if (is_left)
                {
                    file.seekp(sizeof(long) + parent * sizeof(Record) + sizeof(Record) - 3 * sizeof(long), ios::beg);
                    file.write((char *)&newTree, sizeof(long));
                }
            }
            else
            {
                pos_root = leftRotation(pos_node, file);
                updateRoot(pos_root);
            }
        }
        if (getBalanceFactor(pos_node, file) >= 2 && getBalanceFactor(current.left, file) > 0)
        {
            if (parent != -1)
            {
                long newTree = rightRotation(pos_node, file);
                if (is_right)
                {
                    file.seekp(sizeof(long) + parent * sizeof(Record) + sizeof(Record) - 2 * sizeof(long), ios::beg);
                    file.write((char *)&newTree, sizeof(long));
                }
                else if (is_left)
                {
                    file.seekp(sizeof(long) + parent * sizeof(Record) + sizeof(Record) - 3 * sizeof(long), ios::beg);
                    file.write((char *)&newTree, sizeof(long));
                }
            }
            else
            {
                pos_root = rightRotation(pos_node, file);
                updateRoot(pos_root);
            }
        }
        if (getBalanceFactor(pos_node, file) >= 2 && getBalanceFactor(current.left, file) < 0)
        {
            long prevTree = leftRotation(current.left, file);
            current.left = prevTree;
            file.seekp(repos, ios::beg);
            file.write((char *)&current, sizeof(Record));

            long newTree = rightRotation(pos_node, file);
            if (parent != -1)
            {
                Record parentRecord;
                file.seekg(repos, ios::beg);
                file >> parentRecord;
                if (is_right)
                {
                    file.seekp(sizeof(long) + parent * sizeof(Record), ios::beg);
                    parentRecord.right = newTree;
                    file << parentRecord;
                }
                else if (is_left)
                {
                    file.seekp(sizeof(long) + parent * sizeof(Record), ios::beg);
                    parentRecord.left = newTree;
                    file << parentRecord;
                }
            }
            else
            {
                pos_root = newTree;
                updateRoot(pos_root);
                updateHeight(pos_root, file);
            }
        }
        if (getBalanceFactor(pos_node, file) <= -2 && getBalanceFactor(current.right, file) > 0)
        {
            long prevTree = rightRotation(current.right, file);
            current.right = prevTree;
            file.seekp(repos, ios::beg);
            file.write((char *)&current, sizeof(Record));

            long newTree = leftRotation(pos_node, file);

            if (parent != -1)
            {
                Record parentRecord;
                file.seekg(repos, ios::beg);
                file >> parentRecord;
                if (is_right)
                {
                    file.seekp(sizeof(long) + parent * sizeof(Record), ios::beg);
                    parentRecord.right = newTree;
                    file << parentRecord;
                }
                else if (is_left)
                {
                    file.seekp(sizeof(long) + parent * sizeof(Record), ios::beg);
                    parentRecord.left = newTree;
                    file << parentRecord;
                }
            }
            else
            {
                pos_root = newTree;
                updateRoot(pos_root);
                updateHeight(pos_root, file);
            }
        }
    }

    long rightRotation(long pos_node, fstream &file)
    {
        Record current, leftChild;
        long repos = sizeof(long) + pos_node * sizeof(Record);
        file.seekg(repos, ios::beg);
        file >> current;

        long left = current.left;
        file.seekg(repos, ios::beg);
        file >> leftChild;

        current.left = leftChild.right;
        leftChild.right = pos_node;

        current.height = 1 + max(height(current.left, file), height(current.right, file));
        leftChild.height = 1 + max(height(leftChild.left, file), height(leftChild.right, file));

        file.seekp(repos, ios::beg);
        file.write((char *)&current, sizeof(Record));
        file.seekp(repos + sizeof(Record) - sizeof(long), ios::beg);
        file.write((char *)&leftChild.right, sizeof(long));

        return left;
    }
    long leftRotation(long pos_node, fstream &file)
    {
        Record current, rightChild;
        long repos = sizeof(long) + pos_node * sizeof(Record);
        file.seekg(repos, ios::beg);
        file >> current;

        long right = current.right;
        file.seekg(repos, ios::beg);
        file >> rightChild;

        current.right = rightChild.left;
        rightChild.left = pos_node;

        current.height = 1 + max(height(current.left, file), height(current.right, file));
        rightChild.height = 1 + max(height(rightChild.left, file), height(rightChild.right, file));

        file.seekp(repos, ios::beg);
        file.write((char *)&current, sizeof(Record));
        file.seekp(repos, ios::beg);
        file.write((char *)&rightChild, sizeof(Record));

        return right;
    }

    void updateHeight(long pos_node, fstream &file)
    {
        long repos = sizeof(long) + pos_node * sizeof(Record);
        file.seekg(repos, ios::beg);
        Record current;
        file >> current;
        int new_height = 1 + max(height(current.left, file), height(current.right, file));
        file.seekp(repos + sizeof(Record) - sizeof(int), ios::beg);
        file.write((char *)&new_height, sizeof(int));
    }
    int height(long pos_node, fstream &file)
    {
        if (pos_node == -1)
        {
            return 0;
        }
        long repos = sizeof(long) + pos_node * sizeof(Record);
        file.seekg(repos, ios::beg);
        Record current;
        file >> current;
        return current.height;
    }
    int getBalanceFactor(long pos_node, fstream &file)
    {
        if (pos_node == -1)
        {
            return 0;
        }
        long repos = sizeof(long) + pos_node * sizeof(Record);
        file.seekg(repos, ios::beg);
        Record current;
        file >> current;
        return height(current.left, file) - height(current.right, file);
    }

    void inorder(long pos_node, fstream &file, vector<Record> &result)
    {
        if (pos_node == -1)
        {
            return;
        }
        long repos = sizeof(long) + pos_node * sizeof(Record);
        file.seekg(repos, ios::beg);
        Record current;
        file >> current;
        inorder(current.left, file, result);
        result.push_back(current);
        inorder(current.right, file, result);
    }
};

void writeFile(string filename)
{
    AVLFile::deleteFile(filename);
    AVLFile file(filename);

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
    AVLFile file(filename);

    cout << "--------- show all sorted data -----------\n";
    vector<Record> result = file.inorder();
    for (Record re : result)
    {
        re.showData();
    }
}

int main()
{
    string filename = "data.dat";

    writeFile(filename);
    readFile(filename);
    return 0;
}
