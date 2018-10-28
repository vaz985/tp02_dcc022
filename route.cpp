#include "route.h"

Route::Route(int weight, string target_ip, string neighbour_ip) {
  this->weight = weight;
  this->target_ip = target_ip;
  this->neighbour_ip = neighbour_ip;
  clock_gettime(CLOCK_REALTIME, &this->last_update);
}

Route::Route(const Route& r) {
  this->weight = r.weight;
  this->target_ip = r.target_ip;
  this->neighbour_ip = r.neighbour_ip;
  this->last_update = r.last_update;
}

bool Route::operator< (const class Route& rhs) const {
  int a_w = this->weight;
  int b_w = rhs.weight;
  if(a_w != b_w)
    return a_w < b_w;
  string a_n = this->get_neighbour(), b_n = rhs.get_neighbour();
  return a_n < b_n;
}
