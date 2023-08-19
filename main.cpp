//  Amaan Shah, UIC, Spring 2021
//
// For creative component, enter a 0.
// Here all stop words will be removed from 
// the search index.
//
// For regular test, enter 1.

#include <iostream>
#include "search.h"
using namespace std;

int main() {
    string input;
    string filename;
    cout << "Hello! Creative component (0) or regular test (1): ";
    cin >> input;
    cout << "Which test file would you like to use: ";
    cin >> filename;
    cin.ignore();
    if (input == "1") {
        searchEngine(filename);
    } else if (input == "0") {
        searchEngine2(filename);
    }
    return 0; 
}