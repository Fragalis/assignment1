#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <iomanip>
#include <string>
#include <cstring>
#include <sstream>
#include <fstream>
#include <cassert>

using namespace std;
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
    List<string>* labels;
    List<List<int>*>* data;
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
    List<List<int>*>* getData() const;
    List<string>* getLabels() const;
};

Dataset::Dataset() {
    labels = new MyList<string>();
    data = new MyList<List<int>*>();
};
Dataset::~Dataset() {
    labels->clear();
    for(int i = 0; i < data->length(); ++i) data->get(i)->clear();
    data->clear();
    delete labels;
    delete data;
};

Dataset::Dataset(const Dataset& other) {
    // cout << "Copy Constructor Called" << endl;
    labels = new MyList<string>();
    data = new MyList<List<int>*>();
    int nRows, nCols;
    other.getShape(nRows, nCols);
    for(int i = 0; i < nCols; ++i) {
        labels->push_back(other.getLabels()->get(i));
    }
    for(int i = 0; i < nRows; ++i) {
        MyList<int>* row = new MyList<int>();
        for(int j = 0; j < nCols; ++j) {
            row->push_back(other.getData()->get(i)->get(j));
        }
        data->push_back(row);
    }
};

Dataset& Dataset::operator=(const Dataset& other) {
    cout << "Assignment Operator" << endl;
    if(this != &other) {
        labels->clear();
        for(int i = 0; i < data->length(); ++i) data->get(i)->clear();
        data->clear();
        delete labels;
        delete data;
        labels = new MyList<string>();
        data = new MyList<List<int>*>();
        int nRows, nCols;
        other.getShape(nRows, nCols);
        for(int i = 0; i < nCols; ++i) {
            labels->push_back(other.getLabels()->get(i));
        }
        for(int i = 0; i < nRows; ++i) {
            MyList<int>* row = new MyList<int>();
            for(int j = 0; j < nCols; ++j) {
                row->push_back(other.getData()->get(i)->get(j));
            }
            data->push_back(row);
        }
    }
    return *this;
};
bool Dataset::loadFromCSV(const char* fileName) {
    fstream file;
    file.open(fileName, ios::in);
    if(!file.is_open()) return 0;
    string label, line, data_value;

    // Readling labels:
    getline(file, line);
    stringstream str(line);
    while(getline(str, label, ',')) {
        labels->push_back(label);
    }
    
    // Reading contents:
    while(getline(file, line)) {
        stringstream s(line);
        MyList<int>* row = new MyList<int>();
        while(getline(s, data_value, ',')) {
            row->push_back(stoi(data_value));
        }
        data->push_back(row);
    }
    file.close();
    return 1;
};
void Dataset::printHead(int nRows = 5, int nCols = 5) const {
    // Boundary
    if(nRows < 0 || nCols < 0) return;
    int rowlen = 0;
    int collen = 0;
    getShape(rowlen, collen);
    int nr = min(nRows, rowlen);
    int nc = min(nCols, collen);

    // Print labels:
    cout << labels->get(0);
    for(int i = 1; i < nc; ++i) {
        cout << " ";
        cout << labels->get(i);
    }
    cout << endl;

    // Print data:
    for(int i = 0; i < nc; ++i) {
        cout << data->get(i)->get(0);
        for(int j = 1; j < nr; ++j) {
            cout << " ";
            cout << data->get(i)->get(j);
        }
        cout << endl;
    }
};
void Dataset::printTail(int nRows = 5, int nCols = 5) const {
    // Boundary
    if(nRows < 0 || nCols < 0) return;
    int rowlen = 0;
    int collen = 0;
    getShape(rowlen, collen);
    int nr = min(nRows, rowlen);
    int nc = min(nCols, collen);

    // Print labels:
    for(int i = nc - 1; i > 0; --i) {
        cout << labels->get(collen - 1 - i);
        cout << " ";
    }
    cout << labels->get(collen - 1);
    cout << endl;

    // Print data:
    for(int i = nc - 1; i >= 0; --i) {
        for(int j = nr - 1; j > 0; --j) {
            cout << data->get(rowlen - 1 - i)->get(collen - 1 - j);
            cout << " ";
        }
        cout << data->get(rowlen - 1 - i)->get(collen - 1);
        cout << endl;
    }
};
void Dataset::getRow(int& nRows) const {
    nRows = getData()->length();
};
void Dataset::getCol(int& nCols) const {
    nCols = getLabels()->length();
};
void Dataset::getShape(int& nRows, int& nCols) const {
    getRow(nRows);
    getCol(nCols);
};
void Dataset::columns() const {
    labels->print();
    
};
bool Dataset::drop(int axis = 0, int index = 0, std::string columns = "") {
    if(axis != 0 && axis != 1) return false;
    int nRows = 0, nCols = 0;
    getShape(nRows, nCols);
    // Delete row (axis = 0):
    if(axis == 0) {
        if(index < 0 || index >= nRows) return false;
        data->remove(index);
        return true;
    }
    // Delete col (axis = 1):
    // Find the name
    for(int i = 0; i < nCols; ++i) {
        // If name found
        if(labels->get(i) == columns) {
            // Delete every instance
            for(int j = 0; j < nRows; ++j) data->get(j)->remove(i);
            labels->remove(i);
            return true;
        }
    }
    // Cannot find the name
    return false;
};
Dataset Dataset::extract(int startRow = 0, int endRow = -1, int startCol = 0, int endCol = -1) const {
    Dataset* result = new Dataset(*this);
    int nCols = 0, nRows = 0;
    result->getShape(nRows, nCols);
    // Row removal
    endRow = (endRow == -1)? nRows : endRow;
    endCol = (endCol == -1)? nCols : endCol;
    for(int i = 0; i < nRows; ++i) {
        if(i < startRow) result->drop(0, 0, "");
        if(i > endRow) result->drop(0, endRow + 1 - startRow, "");
    }
    result->getShape(nRows, nCols);
    for(int i = 0; i < nCols; ++i) {
        if(i < startCol) {
            for(int j = 0; j < nRows; ++j) result->getData()->get(j)->remove(0);
            result->getLabels()->remove(0);
        }
        if(i > endCol) {
            for(int j = 0; j < nRows; ++j) result->getData()->get(j)->remove(endCol + 1 - startCol);
            result->getLabels()->remove(endCol + 1 - startCol);
        }
    }
    result->getShape(nRows, nCols);
    cout << "Result: " << nRows << "x" << nCols << endl;
    return *result;
};

