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

void Table::remove_routes(string ip) {
  auto n_weight = this->get_neighbours_weight();
  if(!n_weight.count(ip))
    this->neighbours_router_weight.erase(ip);
  auto k_routes = this->get_routes();
  for(auto e : k_routes) {
    if(e.first == ip)
      this->remove_source_route(ip);
    else {
      auto route_set =  e.second;
      for(auto route_set_it : route_set) {
        if(route_set_it.second == ip) {
          route_set.erase(route_set_it);
        }
        if(route_set.empty())
          this->remove_source_route(e.first);
      }
    }
  }

}

void Table::del_edge(string ip) {
  if(!neighbours_router_weight.count(ip)) {
    cerr << "Trying to delete non-existent ip " << ip << endl;
    exit(1);
  }
  neighbours_router_weight.erase(ip);
  this->remove_routes(ip);
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

  if(source_ip == this->router_ip)
    source_ip = dest_ip;

  if(target_ip != this->known_routes.end()) {
    target_ip->second.insert( pair<int, string>(weight, source_ip) );
  }
  else {
    set<pair<int, string>> routes;
    routes.insert( pair<int, string>(weight, source_ip) ); 
    this->known_routes[dest_ip] = routes;
		//this->distances[dest_ip] = weight;
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
