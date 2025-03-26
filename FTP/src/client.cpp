#include "../include/client.hpp"
#include <cstdio>
#include <cstring>
#include <future>
#include <sstream>
#include <sys/socket.h>

client::client() {
    client_sock=socket(AF_INET, SOCK_STREAM, 0);
    if(client_sock == -1) {
        perror("socket failed");
    }

    server_addr.sin_family=AF_INET;
    server_addr.sin_port=htons(PORT);
    inet_pton(AF_INET,SERVER_IP,&server_addr.sin_addr);

}

void client::client_connect() {
    if(connect(client_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("connect failed");
    }
    printf("connect to server\n");
}

void client::client_control() {
    int server_file = 0;
    char buffer[1024] = {0};

    while(1) {
        string order;
        vector<string> result;
        
        cin>>order;
      
        result=split(order);

        memset(buffer,0, sizeof(buffer));

        if(result[0] == "q" || result[0] == "quit") {
            shutdown(client_sock, SHUT_WR);
        }

    
        if(order.size() > 0){

            send(client_sock, order.c_str(), order.size(), 0);
        }

        int bytes_received = recv(client_sock,buffer, sizeof(buffer), 0);

        if(bytes_received <= 0) {
            printf("server disconnected\n");
            break;
        }
        else {
            buffer[bytes_received] = '\0';
            cout<<"server recvived : "<<buffer<<endl;
        }

        if(result[0] == "PASV") {
            future<int> result = async(launch::async,&client::client_data_connectivity,this);
            server_file = result.get();
        }
        else if(result[0] == "LIST") {
            if(server_file == 0) {
                cout<<"data connection channel not established"<<endl;
                continue;
            }

            thread read_dir(&client::client_read_catelog,this,server_file);
            read_dir.detach();
        }
        else if(result[0] == "STOR") {
            if(server_file == 0) {
                cout<<"data connection channel not established"<<endl;
                continue;
            }

            thread upload_file(&client::client_upload_file,this,server_file);
            upload_file.detach();
        }
        else if(result[0] == "RETR") {
            if(server_file == 0) {
                cout<<"data connection channel not established"<<endl;
                continue;
            }

            thread download_file(&client::client_download_file,this,server_file);
            download_file.detach();
        }
    }
}

vector<string> client::split(string order) {
    vector<string> result;
    int start = 0, end;
    
    while((end = order.find(' ',start)) != string::npos) {
        result.push_back(order.substr(start,end-start));
        start = end+1;
    }

    result.push_back(order.substr(start));
    return result;
}

void client::client_read_catelog(int server_file) {
    
}

void client::client_download_file(int server_file) {

}

void client::client_upload_file(int server_file) {

}

int client::client_data_connectivity() {
    string port_recvived;
    port_recvived.resize(128);

    int byte_recvivied = recv(client_sock,port_recvived.data(),port_recvived.size(),0);
    
    if(byte_recvivied > 0) {
        cout<<"recvived : "<<port_recvived<<endl;
    }
    else {
        cout<<"get port failed"<<endl;
        //perror("recv failed");
        return -1;
    }
    int data_port = analysis_port(port_recvived);

    int server_file = socket(AF_INET,SOCK_STREAM,0);

    data_addr.sin_family = AF_INET;
    data_addr.sin_port=htons(data_port);
    inet_pton(AF_INET,SERVER_IP,&data_addr.sin_addr);

    if(connect(server_file, (struct sockaddr*)&data_addr, sizeof(data_addr)) == -1) {
        perror("data transfer connect failed");
    }
    
    return server_file;
}

int client::analysis_port(string port) {
    stringstream haha(port);
    int m,n;
    haha>>m>>n;
    return m*256+n;
}



client::~client() {
    close(client_sock);
}

int main() {
    client haha;
    haha.client_connect();
    haha.client_control();
    return 0;
}