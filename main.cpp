#include <iostream>
#include <cctype>
#include "Boyer_Moore.h"
#include <fstream>
#include <sstream>
using namespace std;

long long compute_hash(std::string const& str) //computes the mod value
{
    int p = 31;
    long long power_of_p = 1;
    long long hash_val = 0;
    for (int i = 0; i < str.length(); i++) {
        if (!ispunct(str[i])) {
            if (str[i] <= 57 && str[i] >= 48) {
                hash_val
                        = (hash_val
                           + (str[i] + 97 - 'a' + 1) * power_of_p)
                          % (174 * 2);
                power_of_p
                        = (power_of_p * p) % (174 * 2);
            }
            else {
                hash_val
                        = (hash_val
                           + (str[i] - 'a' + 1) * power_of_p)
                          % (174 * 2);
                power_of_p
                        = (power_of_p * p) % (174 * 2);
            }
        }
    }
    return hash_val;
}

string readFile(string filename) {

    string hTable[174 * 2];

    fstream file;
    string word;
    file.open(filename.c_str());
    string alltext;

    fstream stop_file;
    string stopW;

    string s_filename = "stop_words.txt";
    stop_file.open(s_filename.c_str());

    while (stop_file >> stopW) {
        long long index = compute_hash(stopW);

        string wordTemp = "";
        for (int i = 0, stringSize = stopW.size(); i < stringSize; i++) {
            wordTemp += tolower(stopW[i]);
        }
        while (!hTable[index].empty()) {
            if (index == (174 * 2 - 1)) {
                index = 0;
            }
            else { index++; }
        }
        hTable[index] = wordTemp;
    }
    stop_file.close();

    bool file_empty = true;
    ofstream outfile("filtered_questions.txt");
    if (filename == "questions.txt") {
        file_empty = false;
        bool flag = false;
        while (file >> word) {
            if (flag) {
                flag = false;
                continue;
            }
            word[0] = tolower(word[0]);
            if (word == "what" || word == "how" || word == "which") {
                flag = true;
                continue;
            }
            outfile << word << " ";
            if (word[word.size() - 1] == '?') { outfile << endl; }
        }
        file.clear();
        file.seekg(0, ios::beg);
        outfile.close();
    }
    if (!file_empty) {
        file.close();
        filename = "filtered_questions.txt";
        file.open(filename.c_str());
    }

    while (file >> word)
    {
        string wordTemp = "";
        for (int i = 0, stringSize = word.size(); i < stringSize; i++) {
            if (wordTemp[i] == '"') { continue; }
            wordTemp += tolower(word[i]);
        }

        int temp_index = compute_hash(wordTemp);

        bool isSP = false;//true if the word is stopword

        while (!hTable[temp_index].empty()) {
            if (hTable[temp_index] != wordTemp) {
                if (temp_index == (174 * 2 - 1)) { temp_index = 0; }
                else { temp_index++; }
            }
            else {
                isSP = true;
                break;
            }
        }
        if (!isSP) { alltext += wordTemp + " "; }
    }
    file.close();

    return alltext;
}


int * find_texts_start(int index, string text) {// text is the whole story .txt index is the start index result from boyer moore
    int start = 0;
    int end = 0;
    static int a[2];
    for (int i = index; i >= 0; i--) {
        if (text[i] == '.' || text[i] == '?' || text[i] == '!') {
            start = i + 2;
            break;
        }
    }
    for (int i = index + 1; i < text.length(); i++) {
        if (text[i] == '.' || text[i] == '?' || text[i] == '!') {
            end = i;
            break;
        }
    }
    a[0] = start;
    a[1] = end;
    return a;
}

string read_sentence(int start, int end, string text) {
    string temp = "";
    for (int i = start; i < end; i++) {
        temp += text[i];
    }
    return temp;
}

vector<int> search_next_word(vector<int> match_words, Boyer_Moore b1, const string& text, string pat) {
    vector<int> temp;
    for (int j = 0; j < match_words.size(); j++) {
        vector<int> temp_v;
        int *p;
        p = find_texts_start(match_words[j], text);
        string s = read_sentence(*(p + 0), *(p + 1), text);
        b1.setTxt(s);
        temp_v = b1.search(pat);
        if (!temp_v.empty()) {
            temp.push_back(*(p + 0));
        }
    }
    return temp;
}

bool IsEqual(string x, string y) {
    //int lengthX = 0;
    //int lengthY = 0;
    int length;

    if (x.size() > y.size()) {length = y.size()-1;}
    else { length = x.size()-1; }

    if (x[0] != y[0]) return false;

    for (int i = 1; i < length; i++) {
        if (x[i] != y[i]) { return false; }
    }
    return true;
}

