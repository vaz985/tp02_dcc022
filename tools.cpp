#include "tools.h"

const set<char> ignorable = {'\n', ' ', ',', '\t', ':'};

string readQuote(const string &input, size_t &pos) {
	while(ignorable.count(input[pos])) pos++;
	assert(input[pos++] == '"');
	string ret;
	while(true) {
		if(input[pos] == '"' and input[pos - 1] != '\\') break;
		ret.push_back(input[pos++]);
	}
	pos++;
	return ret;
}

string readOpenClose(const string &input, size_t &pos, char open_c, char close_c) {
	string ret;
	assert(input[pos] == open_c);
	while(true) {
		ret.push_back(input[pos]);
		pos++;
		if(input[pos - 1] == close_c) break;
	}
	return ret;
}

string readUntilComma(const string &input, size_t &pos) {
	string ret;
	while(true) {
		if(pos == input.size() or input[pos] == ',') break;
		ret.push_back(input[pos]);
		pos++;
	}
	if(pos < input.size())
		pos++;
	return ret;
}

string readValue(const string &input, size_t &pos) {
	while(input[pos] == ' ' or input[pos] == '\n' or input[pos] == '\t') pos++;
	if(input[pos] == '[') return readOpenClose(input, pos, '[', ']');
	else if(input[pos] == '{') return readOpenClose(input, pos, '{', '}');
	else if(input[pos] == '"') {
		string ret = readQuote(input, pos);
		ret = "\"" + ret + "\"";
		return ret;
	}
	else {
		return readUntilComma(input, pos);
	}
}

map<string, string> json2Map(string input) {
	input = input.substr(input.find('{') + 1);
	while(input.back() != '}') input.pop_back();
	input.pop_back();
	while(input.back() == ' ' or input.back() == '\n') input.pop_back();

	map<string, string> data;
	string key;
	string value;
	size_t pos = 0;

	while(pos < input.size()) {
		if(ignorable.count(input[pos])) {
			pos++;
			continue;
		}
		key = readQuote(input, pos);
		
		while(input[pos] != ':') pos++;
		pos++;
		
		value = readValue(input, pos);
		data[key] = value;
	}
	return data;
}

string map2Json(const map<string, string> &data) {
	string json = "{";
	for(auto p : data) {
		json += "\n\t\"";
		json += p.first;
		json += "\": ";
		json += p.second;
		json += ",";
	}
	json.pop_back();
	json += "\n}";
	return json;
}

string vec2Json(const vector< pair<string, string> > &data) {
	map<string, string> map_data;
	for(auto p : data) {
		map_data[p.first] = p.second;
	}
	return map2Json(map_data);
}

vector< pair<string, string> > json2Vec(const string &input) {
	vector< pair<string, string> > ret_data;
	map<string, string> data = json2Map(input);
	for(auto p : data) {
		ret_data.push_back(p);
	}
	return ret_data;
}

void catHop(map<string, string> &data, string ip) {
  if(data["type"] != string("\"trace\"")) {
    cerr << "Concat in wrong json type" << endl;
    exit(1);
  }
  string new_hop = data["hops"];  
  new_hop[new_hop.size()-1] = ',';
  new_hop += " \"" + ip + "\"]";
  data["hops"] = new_hop;
}


string recvMsg(int udp_socket) {
	struct sockaddr_in from_addr;
	static char buf[MAX_SZ];
	uint32_t addr_len = sizeof(struct sockaddr_in);

	int recvlen = recvfrom(udp_socket, buf, MAX_SZ, 0, (struct sockaddr *)&from_addr, &addr_len);
	if(recvlen <= 0) return "";

	uint32_t sz;
	memcpy(&sz, buf, sizeof(sz));
	sz = ntohl(sz);

	static char c_msg[MAX_SZ];
	memcpy(&c_msg, buf + sizeof(sz), sz);
	c_msg[sz] = '\0';

	string msg = c_msg;
	return msg;
}

string make_update_msg(string source, string dest, const map<string, string> &distances) {
  map<string, string> data;
  data[string("type")]   = "\"update\"";
  data[string("source")] = "\"" + source + "\"";
  data[string("destination")]   = "\"" + dest + "\"";
  data[string("distances")] = map2Json(distances);
  return map2Json(data);
}

string make_data_msg(string msg, string source, string dest) {
  map<string, string> data;
  data[string("type")]   = "\"data\"";
  data[string("source")] = "\"" + source + "\"";
  data[string("destination")]   = "\"" + dest + "\"";
  data[string("payload")] = msg;
	return map2Json(data);
}

string make_trace_msg(string source, string dest) {
  map<string, string> data;
  data[string("type")]   = "\"trace\"";
  data[string("source")] = "\"" + source + "\"";
  data[string("destination")]   = "\"" + dest + "\"";
  data[string("hops")] = "[" + data["source"] + "]";
	return map2Json(data);
}

void dequotize(string &s) {
	if(s.size() and s[0] == '"') {
		s = s.substr(1, s.size() - 2);
	}
}

string escape(const string &s) {
	string escaped;
	for(char c : s) {
		if(c == '"') {
			escaped.push_back('\\');
		}
		escaped.push_back(c);
	}
	return escaped;
}
