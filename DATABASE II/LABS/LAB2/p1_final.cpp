#include <iostream>
#include <vector>
#include <string>
#include <fstream>

using namespace std;

struct Record
{
	int cod;
	char nombre[12];
	int ciclo;

	long left = -1, right = -1;
	int height = 0;

	void setData()
	{
		cout << "Codigo:";
		cin >> cod;
		cout << "Nombre: ";
		cin >> nombre;
		cout << "Ciclo: ";
		cin >> ciclo;
	}

	void showData()
	{
		cout << "\nCodigo: " << cod;
		cout << "\nNombre: " << nombre;
		cout << "\nCiclo : " << ciclo << endl;
		cout << "Izq : " << left << endl;
		cout << "Der : " << right << endl;
		cout << "Altura : " << height << endl;
	}
};

class AVLFile
{
private:
	string filename;
	long pos_root;
	long pre_root;
public:
	AVLFile(string filename)
	{
		fstream file(filename, ios::binary | ios::in | ios::out);
		this->pos_root = -1;
		this->filename = filename;
		if (!file)
		{
			cout << "File not found, creating new file\n";
			ofstream file2(this->filename, ios::binary);
			file2.write((char *)(&pos_root), sizeof(long));
			file2.close();
		}
		else
		{
			file.read((char *)(&pos_root), sizeof(long));
		}
		file.close();
	}

	Record find(int key)
	{
		fstream file(this->filename, ios::binary | ios::in | ios::out);
		Record record = find(pos_root, key, file);
		file.close();
		return record;
	}

	void insert(Record record)
	{
		fstream file(this->filename, ios::binary | ios::in | ios::out);
		insert(-1, pos_root, record, file);
		file.close();
	}

	vector<Record> inorder()
	{
		fstream file(this->filename, ios::binary | ios::in | ios::out);
		vector<Record> result;
		inorder(pos_root, result, file);
		file.close();
		return result;
	}

	void printAll()
	{
		fstream file(this->filename, ios::binary | ios::in | ios::out);
		long pos = 0;
		Record record;
		// get the root
		file.seekg(0, ios::beg);
		file.read((char *)(&pos), sizeof(long));

		cout << "Root: " << pos << endl;

		int count = 0;
		while (file.read((char *)(&record), sizeof(Record)))
		{
			cout << count++ << ": " << record.cod << " | " << record.nombre << " | " << record.ciclo << " | " << record.left << " | " << record.right << " | " << record.height << endl;
		}
		file.close();
	}