int main() {
    string filename = "the_truman_show_script.txt";
    string q_filename = "questions.txt";
    string txt;
    vector<string> questions;
    vector<vector<string>> question;

    txt = readFile(filename);

    string q = readFile(q_filename);
    string temp = "";
    for (int i = 0; i < q.length(); ++i) {
        if (q[i] == '?') {
            questions.push_back(temp);
            temp = "";
        }
        else {
            temp += q[i];
        }
    }
    int i = 0;
    temp = "";
    vector<string> x;
    while (i < questions.size()) {
        for (int j = 0; j <= questions[i].length(); j++) {
            if (questions[i][j] == ' ') {
                if (j > 0 && questions[i][j - 1] != '?') {
                    x.push_back(temp);
                    temp = "";
                }
            }
            else { temp += questions[i][j]; }
        }
        x.push_back(temp);
        question.push_back(x);
        temp = "";
        x.clear();
        i++;
    }


    string pat;
    Boyer_Moore b(txt, pat);
    vector<int> matched_words;
    vector<int> matched_second_words;
    vector<int> matched_third_words;
    vector<int>final_match;


    for (int j = 0; j < questions.size(); j++) {
        pat = question[j][0];
        matched_words = b.search(pat);
        if (matched_words.size() > 1) {
            matched_second_words = search_next_word(matched_words, b, txt, question[j][1]);//,question[j][0]
            if (matched_second_words.size() > 1) {
                matched_third_words = search_next_word(matched_second_words, b, txt, question[j][2]);//, question[j][1]

                for (int k = 0; k < matched_third_words.size(); k++) {
                    final_match.push_back(matched_third_words[k]);
                }

            }
            else {
                for (int k = 0; k < matched_second_words.size(); k++) {
                    final_match.push_back(matched_second_words[k]);
                }
            }
        }
        else {
            for (int k = 0; k < matched_words.size(); k++) {
                final_match.push_back(matched_words[k]);
            }
        }
    }

    vector<vector<string>> answer_sentences;
    vector<string> sentence;

    temp = "";
    for (int k = 0; k < final_match.size(); k++) {
        for (int j = final_match[k]; j < 9999999; j++) {
            if (!(txt[j] == '?' || txt[j] == '.' || txt[j] == '!')) {

                if (txt[j] != ' ') {
                    temp += txt[j];
                }
                else {
                    sentence.push_back(temp);
                    temp = "";
                }
            }
            else {
                sentence.push_back(temp);
                temp = "";
                break;
            }
        }
        answer_sentences.push_back(sentence);
        sentence.clear();
    }

    vector<string> answer;
    int index = 0;
    bool isEqual;

    for (int k = 0; k < question.size(); k++) {
        for (int m = 0; m < question[k].size(); m++) {
            for (int c = 0; c < question[k][m].size(); c++) {
                if (ispunct(question[k][m][c]) && answer_sentences[k][m][c] != ':')
                    question[k][m].erase(c--, 1);
            }
        }
    }
    for (int k = 0; k < answer_sentences.size(); k++) {
        for (int m = 0; m < answer_sentences[k].size(); m++) {
            for (int c = 0; c < answer_sentences[k][m].size(); c++) {
                if (ispunct(answer_sentences[k][m][c]) && answer_sentences[k][m][c] != ':')
                    answer_sentences[k][m].erase(c--, 1);
            }
        }
    }


    int m, j;
    bool Flag, endFlag;
    for (int questionIndex = 0; questionIndex < question.size(); questionIndex++) {
        Flag = false;
        m = 0;
        while (m < answer_sentences[questionIndex].size() - 1) {
            j = 0;
            endFlag = false;
            while (j <= question[questionIndex].size() - 1) {
                if (IsEqual(answer_sentences[questionIndex][m], question[questionIndex][j])) {
                    if (m >= answer_sentences[questionIndex].size() - 1) {
                        j = question[questionIndex].size();
                    }
                    else {
                        m++;
                        j = 0;
                    }
                    Flag = true;
                }
                else {
                    if (j == question[questionIndex].size() - 1) {
                        m++;
                        j++;
                        endFlag = true;
                    }
                    else {
                        j++;
                    }
                    Flag = false;
                }
            }
            if (j >= question[questionIndex].size() - 1 && m - answer_sentences[questionIndex].size() == 2) {
                answer.push_back(answer_sentences[questionIndex][m+1]);
            }
            if (!Flag) {
                if (endFlag) {
                    answer.push_back(answer_sentences[questionIndex][m - 1]);
                }
                else {
                    answer.push_back(answer_sentences[questionIndex][m]);
                }
            }
        }
    }

    filename="questions.txt";
    fstream file;
    file.open(filename.c_str());

    string str;
    int w=0;
    while (getline(file, str))
    {
        cout<<w+1<<") "<<str<<endl;
        cout<<answer[w]<<endl<<endl;
        w++;
    }
    file.close();


    return 0;
}
