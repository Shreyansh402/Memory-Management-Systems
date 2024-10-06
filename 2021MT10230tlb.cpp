// Operating Systems
// Assignment 3 - Problem 1
// Name: Shreyansh Jain
// Entry Number: 2021MT10230
// Simulate Virtual Memory Management using TLB and Page Table with different page replacement algorithms

#include <iostream>
#include <vector>
#include <string>

using namespace std;

// Implementing Hash Map
class HashMap
{
public:
    vector<pair<int, int>> *buckets;
    int bucket_size;
    int total_elements;
    float max_load_factor;

    int hash(int key)
    {
        return key % bucket_size;
    }

    HashMap(int bucket_size = 100, float max_load_factor = 0.75)
    {
        this->bucket_size = bucket_size;
        this->max_load_factor = max_load_factor;
        buckets = new vector<pair<int, int>>[bucket_size];
        total_elements = 0;
    }

    void insert(int key, int value)
    {
        int index = hash(key);
        buckets[index].push_back({key, value});
        total_elements++;
        if ((float)total_elements / bucket_size > max_load_factor)
        {
            rehash();
        }
    }

    void rehash()
    {
        vector<pair<int, int>> *old_buckets = buckets;
        int old_bucket_size = bucket_size;
        bucket_size *= 2;
        buckets = new vector<pair<int, int>>[bucket_size];
        total_elements = 0;
        for (int i = 0; i < old_bucket_size; i++)
        {
            for (auto &ele : old_buckets[i])
            {
                insert(ele.first, ele.second);
            }
        }
        delete[] old_buckets;
    }

    int search(int key)
    {
        int index = hash(key);
        for (auto &ele : buckets[index])
        {
            if (ele.first == key)
            {
                return ele.second;
            }
        }
        return 0;
    }

    void remove(int key)
    {
        int index = hash(key);
        for (auto it = buckets[index].begin(); it != buckets[index].end(); it++)
        {
            if (it->first == key)
            {
                buckets[index].erase(it);
                total_elements--;
                return;
            }
        }
    }

    int least()
    {
        int ans = INT_MAX;
        int key;
        for (int index = 0; index < bucket_size; index++)
        {
            for (auto it = buckets[index].begin(); it != buckets[index].end(); it++)
            {
                if (it->second < ans)
                {
                    ans = it->second;
                    key = it->first;
                }
            }
        }
        return key;
    }

    ~HashMap()
    {
        delete[] buckets;
    }
};

// TLB with FIFO
class TLB_FIFO
{
public:
    unsigned capacity;
    HashMap *hashMap;
    int front, rear, size;
    int *array;
    int hits;

    TLB_FIFO(unsigned capacity)
    {
        this->capacity = capacity;
        front = size = 0;
        hits = 0;
        rear = capacity - 1;
        array = new int[this->capacity];
        hashMap = new HashMap();
    }

    bool isFull()
    {
        return (size == capacity);
    }

    bool isEmpty()
    {
        return (size == 0);
    }

    void add(int item, int value)
    {
        rear = (rear + 1) % capacity;
        if (isFull())
        {
            hashMap->remove(array[rear]);
            size--;
        }
        array[rear] = item;
        hashMap->insert(item, value);
        size++;
    }

    void access(int key)
    {
        if (hashMap->search(key))
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
    unsigned capacity;
    HashMap *hashMap;
    int size;
    int last;
    int hits;

    TLB_LIFO(unsigned capacity)
    {
        this->capacity = capacity;
        size = 0;
        hits = 0;
        last = -1;
        hashMap = new HashMap();
    }

    bool isFull()
    {
        return (size == capacity);
    }

    bool isEmpty()
    {
        return (size == 0);
    }

    void add(int item, int value)
    {
        if (isFull())
        {
            hashMap->remove(last);
            size--;
        }
        last = item;
        hashMap->insert(item, value);
        size++;
    }

    void access(int key)
    {
        if (hashMap->search(key))
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
    unsigned capacity;
    HashMap *hashMap;
    int size;
    int hits;
    int counter;

    TLB_LRU(unsigned capacity)
    {
        this->capacity = capacity;
        size = 0;
        hits = 0;
        counter = 1;
        hashMap = new HashMap();
    }

    bool isFull()
    {
        return (size == capacity);
    }

    bool isEmpty()
    {
        return (size == 0);
    }

    void add(int item, int value)
    {
        if (isFull())
        {
            int key = hashMap->least();
            hashMap->remove(key);
            size--;
        }
        hashMap->insert(item, value);
        size++;
    }

    void access(int key)
    {
        if (hashMap->search(key))
        {
            hits++;
            hashMap->remove(key);
        }
        add(key, counter++);
    }
};

// Optimal TLB
class TLB_Optimal
{
public:
    unsigned capacity;
    HashMap *hashMap;
    vector<int> *accesses;
    int size;
    int hits;
    int counter;

    TLB_Optimal(unsigned capacity, vector<int> *accesses)
    {
        this->capacity = capacity;
        size = 0;
        hits = 0;
        counter = 0;
        hashMap = new HashMap();
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

    void add(int item, int value)
    {
        if (isFull())
        {
            int key = hashMap->least();
            hashMap->remove(key);
            size--;
        }
        hashMap->insert(item, value);
        size++;
    }

    int nextAccess(int key)
    {
        for (int i = counter + 1; i < accesses->size(); i++)
        {
            if (accesses->at(i) == key)
            {
                return i;
            }
        }
        return INT_MAX;
    }

    void access(int key)
    {
        if (hashMap->search(key) < 0)
        {
            hits++;
            hashMap->remove(key);
        }
        int next = nextAccess(key);
        add(key, -next);
    }
};

int main(int argc, char *argv[])
{
    int T;
    cin >> T;
    while (T--)
    {
        int S;
        int P, K, N;
        cin >> S; // Address Space in MB
        cout << "size:";
        cout << S << endl;
        cin >> P; // Page Size in KB
        cin >> K; // TLB Size
        cin >> N; // Number of Memory Access Requests
        cout << "num:";
        cout << N << endl;

        int num_pages = (S * 1024) / P;
        int page_size = P * 1024;

        vector<int> accesses;
        TLB_FIFO *fifo = new TLB_FIFO(K);
        TLB_LIFO *lifo = new TLB_LIFO(K);
        TLB_LRU *lru = new TLB_LRU(K);

        for (int i = 0; i < N; i++)
        {
            // Read the memory access request in Hexadecimal format
            int address;
            cin >> hex >> address;

            cout << "access:";
            cout << address << endl;

            // Calculate the page number
            int page = address / page_size;

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