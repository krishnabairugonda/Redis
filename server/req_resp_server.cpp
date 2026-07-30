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
#include<string>


void msg(const char *msg){
    std::cout<<msg<<std::endl;
}

void die(const char *msg){
    int err = errno;
    std::cout<<err<<" "<<msg<<std::endl;
    abort();
}

ssize_t read_full(int connfd, char* buf, ssize_t n){
    while(n>0){
        ssize_t rv = read(connfd, buf, n);
        if(rv<=0){
            return -1;  //
        }
        n -= rv;
        buf += rv;
    }
    return 0;
}

ssize_t write_all(int connfd, const char* buf, ssize_t n){
    while(n>0){
        ssize_t rv = write(connfd, buf, n);
        if(rv<=0){
            return -1;  //EOF or error
        }
        n -= rv;
        buf += rv;
    }
    return 0;
}


const uint32_t k_max_msg = 20550;
static int32_t one_request(int connfd){

    //read the data
    uint32_t len = 0;
    char rbuf[k_max_msg+4];
    errno = 0;
    ssize_t err = read_full(connfd, rbuf, 4);
    if(err<0){
        std::cerr<<strerror(errno)<<std::endl;
        return -1;
    }

    memcpy(&len, rbuf, 4);
    if(len>k_max_msg){
        msg("Message too long to store in buffer");
        return -1;
    }

    std::cout<<"Length of the request is:"<<len<<std::endl;

    err = read_full(connfd, rbuf+4, len);
    if(err<0){
        std::cerr<<strerror(errno)<<std::endl;
        return -1;
    }
    
    // std::cout<<"Client says:"<<&rbuf[4]<<std::endl;
    // std::cout<<"Server says:"<<std::string(&rbuf[4], len)<<std::endl;
    printf("server says: %.*s\n", len, &rbuf[4]);

    // printf()


    //response to the data(echoing the data)
    char wbuf[k_max_msg+4];
    // memcpy(wbuf, len, 4);
    memcpy(wbuf, rbuf, len+4);
    return write_all(connfd, wbuf, len+4);
}

//accept connections
static int acceptConn(int fd){
    struct sockaddr_in client_addr = {};
    socklen_t addrlen = sizeof(client_addr);
    int connfd = accept(fd, (struct sockaddr *)&client_addr, &addrlen);

    return connfd;
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
            int32_t err = one_request(connfd);
            if(err){
                break;
            }
        }
        close(connfd);
    }

    return 0;
}