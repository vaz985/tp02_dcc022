#include <bits/stdc++.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

#define PORT 55151
#define SOCKETTO 0.5

using namespace std;

class Router {
  public:
    uint32_t ip_network, ip_host;
    uint16_t port_network, port_host;
    double period;

    int udp_socket;
    struct sockaddr_in router_addr;
    unsigned int addr_len = sizeof(struct sockaddr_in);

    Router(string addr, double period) {
      this->port_host    = (uint16_t)PORT;
      this->port_network = htons(this->port_host);
      this->ip_host      = (uint32_t)stoi(addr);
      this->ip_network   = ntohl(this->ip_host);
      this->period       = period;

      this->setupSocket();
    };

    ~Router() {
      close(this->udp_socket); 
    };
  private:
    void setupSocket() {
      this->udp_socket = socket(AF_INET, SOCK_DGRAM, 0); 

      this->router_addr.sin_family = AF_INET;
      this->router_addr.sin_port   = this->port_network;
      this->router_addr.sin_addr.s_addr = this->ip_network;
      // Add timeout
      
      bind(this->udp_socket, (struct sockaddr *)&this->router_addr, 
           sizeof(struct sockaddr));
    };
};

int main() {

  Router r("127.0.0.1", 1.0);

  return 0;
}
