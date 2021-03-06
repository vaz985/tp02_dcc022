#ifndef TABLE_H
#define TABLE_H
#include <bits/stdc++.h>
#include "route.h"

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

    map<string, string> get_distances(string dest_ip = "");

    void set_my_ip(string ip) {
      this->router_ip = ip;
    }
    
    void add_route(string dest_ip, string source_ip, int weight);

		string get_first_step(string dest_ip);

    map<string, Route> get_best_routes();

    bool is_neighbour(string ip) {
      return (this->neighbours_ip.find(ip)!=this->neighbours_ip.end()) ? true : false;
    }

    map<string, set<Route>> get_routes() {
      return this->known_routes;
    }

    void check_times(int period);
      


  private:
    set<string> neighbours_ip;
    // neighbour -> weight
    map<string, int> neighbours_router_weight;
    // source -> [(weight, neighbour), ...]
    map<string, set<Route>> known_routes;
    // ip -> weight
    map<string, string> distances;

    vector<pair<string, string>> get_routes_best_weights();

    map<string, int>& get_neighbours_weight() {
      return this->neighbours_router_weight;
    };


    void remove_source_route(string ip) {
      this->known_routes.erase(ip);  
    }

    void remove_routes(string ip);

};
#endif
