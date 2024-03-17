#include "kNN.hpp"

/* TODO: You can implement methods, functions that support your data structures here.
 * */

Dataset::Dataset() {
    labels = new MyList<string>();
    data = new MyList<List<int>*>();
};
Dataset::~Dataset() {
    labels->clear();
    for(int i = 0; i < data->length(); ++i) data->get(i)->clear();
    data->clear();
    labels = NULL;
    data = NULL;
};

Dataset::Dataset(const Dataset& other) {
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
    for(int i = 0; i < nr; ++i) {
        cout << data->get(i)->get(0);
        for(int j = 1; j < nc; ++j) {
            cout << " ";
            cout << data->get(i)->get(j);
        }
        if(i < nr - 1) cout << endl;
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
    for(int i = nr - 1; i >= 0; --i) {
        for(int j = nc - 1; j > 0; --j) {
            cout << data->get(rowlen - 1 - i)->get(collen - 1 - j);
            cout << " ";
        }
        cout << data->get(rowlen - 1 - i)->get(collen - 1);
        if(i > 0) cout << endl;
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
    Dataset result;
    int nCols = 0, nRows = 0;
    getShape(nRows, nCols);
    endRow = (endRow == -1)? nRows - 1 : endRow;
    endCol = (endCol == -1)? nCols - 1 : endCol;
    for(int i = startRow; i <= endRow; ++i) {
        List<int>* row = new MyList<int>();
        for(int j = startCol; j <= endCol; j++) {
            row->push_back(data->get(i)->get(j));
        }
        result.data->push_back(row);
    }
    for(int j = startCol; j <= endCol; j++) {
        result.labels->push_back(labels->get(j));
    }
    return result;
};

List<List<int>*>* Dataset::getData() const {
    return this->data;
};
List<string>* Dataset::getLabels() const {
    return this->labels;
};

double dist(const List<int>& a, const List<int>& b) {
    double sum = 0.0;
    int length = a.length();
    for(int i = 0; i < length; ++i) {
        sum += pow(a.get(i) - b.get(i), 2);
    }
    return sqrt(sum);
};

void kNN::fit(const Dataset& X_train, const Dataset& y_train) {
    this->X_train = X_train;
    this->y_train = y_train;
};

Dataset kNN::predict(const Dataset& X_test) {
    Dataset y_pred;
    y_pred.getLabels()->push_back("label");
    int test_row = 0;
    int train_row = 0;
    X_train.getRow(train_row);
    X_test.getRow(test_row);
    MyList<double> distance_list;
    MyList<int> label_list;
    for(int i = 0; i < test_row; ++i) {
        distance_list.clear();
        label_list.clear();
        for(int j = 0; j < train_row; ++j) {
            int label = y_train.getData()->get(j)->get(0);
            double distance = dist(*X_test.getData()->get(i), *X_train.getData()->get(j));
            // finding index
            int index = 0;
            for(; index < distance_list.length(); ++index) {
                if(distance_list.get(index) > distance) break;
            }
            // adding stuff
            distance_list.insert(index, distance);
            label_list.insert(index, label);
            // remove end if length > k
            // we only need k elements
            if(distance_list.length() > k) distance_list.remove(k);
            if(label_list.length() > k) label_list.remove(k);
        }
        int count[10] = {0,0,0,0,0,0,0,0,0,0};
        for(int pos = 0; pos < label_list.length(); ++pos) {
            ++count[label_list.get(pos)];
        }
        int max_count = 0, num = 0;
        for(int pos = 0; pos < 10; ++pos) {
            if(max_count < count[pos]) {
                num = pos;
                max_count = count[pos];
            }
        }
        MyList<int> *pred = new MyList<int>();
        pred->push_back(num);
        y_pred.getData()->push_back(pred);
    }
    return y_pred;
};

double kNN::score(const Dataset& y_test, const Dataset& y_pred) {
    int test_count = 0;
    y_test.getRow(test_count);
    int correct_count = 0;
    List<List<int>*>* test_data = y_test.getData();
    List<List<int>*>* pred_data = y_pred.getData();
    for(int i = 0; i < test_count; ++i) {
        if(test_data->get(i)->get(0) == pred_data->get(i)->get(0)) ++correct_count;
    }
    return (double)correct_count / (double)test_count;
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
