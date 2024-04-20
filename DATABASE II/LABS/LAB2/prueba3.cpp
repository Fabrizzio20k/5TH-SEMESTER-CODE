#include <iostream>
#include <fstream>
#include <cstdio>
#include <map>
#include <vector>
#include <iomanip>
#include <cstring>
#include <string>
#define ll long long
using namespace std;
struct Record
{
    ll cod{};
    char nombre[16]{};
    int ciclo{};

    ll left{}, right{}, height{};
    Record() = default;
    Record(int cod, const char *nombre, int ciclo) : cod(cod), ciclo(ciclo), left(-1), right(-1), height(1)
    {
        strcpy(this->nombre, nombre);
    }

    void Display()
    {
        cout << "CODIGO : " << cod << setw(5) << "\tNOMBRE : " << nombre << setw(5) << "\tLEFT   : " << 1 + left << setw(5) << "\tRIGHT  : " << 1 + right << setw(5) << "\tHEIGHT   : " << height << endl;
    }
    friend fstream &operator>>(fstream &file, Record &record1);
    void Display_uwu_mode(const string &filename)
    {
        fstream display(filename, ios::in | ios::binary);

        ll code_left = -1;
        ll code_right = -1;

        if (left != -1)
        {
            display.seekg(sizeof(ll) + sizeof(Record) * left, ios::beg);
            Record r;
            if (display >> r)
            {
                code_left = r.cod;
            }
            else
            {
                cerr << "Failed to read left child record at position: " << left << endl;
            }
        }

        if (right != -1)
        {
            display.seekg(sizeof(ll) + sizeof(Record) * right, ios::beg);
            Record r2;
            if (display >> r2)
            {
                code_right = r2.cod;
            }
            else
            {
                cerr << "Failed to read right child record at position: " << right << endl;
            }
        }

        cout << "CODIGO : " << cod << setw(10) << "\tNOMBRE : " << nombre << setw(10)
             << "\t LINK LEFT CODE:" << setw(5) << code_left << "\t LINK RIGHT CODE:" << setw(5) << code_right << endl;
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
class MY_AVL_FILE
{

private:
    string file_name;
    ll pos_root;
    ll root_metadata_size = sizeof(ll);
    ll record_size = sizeof(Record);
    ll pointer_size = sizeof(ll);

public:
    MY_AVL_FILE(string file_name) : file_name(file_name), pos_root(-1)
    {
        fstream file(file_name, ios::binary | ios::in | ios::out); // Open for reading and writing
        if (!file.is_open())
        {
            // File does not exist, so create it
            file.open(file_name, ios::binary | ios::out);
            write_root(file, -1); // Write initial root position
            file.close();
        }
        else
        {
            // File exists, read root
            file.seekg(0, ios::beg);
            if (file.read((char *)&pos_root, sizeof(pos_root)))
            {
                cout << "Root read successfully: " << pos_root << endl;
            }
            else
            {
                // File was empty or read failed, initialize root
                write_root(file, -1);
            }
            file.close();
        }
    }
    Record find(TK key)
    {
        fstream explorer_file(file_name, ios::in | ios::binary);

        Record r = find(key, explorer_file, pos_root);
        explorer_file.close();
    }
    void insert(Record record)
    {
        fstream insert_file(file_name, ios::in | ios::out | ios::binary);
        insert_file.seekp(0, ios::end);
        cout << insert_file.tellp() << "--> before insert ..." << endl;
        cout << "__________________________ANOTHER INSERT RECURSION_________________________________" << endl;
        cout << endl;
        insert_file.seekg(0, ios::beg);
        insert_file.read((char *)&pos_root, root_metadata_size);
        insert(record, pos_root, -1, insert_file, false, false);
        cout << endl;
        cout << "_________________________________ANOTHER INSERT RECURSION END_________________________________" << endl;
        insert_file.close();
    }
    vector<Record> inorder()
    {
        fstream file_explorer(file_name, ios::in | ios::out | ios::binary);
        vector<Record> ram_buffer;
        file_explorer.seekg(0, ios::beg);
        file_explorer.read((char *)&pos_root, root_metadata_size);
        cout << "SENDING FROM :\t" << pos_root << endl;
        inorder(pos_root, file_explorer, ram_buffer);
        file_explorer.close();

        return ram_buffer;
    }

private:
    void write_root(fstream &file, ll index_root)
    {
        file.seekp(0, ios::beg);
        file.write((char *)&index_root, pointer_size);
        file.flush(); // Make sure to flush to ensure it's written
    }
    ll read_root(fstream &filename)
    {
        filename.seekg(0, ios::beg);
        ll index_root;
        filename.read((char *)&index_root, sizeof(ll));
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

    void inorder(int current_pos, fstream &file_explorer, vector<Record> &ram_buffer)
    {
        if (current_pos == -1)
            return;

        file_explorer.seekg(root_metadata_size + current_pos * record_size);
        Record r;
        file_explorer >> r;
        inorder(r.left, file_explorer, ram_buffer);
        ram_buffer.push_back(r);
        inorder(r.right, file_explorer, ram_buffer);
    }
    Record find(TK key, fstream &explorer_file, ll current_pos)
    {
        explorer_file.seekg(root_metadata_size + current_pos * record_size);
        Record record;
        explorer_file >> record;
        if (key > record.cod)
            find(key, explorer_file, record.right);
        else if (key < record.right)
            find(key, explorer_file, record.left);
        else
            return record;
    }

    ll get_height(ll current_pos, fstream &file)
    {
        if (current_pos == -1)
            return 0;

        ll positioning = root_metadata_size + current_pos * record_size;
        file.seekg(positioning, ios::beg);
        Record r;
        if (!(file >> r))
        {
            cerr << "Failed to read record for height at position: " << current_pos << endl;
            return -1;
        }
        return r.height;
    }

    ll get_balance(ll current_pos, fstream &file)
    {
        if (current_pos == -1)
            return 0;

        file.seekg(root_metadata_size + current_pos * record_size, ios::beg);
        Record r;
        if (!(file >> r))
        {
            cerr << "Error reding record at position: " << current_pos << endl;
            return -1;
        }
        return get_height(r.left, file) - get_height(r.right, file);
    }

    void update_height(ll current_pos, fstream &file)
    {
        ll positioning = root_metadata_size + current_pos * record_size;
        file.seekg(positioning, ios::beg);
        Record r;
        if (!(file >> r))
        {
            cerr << "Error reading record at position: " << current_pos << endl;
            return;
        }

        ll new_height = 1 + max(get_height(r.left, file), get_height(r.right, file));
        if (new_height == 0)
        {
            cerr << "Height calculation error, aborting update." << endl;
            return;
        }

        file.seekp(positioning + record_size - pointer_size); // Update height location
        if (!file.write((char *)&new_height, sizeof(ll)))
        {
            cerr << "Failed to write new height at position: " << current_pos << endl;
        }
    }

    ll mini_insert(fstream &file, Record record)
    {
        file.seekp(0, ios::end);
        ll new_record_pos = (file.tellp() - root_metadata_size) / record_size;
        if (!file.write((char *)&record, sizeof(Record)))
        {
            cerr << "Failed to insert record" << endl;
            return -1; // Indicate failure
        }
        return new_record_pos;
    }
    void insert(Record record, ll current_pos, ll parent_pos, fstream &file, bool from_left, bool from_right)
    {
        if (current_pos == -1)
        {
            file.seekp(0, ios::end); // Assuming the root is at the end initially
            if (!file.write((char *)&record, sizeof(Record)))
            {
                cerr << "Failed to write root record" << endl;
                return;
            }
            pos_root = 0; // Should be calculated based on file position
            write_root(file, pos_root);
            return;
        }

        ll positioning = root_metadata_size + current_pos * record_size;
        cout << "CURRENT POS " << current_pos << endl;
        cout << "POSITIONING " << positioning << endl;
        file.seekg(positioning, ios::beg);
        Record current_record;
        if (!(file >> current_record))
        {
            cerr << "Failed to read current record at position: " << current_pos << endl;
            return;
        }

        if (record.cod > current_record.cod)
        {
            if (current_record.right == -1)
            {
                ll right_pointer = mini_insert(file, record);
                if (right_pointer == -1)
                    return;
                file.seekp(positioning + record_size - 2 * pointer_size, ios::beg);
                file.write((char *)&right_pointer, pointer_size);
            }
            else
            {
                insert(record, current_record.right, current_pos, file, false, true);
            }
        }
        else if (record.cod < current_record.cod)
        {
            if (current_record.left == -1)
            {
                ll left_pointer = mini_insert(file, record);
                if (left_pointer == -1)
                    return;
                file.seekp(positioning + record_size - 3 * pointer_size, ios::beg);
                file.write((char *)&left_pointer, pointer_size);
            }
            else
            {
                insert(record, current_record.left, current_pos, file, true, false);
            }
        }
        // Recursion upwards stream
        update_height(current_pos, file); // Update height after insertion

        cout << "NOMBRE : " << current_record.nombre << endl;
        cout << "BALANCE FACTOR ..." << get_balance(current_pos, file) << endl;

        ll balance_factor = get_balance(current_pos, file);
        // left rotation
        if (balance_factor <= -2 and get_balance(current_record.right, file) < 0)
        {
            cout << "LEFT ROTATION START ..." << endl;
            if (parent_pos != -1)
            {
                auto new_sub_tree_root_pos = left_rotation(current_pos, file);

                if (from_right)
                {

                    file.seekp(root_metadata_size + parent_pos * record_size + record_size - 2 * pointer_size);
                    file.write((char *)&new_sub_tree_root_pos, pointer_size);
                }
                else if (from_left)
                {
                    file.seekp(root_metadata_size + parent_pos * record_size + record_size - 3 * pointer_size);
                    file.write((char *)&new_sub_tree_root_pos, pointer_size);
                }
            }
            else
            {

                pos_root = left_rotation(current_pos, file);
                cout << "MY NEW POS_ROOT:\t" << pos_root << endl;
                write_root(file, pos_root);
            }
        }
        // right rotation
        if (balance_factor >= 2 and get_balance(current_record.left, file) > 0)
        {
            cout << "RIGHT ROTATION START ..." << endl;
            if (parent_pos != -1)
            {
                auto new_sub_tree_root_pos = right_rotation(current_pos, file);
                cout << "MY NEW RIGHT POS " << new_sub_tree_root_pos << endl;
                if (from_right)
                {

                    file.seekp(root_metadata_size + parent_pos * record_size + record_size - 2 * pointer_size);
                    file.write((char *)&new_sub_tree_root_pos, pointer_size);
                }
                else if (from_left)
                {
                    file.seekp(root_metadata_size + parent_pos * record_size + record_size - 3 * pointer_size);
                    file.write((char *)&new_sub_tree_root_pos, pointer_size);
                }
            }
            else
            {
                pos_root = right_rotation(current_pos, file);
                cout << "MY NEW POS_ROOT:\t" << pos_root << endl;
                write_root(file, pos_root);
            }
        }
        // left-right rotation
        if (balance_factor >= 2 and get_balance(current_record.left, file) < 0)
        {
            cout << "L-R ROTATION RUNNING ..." << endl;
            ll partial_sub_tree_root_pos = left_rotation(current_record.left, file);
            cout << "PARTIAL SUBTREE NODE ROOT POS\t" << partial_sub_tree_root_pos << endl;
            current_record.left = partial_sub_tree_root_pos;
            file.seekp(root_metadata_size + current_pos * record_size, ios::beg);
            file << current_record;

            ll new_sub_tree_root_pos = right_rotation(current_pos, file);

            if (parent_pos != -1)
            {
                Record parent_node;
                file.seekg(root_metadata_size + parent_pos * record_size, ios::beg);
                file >> parent_node;

                if (from_right)
                {

                    file.seekp(root_metadata_size + parent_pos * record_size, ios::beg);
                    parent_node.right = new_sub_tree_root_pos;
                    file << parent_node;
                }
                else if (from_left)
                {
                    file.seekp(root_metadata_size + parent_pos * record_size, ios::beg);
                    parent_node.left = new_sub_tree_root_pos;
                    file << parent_node;
                }
            }
            else
            {
                pos_root = new_sub_tree_root_pos;
                write_root(file, pos_root);
                update_height(pos_root, file);
            }
        }
        // right-left rotation
        if (balance_factor <= -2 and get_balance(current_record.right, file) > 0)
        {
            //            cout<<"R-L ROTATION RUNNING ..."<<endl;
            ll partial_sub_tree_root_pos = right_rotation(current_record.right, file);
            //            cout<<"PARTIAL SUBTREE NODE ROOT POS\t"<<partial_sub_tree_root_pos<<endl;
            current_record.right = partial_sub_tree_root_pos;
            file.seekp(root_metadata_size + current_pos * record_size, ios::beg);
            file << current_record;

            ll new_sub_tree_root_pos = left_rotation(current_pos, file);

            if (parent_pos != -1)
            {
                Record parent_node;
                file.seekg(root_metadata_size + parent_pos * record_size, ios::beg);
                file >> parent_node;

                if (from_right)
                {

                    file.seekp(root_metadata_size + parent_pos * record_size, ios::beg);
                    parent_node.right = new_sub_tree_root_pos;
                    file << parent_node;
                }
                else if (from_left)
                {
                    file.seekp(root_metadata_size + parent_pos * record_size, ios::beg);
                    parent_node.left = new_sub_tree_root_pos;
                    file << parent_node;
                }
            }
            else
            {
                pos_root = new_sub_tree_root_pos;
                write_root(file, pos_root);
                update_height(pos_root, file);
            }
        }
    }

    ll right_rotation(ll current_node_pos, fstream &file)
    {

        Record current_node, left_node;
        file.seekg(root_metadata_size + current_node_pos * record_size, ios::beg);
        file.read((char *)&current_node, sizeof(Record));

        ll left_node_pos = current_node.left;
        if (left_node_pos == -1)
        {
            cerr << "Right rotation failed: no left child." << endl;
            return -1;
        }

        file.seekg(root_metadata_size + left_node_pos * record_size, ios::beg);
        file.read((char *)&left_node, sizeof(Record));

        current_node.left = left_node.right;
        left_node.right = current_node_pos;

        current_node.height = 1 + max(get_height(current_node.left, file), get_height(current_node.right, file));
        left_node.height = 1 + max(get_height(left_node.left, file), get_height(current_node_pos, file));

        file.seekp(root_metadata_size + current_node_pos * record_size, ios::beg);
        file << current_node;
        file.seekp(root_metadata_size + left_node_pos * record_size, ios::beg);
        file << left_node;

        return left_node_pos; // El nuevo root de este subárbol
    }
    ll left_rotation(ll current_node_pos, fstream &file)
    {
        Record current_node, right_node;

        file.seekg(root_metadata_size + current_node_pos * record_size, ios::beg);
        if (!(file >> current_node))
        {
            cerr << "Error record cant be read at position..." << current_node_pos << endl;
        }
        ll right_node_pos = current_node.right; // right node here

        if (right_node_pos == -1)
        {
            cerr << "Left rotation failed: no right child." << endl;
            return -1; // Error, no right child to rotate
        }

        // Read
        file.seekg(root_metadata_size + right_node_pos * record_size, ios::beg);
        if (!(file >> right_node))
        {
            cerr << "Error record cant be read at position..." << right_node_pos << endl;
        }

        ll left_pos_of_right = right_node.left; // left pointer of the right node

        current_node.right = right_node.left;
        right_node.left = current_node_pos;

        // Heights
        current_node.height = 1 + max(get_height(current_node.left, file), get_height(current_node.right, file));
        right_node.height = 1 + max(get_height(current_node_pos, file), get_height(right_node.right, file));

        file.seekp(root_metadata_size + current_node_pos * record_size, ios::beg);
        file << current_node;
        file.seekp(root_metadata_size + right_node_pos * record_size, ios::beg);
        file << right_node;
        return right_node_pos;
    }
};

void TEST_ADD(const string &filename)
{
    MY_AVL_FILE<ll> file(filename);

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

    auto printer = file.inorder();

    for (auto val : printer)
        val.Display_uwu_mode(filename);
}

int main()
{

    TEST_ADD("test4.bin");
}
