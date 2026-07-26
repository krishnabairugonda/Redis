#include<sys/types.h>
#include<sys/socket.h>
#include<stdint.h>
#include<stdio.h>
#include<arpa/inet.h>
#include<errno.h>
#include<stdlib.h>
#include<netinet/ip.h>
#include<unistd.h>
#include<string.h>
#include<cstdio>
#include<iostream>
#include<assert.h>


void msg(const char *msg){
    std::cout<<msg<<std::endl;
}

void die(const char *msg){
    int err = errno;
    std::cout<<err<<" "<<msg<<std::endl;
    abort();
}


int main() {
    
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    int val = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));

    sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(1234); //port
    addr.sin_addr.s_addr = htonl(0);   //wildcard IP 0.0.0.0

    int rv = connect(fd, (const struct sockaddr *)&addr, sizeof(addr));

    if(rv){
        die("connect()");
    }
    while(true){

        std::string txt;
        std::cout<<"Enter the text to be sent:";
        std::getline(std::cin, txt);

        if(txt=="q"){
            msg("Closing the connection");
            break;
        }
        // txt += '\n';
        
        ssize_t n = write(fd, txt.c_str(), txt.size());
        if(n<0){
            msg("write() error");
            break;
        }
        if(n==0){
            msg("No message sent");
            continue;
        }

        char rbuf[64] = {};
        n =read(fd, rbuf, sizeof(rbuf)-1);
        
        if(n<0){
            die("read");
        }
        if(n==0){
            msg("Connection is closed");
            break;
        }
        
        std::cout<<"Server says:"<<rbuf<<std::endl;
        
    }
    close(fd);

    return 0;
}