#include "tools.h"


map<string, string> json2Map(string input) {
	input = input.substr(input.find('{') + 1);
	while(input.back() != '}') input.pop_back();
	input.pop_back();
	while(input.back() == ' ' or input.back() == '\n') input.pop_back();

	map<string, string> data;
	string key;
	string value;
	size_t pos = 0;
	bool quote_open = false;
	bool getting_key = true;
	while(pos < input.size()) {
		if(!quote_open and (input[pos] == ' ' or input[pos] == '\n' or input[pos] == ',' or input[pos] == '\t' or input[pos] == ':')) {
			pos++;
			continue;
		}
		if(input[pos] == '"') {
			if(pos == 0 or input[pos - 1] != '\\') {
				if(quote_open) {
					quote_open = false;
					if(!getting_key) {
						data[key] = value;
						key.clear();
						value.clear();
						getting_key = true;
					}
					else {
						getting_key = false;
					}
				}
				else {
					quote_open = true;
				}
				pos++;
				continue;
			}
		}
		assert(quote_open);
		if(getting_key) key.push_back(input[pos]);
		else value.push_back(input[pos]);
		pos++;
	}
	return data;
}

string map2Json(const map<string, string> &data) {
	string json;
	json += "{";
	for(auto p : data) {
		json += "\n\t\"";
		json += p.first;
		json += "\": \"";
		json += p.second;
		json += "\",";
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

