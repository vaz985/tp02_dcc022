#include "tools.h"

int main() {
	string s;
	while(!cin.eof()) {
		string s2;
		getline(cin, s2);
		s += s2;
		s.push_back('\n');
	}
	map<string, string> data = json2Map(s);
	cout << "data from json is\n";
	for(auto p : data) {
		cout << p.first << " " << p.second << endl;
	}
	cout << endl << endl;

	cout << "json from data is\n";
	string ss = map2Json(data);
	cout << ss << endl << endl << endl;
	
	cout << "vector from generated json:\n";
	vector< pair<string, string> > data2 = json2Vec(s);
	
	for(auto p : data2) {
		cout << p.first << " " << p.second << endl;
	}
	cout << endl << endl;

	
	cout << "json from vector data is\n";
	string s2 = vec2Json(data2);
	cout << s2 << endl << endl << endl;

	assert(data == json2Map(s2));

	return 0;
}
