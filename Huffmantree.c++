#include <iostream>
#include <fstream>
#define INTERNAL_NODE_CHARACTER char(188)
#define PSEUDO_EOF char(129)
#define CHARACTER_CODE_SEPERATOR char(189)
#define HEADER_ENTRY_SEPERATOR char(190)
#define HEADER_TEXT_SEPERATOR char(191)
#include <unordered_map>
#include <queue>
#include <unistd.h>
#include <windows.h>
#include <algorithm>


void entry();

class tree
{
public:
    char ch;
    int freq;
    tree *right;
    tree *left;
    tree() {}
    tree(char ch, tree *left = NULL, tree *right = NULL)
    {
        this->ch = ch;
        this->left = left;
        this->right = right;
    }
    tree(char ch, int freq, tree *left = NULL, tree *right = NULL)
    {
        this->ch = ch;
        this->freq = freq;
        this->left = left;
        this->right = right;
    }
};
void freqcount(char text, std::unordered_map<char, int> &counter) // assign frequency of occurance to characters
{

    counter[text]++;
}
int calctotal(std::unordered_map<char, int> counter)
{

    int total = 0;
    for (auto i : counter)
    {
        total += i.second;
    }
    return total;
}
void prcount(std::vector<std::pair<char, float>> &p, std::unordered_map<char, int> counter)
{
    char first;
    float second;
    int total = 0;
    for (auto i : counter)
    {
        total += i.second;
    }
    for (auto i : counter)
    {
        first = i.first;
        second = ((float)i.second / (float)total);
        p.push_back(std::make_pair(first, second));
    }
}

int findbreak(std::vector<std::pair<char, float>> A, int start, int end)
{
    float sum = 0;
    for (int i = start; i <= end; i++)
    {
        sum += A[i].second;
    }

    float half = sum / 2;
    float s = 0;
    for (int j = start; j < end; j++)
    {
        s += A[j].second;

        if (s >= half)
        {
            if (j == start || s == half)
            {
                return j;
            }
            float prevs = s - A[j].second;
            if ((half - prevs) < (s - half))
                return j - 1;
            return j;
        }
    }

    return start;
}
bool cmp(std::pair<char, float> &a, std::pair<char, float> &b)
{
    return a.second > b.second;
}
void assignsf(std::vector<std::pair<char, std::string>> &v, std::vector<std::pair<char, float>> p, int start, int end)
{
    int size = end - start + 1;
    char ch;
    if (size <= 1)
    {
        return;
    }

    int j = findbreak(p, start, end);
    for (int k = start; k < (j + 1); k++)
    {

        v[k].second += "0";
    }
    for (int k = j + 1; k <= end; k++)
    {

        v[k].second += "1";
    }

    assignsf(v, p, start, j);
    assignsf(v, p, j + 1, end);

    return;
}
void assign(tree *root, std::string value, std::unordered_map<char, std::string> &bitcode);
class comparenode // for making of minimum priority queue
{
public:
    bool operator()(tree *l, tree *r)

    {
        return (l->freq > r->freq);
    }
};

