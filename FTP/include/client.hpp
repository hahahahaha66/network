#include <arpa/inet.h>
#include <cstdlib>
#include <iostream>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <map>
#include <vector>
#include <thread>
#include <random>
#include <unistd.h>
using namespace std;

constexpr char SERVER_IP[] = "127.0.0.1";
const int PORT = 8081;
const int BUFFER_SIZE = 1024;

class client {
public:
  client();
  ~client();
  void client_connect();
  void client_control();
  int client_data_connectivity();
  void client_read_catelog();
  void client_download_file();
  void client_upload_file();
  int analysis_port(string port);

  int client_sock;
  struct sockaddr_in data_addr;
  struct sockaddr_in server_addr;
  char buffer[];
};