	long getRoot(){
		return pos_root;
	}

private:
	Record find(long pos_node, int key, fstream &file)
	{
		if (pos_node == -1) return{};

		Record record;
		file.seekg(sizeof(long) + pos_node * sizeof(Record), ios::beg);
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

	void insert(long parent, long pos_node, Record record, fstream &file)
	{
		if (pos_node == -1)
		{
			this->pos_root = 0;
			file.seekp(0, ios::beg);
			file.write((char *)(&pos_root), sizeof(long));
			file.write((char *)(&record), sizeof(Record));
			return;
		}
		Record a;
		file.seekg(sizeof(long) + pos_node * sizeof(Record), ios::beg);
		file.read((char *)(&a), sizeof(Record));

		if (record.cod > a.cod)
		{
			if (a.right == -1)
			{
				int indx = 0;
				file.seekg(0, ios::end);
				indx = file.tellp() / sizeof(Record);
				file.write((char *)(&record), sizeof(Record));

				file.seekg(sizeof(int) + pos_node * sizeof(Record) + sizeof(int) + sizeof(a.nombre) + sizeof(int) + sizeof(long), ios::beg);
				file.write((char *)(&indx), sizeof(int));
			} else {
				insert(pos_node, a.right, record, file);
			}
		}
		else if (record.cod < a.cod)
		{
			if (a.left == -1)
			{
				int indx = 0;
				file.seekg(0, ios::end);
				indx = file.tellg() / sizeof(Record);
				file.write((char *)(&record), sizeof(Record));

				file.seekg(sizeof(int) + pos_node * sizeof(Record) + sizeof(int) + sizeof(a.nombre) + sizeof(int), ios::beg);
				file.write((char *)(&indx), sizeof(int));
			} else {
				insert(pos_node, a.left, record, file);
			}
		}

		balance(parent, pos_node, file);
	}

	vector<Record> inorder(long pos_node, vector<Record> &result, fstream &file)
	{
		if (pos_node == -1) return result;

		Record a;
		file.seekg(sizeof(long) + pos_node * sizeof(Record), ios::beg);
		file.read((char *)(&a), sizeof(Record));
		inorder(a.left, result, file);
		result.push_back(a);
		inorder(a.right, result, file);
		return result;
	}

	/* AVL Stuff */

	int height(long pos_node, fstream &file)
	{
		if (pos_node == -1) return -1;
		
		Record a;
		file.seekg(sizeof(long) + pos_node * sizeof(Record), ios::beg);
		file.read((char *)(&a), sizeof(Record));
		return a.height;
	}

	bool isBalanced(long pos_node, fstream &file)
	{
		if (pos_node == -1) return false;

		Record a;
		file.seekg(sizeof(long) + pos_node * sizeof(Record), ios::beg);
		file.read((char *)(&a), sizeof(Record));

		int left = height(a.left, file);
		int right = height(a.right, file);

		if (abs(left - right) > 1) return false;

		return isBalanced(a.left, file) && isBalanced(a.right, file);
	}

	int balancingFactor(long pos_node, fstream &file)
	{
		if (pos_node == -1) return 0;

		Record a;
		file.seekg(sizeof(long) + pos_node * sizeof(Record), ios::beg);
		file.read((char *)(&a), sizeof(Record));

		int h = height(a.left, file) - height(a.right, file);

		return h;
	}

	void updateHeight(long pos_node, fstream &file)
	{
		if (pos_node == -1) return;

		Record a;
		file.seekg(sizeof(long) + pos_node * sizeof(Record), ios::beg);
		file.read((char *)(&a), sizeof(Record));

		a.height = max(height(a.left, file), height(a.right, file)) + 1;

		file.seekp(sizeof(long) + pos_node * sizeof(Record), ios::beg);
		file.write((char *)(&a), sizeof(Record));
	}

	void LL(long parent, long pos_node, fstream &file, bool flag = false)
	{
		if (pos_node == -1) return;

		Record a;
		file.seekg(sizeof(long) + pos_node * sizeof(Record), ios::beg);
		file.read((char *)(&a), sizeof(Record));

		int n2 = a.left;

		if (pos_node == pos_root) {
			pos_root = a.left;
			file.seekp(0, ios::beg);
			file.write((char *) (&pos_root), sizeof(long));
		}

		Record b;
		file.seekg(sizeof(long) + n2 * sizeof(Record), ios::beg);
		file.read((char *)(&b), sizeof(Record));

		a.left = b.right;
		b.right = pos_node;

		a.height = max(height(a.left, file), height(a.right, file)) + 1;
		file.seekp(sizeof(long) + pos_node * sizeof(Record), ios::beg);
		file.write((char *)(&a), sizeof(Record));

		b.height = max(height(b.left, file), height(b.right, file)) + 1;
		file.seekp(sizeof(long) + n2 * sizeof(Record), ios::beg);
		file.write((char *)(&b), sizeof(Record));

		Record c;
		if (flag){
			parent++;
		}
		file.seekg(sizeof(long) + parent * sizeof(Record), ios::beg);
		file.read((char *)(&c), sizeof(Record));
		if(flag)
			c.right = n2;
		else
			c.left = n2;
		//c.left = n2;
		file.seekp(sizeof(long) + parent * sizeof(Record), ios::beg);
		file.write((char *)(&c), sizeof(Record));
	}

	void RR(long parent, long pos_node, fstream &file, bool flag = false)
	{
		if (pos_node == -1) return;
		Record a;
		file.seekg(sizeof(long) + pos_node * sizeof(Record), ios::beg);
		file.read((char *)(&a), sizeof(Record));

		int n2 = a.right;

		if (pos_node == pos_root) {
			pos_root = a.right;
			file.seekp(0, ios::beg);
			file.write((char *) (&pos_root), sizeof(long));
		}

		Record b;
		file.seekg(sizeof(long) + n2 * sizeof(Record), ios::beg);
		file.read((char *)(&b), sizeof(Record));

		a.right = b.left;
		b.left = pos_node;

		a.height = max(height(a.left, file), height(a.right, file)) + 1;
		file.seekp(sizeof(long) + pos_node * sizeof(Record), ios::beg);
		file.write((char *)(&a), sizeof(Record));

		b.height = max(height(b.left, file), height(b.right, file)) + 1;
		file.seekp(sizeof(long) + n2 * sizeof(Record), ios::beg);
		file.write((char *)(&b), sizeof(Record));

		Record c;
		if (flag)
			parent++;
		//parent++;
		file.seekg(sizeof(long) + parent * sizeof(Record), ios::beg);
		file.read((char *)(&c), sizeof(Record));

		if (flag){
			c.left = n2;
		}else{
			c.right = n2;
		}
		file.seekp(sizeof(long) + parent * sizeof(Record), ios::beg);
		file.write((char *)(&c), sizeof(Record));
	}

	// LR rotation
	void left_rotate(long parent, long pos_node, fstream &file)
	{
		if (pos_node == -1) return;

		Record a;
		file.seekg(sizeof(long) + pos_node * sizeof(Record), ios::beg);
		file.read((char *)(&a), sizeof(Record));

		if (balancingFactor(a.left, file) >= 0)
			LL(parent, pos_node, file);
		else {
			RR(parent, a.left, file, true);
			LL(parent, pos_node, file);
		}
	}

	void right_rotate(long parent, long pos_node, fstream &file)
	{
		if (pos_node == -1) return;

		Record a;
		file.seekg(sizeof(long) + pos_node * sizeof(Record), ios::beg);
		file.read((char *)(&a), sizeof(Record));

		if (balancingFactor(a.right, file) <= 0) 
			RR(parent, pos_node, file);
		else {
			LL(parent, a.right, file, true);
			RR(parent, pos_node, file);
		}
	}

	void balance(long parent, long pos_node, fstream &file)
	{
		if (pos_node == -1) return;

		int h = balancingFactor(pos_node, file);

		if (h > 1)
			left_rotate(parent, pos_node, file);
		else if (h < -1)
			right_rotate(parent, pos_node, file);
		else
			updateHeight(pos_node, file);
	}
};

void writeFile(string filename)
{
	// clean file
	ofstream file2(filename, ios::binary);
	file2.close();
	AVLFile file(filename);

	Record r0 = {1000, "Juan", 1};
	Record r1 = {1001, "Maria", 2};
	Record r2 = {1002, "Luis", 3};
	Record r3 = {1003, "Jose", 4};
	Record r4 = {1004, "Miguel", 5};
	Record r5 = {1005, "Fabricio", 5};
	Record r6 = {1006, "Carlos", 6};
	Record r7 = {1007, "Pedro", 7};
	Record r8 = {1008, "Jorge", 8};
	Record r9 = {1009, "Raul", 9};
	file.insert(r1);
	file.insert(r8);
	file.insert(r2);
	file.insert(r9);
	file.insert(r4);
	file.insert(r3);



//	Record r1 = {1001, "Juan", 1};
//	Record r2 = {1008, "Pedro", 5};
//	Record r3 = {1002, "Maria", 2};
//	Record r4 = {1004, "Jose", 4};
//	Record r5 = {1003, "Luis", 3};
//	Record r6 = {1009, "Carlos", 9};
//	file.insert(r1);
//	file.insert(r2);
//	file.insert(r3);
//	file.insert(r4);
//	file.insert(r5);
//	file.insert(r6);
}

void readFile(string filename)
{
	AVLFile file(filename);
	cout << "--------- show all sorted data -----------\n";
	file.printAll();
//	vector<Record> result = file.inorder();
//	for (Record re : result)
//	{
//		re.showData();
//	}
//
//	cout << "--------- find data -----------\n";
//	Record record = file.find(1002);
//	record.showData();
}

int main()
{
	writeFile("data.dat");
	readFile("data.dat");
	return 0;
}
