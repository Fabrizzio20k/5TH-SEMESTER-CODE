#include <iostream>
#include <fstream>
using namespace std;

struct Alumno
{
    char nombre[20];
    char carrera[10];
};

ostream &operator<<(ostream &stream, Alumno &record)
{
    stream.write(record.nombre, 20);
    stream.write(record.carrera, 10);
    stream << "\n";
    stream << flush;
    return stream;
}

istream &operator>>(istream &stream, Alumno &record)
{
    stream.read(record.nombre, 20);
    stream.read(record.carrera, 10);
    stream.get();
    return stream;
}

void testWrite()
{
    Alumno al1{"Pedro", "CS"};
    Alumno al2{"Marta", "IND"};
    Alumno al3{"Santos", "MEC"};
    Alumno al4{"Javier", "CD"};
    Alumno al5{"Elena", "CS"};
    ofstream ofile("datos.txt", ios::binary | ios::out);

    if (!ofile.is_open())
    {
        cerr << "Error al abrir el archivo" << endl;
        exit(1);
    }

    ofile << al1;
    ofile << al2;
    ofile << al3;
    ofile << al4;
    ofile << al5;
    ofile.close();
}

void testRead()
{
    ifstream ifile("datos.txt", ios::binary);
    int i = 1;
    if (!ifile.is_open())
    {
        cerr << "Error al abrir el archivo" << endl;
        exit(1);
    }

    Alumno al;
    while (ifile >> al)
    {
        cout << i << ": " << al.nombre << "-" << al.carrera << endl;
        i++;
    }
    ifile.close();
}

void testRead(int i)
{
    i--;
    ifstream ifile("datos.txt", ios::binary);
    if (!ifile.is_open())
    {
        cerr << "Error al abrir el archivo" << endl;
        exit(1);
    }

    ifile.seekg(i * (sizeof(Alumno) + 1), ios::beg);

    Alumno al;
    ifile >> al;
    cout << i + 1 << ": " << al.nombre << "-" << al.carrera << endl;

    ifile.close();
}

int main()
{
    testWrite();
    cout << "Lectura de todos los registros" << endl;
    testRead();
    cout << "Lectura de un registro" << endl;
    testRead(5);
    return 0;
}