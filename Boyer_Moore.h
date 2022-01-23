#pragma once
#include <string>
#include <vector>
using namespace std;

class Boyer_Moore
{
public:
    Boyer_Moore(string &t, string &p);

    void setTxt(string &t); //change text,
    vector<int> search(string &pat);

private:
    int maxVal(const int &x, const int &y);
    void badHeuristic(string &pat, int &size);

    string txt;
    string pat;
    vector<int> badChars;
};
