//  Amaan Shah, UIC, Spring 2021
//
// Function that takes a file and
// creates a search engine for user queries.

#pragma once

#include <iostream>
#include <set>
#include <map>
#include <cctype>
#include <sstream>
#include <fstream>
#include <string>
using namespace std;


// Given a string, removes punctuation at beginning and end of string.
// If the string contains no letters, the empty string is returned.
// If the string contains letters, return cleaned and lowercase string.
string cleanToken(string s) {
    while (true) {
        if (ispunct(s.back())) {
            s.pop_back();
        } else if (ispunct(s.front())) {
            s.erase(s.begin());
        } else {
            break;
        }
    }
    bool lettersInStr = false;
    int n = s.length();
    for (int i = 0; i < n; i++) {
        if (isalpha(s[i])) {
            s[i] = tolower(s[i]);
            lettersInStr = true;
        }
    }
    if (lettersInStr == false) {
        return "";
    }
    return s;
}

// Given a string of words, parses and cleans each word using cleanToken.
// Once cleaned if the string is not empty or a repeated
// word it is inserted into the tokens set.
set<string> gatherTokens(string text) {
    set<string> tokens;
    stringstream line(text);
    string word;
    while (line >> word) {
        word = cleanToken(word);
        if (word != "") {
            tokens.insert(word);
        }
    }
    return tokens;
}

// Given the filename, creates an inverted index map where the keys are
// the tokens and the values are a set of urls that contain the token.
// Returns the number of websites processed.
int buildIndex(string filename, map<string, set<string>>& index) {
    fstream infile(filename);
    if (!infile.good()) {
        return 0;
    }
    int numURL = 0;
    string url;
    string line;
    while (!infile.eof()) {
        getline(infile, url);
        if (!infile.fail()) {
            numURL++;
            getline(infile, line);
            set<string> tokens = gatherTokens(line);
            for (auto token : tokens) {
                index[token].insert(url);
            }
        }
    }
    return numURL;
}

// Given the first word from a query returns the urls for this word.
// If the word is not in the index, the set of results is an empty set.
void buildFirst(map<string, set<string>> &index,
set<string> &result, string word, int &wordnum) {
    word = cleanToken(word);
    if (word != "") {
        if (index.count(word) == 1) {
            result = index[word];
        } else {
            result.clear();
        }
    }
}

// Given the set of curent result urls and the
// next word in the query, performs an intersection.
// Returns the set of urls that the results and new word had in common.
// If word is valid but not in index returns an empty set of results.
void intersectResults(map<string, set<string>> &index,
set<string> &result, set<string> &next, string word) {
    set<string> newResult;
    word = cleanToken(word);
    if (word != "") {
        if (index.count(word) == 1) {
            next = index[word];
            for (auto &url : result) {
                for (auto &x : next) {
                    if (url == x) {
                        newResult.insert(url);
                    }
                }
            }
            result = newResult;
        } else {  // valid word but not in index
            result.clear();
        }
    } else {  // not a valid search word
        result.clear();
    }
}

// Given the set of curent result urls and the next
// word in the query, performs a difference.
// Returns the set of urls from the current results
// that did not contain the next word.
void diffResults(map<string, set<string>> &index,
set<string> &result, set<string> &next, string word) {
    word = cleanToken(word);
    if (word != "") {
        if (index.count(word) == 1) {
            next = index[word];
            for (auto &url : next) {
                if (result.count(url) == 1) {
                    result.erase(url);
                }
            }
        }
    }
}

// Given the set of current results and the next
// word in the query, performs a union.
// Returns the set of urls that were in the current
// results or those that conained the next word.
void unionResults(map<string, set<string>> &index,
set<string> &result, set<string> &next, string word) {
    word = cleanToken(word);
    if (word != "") {
        if (index.count(word) == 1) {
            next = index[word];
            for (auto &url : next) {
                result.insert(url);
            }
        }
    }
}

// Given a map with an inverted index and a query,
// performs the required search.
// For one word/the first word in a sequence
// calls the buildFirst helper function.
// For a word with a + operator, calls inresection function.
// For a word with a - operator, calls  difference function.
// Else, calls the union function.
set<string> findQueryMatches(map<string, set<string>>& index, string sentence) {
    set<string> result;
    set<string> next;
    stringstream query(sentence);
    string word;
    int wordnum = 0;
    while (query >> word) {
        if (wordnum == 0) {
            buildFirst(index, result, word, wordnum);
        } else {
            if (word[0] == '+') {
                intersectResults(index, result, next, word);
            } else if (word[0] == '-') {
                diffResults(index, result, next, word);
            } else {
                unionResults(index, result, next, word);
            }
        }
        ++wordnum;
    }
    return result;
}

// Helper function to print out search results
// given the set of result urls.
void printResults(set<string> &searchResults) {
    for (auto &x : searchResults) {
        cout << x << endl;
    }
}

// Given the filename, creates a user interface to
// enter and print search results.
void searchEngine(string filename) {
    map<string, set<string>> index;
    int numPages = buildIndex(filename, index);
    int numTerms = index.size();
    cout << "Stand by while building index..." << endl;
    cout << "Indexed " << numPages << " pages containing ";
    cout << numTerms << " unique terms" << endl << endl;
    while (true) {
        cout << "Enter query sentence (press enter to quit): ";
        string query;
        getline(cin, query);
        if (query == "") {
            cout << "Thank you for searching!" << endl;
            break;
        }
        set<string> searchResults = findQueryMatches(index, query);
        int numResults = searchResults.size();
        cout << "Found " << numResults << " matching pages" << endl;
        if (numResults != 0) {
            printResults(searchResults);
        }
        cout << endl;
    }
}

// Helper function to build set of stopWords
void buildStopWords(string filename, set<string> &stopWords) {
    fstream infile(filename);
    string word;
    while (!infile.eof()) {
        getline(infile, word);
        if (!infile.fail()) {
            word = cleanToken(word);
            if (word != "") {
                stopWords.insert(word);
            }
        }
    }
}

// Given the index for the search file and the
// set of stop words, removes any of these words
// from the index map.
int removeStopWords(map<string, set<string>> &index, set<string> &stopWords) {
    int numRemoved = 0;
    for (auto &x : stopWords) {
        if (index.count(x) == 1) {
            index.erase(x);
            numRemoved++;
        }
    }
    return numRemoved;
}

// Snowball Stopword List: stopWords.txt list was
// found at https://kavita-ganesan.com/what-are-stop-words/#.YB3-2OhKiUn
// Search engine that removes stop words from results
void searchEngine2(string filename) {
    set<string> stopWords;
    map<string, set<string>> index;
    string wordFile = "stopWords.txt";
    buildStopWords(wordFile, stopWords);
    int numPages = buildIndex(filename, index);
    int numTerms = index.size();
    cout << endl << "Stand by while building index..." << endl;
    int numRemoved = 0;
    numRemoved = removeStopWords(index, stopWords);
    cout << "Indexed " << numPages << " pages containing ";
    cout << numTerms << " unique terms" << endl;
    cout << numRemoved << " stop words removed from index";
    cout << endl << endl;
    while (true) {
        cout << "Enter query sentence (press enter to quit): ";
        string query;
        getline(cin, query);
        if (query == "") {
            cout << "Thank you for searching!" << endl;
            break;
        }
        set<string> searchResults = findQueryMatches(index, query);
        int numResults = searchResults.size();
        cout << "Found " << numResults << " matching pages" << endl;
        if (numResults != 0) {
            printResults(searchResults);
        }
        cout << endl;
    }
}


