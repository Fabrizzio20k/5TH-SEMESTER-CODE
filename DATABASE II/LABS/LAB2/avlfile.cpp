#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include "record.h"

using namespace std;

class AVLFile
{
private:
	string filename;
	long pos_root;
	void updateRoot(long pos_root)
	{
		fstream file(filename, ios::in | ios::out | ios::binary);
		file.seekp(0);
		file.write((char *)&pos_root, sizeof(long));
		file.close();

		this->pos_root = pos_root;
	}
	void getAndSetRoot()
	{
		fstream file(filename, ios::in | ios::binary);
		file.read((char *)&pos_root, sizeof(long));
		file.close();
	}
	void readNode(long pos_node, Record &record)
	{
		fstream file(filename, ios::in | ios::binary);
		file.seekg(sizeof(long) + pos_node * sizeof(Record));
		file.read((char *)&record, sizeof(Record));
		file.close();
	}
	void writeNode(long pos_node, Record record)
	{
		fstream file(filename, ios::in | ios::out | ios::binary);
		file.seekp(sizeof(long) + pos_node * sizeof(Record));
		file.write((char *)&record, sizeof(Record));
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
			getAndSetRoot();
			cout << "File found, root: " << pos_root << endl;
		}
	}

	Record find(int key)
	{
		return find(pos_root, key);
	}

	void insert(Record record)
	{
		fstream file(filename, ios::in | ios::out | ios::binary);
		insert(pos_root, record, file);
		file.close();
	}

	vector<Record> inorder()
	{
		fstream file(filename, ios::in | ios::out | ios::binary);
		vector<Record> result;
		inorder(pos_root, result, file);
		return result;
	}

	vector<Record> readByLine()
	{
		vector<Record> result;
		Record record;
		fstream file(filename, ios::in | ios::binary);
		file.seekg(sizeof(long));
		while (file.read((char *)&record, sizeof(Record)))
		{
			result.push_back(record);
		}
		file.close();
		return result;
	}

private:
	Record find(long pos_node, int key)
	{
		if (pos_node == -1)
		{
			cout << "Record not found\n";
			return Record();
		}
		Record node;
		readNode(pos_node, node);
		if (key == node.cod)
		{
			return node;
		}
		if (key > node.cod)
		{
			return find(node.right, key);
		}
		if (key < node.cod)
		{
			return find(node.left, key);
		}
	}

	void inorder(long pos_node, vector<Record> &result, fstream &file)
	{
		if (pos_node == -1)
		{
			return;
		}
		Record record;
		file.seekg(sizeof(int) + pos_node * sizeof(Record), ios::beg);
		file.read((char *)(&record), sizeof(Record));
		inorder(record.left, result, file);
		result.push_back(record);
		inorder(record.right, result, file);
		return;
	}

	void insert(long pos_node, Record record, fstream &file)
	{
		if (pos_node == -1)
		{
			appendNode(record);
			updateRoot(0);
			return;
		}
		Record node;
		readNode(pos_node, node);
		if (record.cod > node.cod)
		{
			if (node.right == -1)
			{
				long indx = 0;
				file.seekg(0, ios::end);
				indx = file.tellp() / sizeof(Record);
				file.write((char *)(&record), sizeof(Record));

				file.seekg(sizeof(long) + pos_node * sizeof(Record) + sizeof(int) + sizeof(node.nombre) + sizeof(int) +
							   sizeof(long),
						   ios::beg);
				file.write((char *)(&indx), sizeof(long));
				return;
			}
			insert(node.right, record, file);
		}
		else if (record.cod < node.cod)
		{
			if (node.left == -1)
			{
				long indx = 0;
				file.seekg(0, ios::end);
				indx = file.tellg() / sizeof(Record);
				file.write((char *)(&record), sizeof(Record));

				file.seekg(sizeof(long) + pos_node * sizeof(Record) + sizeof(int) + sizeof(node.nombre) + sizeof(int), ios::beg);
				file.write((char *)(&indx), sizeof(long));
				return;
			}
			insert(node.left, record, file);
		}
		else
		{
			cout << "Record already exists\n";
		}

		updateHeight(pos_node);
		// balance(pos_node, file);
	}

	int height(long pos_node)
	{
		if (pos_node == -1)
		{
			return -1;
		}
		Record node;
		readNode(pos_node, node);
		return node.height;
	}

	void updateHeight(long pos_node)
	{
		// update the height of the node in a correct way
		if (pos_node == -1)
		{
			return;
		}
		Record node;
		readNode(pos_node, node);
		node.height = 1 + max(height(node.left), height(node.right));
		writeNode(pos_node, node);
		return;
	}
	int balancingFactor(long pos_node)
	{
		if (pos_node == -1)
		{
			return 0;
		}
		Record node;
		readNode(pos_node, node);
		return height(node.left) - height(node.right);
	}

	void balance(long pos_node, fstream &file)
	{
		int bf = balancingFactor(pos_node);
		Record node;
		readNode(pos_node, node);

		if (bf >= 2)
		{
			if (balancingFactor(node.left) <= 1)
				leftRota(node.left, file);
			rightRota(pos_node, file);
		}
		else if (bf <= -2)
		{
			if (balancingFactor(node.right) >= -1)
				rightRota(node.right, file);
			leftRota(pos_node, file);
		}
		return;
	}
	void leftRota(long &pos_node, fstream &file)
	{
		Record node;
		readNode(pos_node, node);
		long temp = node.right;
		Record right;
		readNode(node.right, right);
		node.right = right.left;
		right.left = pos_node;
		writeNode(pos_node, node);
		writeNode(temp, right);
		updateHeight(pos_node);
		updateHeight(temp);
		pos_node = temp;
		// update the root
		if (pos_root == pos_node)
		{
			updateRoot(temp);
		}
	}
	void rightRota(long &pos_node, fstream &file)
	{
		Record node;
		readNode(pos_node, node);
		long temp = node.left;
		Record left;
		readNode(node.left, left);
		node.left = left.right;
		left.right = pos_node;
		writeNode(pos_node, node);
		writeNode(temp, left);
		updateHeight(pos_node);
		updateHeight(temp);
		pos_node = temp;
		// update the root
		if (pos_root == pos_node)
		{
			updateRoot(temp);
		}
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
	/*
	Record record;
	for (int i = 0; i < 4; i++)
	{
		record.setData();
		file.insert(record);
	}
	*/
}

void readFile(string filename)
{
	cout << "--------- show all data -----------\n";
	AVLFile file(filename);
	vector<Record> records = file.readByLine();
	for (const auto &record : records)
	{
		record.showData();
	}

	cout << "--------- show all sorted data -----------\n";
	vector<Record> result = file.inorder();
	for (Record re : result)
	{
		re.showData();
	}
}

int main()
{
	writeFile("data.dat");
	readFile("data.dat");
	return 0;
}
