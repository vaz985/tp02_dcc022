#ifndef TABLE_H
#define TABLE_H
#include <bits/stdc++.h>
#include "route.h"


struct route_compare {
  bool operator() (const class Route& a, const class Route& b) const {
    int a_w = a.weight;
    int b_w = b.weight;
    return a_w < b_w;
  }
};

using namespace std;
class Table {
  public:
    string router_ip;

    void add_edge(string ip, int weight);
		void del_edge(string ip);
    void update_distance_list();

    set<string> get_neighbours() {
      return this->neighbours_ip;
    }

    map<string, string> get_distances() {
      this->update_distance_list();
      return this->distances;
    }

    void set_my_ip(string ip) {
      this->router_ip = ip;
    }
    
		void add_route(string dest_ip, string source_ip, int weight);

    void add_route_(string dest_ip, string source_ip, int weight);

		string get_first_step(string dest_ip);

    map<string, pair<int, string>> get_best_routes();

    bool is_neighbour(string ip) {
      return (this->neighbours_ip.find(ip)!=this->neighbours_ip.end()) ? true : false;
    }

  private:
    set<string> neighbours_ip;
    // neighbour -> weight
    map<string, int> neighbours_router_weight;
    // source -> [(weight, neighbour), ...]
    map<string, set<pair<int, string>>> known_routes;
    map<string, set<class Route, route_compare>> known_routes_;
    // ip -> weight
    map<string, string> distances;

    vector<pair<string, string>> get_routes_best_weights();

    map<string, int>& get_neighbours_weight() {
      return this->neighbours_router_weight;
    };

    map<string, set<pair<int, string>>> get_routes() {
      return this->known_routes;
    };


    void remove_source_route(string ip) {
      this->known_routes.erase(ip);  
    }

    void remove_routes(string ip);

};
#endif
