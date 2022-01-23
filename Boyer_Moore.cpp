#include "Boyer_Moore.h"
#include <string>
#include <iostream>
using namespace std;

//---------------------------------------------------Conctructors-------------------------------------------------

Boyer_Moore::Boyer_Moore(string &t, string &p) {
    txt = t;
    pat = p;
    badChars.resize(256);
}

void Boyer_Moore::setTxt(string &t) {
    txt = t;
    pat = "";
}


//---------------------------------------------------Boyer_Moore--------------------------------------------------


int Boyer_Moore::maxVal(const int &x, const int &y) { return (x > y) ? x : y; }

void Boyer_Moore::badHeuristic(string &pat, int &pat_s) {

    for (int i = 0; i < badChars.size(); i++) // Initialize all occurrences as -1
        badChars[i] = -1;

    for (int i = 0; i < pat_s; i++) // Fill the actual value of last occurrence of a character
        badChars[(int)pat[i]] = i;
}

vector<int> Boyer_Moore::search(string &pat)
{
    int patL = pat.length();
    int txtLen = txt.length();

    badHeuristic(pat, patL);

    vector<int> ret_vec;

    int txtShift = 0;
    while (txtShift <= (txtLen - patL + 1))
    {
        int j = patL - 1;

        while (j >= 0 && pat[j] == txt[txtShift + j])
            j--;

        if (j < 0) {
            ret_vec.push_back(txtShift);
            txtShift += (txtShift + patL < txtLen) ? patL - badChars[txt[txtShift + patL]] : 1;
        }
        else {
            txtShift += maxVal(1, j - badChars[txt[txtShift + j]]);
        }
    }

    return ret_vec;
}

