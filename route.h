#ifndef ROUTE_H
#define ROUTE_H
#include <bits/stdc++.h>
using namespace std;


class Route {
  public:
    int weight;
    string target_ip, neighbour_ip;
    struct timespec last_update;

  bool operator< (const class Route& rhs) const;

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
    
    void update_time() {
      clock_gettime(CLOCK_REALTIME, &this->last_update);
    }

    struct timespec get_time() const {
      return this->last_update;
    }
};



#endif
