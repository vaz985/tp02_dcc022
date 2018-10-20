#include "route.h"

Route::Route(int weight, string target_ip, string neighbour_ip) {
  this->weight = weight;
  this->target_ip = target_ip;
  this->neighbour_ip = neighbour_ip;
}

Route::Route(const Route& r) {
  this->weight = r.weight;
  this->target_ip = r.target_ip;
  this->neighbour_ip = r.neighbour_ip;
}
