#include "tools.h"
#include "table.h"

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

const double SOCKETTO 0.5
const uint16_t PORT 55151

class Router {
  public:
    string ip;
    uint32_t ip_network, ip_host;
    uint16_t port_network, port_host;
    double period;

    int udp_socket;
    struct sockaddr_in router_addr;
    unsigned int addr_len = sizeof(struct sockaddr_in);

    class Table table;

    Router(string addr, double period) {
      this->port_host    = (uint16_t)PORT;
      this->port_network = htons(this->port_host);
      this->ip           = addr;
      this->ip_host      = (uint32_t)stoi(addr);
      this->ip_network   = ntohl(this->ip_host);
      this->period       = period;

      this->setup_socket();
    };

    Router(int argc, char* argv[]) {
      string addr_cmd    = "--addr";
      string upd_cmd     = "--update-period";
      string startup_cmd = "--startup-commands";

      vector<string> argv_;
      for(int i=1; i<argc; i++)
        argv_.push_back(string(argv[i]));

      map<string, string> args = parse_args(argv_);

      this->port_host    = (uint16_t)PORT;
      this->port_network = htons(this->port_host);
      this->ip         = args[addr_cmd];
      this->ip_host    = (uint32_t)stoi(this->ip);
      this->ip_network = ntohl(this->ip_host);
      this->period     = stod(args[upd_cmd]);
      this->setup_socket();
      this->table.add_edge(this->ip, 0.0);
      if(args.count(startup_cmd)) {
        // Run comands
      }
    }

    ~Router() {
      close(this->udp_socket); 
    };

    void print_configs() {
      cout << "IP:     " << this->ip << endl;
      cout << "Port:   " << this->port_host << endl;
      cout << "Period: " << this->period << endl;
    }


		void treat_input() {
			string op_type;
			cin >> op_type;
			if(op_type == "add") {
				string ip;
				cin >> ip;
				int weight;
				cin >> weight;
				//addEdge(ip, weight);
			}
			else if(op_type == "del"){
				string ip;
				cin >> ip;
				//delEdge(ip);
			}
			else {
				cerr << op_type << " is not a valid operation.\n";
			}
		}

    void send_update_msg() {
      
    }

    void recv_msg() {
    }

  private:

    static map<string, string> parse_args(vector<string> &args) {
      string addr_cmd    = "--addr";
      string upd_cmd     = "--update-period";
      string startup_cmd = "--startup-commands";
      bool error = false;
      map<string, string> args_;
      unsigned int args_n = args.size();
      if(args_n == 2 || args_n == 3) {
        args_[addr_cmd] = args[0];
        args_[upd_cmd]  = args[1];
        if(args_n == 3)
          args_[startup_cmd] = args[2];
      }
      else if(args_n == 4 || args_n == 6) {
        args_[args[0]] = args[1];
        args_[args[2]] = args[3];
        if(args_n == 6)
          args_[args[4]] = args[5];
      }
      else 
        error = true;

      if(error) {
        cerr << "Invalid arguments" << endl;
        exit(1); 
      }
      return args_;
    }

    void setup_socket() {
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
  r.print_configs();
  while(1) {
    r.recv_msg();
  }
  return 0;
}
