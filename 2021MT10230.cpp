// Operating Systems
// Assignment 3 - Problem 1
// Name: Shreyansh Jain
// Entry Number: 2021MT10230
// Simulate Virtual Memory Management using TLB and Page Table with different page replacement algorithms

#include <iostream>
#include <string>

using namespace std;

// TLB with FIFO
class TLB_FIFO
{
public:
    unsigned int capacity;
    // HashMap *hashMap;
    unordered_map<unsigned int, int> *hashMap;
    unsigned int front, rear, size;
    unsigned int *array;
    unsigned int hits;

    TLB_FIFO(unsigned int capacity)
    {
        this->capacity = capacity;
        front = size = 0;
        hits = 0;
        rear = capacity - 1;
        array = new unsigned int[this->capacity];
        hashMap = new unordered_map<unsigned int, int>();
    }

    bool isFull()
    {
        return (size == capacity);
    }

    bool isEmpty()
    {
        return (size == 0);
    }

    void add(unsigned int item, int value)
    {
        rear = (rear + 1) % capacity;
        if (isFull())
        {
            hashMap->erase(array[rear]);
            size--;
        }
        array[rear] = item;
        hashMap->insert({item, value});
        size++;
    }

    void access(unsigned int key)
    {
        if (hashMap->find(key) != hashMap->end())
        {
            hits++;
        }
        else
        {
            add(key, 1);
        }
    }
};

// TLB with LIFO
class TLB_LIFO
{
public:
    unsigned int capacity;
    unordered_map<unsigned int, int> *hashMap;
    unsigned int size;
    unsigned int last;
    unsigned int hits;

    TLB_LIFO(unsigned int capacity)
    {
        this->capacity = capacity;
        size = 0;
        hits = 0;
        last = -1;
        hashMap = new unordered_map<unsigned int, int>();
    }

    bool isFull()
    {
        return (size == capacity);
    }

    bool isEmpty()
    {
        return (size == 0);
    }

    void add(unsigned int item, int value)
    {
        if (isFull())
        {
            hashMap->erase(last);
            size--;
        }
        last = item;
        hashMap->insert({item, value});
        size++;
    }

    void access(unsigned int key)
    {
        if (hashMap->find(key) != hashMap->end())
        {
            hits++;
        }
        else
        {
            add(key, 1);
        }
    }
};

// TLB with LRU
class TLB_LRU
{
public:
    unsigned int capacity;
    unordered_map<unsigned int, int> *hashMap;
    unsigned int size;
    unsigned int hits;
    unsigned int counter;

    TLB_LRU(unsigned int capacity)
    {
        this->capacity = capacity;
        size = 0;
        hits = 0;
        counter = 1;
        hashMap = new unordered_map<unsigned int, int>();
    }

    bool isFull()
    {
        return (size == capacity);
    }

    bool isEmpty()
    {
        return (size == 0);
    }

    void add(unsigned int item, int value)
    {
        if (isFull())
        {
            // unsigned int key = hashMap->least();
            unsigned int key = 0;
            int minn = INT_MAX;
            for (auto it = hashMap->begin(); it != hashMap->end(); it++)
            {
                if (it->second < minn)
                {
                    minn = it->second;
                    key = it->first;
                }
            }
            hashMap->erase(key);
            size--;
        }
        hashMap->insert({item, value});
        size++;
    }

    void access(unsigned int key)
    {
        if (hashMap->find(key) != hashMap->end())
        {
            hits++;
            hashMap->erase(key);
        }
        add(key, counter++);
    }
};

// Optimal TLB
class TLB_Optimal
{
public:
    unsigned int capacity;
    unordered_map<unsigned int, int> *hashMap;
    vector<unsigned int> *accesses;
    unsigned int size;
    unsigned int hits;
    unsigned int counter;

    TLB_Optimal(unsigned int capacity, vector<unsigned int> *accesses)
    {
        this->capacity = capacity;
        size = 0;
        hits = 0;
        counter = 0;
        hashMap = new unordered_map<unsigned int, int>();
        this->accesses = accesses;
    }

    void simulate()
    {
        for (counter = 0; counter < accesses->size(); counter++)
        {
            access(accesses->at(counter));
        }
    }

    bool isFull()
    {
        return (size == capacity);
    }

    bool isEmpty()
    {
        return (size == 0);
    }

    void add(unsigned int item, int value)
    {
        if (isFull())
        {
            unsigned int key = 0;
            int maxx = INT_MIN;
            for (auto it = hashMap->begin(); it != hashMap->end(); it++)
            {
                if (it->second > maxx)
                {
                    maxx = it->second;
                    key = it->first;
                }
            }
            hashMap->erase(key);
            size--;
        }
        hashMap->insert({item, value});
        size++;
    }

    int nextAccess(unsigned int key)
    {
        for (unsigned int i = counter + 1; i < accesses->size(); i++)
        {
            if (accesses->at(i) == key)
            {
                return i;
            }
        }
        return INT_MAX;
    }

    void access(unsigned int key)
    {
        if (hashMap->find(key) != hashMap->end())
        {
            hits++;
            hashMap->erase(key);
        }
        int next = nextAccess(key);
        add(key, next);
    }
};

int main(int argc, char *argv[])
{
    unsigned int T;
    cin >> T;
    while (T--)
    {
        unsigned int S;
        unsigned int P, K, N;
        cin >> S; // Address Space in MB
        cin >> P; // Page Size in KB
        cin >> K; // TLB Size
        cin >> N; // Number of Memory Access Requests

        unsigned int num_pages = (S * 1024) / P;
        unsigned int page_size = P * 1024;

        vector<unsigned int> accesses;
        TLB_FIFO *fifo = new TLB_FIFO(K);
        TLB_LIFO *lifo = new TLB_LIFO(K);
        TLB_LRU *lru = new TLB_LRU(K);

        for (unsigned int i = 0; i < N; i++)
        {
            // Read the memory access request in Hexadecimal format
            unsigned int address;
            cin >> hex >> address;

            // Calculate the page number
            unsigned int page = address / page_size;

            // Add the page to the list of accesses
            accesses.push_back(page);

            // Access the page in TLB
            fifo->access(page);
            lifo->access(page);
            lru->access(page);
        }

        // Simulate the Optimal TLB
        TLB_Optimal *optimal = new TLB_Optimal(K, &accesses);
        optimal->simulate();

        // Print the number of hits for each TLB
        cout << fifo->hits << " " << lifo->hits << " " << lru->hits << " " << optimal->hits << endl;

        delete fifo;
        delete lifo;
        delete lru;
        delete optimal;
    }
    return 0;
}