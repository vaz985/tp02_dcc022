#ifndef TOOLS_H
#define TOOLS_H
#include<bits/stdc++.h>



using namespace std;

map<string, string> json2Map(string input);

string map2Json(const map<string, string> &data);

vector< pair<string, string> > json2Vec(const string &input);

string vec2Json(const vector< pair<string, string> > &data);

#endif
