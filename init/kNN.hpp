#include "main.hpp"

/* TODO: Please design your data structure carefully so that you can work with the given dataset
 *       in this assignment. The below structures are just some suggestions.
 */

template<typename T>
class List {
public:
    virtual ~List() = default;
    virtual void push_back(T value) = 0;
    virtual void push_front(T value) = 0;
    virtual void insert(int index, T value) = 0;
    virtual void remove(int index) = 0;
    virtual T& get(int index) const = 0;
    virtual int length() const = 0 ;
    virtual void clear() = 0;
    virtual void print() const = 0;
    virtual void reverse() = 0;
};

template <typename T> 
class Node {
public:
    T data;
    Node<T>* next;
    Node<T>* prev;
    Node(const T &val, Node<T>* next = NULL, Node<T>* prev = NULL): data(val), next(next), prev(prev) {}
};

template<typename T>
class MyList : public List<T> { 
private:
    Node<T>* head;
    Node<T>* tail;
    int size;
public:
    MyList() {
        // cout << "MyList Constructor" << endl;
        head = tail = NULL;
        size = 0;
    };

    ~MyList() {
        clear();
        // cout << "MyList Destructor" << endl;
    }

    MyList& operator=(const MyList& list) {
        for(int i = 0; i < list.length(); ++i) {
            this->push_back(list.get(i));
        }
        return this;
    }

    void push_back(T value) override {
        Node<T> *temp = new Node<T>(value);
        if(size > 0) {
            temp->prev = tail;
            tail->next = temp;
            tail = temp;
        }
        else {
            head = tail = temp;
        }
        ++size;
    }

    void push_front(T value) override {
        Node<T> *temp = new Node<T>(value);
        if(size > 0) {
            head->prev = temp;
            temp->next = head;
            head = temp;
        }
        else {
            head = tail = temp;
        }
        ++size;
    }

    void insert(int index, T value) override {
        // Bound
        if(index < 0 || index > size) return;

        // Insert before head
        if(index == 0) {
            push_front(value);
            return;
        }

        // Insert after tail
        if(index == size) {
            push_back(value);
            return;
        }

        // Insert in the middle of the list
        Node<T> *temp;
        if(index <= size / 2) {
            temp = head;
            int idx = 0; 
            while(idx < index) {
                temp = temp->next;
                ++idx;
            }
        }
        else {
            temp = tail;
            int idx = size - 1; 
            while(idx > index) {
                temp = temp->prev;
                --idx;
            }
        }
        Node<T> *insert_node = new Node<T>(value, temp, temp->prev);
        temp->prev->next = insert_node;
        temp->prev = insert_node;
        ++size;
        return;
    }

    void remove(int index) override {
        // Bound
        if(index < 0 || index >= size) return;

        // List has 1 element
        if(size == 1) {
            delete head;
            head = tail = NULL;
            --size;
            return;
        }
        // List has more than 1 element
        // Remove head:
        Node<T> *temp;
        if(index == 0) {
            temp = head->next;
            delete head;
            head = temp;
            if(head->prev) head->prev = NULL;
            --size;
            return;
        }
        
        // Remove tail:
        if(index == size - 1) {
            temp = tail->prev;
            delete tail;
            tail = temp;
            if(tail->next) tail->next = NULL;
            --size;
            return;
        }

        // Remove in list:
        if(index < size / 2) {
            temp = head;
            int idx = 0; 
            while(idx < index) {
                temp = temp->next;
                ++idx;
            }
        }
        else {
            temp = tail;
            int idx = size - 1; 
            while(idx > index) {
                temp = temp->prev;
                --idx;
            }
        }
        if(temp->prev) temp->next->prev = temp->prev;
        if(temp->next) temp->prev->next = temp->next;
        delete temp;
        --size;
    }

    T& get(int index) const override {
        if(index < 0 || index >= size) {
            throw std::out_of_range("get(): Out of range");
        }
        Node<T>* temp;
        if(index < size / 2) {
            temp = head;
            int idx = 0; 
            while(idx < index) {
                temp = temp->next;
                ++idx;
            }
        }
        else {
            temp = tail;
            int idx = size - 1; 
            while(idx > index) {
                temp = temp->prev;
                --idx;
            }
        }
        return temp->data;
    }

    int length() const override {
        return size;
    }

    void clear() override {
        while(size) remove(0);
    }

    void print() const override {
        if(size == 0) return;
        cout << this->get(0);
        for(int i = 1; i < size; ++i) {
            cout << " ";
            cout << this->get(i);
        }
    }
    
    void reverse() override {
        tail = head;
        Node<T> *curr = head;
        Node<T> *prev = NULL;
        Node<T> *next = NULL;
        while(curr) {
            next = curr->next;
            curr->next = prev;
            curr->prev = next;
            prev = curr;
            curr = next;
        }
        head = prev;
    }
};

class Dataset {
private:
    MyList<string>* labels;
    MyList<MyList<int>*>* data;
public:
    //You may need to define more
    Dataset();
    ~Dataset();
    Dataset(const Dataset& other);
    Dataset& operator=(const Dataset& other);
    bool loadFromCSV(const char* fileName);
    void printHead(int nRows, int nCols) const;
    void printTail(int nRows, int nCols) const;
    void getRow(int& nRows) const;
    void getCol(int& nCols) const;
    void getShape(int& nRows, int& nCols) const;
    void columns() const;
    bool drop(int axis, int index, string columns);
    Dataset extract(int startRow, int endRow, int startCol, int endCol) const;
    MyList<MyList<int>*>* getData() const;
    MyList<string>* getLabels() const;
};

class kNN {
private:
    int k;
    //You may need to define more
public:
    kNN(int k = 5);
    void fit(const Dataset& X_train, const Dataset& y_train);
    Dataset predict(const Dataset& X_test);
    double score(const Dataset& y_test, const Dataset& y_pred);
};

void train_test_split(Dataset& X, Dataset& y, double test_size, 
                        Dataset& X_train, Dataset& X_test, Dataset& y_train, Dataset& y_test);

// Please add more or modify as needed