tree *buildtree(std::unordered_map<char, int> &counter, std::priority_queue<tree *, std::vector<tree *>, comparenode> &minheap) // to build tree
{
    for (auto i : counter)
    {
        tree *t = new tree(i.first, i.second);
        minheap.push(t);
    }

    while (minheap.size() != 1)
    {
        tree *smallleft = minheap.top();
        minheap.pop();
        tree *smallright = minheap.top();
        minheap.pop();
        int parentfreq = smallleft->freq + smallright->freq;
        tree *parent = new tree(INTERNAL_NODE_CHARACTER, parentfreq, smallleft, smallright);
        minheap.push(parent);
    }

    return minheap.top();
}
void encodesf(std::string filename, std::string encodefile, std::unordered_map<char, int> &counter, std::unordered_map<char, std::string> &bitcode, std::vector<std::pair<char, float>> &p)
{
    int esize = 0;
    std::ofstream fout;
    fout.open(encodefile, std::ios::app | std::ios::binary);
    std::ifstream fin;
    fin.open(filename);
    char c;
    while (fin.get(c))
    {
        freqcount(c, counter);
    }
    counter[PSEUDO_EOF] = 1;
    // int total = calctotal(counter);
    prcount(p, counter);
    sort(p.begin(), p.end(), cmp);
    int sizep = p.size();
    std::vector<std::pair<char, std::string>> v;
    for (int i = 0; i < sizep; i++)
    {
        v.push_back(std::make_pair(p[i].first, ""));
    }
    assignsf(v, p, 0, sizep - 1);
    for (auto i : v)
    {

        bitcode[i.first] = i.second;
    }
    // for (auto i : bitcode)
    // {
    //     cout << i.first << ": " << i.second << "\n";
    // }
    fin.close();

    fin.open(filename);
    char cr;
    std::string code;

    for (auto i : bitcode)
    {
        cr = i.first;
        code = i.second;
        esize += 3 + code.length();
        fout << cr << CHARACTER_CODE_SEPERATOR << code << HEADER_ENTRY_SEPERATOR;
    }
    fout << HEADER_TEXT_SEPERATOR;
    esize++;
    int bufcount = 0;
    char buff = '\0';

    while (fin.get(c))
    {

        std::string temp = bitcode[c];
        int lengthi = temp.size();
        for (int i = 0; i < lengthi; i++)
        {
            if (temp[i] == '1')
            {
                buff |= 1 << (7 - bufcount);
            }
            bufcount++;
            if (bufcount == 8)
            {
                esize++;
                fout << buff;
                bufcount = 0;
                buff = '\0';
            }
        }
    }
    std::string temp = bitcode[PSEUDO_EOF];
    int lengthi = temp.size();
    for (int i = 0; i < lengthi; i++)
    {
        if (temp[i] == '1')
        {
            buff |= 1 << (7 - bufcount);
        }
        bufcount++;
        if (bufcount == 8)
        {
            esize++;
            fout << buff;
            bufcount = 0;
            buff = '\0';
        }
    }
    if (buff != '\0')
    {
        esize++;
        fout << buff;
    }
    int osize = calctotal(counter) - 1;
    std::cout << "\n\t\toriginal file size : " << osize << " \n\t\tcompressed file size : " << esize << " \n\t\tratio: " << (float)esize / (float)osize << "\n";
    fin.close();
    fout.flush();
    fout.close();
}
tree *encode(std::string filename, std::string encodefile, std::unordered_map<char, int> &counter, std::priority_queue<tree *, std::vector<tree *>, comparenode> &minheap, std::unordered_map<char, std::string> &bitcode)
{
    int esize = 0;
    std::ofstream fout;
    fout.open(encodefile, std::ios::app | std::ios::binary);
    std::ifstream fin;
    fin.open(filename);
    char c;
    while (fin.get(c))
    {
        freqcount(c, counter);
    }
    counter[PSEUDO_EOF] = 1;

    tree *root = buildtree(counter, minheap);
    assign(root, "", bitcode);
    fin.close();

    fin.open(filename);
    char cr;
    std::string code;

    for (auto i : bitcode)
    {
        cr = i.first;
        code = i.second;
        fout << cr << CHARACTER_CODE_SEPERATOR << code << HEADER_ENTRY_SEPERATOR;
        esize = 3 + code.length();
    }
    fout << HEADER_TEXT_SEPERATOR;
    esize++;

    int bufcount = 0;
    char buff = '\0';

    while (fin.get(c))
    {

        std::string temp = bitcode[c];
        int lengthi = temp.size();
        for (int i = 0; i < lengthi; i++)
        {
            if (temp[i] == '1')
            {
                buff |= 1 << (7 - bufcount);
            }
            bufcount++;
            if (bufcount == 8)
            {

                fout << buff;
                esize++;
                bufcount = 0;
                buff = '\0';
            }
        }
    }
    std::string temp = bitcode[PSEUDO_EOF];
    int lengthi = temp.size();
    for (int i = 0; i < lengthi; i++)
    {
        if (temp[i] == '1')
        {
            buff |= 1 << (7 - bufcount);
        }
        bufcount++;
        if (bufcount == 8)
        {
            fout << buff;
            esize++;
            bufcount = 0;
            buff = '\0';
        }
    }
    if (buff != '\0')
    {
        fout << buff;
        esize++;
    }
    int osize = calctotal(counter) - 1;
    std::cout << "\n\t\toriginal file size : " << osize << " \n\t\tcompressed file size : " << esize << " \n\t\tratio: " << (float)esize / (float)osize << "\n";
    fin.close();
    fout.flush();
    fout.close();
    return root;
}
void decode(std::string encodefilename, std::string decodefilename, std::unordered_map<char, std::string> &bitcode, std::unordered_map<std::string, char> &charac)
{
    std::ofstream fout;
    std::ifstream fin;
    fout.open(decodefilename, std::ios::app);
    fin.open(encodefilename, std::ios::app | std::ios::binary);

    char cr;
    char key;
    std::string code;
    cr = fin.get();
    key = cr;

    while (cr != HEADER_TEXT_SEPERATOR)
    {
        if (cr == CHARACTER_CODE_SEPERATOR)
        {
            fin.get(cr);
            while (cr != HEADER_ENTRY_SEPERATOR)
            {
                bitcode[key] += cr;
                fin.get(cr);
            }
        }
        key = cr;
        fin.get(cr);
    }

    for (auto i : bitcode)
    {
        charac[i.second] = i.first;
    }

    std::string br = "";
    char ct;

    while (!fin.eof() && fin.get(ct))
    {
        for (int i = 0; i < 8; i++)
        {
            if ((ct >> (7 - i)) & 1)
            {
                br += "1";
            }
            else
            {
                br += "0";
            }
            if (charac.count(br))
            {
                if (charac[br] == PSEUDO_EOF)
                {
                    break;
                }
                fout << charac[br];
                br = "";
            }
        }
    }

    fin.close();
    fout.flush();
    fout.close();
}

