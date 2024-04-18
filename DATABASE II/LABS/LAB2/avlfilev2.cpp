#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cstring>

using namespace std;

struct Record
{
    int cod{};
    char nombre[12]{};
    int ciclo{};

    long left{}, right{};
    Record() = default;
    Record(int cod, char *nombre, int ciclo)
    {
        this->cod = cod;
        strcpy(this->nombre, nombre);
        this->ciclo = ciclo;
        left = -1;
        right = -1;
    }

    __attribute__((unused)) void setData()
    {
        cout << "Codigo:";
        cin >> cod;
        cout << "Nombre: ";
        cin >> nombre;
        cout << "Ciclo: ";
        cin >> ciclo;
        left = -1;
        right = -1;
    }

    void showData()
    {
        cout << "\nCodigo: " << cod;
        cout << "\nNombre: " << nombre;
        cout << "\nCiclo : " << ciclo;
        cout << "\nIzq : " << left;
        cout << "\nDer : " << right << endl;
    }
};

class AVLFile
{
private:
    string filename;
    long pos_root;

public:
    explicit AVLFile(const string &filename)
    {
        fstream file(filename, ios::binary | ios::in | ios::out);
        this->pos_root = -1;
        this->filename = filename;
        if (!file)
        {
            cout << "No existe el archivo... creandolo....." << endl;
            ofstream file2(this->filename, ios::binary);
            file2.write((char *)(&pos_root), sizeof(int));
            file2.close();
        }
        else
        {
            file.read((char *)(&pos_root), sizeof(int));
        }
        file.close();
    }

    Record find(int key)
    {
        fstream file(filename, ios::in | ios::app | ios::binary);
        Record record = find(pos_root, key, file);
        file.close();
        return record;
    }

    void insert(Record record)
    {
        fstream file(this->filename, ios::binary | ios::in | ios::out);
        insert(pos_root, record, file);
        file.close();
    }

    vector<Record> inorder()
    {
        vector<Record> result;
        fstream file(this->filename, ios::binary | ios::in | ios::out);
        inorder(pos_root, result, file);
        file.close();
        return result;
    }

private:
    Record find(long pos_node, int key, fstream &file)
    {
        if (pos_node == -1)
        {
            return {};
        }
        Record record;
        file.seekg(sizeof(int) + pos_node * sizeof(Record), ios::beg);
        file.read((char *)&record, sizeof(Record));
        if (key < record.cod)
        {
            return find(record.left, key, file);
        }
        else if (record.cod < key)
        {
            return find(record.right, key, file);
        }
        else
        {
            return record;
        }
    }

    void insert(long pos_node, Record record, fstream &filen)
    {
        if (pos_node == -1)
        {
            this->pos_root = 0;
            filen.seekg(0, ios::beg);
            filen.write((char *)(&pos_root), sizeof(int));
            filen.write((char *)(&record), sizeof(Record));
            return;
        }
        Record a;
        filen.seekg(sizeof(int) + pos_node * sizeof(Record), ios::beg);
        filen.read((char *)(&a), sizeof(Record));
        if (record.cod > a.cod)
        {
            if (a.right == -1)
            {
                int indx = 0;
                filen.seekg(0, ios::end);
                indx = filen.tellp() / sizeof(Record);
                filen.write((char *)(&record), sizeof(Record));

                filen.seekg(sizeof(int) + pos_node * sizeof(Record) + sizeof(int) + sizeof(a.nombre) + sizeof(int) +
                                sizeof(long),
                            ios::beg);
                filen.write((char *)(&indx), sizeof(int));
                return;
            }
            insert(a.right, record, filen);
        }
        else if (record.cod < a.cod)
        {
            if (a.left == -1)
            {
                int indx = 0;
                filen.seekg(0, ios::end);
                indx = filen.tellg() / sizeof(Record);
                filen.write((char *)(&record), sizeof(Record));

                filen.seekg(sizeof(int) + pos_node * sizeof(Record) + sizeof(int) + sizeof(a.nombre) + sizeof(int), ios::beg);
                filen.write((char *)(&indx), sizeof(int));
                return;
            }
            insert(a.left, record, filen);
        }
    }

    vector<Record> inorder(long pos_node, vector<Record> &result, fstream &filen)
    {
        if (pos_node == -1)
        {
            return result;
        }
        Record a;
        filen.seekg(sizeof(int) + pos_node * sizeof(Record), ios::beg);
        filen.read((char *)(&a), sizeof(Record));
        inorder(a.left, result, filen);
        result.push_back(a);
        inorder(a.right, result, filen);
        return result;
    }
};

void writeFile(const string &filename)
{
    AVLFile file(filename);
    /*Record record;
    for (int i = 0; i < 4; i++){
        record.setData();
        file.insert(record);
    }*/
    Record recorda(160, "Rodrigo", 5);
    file.insert(recorda);
    Record recordb(115, "Pepe", 7);
    file.insert(recordb);
    Record recordc(114, "Juan", 4);
    file.insert(recordc);
    Record recordd(101, "Ivan", 1);
    file.insert(recordd);
    Record recorde(105, "Raul", 1);
    file.insert(recorde);
    Record recordf(170, "Boto", 1);
    file.insert(recordf);
    Record recordg(180, "Edu", 1);
    file.insert(recordg);
    Record recordh(190, "Eguz", 1);
    file.insert(recordh);
    Record recordi(185, "Cavez", 1);
    file.insert(recordi);
    Record recordj(191, "Fabrizio", 1);
    file.insert(recordj);
}

void readFile(const string &filename)
{
    AVLFile file(filename);
    cout << "--------- show all sorted data -----------\n";
    vector<Record> result = file.inorder();
    cout << "Tamanio: " << result.size() << endl;
    for (Record re : result)
    {
        re.showData();
    }
}

void testFind(const string &filename)
{
    AVLFile file(filename);
    Record a = file.find(191);
    a.showData();
    a = file.find(185);
    a.showData();
    a = file.find(105);
    a.showData();
    a = file.find(101);
    a.showData();
}

int main()
{
    // writeFile("data1.dat");
    readFile("data1.dat");
    // testFind("data1.dat");
    return 0;
}
