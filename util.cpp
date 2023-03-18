//
// Created by 26685 on 2023/3/18.
//
#include "util.h"

vector<string> split(string s, char del) {
    vector<string> res;
    int end = s.find(del);
    while (end != -1) {
        res.push_back(s.substr(0, end));
        s.erase(s.begin(), s.begin()+end+1);
        end = s.find(del);
    }
    res.push_back(s.substr(0, end));
    return res;
}

bool contain(string str, string delta) {
    return str.find(delta) != string::npos ? true : false;
}