List<List<int>*>* Dataset::getData() const {
    return this->data;
};
List<string>* Dataset::getLabels() const {
    return this->labels;
};

class kNN {
private:
    int k;
    Dataset X_train;
    Dataset y_train;
    //You may need to define more
public:
    kNN(int k = 5) {
        this->k = k;
    };
    void fit(const Dataset& X_train, const Dataset& y_train) {
        this->X_train = X_train;
        this->y_train = y_train;
    };
    Dataset predict(const Dataset& X_test) {
        Dataset y_pred;
        int test_row = 0;
        int train_row = 0;
        X_train.getRow(train_row);
        X_test.getRow(test_row);
        for(int i = 0; i < test_row; ++i) {
            MyList<double> *distance_list;
            MyList<int> *label_list;
            for(int j = 0; j < train_row; ++j) {
                double distance = dist(*X_test.getData()->get(i), *X_train.getData()->get(j));
                // finding index
                int index = 0;
                for(int k = 0; k < distance_list->length(); ++k) {
                    if(distance_list->get(k) >= distance) {
                        index = k;
                        break;
                    }
                }
                // adding stuff
                distance_list->insert(index, distance);
                label_list->insert(index, y_train.getData()->get(index)->get(0));
                if(distance_list->length() > k) distance_list->remove(distance_list->length() - 1);
                if(label_list->length() > k) label_list->remove(label_list->length() - 1);
            }
            int count[10] = {0,0,0,0,0,0,0,0,0,0};
            for(int k = 0; k < label_list->length(); ++k) {
                ++count[label_list->get(k)];
            }
            int max_count = 0;
            for(int k = 0; k < 10; ++k) {
                if(max_count < count[k]) max_count = count[k];
            }
            MyList<int> *pred = new MyList<int>();
            pred->push_back(max_count);
            y_pred->getData()->push_back(pred);
        }
        return y_pred;
    };
    double score(const Dataset& y_test, const Dataset& y_pred) {
        int test_count = 0;
        y_test.getRow(test_count);
        int correct_count = 0;
        List<List<int>*>* test_data = y_test.getData();
        List<List<int>*>* pred_data = y_pred.getData();
        for(int i = 0; i < test_count; ++i) {
            if(test_data->get(i) == pred_data->get(i)) ++correct_count;
        }
        return (double)correct_count / (double)test_count;
    };
};

