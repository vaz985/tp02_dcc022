#ifndef TOOLS_H
#define TOOLS_H
#include<bits/stdc++.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

#define MAX_SZ 65536

using namespace std;

string readQuote(const string &input, size_t &pos);

string readOpenClose(const string &input, size_t &pos, char open_c, char close_c);

string readUntilComma(const string &input, size_t &pos);

string readValue(const string &input, size_t &pos);



map<string, string> json2Map(string input);

string map2Json(const map<string, string> &data);

vector< pair<string, string> > json2Vec(const string &input);

string vec2Json(const vector< pair<string, string> > &data);

void catHop(map<string, string>& jsonMap, string ip);

//void sendMsg(const string &msg, const string &ip, int udp_socket);

string recvMsg(int udp_socket);

string make_update_msg(string source, string dest, const map<string, string> &distances);

string make_data_msg(string msg, string source, string dest);

string make_trace_msg(string source, string dest);

void dequotize(string &s);

string escape(const string &s);

#endif
