#include <iostream>
#include <string>
using namespace std;

struct Record
{
    int cod;
    char nombre[12];
    int ciclo;

    long left, right;

    int height;

    Record() : left(-1), right(-1), height(0) {}
    Record(int cod, string nombre, int ciclo) : cod(cod), ciclo(ciclo), left(-1), right(-1), height(0)
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
        cout << endl
             << "Left: " << left << " | " << "Right: " << right << " | " << "Height: " << height;
        cout << "\nCodigo: " << cod;
        cout << "\nNombre: " << nombre;
        cout << "\nCiclo : " << ciclo << endl;
    }
};