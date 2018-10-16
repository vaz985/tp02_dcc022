#include "table.h"

void Table::add_edge(string ip, int weight) {
  if(neighbours_router_weight.count(ip)) {
    cerr << "Add to existing ip " << ip << endl;
    exit(1);
  }
  neighbours_ip.insert(ip);
  neighbours_router_weight[ip] = weight;
  this->add_route(ip, this->router_ip, weight);
  this->update_distance_list();
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
  for(auto it : this->get_routes_best_weights()) 
    this->distances[it.first] = it.second;
}

vector<pair<string, string>> Table::get_routes_best_weights() {
  vector<pair<string, string>> best_weights;
  for(auto it : this->known_routes) {
    string ip = it.first;
    set<pair<int, string>> &ip_known_routes = it.second;
    int best_weight = ip_known_routes.begin()->first;

    pair<string, string> route(ip, to_string(best_weight));
    best_weights.push_back(route);
  }
  return best_weights;
}

void Table::add_route(string dest_ip, string source_ip, int weight) {
  // find a better name
	if(!neighbours_router_weight.count(source_ip)) {
		cerr << "Trying to add route from non-neighbour address\n";
		return;
	}
	weight += neighbours_router_weight[source_ip];
  auto target_ip = this->known_routes.find(dest_ip);
  if(target_ip != this->known_routes.end()) {
    target_ip->second.insert( pair<int, string>(weight, source_ip) );
		this->distances[dest_ip] = to_string(min(stoi(this->distances[dest_ip]), weight));
  }
  else {
    set<pair<int, string>> routes;
    routes.insert( pair<int, string>(weight, source_ip) ); 
    this->known_routes[dest_ip] = routes;
		this->distances[dest_ip] = weight;
  }
}

string Table::get_first_step(string dest_ip) {
	if(!this->known_routes.count(dest_ip)) {
		cerr << "Can't find route to " << dest_ip << endl;
		return "";
	}
	//TODO: implement load balancing here.
	auto &ip_known_routes = this->known_routes[dest_ip];
	return ip_known_routes.begin()->second;
}

map<string, pair<int, string>> Table::get_best_routes() {
  map<string, pair<int, string>> data;
  for(auto it : this->known_routes) {
    string ip = it.first;
    set<pair<int, string>> &ip_known_routes = it.second;
    pair<int, string> route = *ip_known_routes.begin();
    data[ip] = route;
  }
  return data;
}