void train_test_split(Dataset& X, Dataset& y, double test_size, 
                        Dataset& X_train, Dataset& X_test, Dataset& y_train, Dataset& y_test)
{
    int number_of_dataset = 0;
    y.getRow(number_of_dataset);
    int train_set_count = number_of_dataset - (int)((double)number_of_dataset * test_size + 0.999);
    X_train = X.extract(0, train_set_count - 1, 0, -1);
    X_test = X.extract(train_set_count, -1, 0, -1);
    y_train = y.extract(0, train_set_count - 1, 0, -1);
    y_test = y.extract(train_set_count, -1, 0, -1);
};

double dist(const List<int>& a, const List<int>& b) {
    double sum = 0.0;
    int length = a.length();
    for(int i = 0; i < length; ++i) {
        sum += pow(a.get(i) - b.get(i), 2);
    }
    return sqrt(sum);
}

void myList_tester() {
    // MyList Test:
    MyList<int> *test = new MyList<int>();
    // Push_back:
    for(int i = 0; i < 10; ++i) test->push_back(i);
    test->print();
    // 0 1 2 3 4 5 6 7 8 9
    cout << endl;
    test->clear();
    test->print();
    // 
    cout << endl;
    // Push_front:
    for(int i = 0; i < 10; ++i) test->push_front(i);
    test->print();
    // 9 8 7 6 5 4 3 2 1 0
    cout << endl;
    // Insert:
    test->insert(4,12831);
    test->print();
    // 9 8 7 6 12831 5 4 3 2 1 0
    cout << endl;
    // Remove:
    test->remove(3);
    test->print();
    // 9 8 7 12831 5 4 3 2 1 0
    cout << endl;
    // Reverse:
    test->reverse();
    test->print();
    // 0 1 2 3 4 5 12831 7 8 9
    cout << endl;
    cout << test->length() << endl;
    test->~MyList();

    cout << "test2" << endl;
    MyList<MyList<int>*>* test2 = new MyList<MyList<int>*>();
    for(int i = 0; i < 10; ++i) {
        MyList<int>* temp = new MyList<int>();
        for(int j = 0; j < 10; ++j) {
            temp->push_back(i * 10 + j);
        }
        test2->push_back(temp);
    }
    for(int i = 0; i < test2->length(); ++i) {
        test2->get(i)->print();
        cout << endl;
    }
    test2->remove(0);
    cout << test2->length() << endl;
    for(int i = 0; i < test2->length(); ++i) {
        test2->get(i)->print();
        cout << endl;
    }
    for(int i = 0; i < test2->length(); ++i) {
        test2->get(i)->remove(8);
        test2->get(i)->remove(8);
    }
    for(int i = 0; i < test2->length(); ++i) {
        test2->get(i)->print();
        cout << endl;
    }
}

void dataset_tester() {
    cout << "dataset tester" << endl;
    Dataset dataset;
    dataset.loadFromCSV("mnist.csv");
    dataset.printHead();
    dataset.printTail();
    int nRows, nCols;
    dataset.getShape(nRows, nCols);
    cout << "Dataset: " << nRows << "x" << nCols << endl;
    Dataset feature;
    feature = dataset.extract(5, 10, 15, 20);
    cout << "dataset: " << nRows << "x" << nCols << endl;
    feature.getShape(nRows, nCols);
    cout << "feature: " << nRows << "x" << nCols << endl;
}

void kNN_tester() {
    cout << "kNN tester" << endl;
    Dataset dataset;
    dataset.loadFromCSV("mnist.csv");
    dataset.printHead();
    dataset.printTail();
    kNN knn;
    Dataset X_train, X_test, y_train, y_test;
    Dataset feature = dataset.extract(0, -1, 1, -1);
    Dataset label = dataset.extract(0, -1, 0, 0);
    train_test_split(feature, label, 0.2, X_train, X_test, y_train, y_test);
    int nRows, nCols;
    X_train.getShape(nRows, nCols);
    cout << "X_train: " << nRows << "x" << nCols << endl;
    y_train.getShape(nRows, nCols);
    cout << "y_train: " << nRows << "x" << nCols << endl;
    knn.fit(X_train, y_train);
    Dataset y_pred = knn.predict(X_test);
    double accuracy = knn.score(y_test, y_pred);
    cout << "Accuracy: " << accuracy << endl;
}

void train_test_split(Dataset& X, Dataset& y, double test_size, 
                        Dataset& X_train, Dataset& X_test, Dataset& y_train, Dataset& y_test);

// Please add more or modify as needed

int main() {
    // myList_tester();
    // dataset_tester();
    kNN_tester();
}
