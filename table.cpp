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
  for(auto& it : this->known_routes) {
    const string& target_ip = it.first;
    set<Route>& routes      = it.second;
    for(auto& route : routes) {
      if(route.get_neighbour() == ip) {
        routes.erase(route);
      }
    }
    if(routes.empty()) {
      this->known_routes.erase(target_ip);
    }
  }
}

void Table::del_edge(string ip) {
  if(!this->neighbours_router_weight.count(ip)) {
    cerr << "Trying to delete non-existent ip " << ip << endl;
    return;
    //exit(1);
  }
  this->neighbours_ip.erase(ip);
  this->neighbours_router_weight.erase(ip);
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
		cerr << "Trying to add route from non-neighbour address(" << source_ip << ")\n";
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

void Table::check_times(int period) {
  //cout << "Checking" << endl;
  struct timespec cur_time;
  clock_gettime(CLOCK_REALTIME, &cur_time);
  for(auto& it : this->known_routes) {
    const string& target_ip = it.first;
    set<Route>& routes      = it.second;
    for(auto& route : routes) {
      if(route.get_neighbour() == this->router_ip) continue;
      if(cur_time.tv_sec - route.get_time().tv_sec > period) {
        //cerr << "Erasing.." << endl;
        routes.erase(route);
      }
    }

    if(routes.empty())
      this->known_routes.erase(target_ip);
  }
}

map<string, string> Table::get_distances(string dest_ip) {
	this->update_distance_list();
	map<string, string> split_horizon;
	for(auto it : this->known_routes) {
    const string& ip = it.first;
		if(ip == dest_ip) continue;
    set<Route>& routes = it.second;
    auto cur_route = routes.begin();
		int best_weight = cur_route->weight;
		bool no_send = false;
		while(cur_route != routes.end() and cur_route->weight == best_weight) {
			if(cur_route->neighbour_ip == dest_ip) {
				no_send = true;
				break;
			}
			cur_route++;
		}
		if(no_send) continue;
		split_horizon[ip] = to_string(best_weight);	
	}
	return split_horizon;
}
