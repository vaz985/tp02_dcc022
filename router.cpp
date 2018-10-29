#include "tools.h"
#include "table.h"

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <thread>

const double SOCKETTO = 0.5;
const uint16_t PORT = 55151;

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

    int get_period() {
      return this->period;
    }

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
      this->table.set_my_ip(this->ip);
      this->table.add_edge(this->ip, 0);
      if(args.count(startup_cmd)) {
        string filename = args[startup_cmd];
        ifstream ifs(filename);
        string buf, ip, op_type;
        int weight;
        while(ifs.good()) {
          if(!(ifs >> op_type))
            break;
			    if(op_type == "add") {
			    	ifs >> ip;
			    	ifs >> weight;
			    	table.add_edge(ip, weight);
			    }
			    else if(op_type == "del"){
            ifs >> ip;
			    	table.del_edge(ip);
          }
        }
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
				table.add_edge(ip, weight);
			}
			else if(op_type == "del"){
				string ip;
				cin >> ip;
				table.del_edge(ip);
			}
			else if(op_type == "trace") {
				string ip;
				cin >> ip;
				send_trace_msg(ip);
			}
      else if(op_type == "dist") {
        map<string, string> distances = this->table.get_distances();
        for(auto it : distances)
          cout << it.first << ": " << it.second << endl;
      }
      else if(op_type == "routes") {
        for(auto it : this->table.get_best_routes()) {
          string target_ip = it.first;
          string neigh_ip  = it.second.get_neighbour();
          int cost = it.second.get_weight();
          cout << "To: " << target_ip << endl;
          cout << "    From: " << neigh_ip << endl;
          cout << "    With cost: " << cost << endl;
          cout << "    Time: ";
          struct timespec time_ = it.second.get_time();
          static char buff[100];
          strftime(buff, sizeof(buff), "%D %T", gmtime(&time_.tv_sec));
          cout << buff << endl;
        }
      }
      else if(op_type == "allroutes") {
        for(auto it : this->table.get_routes()) {
          const string& target_ip = it.first;
          set<Route>& routes = it.second;
          cout << "To: " << target_ip << endl;
          for(auto r : routes) {
            cout << "    From: " << r.get_neighbour() << endl;
            cout << "    With cost: " << r.get_weight() << endl;
            cout << "    Time: ";
            struct timespec time_ = r.get_time();
            static char buff[100];
            strftime(buff, sizeof(buff), "%D %T", gmtime(&time_.tv_sec));
            cout << buff << endl;
          }
        }
      }
			else {
				cerr << op_type << " is not a valid operation.\n";
			}
		}

		void send_trace_msg(string dest_ip) {
			string msg = make_trace_msg(this->ip, dest_ip);
			send_indirect_msg(msg, dest_ip);
		}

    void send_update_msg() {
      map<string, string> distances = table.get_distances();
      set<string> destination_ip = table.get_neighbours();
      for(auto dest_ip : destination_ip) {
        if(dest_ip == this->ip) continue;
        //cout << dest_ip << endl;
        string json_msg = make_update_msg(this->ip, dest_ip, distances);
        this->send_msg(json_msg, dest_ip);
      }
    }

		void send_data_msg(string msg, string dest_ip) {
			string data = make_data_msg(msg, this->ip, dest_ip);
			send_indirect_msg(data, dest_ip);
		}

		void handle_upd_msg(map<string, string> &data) {
			map<string, string> distances = json2Map(data["distances"]);
			string source_ip = data["source"];
			dequotize(source_ip);
      if(!this->table.is_neighbour(source_ip)) {
        cerr << "Receiving update from invalid neighbour" << endl; 
        return;
      }
			for(auto p : distances) {
        string target_ip = p.first;
				int weight = stoi(p.second);
				table.add_route(target_ip, source_ip, weight);
			}
		}

		void handle_trace_msg(map<string, string> data) {
			string dest_ip = data["destination"];
			string source_ip = data["source"];
			dequotize(source_ip);
			dequotize(dest_ip);
			catHop(data, this->ip);
			if(dest_ip == this->ip) {
				send_data_msg(escape(map2Json(data)), source_ip);
			}
			else {
				send_indirect_msg(map2Json(data), dest_ip);
			}
		}
		
		void handle_data_msg(map<string, string> &data) {
			string dest_ip = data["destination"];
			dequotize(dest_ip);
			if(dest_ip == this->ip) {
				cout << "received data:\n" << data["payload"] << endl;
			}
			else {
				send_indirect_msg(map2Json(data), dest_ip);
			}
		}

		void handle_msg(string &s) {
			map<string, string> data = json2Map(s);
			if(!data.count("type")) {
				cerr << "Unexpected json format. There's no type information.\n";
				exit(1);
			}
			string type = data["type"];
			
			if(type == "\"update\"") {
				handle_upd_msg(data);
			}
			else if(type == "\"trace\"") {
				handle_trace_msg(data);
			}
			else if(type == "\"data\"") {
				handle_data_msg(data);
			}
		}

    void erase_expired_routes() {
      this->table.check_times(4*this->period);
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
      struct hostent *host = (struct hostent *) gethostbyname(this->ip.c_str());

      this->router_addr.sin_family = AF_INET;
      this->router_addr.sin_port   = this->port_network;
      //this->router_addr.sin_addr.s_addr = this->ip_network;
     	this->router_addr.sin_addr = *((struct in_addr *)host->h_addr);
      
      // Add timeout
      struct timeval tv;
      tv.tv_sec = 0;
      tv.tv_usec = 100000;
      setsockopt(udp_socket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
      
      bind(this->udp_socket, (struct sockaddr *)&this->router_addr, 
           sizeof(struct sockaddr));
    }

    void send_msg(const string &msg, const string &ip) {
	    struct sockaddr_in to_addr;
	    to_addr.sin_family = AF_INET;
	    to_addr.sin_port = htons(PORT);

	    inet_aton(ip.c_str(), (struct in_addr*) &to_addr.sin_addr.s_addr);
	    
	    static char c_msg[MAX_SZ];

	    char *pos = &c_msg[0];
	    uint32_t sz = htonl((uint32_t)msg.size());

	    memcpy(pos, &sz, sizeof(uint32_t));
	    pos += sizeof(uint32_t);

	    memcpy(pos, msg.c_str(), (uint32_t)msg.size());

	    sendto(this->udp_socket, &c_msg[0], (uint32_t)msg.size() + sizeof(uint32_t), 0, (struct sockaddr *)&to_addr, sizeof(struct sockaddr_in));
    }

		void send_indirect_msg(const string &msg, const string &ip) {
			string neighbour_ip = table.get_first_step(ip);
			if(neighbour_ip == "") {
				cerr << "Unable to send message to " << ip << endl;
			}
			else {
				this->send_msg(msg, neighbour_ip);
			}
		}

};

void IO_handler(Router &r) {
	while(true) {
		r.treat_input();
	}
}

int main(int argc, char* argv[]) {
  Router r(argc, argv);
  r.print_configs();
	
	thread t1(IO_handler, ref(r));
  string s;
  struct timespec last;
  clock_gettime(CLOCK_REALTIME, &last);
  while(1) {
    struct timespec cur_time;
    clock_gettime(CLOCK_REALTIME, &cur_time);
    if(cur_time.tv_sec - last.tv_sec > r.get_period()) {
      r.send_update_msg();
      r.erase_expired_routes();
      clock_gettime(CLOCK_REALTIME, &last);
    }
		s = recvMsg(r.udp_socket);
		if(s.size()) r.handle_msg(s);
	}

	t1.join();
  return 0;
}
