#include "kNN.hpp"

/* TODO: You can implement methods, functions that support your data structures here.
 * */

Dataset::Dataset() {
    labels = new MyList<string>();
    data = new MyList<MyList<int>*>();
};
Dataset::~Dataset() {
    labels->clear();
    for(int i = 0; i < data->length(); ++i) data->get(i)->clear();
    data->clear();
    labels = NULL;
    data = NULL;
};

Dataset::Dataset(const Dataset& other) {
    for(int i = 0; i < other.labels->length(); ++i) {
        labels->push_back(other.labels->get(i));
    }
    for(int i = 0; i < other.data->length(); ++i) {
        MyList<int>* row = new MyList<int>();
        for(int j = 0; j < other.data->get(i)->length(); ++j) {
            row->push_back(other.data->get(i)->get(j));
        }
        data->push_back(row);
    }
};
Dataset& Dataset::operator=(const Dataset& other) {
    Dataset *dataset = new Dataset(other);
    return *dataset;
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
    Dataset* result = new Dataset();
    result->data = getData();
    result->labels = getLabels();
    int nCols = 0, nRows = 0;
    result->getShape(nRows, nCols);
    cout << nRows << " " << nCols << endl;
    // Row removal
    endRow = (endRow == -1)? nRows : endRow;
    endCol = (endCol == -1)? nCols : endCol;
    for(int i = 0; i < nRows; ++i) {
        if(i < startRow) result->drop(0, 0, "");
        if(i > endRow) result->drop(0, endRow + 1 - startRow, "");
        if(i < startRow || i > endRow) {
            // Delete every instance
            cout << "Remove row " << i << endl;
        }
    }
    result->getShape(nRows, nCols);
    cout << nRows << " " << nCols << endl;
    for(int i = 0; i < nCols; ++i) {
        if(i < startCol) {
            for(int j = 0; j < nRows; ++j) result->getData()->get(j)->remove(0);
            cout << "Remove col " << i << endl;
            result->getLabels()->remove(0);
        }
        if(i > endCol) {
            for(int j = 0; j < nRows; ++j) result->getData()->get(j)->remove(endCol + 1 - startCol);
            cout << "Remove col " << i << endl;
            result->getLabels()->remove(endCol + 1 - startCol);
        }
    }
    result->getShape(nRows, nCols);
    cout << nRows << " " << nCols << endl;
    return *result;
};

MyList<MyList<int>*>* Dataset::getData() const {
    return this->data;
};
MyList<string>* Dataset::getLabels() const {
    return this->labels;
};