#include "table.h"

void Table::add_edge(string ip, int weight) {
  if(neighbours_router_weight.count(ip)) {
    cerr << "Add to existing ip " << ip << endl;
    exit(1);
  }
  neighbours_router_weight[ip] = weight;
}

void Table::del_edge(string ip) {
  if(!neighbours_router_weight.count(ip)) {
    cerr << "Trying to delete non-existent ip " << ip << endl;
    exit(1);
  }
  neighbours_router_weight.erase(ip);
}

void Table::update_distance_list() {
  this->distances.clear();
  for(auto it : neighbours_router_weight)
    this->distances[it.first] = it.second;
  for(auto it : this->get_routes_best_weights()) 
    this->distances[it.first] = it.second;
}

vector<pair<string, double>> Table::get_routes_best_weights() {
  vector<pair<string, double>> best_weights;
  for(auto it : this->known_routes) {
    string ip = it.first;
    set<pair<double, string>> ip_known_routes = it.second;
    double best_weight = ip_known_routes.begin()->first;

    pair<string, double> route(ip, best_weight);
    best_weights.push_back(route);
  }
  return best_weights;
}
