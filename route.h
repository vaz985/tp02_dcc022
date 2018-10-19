#ifndef ROUTE_H
#define ROUTE_H
#include <bits/stdc++.h>
using namespace std;


class Route {
  public:
    int weight;
    string target_ip, neighbour_ip;

    Route(int weight, string target_ip, string neighbour_ip);

    int get_weight() {
      return this->weight;
    }
};



#endif
