#ifndef ROUTE_H
#define ROUTE_H
#include <bits/stdc++.h>
using namespace std;


class Route {
  public:
    int weight;
    string target_ip, neighbour_ip;
    struct timespec last_update;

    Route(int weight, string target_ip, string neighbour_ip);

    Route(const Route& r);

    int get_weight() const {
      return this->weight;
    }

    string get_target() const {
      return this->target_ip;
    }

    string get_neighbour() const {
      return this->neighbour_ip;
    }
};



#endif