void printtree(tree *root) // to check tree made
{
    if (root == NULL)
    {
        return;
    }
    if (root->ch != '\0')
    {
        std::cout << root->ch << ":" << root->freq;
    }
    else
    {
        std::cout << ":" << root->freq;
    }
    std::cout << "\n";
    std::cout << " L:";
    printtree(root->left);
    std::cout << " R:";
    printtree(root->right);
    std::cout << "\n";

    return;
}

tree *decodetree(tree *root, std::unordered_map<char, std::string> &bitcode)
{

    tree *temp;
    for (auto i : bitcode)
    {
        temp = root;
        std::string bits = i.second;
        int j = bits.size() - 1;
        int sizeb = bits.size();
        for (int k = 0; k < sizeb; k++)
        {

            if (bits[k] == '1')
            {
                if (k == j)
                {
                    temp->right = new tree(i.first);
                }
                else
                {

                    if (temp->right == NULL)
                    {
                        temp->right = new tree(INTERNAL_NODE_CHARACTER);
                    }
                    temp = temp->right;
                }
            }
            else
            {
                if (k == j)
                {
                    temp->left = new tree(i.first);
                }
                else
                {

                    if (temp->left == NULL)
                    {
                        temp->left = new tree(INTERNAL_NODE_CHARACTER);
                    }
                    temp = temp->left;
                }
            }
        }
    }
    return root;
}
void printdecode(tree *root)
{
    if (root == NULL)
    {
        return;
    }
    if (root->ch != INTERNAL_NODE_CHARACTER)
    {
        std::cout << root->ch;
    }
    else
    {
        std::cout << ":";
    }
    std::cout << "\n";
    std::cout << " L:";
    printdecode(root->left);
    std::cout << " R:";
    printdecode(root->right);
    std::cout << "\n";

    return;
}
int main()
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 13);
    // entry();

    std::unordered_map<char, int> counter;
    std::priority_queue<tree *, std::vector<tree *>, comparenode> minheap;
    std::unordered_map<char, std::string> bitcode;
    std::unordered_map<std::string, char> charac;
    std::vector<std::pair<char, float>> p;
    std::string filename;
    std::string encodefile;
    std::string decodefile;
    int ch1, ch2;
    ch1 = 0;
    ch2 = 0;
    while (ch1 != 3)
    {

        std::cout << "Press\n"
                  << "\t 1. Encode \n"
                  << "\t 2. Decode \n"
                  << "\t 3. Exit \n";
        std::cin >> ch1;
        switch (ch1)
        {
        case 1:
            std::cout << "enter filename: ";
            std::cin >> filename;
            std::cout << "enter encodefile: ";
            std::cin >> encodefile;
            std::cout << "1. Huffman \n2. Shanon-Fanon\n";
            int choice;
            std::cin >> choice;
            if (choice == 1)
            {

                tree *root;
                root = encode(filename, encodefile, counter, minheap, bitcode);
                std::cout << "Encoded Successfully !!"
                          << "\n"
                          << "\n";
                ch2 = 0;
                while (ch2 != 3)
                {
                    std::cout << "\t\t 1. Print Huffman Encode Tree \n\t\t 2. Print Binary Code \n\t\t 3. Back \n\t\t 4. Exit\n"
                              << "\n";
                    std::cin >> ch2;
                    switch (ch2)
                    {
                    case 1:
                        std::cout << " Printing Tree "
                                  << "\n"
                                  << "\n";
                        printtree(root);
                        break;
                    case 2:
                        std::cout << " Printing Binary code "
                                  << "\n";
                        for (auto i : bitcode)
                        {
                            std::cout << i.first << " : " << i.second << "\n";
                        }
                        break;
                    case 3:
                        bitcode.clear();

                        counter.clear();
                        break;
                    case 4:
                        ch2 = 3;
                        ch1 = 3;
                        break;
                    default:
                        std::cout << "Enter valid input\n";
                        break;
                    }
                }
            }
            else if (choice == 2)
            {
                encodesf(filename, encodefile, counter, bitcode, p);
                ch2 = 0;

                while (ch2 != 3)
                {
                    std::cout << "\t\t 1. Print Probabilites \n\t\t 2. Print Binary Code \n\t\t 3. Back \n\t\t 4. Exit\n";
                    std::cin >> ch2;
                    switch (ch2)
                    {
                    case 1:
                        for (auto i : p)
                        {
                            std::cout << i.first << ": " << i.second << "\n";
                        }
                        break;
                    case 2:
                        for (auto i : bitcode)
                        {
                            std::cout << i.first << ": " << i.second << "\n";
                        }
                        break;
                    case 3:
                        bitcode.clear();
                        p.clear();
                        counter.clear();
                        break;
                    case 4:
                        ch2 = 3;
                        ch1 = 3;
                        break;
                    default:
                        break;
                    }
                }
            }
            break;
        case 2:
            std::cout << "Enter encodefile: ";
            std::cin >> encodefile;
            std::cout << "Enter decodefile: ";
            std::cin >> decodefile;
            decode(encodefile, decodefile, bitcode, charac);
            std::cout << "Decoded Successfully !!"
                      << "\n"
                      << "\n";
            ch2 = 0;
            while (ch2 != 3)
            {
                std::cout << "\t\t 1. Print Huffman Decode Tree \n\t\t 2. Print Binary Code \n\t\t 3. Back \n\t\t 4. Exit\n"
                          << "\n";
                std::cin >> ch2;
                if (ch2 == 1)
                {
                    std::cout << " Printing Tree "
                              << "\n";
                    tree *root2 = new tree(INTERNAL_NODE_CHARACTER);

                    decodetree(root2, bitcode);

                    printdecode(root2);
                }
                else if (ch2 == 2)
                {

                    std::cout << " Printing Binary code "
                              << "\n";
                    for (auto i : bitcode)
                    {
                        std::cout << i.first << " : " << i.second << "\n";
                    }
                }
                else if (ch2 == 3)
                {
                    bitcode.clear();
                    charac.clear();
                }
                else if (ch2 == 4)
                {
                    ch2 = 3;
                    ch1 = 3;
                }
                else
                {
                    std::cout << "Enter correct input\n";
                }
            }

            break;

        case 3:
            break;
        default:
            std::cout << "Enter valid input\n";
            break;
        }
    }
    return 0;
}

