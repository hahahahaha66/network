#include "../include/server.hpp"
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <future>
#include <memory>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
 
void server::set_unlocking(int fd) {
    int flags = fcntl(fd,F_GETFL,0);
    if(fcntl(fd,F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("fcntl failed");
    }
}

server::server() {
    socket_fd = socket(AF_INET,SOCK_STREAM,0);
    if(socket_fd == -1) {
        perror("socket failed");
        exit(EXIT_FAILURE);
        return ;
    }

    sock_addr.sin_family=AF_INET;
    sock_addr.sin_port=htons(PORT);
    inet_pton(AF_INET,IP,&sock_addr.sin_addr);

    int bind_return = bind(socket_fd,(sockaddr* )&sock_addr,sizeof(sock_addr));
    if(bind_return == -1){
        perror("bind failed");
        exit(EXIT_FAILURE);
        return ;
    }

    epfd = epoll_create(1);

}

void server::establishing_session(int cliend_fd) {
    int cliend_file = 0;
    
    while(1){
        string order;
        vector<string> result;
        order.resize(128);
        int bytes = recv(cliend_fd,order.data(),order.size(),0);
        
        if(bytes > 0) {
            send(cliend_fd,order.data(),bytes,0);
            result = split(order);
            if(result[0] == "PASV") {
                future<int> result = async(launch::async,&server::establishing_data_connection,this,cliend_fd);
                cliend_file = result.get();
            }
            else if(result[0] == "LIST") {
                thread read_dir(&server::server_read_catelog,this,cliend_file,result);
                read_dir.detach();
            }
            else if(result[0] == "STOR") {
                thread upload_file(&server::server_upload_file,this,cliend_file,result);
                upload_file.detach();
            }
            else if(result[0] == "RETR") {
                thread download_file(&server::server_download_file,this,cliend_file,result);
                download_file.detach();
            }
            else {
                cout<<order<<endl;
            }
        }
        else if(bytes == -1 && errno == EAGAIN) {
            break;
        }
        else if(bytes == -1 && errno != EAGAIN) {
            cout<<"a error occured ..."<<endl<<"actively disconnect"<<endl;
            cout<<strerror(errno);
            close(cliend_fd);
            break;
        }
        else if(bytes == 0) {
            cout<<"client disconnect"<<endl;
            close(cliend_fd);
            break;
        }
    }
}

vector<string> server::split(string order) {
    vector<string> result;
    int start = 0, end;
    
    while((end = order.find(' ',start)) != string::npos) {
        result.push_back(order.substr(start,end-start));
        start = end+1;
    }

    result.push_back(order.substr(start));
    return result;   
}

void server::printf_permission(filesystem::directory_entry path) {
    try {
        filesystem::file_status filestatus = path.status();
        filesystem::perms permissions = filestatus.permissions();
        std::cout << std::oct << (static_cast<int>(permissions) & static_cast<int>(filesystem::perms::owner_read) ? 'r' : '-') // 用户读取权限
          << (static_cast<int>(permissions) & static_cast<int>(filesystem::perms::owner_write) ? 'w' : '-') // 用户写入权限
          << (static_cast<int>(permissions) & static_cast<int>(filesystem::perms::owner_exec) ? 'x' : '-')   // 用户执行权限
          << (static_cast<int>(permissions) & static_cast<int>(filesystem::perms::group_read) ? 'r' : '-')   // 组读取权限
          << (static_cast<int>(permissions) & static_cast<int>(filesystem::perms::group_write) ? 'w' : '-')  // 组写入权限
          << (static_cast<int>(permissions) & static_cast<int>(filesystem::perms::group_exec) ? 'x' : '-')    // 组执行权限
          << (static_cast<int>(permissions) & static_cast<int>(filesystem::perms::others_read) ? 'r' : '-')  // 其他用户读取权限
          << (static_cast<int>(permissions) & static_cast<int>(filesystem::perms::others_write) ? 'w' : '-') // 其他用户写入权限
          << (static_cast<int>(permissions) & static_cast<int>(filesystem::perms::others_exec) ? 'x' : '-')   // 其他用户执行权限
          <<' ';
    }
    catch(const filesystem::filesystem_error &e) {
        cerr<<"Error: "<<e.what()<<endl;
    }
    return ;
}

void server::server_read_catelog(int cliend_file,vector<string> result) {
    std::filesystem::path work_path;
    string write_buffer;
    write_buffer.resize(1024);
    if(result.size() == 1) {
        std::filesystem::path work_path=std::filesystem::current_path();
    }
    else {
        work_path = result[1];
    }
    try {
        for(const auto &entry : filesystem::directory_iterator(work_path)) {
            printf_permission(entry);

            cout<<entry.file_size()<<' ';

            auto file_time = entry.last_write_time();
            auto sctp = std::chrono::time_point_cast<chrono::system_clock::duration>(file_time - filesystem::file_time_type::clock::now() + std::chrono::system_clock::now());
            time_t cftime = chrono::system_clock::to_time_t(sctp);

            cout<<ctime(&cftime)<<' ';
            cout<<entry.path().filename()<<endl;
        }
    }
    catch (const filesystem::filesystem_error &e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return ;
}

void server::server_upload_file(int cliend_file,vector<string> result) {

}

void server::server_download_file(int cliend_file,vector<string> result) {

}

int server::establishing_data_connection(int cliend_fd) {
    int file_fd = socket(AF_INET,SOCK_STREAM,0);
    if(file_fd == -1){
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    char pasv_resp[64];
    struct sockaddr_in file_addr;
    
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> dis(10000,50000);

    int file_port=0;
    do {file_port = dis(gen);

        memset(&file_addr, 0, sizeof(file_addr));

        cout<<file_port<<endl;
        file_addr.sin_family=AF_INET;
        inet_pton(AF_INET,IP,&file_addr.sin_addr);
        file_addr.sin_port=htons(file_port);
     
    } while((bind(file_fd,(sockaddr*)&file_addr,sizeof(file_addr)) == -1));
    
    //snprintf(pasv_resp, sizeof(pasv_resp), "227 entering passive mode (127,0,0,1,%d,%d)\r\n",file_port/256,file_port%256);
    snprintf(pasv_resp, sizeof(pasv_resp), "%d %d",file_port/256,file_port%256);
    cout<<pasv_resp<<endl;
    send(cliend_fd,pasv_resp,strlen(pasv_resp),0);
    
    if(listen(file_fd,10) == -1) {
        
        perror("listen failed");
        close(file_fd);
        exit(EXIT_FAILURE);
    }
   
    int cliend_file = accept(file_fd,NULL,NULL);
    if(cliend_file == -1) {
        perror("accept failed");
        close(file_fd);
        exit(EXIT_FAILURE);
    }
   
    set_unlocking(cliend_file);

    cout<<"The data connection channel was created successfully"<<endl;

    return cliend_file;
}

server::~server() {
    close(socket_fd);
}


void server::server_accept_with_comminicate() {
    if(listen(socket_fd,10) == -1){
        perror("listen failed");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }

    set_unlocking(socket_fd);
    ev.events = EPOLLIN;
    ev.data.fd = socket_fd;
    if (epoll_ctl(epfd,EPOLL_CTL_ADD,socket_fd,&ev) == -1) {
        perror("epoll_ctl failed");
    }

     while(1){
        int nfds = epoll_wait(epfd,event,MAX_EVENT,-1);
        for(int i = 0;i < nfds; i++){

            if(event[i].data.fd == socket_fd) {
                int client_fd = accept(socket_fd,NULL,NULL);
                if(client_fd == -1) {
                    perror("accept failed");
                    continue;
                }
                else {
                    cout<<"cliend_fd : "<<client_fd<<" connected"<<endl;
                }
                set_unlocking(client_fd);
                ev.events = EPOLLIN | EPOLLET;
                ev.data.fd = client_fd;
                if(epoll_ctl(epfd,EPOLL_CTL_ADD,client_fd,&ev) == -1) {
                    perror("epoll_ctl failed");
                    close(client_fd);
                }
            }
            else{
                int temp=event[i].data.fd;
                thread th(&server::establishing_session,this,temp);
                th.detach();
            }
        }
    }
}

int main() {
    server haha;
    haha.server_accept_with_comminicate();
    return 0;
}