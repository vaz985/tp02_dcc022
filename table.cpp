#include "table.h"

void Table::add_edge(string ip, int weight) {
  if(neighbours_router_weight.count(ip)) {
    cerr << "Add to existing ip " << ip << endl;
    exit(1);
  }
  neighbours_ip.push_back(ip);
  neighbours_router_weight[ip] = weight;
  this->add_route(ip, this->router_ip, weight);
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

vector<pair<string, string>> Table::get_routes_best_weights() {
  vector<pair<string, string>> best_weights;
  for(auto it : this->known_routes) {
    string ip = it.first;
    set<pair<double, string>> ip_known_routes = it.second;
    double best_weight = ip_known_routes.begin()->first;

    pair<string, string> route(ip, to_string(best_weight));
    best_weights.push_back(route);
  }
  return best_weights;
}

void Table::add_route(string dest_ip, string source_ip, double weight) {
  // find a better name
  auto target_ip = this->known_routes.find(dest_ip);
  if(target_ip != this->known_routes.end()) {
    target_ip->second.insert( pair<double, string>(weight, source_ip) );
  }
  else {
    set<pair<double, string>> routes;
    routes.insert( pair<double, string>(weight, source_ip) ); 
    this->known_routes[dest_ip] = routes;
  }
}