void assign(tree *root, std::string value, std::unordered_map<char, std::string> &bitcode) // to assign bitcode to all char in tree
{
    if (root == NULL)
    {
        return;
    }

    if (root->right == NULL && root->left == NULL)
    {
        bitcode[root->ch];

        bitcode[root->ch] = value;
        return;
    }

    std::string valueright = value + "1";
    std::string valueleft = value + "0";

    assign(root->left, valueleft, bitcode);
    assign(root->right, valueright, bitcode);

    return;
}

void entry()
{

    std::string entry = "---------------WELCOME----------------";

    std::string project = "APS LAB PBL";

    std::string made = " MADE BY";
    std::string name1 = "Akshansh Modi";
    std::string name2 = "Saumya Bansal";
    std::string name3 = "Armaan Sharma";

    std::string end = "--------------------------------------";

    std::cout << "\t\t\t\t";
    int el = entry.length();
    int pl = project.length();
    int ml = made.length();
    int nl = name1.length();
    int nl2 = name2.length();
    int nl3 = name3.length();
    int endlength = end.length();
    for (int i = 0; i < el; i++)
    {
        std::cout << entry[i];
        usleep(100000);
    }

    std::cout << "\n\t\t\t\t\t     ";
    for (int i = 0; i < pl; i++)
    {
        std::cout << project[i];
        usleep(100000);
    }
    std::cout << "\n\t\t\t\t\t      ";
    for (int i = 0; i < ml; i++)
    {
        std::cout << made[i];
        usleep(100000);
    }
    std::cout << "\n\t\t\t\t\t    ";
    for (int i = 0; i < nl; i++)
    {
        std::cout << name1[i];
        usleep(100000);
    }
    std::cout << "\n\t\t\t\t\t    ";
    for (int i = 0; i < nl2; i++)
    {
        std::cout << name2[i];
        usleep(100000);
    }
    std::cout << "\n\t\t\t\t\t    ";
    for (int i = 0; i < nl3; i++)
    {
        std::cout << name3[i];
        usleep(100000);
    }
    std::cout << "\n\t\t\t\t";

    for (int i = 0; i < endlength; i++)
    {
        std::cout << end[i];
        usleep(100000);
    }
}