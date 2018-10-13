#ifndef TABLE_H
#define TABLE_H
#include <bits/stdc++.h>
using namespace std;
class Table {
  public:
    string router_ip;

    void add_edge(string ip, int weight);
		void del_edge(string ip);
    void update_distance_list();

    vector<string> get_neighbours() {
      return this->neighbours_ip;
    }

    map<string, string> get_distances() {
      return this->distances;
    }

    void set_my_ip(string ip) {
      this->router_ip = ip;
    }

  private:
    vector<string> neighbours_ip;
    // neighbour -> weight
    map<string, double> neighbours_router_weight;
    // ip -> [(weight, neighbour), ...]
    map<string, set<pair<double, string>>> known_routes;
    // ip -> weight
    map<string, string> distances;

    vector<pair<string, string>> get_routes_best_weights();

    void add_route(string dest_ip, string source_ip, double weight);

};
#endif
