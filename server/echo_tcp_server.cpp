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
#include<bits/stdc++.h>
#include<cstdio>
#include<assert.h>


void msg(const char *msg){
    std::cout<<msg<<std::endl;
}

void die(const char *msg){
    int err = errno;
    std::cout<<err<<" "<<msg<<std::endl;
    abort();
}

static int32_t do_something(int connfd){
    char rbuf[64] = {};
    ssize_t n =read(connfd, rbuf, sizeof(rbuf)-1);
    std::cout<<"Number of bytes read:"<<n<<std::endl;
    if(n<0){
        msg("read() error");
        return -1;
    }
    if(n==0){
        msg("Connection closed");
        return -1;
    }
    std::cout<<"Client says:"<<rbuf<<std::endl;
    n = write(connfd, rbuf, strlen(rbuf));
    std::cout<<"Number of bytes sent:"<<n<<std::endl;
    if(n<0){
        msg("write() error");
        return -1;
    }
    if(n==0){
        msg("Connection closed");
        return -1;
    }

    return 0;
}

int main() {

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    int val = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));

    sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(1234); //port
    addr.sin_addr.s_addr = htonl(0);   //wildcard IP 0.0.0.0

    int rv = bind(fd, (const struct sockaddr *)&addr, sizeof(addr));
    if(rv){
        die("bind()");
    }

    //listen
    rv = listen(fd, SOMAXCONN);
    if(rv){
        die("listen()");
    }

    while(true){
        //accept
        struct sockaddr_in client_addr = {};
        socklen_t addrlen = sizeof(client_addr);
        int connfd = accept(fd, (struct sockaddr *)&client_addr, &addrlen);
        if(connfd<0){
            continue; //error
        }
        while(true){
            int32_t err = do_something(connfd);
            if(err){
                break;
            }
        }
        close(connfd);
    }

    return 0;
}