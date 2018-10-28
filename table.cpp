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
  if(this->neighbours_router_weight.count(ip))
    this->neighbours_router_weight.erase(ip);

  auto k_routes = this->get_routes();
  for(auto e : k_routes) {
    if(e.first == ip)
      this->remove_source_route(ip);
    else {
      auto route_set =  e.second;
      for(auto route_set_it : route_set) {
        if(route_set_it.get_target() == ip) {
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
    const string& ip = it.first;
    set<Route>& routes = it.second;
    Route best_route = *routes.begin();
    best_weights.push_back( make_pair(ip, to_string(best_route.get_weight())) );
  }
  return best_weights;
}

void Table::add_route(string dest_ip, string source_ip, int weight) {
	if(!neighbours_router_weight.count(source_ip)) {
		cerr << "Trying to add route from non-neighbour address\n";
		return;
	}
	weight += neighbours_router_weight[source_ip];
  auto target_ip = this->known_routes.find(dest_ip);

  if(source_ip == this->router_ip)
    source_ip = dest_ip;

  if(target_ip != this->known_routes.end()) {
    set<Route>& routes = target_ip->second;
    Route new_route = Route(weight, dest_ip, source_ip);
    auto route = routes.find(new_route);
    if(route != routes.end())
      routes.erase(route);
    routes.insert(new_route);
  }
  else {
    set<Route> routes;
    routes.insert(Route(weight, dest_ip, source_ip));
    this->known_routes[dest_ip] = routes;
  }
}

string Table::get_first_step(string dest_ip) {
	if(!this->known_routes.count(dest_ip)) {
		cerr << "Can't find route to " << dest_ip << endl;
		return "";
	}
	
	auto &ip_known_routes = this->known_routes[dest_ip];
	auto best_route = ip_known_routes.begin();
	int weight = best_route->get_weight();
	
	vector<string> possibilities;
	while(best_route != ip_known_routes.end() and best_route->get_weight() == weight) {
		possibilities.push_back(best_route->get_neighbour());
		best_route++;
	}
	assert(possibilities.size());
	
	std::mt19937 mt_rand(time(0));
	size_t chosen = mt_rand() % possibilities.size();
	
	return possibilities[chosen];
}

map<string, Route> Table::get_best_routes() {
  map<string, Route> data;
  for(auto it : this->known_routes) {
    const string& ip = it.first;
    set<Route>& routes = it.second;
    Route r = *routes.begin();
    data.insert( make_pair(ip, r) );
    cout << routes.size() << endl;
  }
  return data;
}
