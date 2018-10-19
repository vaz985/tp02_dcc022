#include "route.h"

Route::Route(int weight, string target_ip, string neighbour_ip) {
  this->weight = weight;
  this->target_ip = target_ip;
  this->neighbour_ip = neighbour_ip;
}
