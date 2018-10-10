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
    string ip;
    uint32_t ip_network, ip_host;
    uint16_t port_network, port_host;
    double period;

    int udp_socket;
    struct sockaddr_in router_addr;
    unsigned int addr_len = sizeof(struct sockaddr_in);

    Router(string addr, double period) {
      this->port_host    = (uint16_t)PORT;
      this->port_network = htons(this->port_host);
      this->ip           = addr;
      this->ip_host      = (uint32_t)stoi(addr);
      this->ip_network   = ntohl(this->ip_host);
      this->period       = period;

      this->setupSocket();
    };

    Router(int argc, char* argv[]) {
      string addr, filename;
      string addr_cmd    = "--addr";
      string upd_cmd     = "--update-period";
      string startup_cmd = "--startup-commands";

      double period;
      map<string, string> args;
      bool startup = false;
      if(argv[1][0] == '-') {
        args[string(argv[1])] = string(argv[2]);
        args[string(argv[3])] = string(argv[4]);
        if(argc > 5) {
          args[string(argv[5])] = string(argv[6]);
          startup = true;
        }
      }
      else {
        args[addr_cmd] = string(argv[1]);
        args[upd_cmd]  = string(argv[2]);
        if(argc > 3) {
          args[startup_cmd] = string(argv[3]);
          startup = true;
        }
      }

      this->port_host    = (uint16_t)PORT;
      this->port_network = htons(this->port_host);
      this->ip         = args[addr_cmd];
      this->ip_host    = (uint32_t)stoi(this->ip);
      this->ip_network = ntohl(this->ip_host);
      this->period     = stod(args[upd_cmd]);
      this->setupSocket();
      if(startup) {
        // chama
      } 
    }

    ~Router() {
      close(this->udp_socket); 
    };

    void printConfigs() {
      cout << "IP:     " << this->ip << endl;
      cout << "Port:   " << this->port_host << endl;
      cout << "Period: " << this->period << endl;
    }

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

int main(int argc, char* argv[]) {
  Router r(argc, argv);
  r.printConfigs();
  return 0;
}