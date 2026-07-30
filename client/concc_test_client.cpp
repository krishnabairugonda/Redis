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
#include<vector>


void msg(const char *msg){
    std::cout<<msg<<std::endl;
}

void die(const char *msg){
    int err = errno;
    std::cout<<err<<" "<<msg<<std::endl;
    abort();
}

void print(const uint8_t * buf, size_t n){
    std::cout<<"Reading the data"<<std::endl;
    for(int i=0; i<(int)n; ++i){
        std::cout<<buf[i];
    }
    std::cout<<std::endl;
}

ssize_t read_full(int connfd, uint8_t *buf, ssize_t n){
    // int k=0;
    while(n>0){
        ssize_t rv = read(connfd, buf, n);
        if(rv<0){
            return -1;  //
        }
        n -= rv;
        buf += rv;
        // k += rv;
    }

    std::cout<<"Number of bytes read:"<<sizeof(buf)<<std::endl;

    return 0;
}

ssize_t write_all(int connfd, const uint8_t *buf, ssize_t n){
    while(n>0){
        ssize_t rv = write(connfd, buf, n);
        if(rv<0){
            return -1;  //EOF or error
        }
        n -= rv;
        buf += rv;
        // std::cout<<"Wrote "<<rv<<" bytes of data"<<std::endl;
    }
    return 0;
}

static void buf_append(std::vector<uint8_t> &buf, const uint8_t* data, size_t n){
    buf.insert(buf.end(), data, data+n);
}

uint32_t k_max_msg = 32<<20;
static int32_t write_req(int connfd, const uint8_t *text, size_t n){
    
    // std::cout<<"Hello babe"<<std::endl;
    //write to the server
    // char wbuf[k_max_msg+4];
    std::vector<uint8_t> wbuf;
    uint32_t len = n;

    std::cout<<"Length of the request is:"<<len<<std::endl<<std::endl;

    if(len>k_max_msg){
        msg("Request msg is too long");
        return -1;
    }
    
    buf_append(wbuf, (uint8_t *)&len, 4);
    buf_append(wbuf, text, len);

    // std::cout<<"Size of the request:"<<wbuf.size()<<endl;

    return write_all(connfd, wbuf.data(), wbuf.size());
}

static int32_t read_res(int connfd){

    std::vector<uint8_t> rbuf;  //declare read buffer
    uint32_t len=0;     //length of the response
    errno=0;   //clear the previous error
    int rv = read_full(connfd, (uint8_t *)&len, 4);
    if(rv<0){
        std::cerr<<strerror(errno)<<std::endl;
        return -1;
    }

    std::cout<<"Size of the response:"<<len<<std::endl;

    rbuf.resize(4+len);
    rv = read_full(connfd, rbuf.data()+4, len);
    if(rv<0){
        std::cerr<<strerror(errno)<<std::endl;
        return -1;
    }

    // std::cout<<rbuf.data()<<std::endl;
    print(rbuf.data()+4, len);

    return 0;
}


int main() {
    
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    int val = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));

    sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(1234); //port
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);   //wildcard IP 0.0.0.0

    int rv = connect(fd, (const struct sockaddr *)&addr, sizeof(addr));

    if(rv){
        die("connect()");
    }
    // while(true){

        // std::string txt;
        // std::cout<<std::endl<<"Enter the text to be sent:";
        // std::getline(std::cin, txt);

        // std::cout<<txt.size()<<std::endl<<std::endl;
        std::vector<std::string> query={"hello1", "hello2",std::string (32<<20, 'x'), "hello3"};
        
        // std::cout<<"size of the query:"<<txt.size()<<std::endl;
        for(std::string &txt:query){
            int err = write_req(fd, (uint8_t *)txt.data(), txt.size());
            if(err<0){
                std::cerr<<strerror(errno)<<std::endl;
                // break;
            }
        }
        for(int i=0; i<(int)query.size(); i++){
            int err = read_res(fd);
            if(err<0){
                std::cerr<<strerror(errno)<<std::endl;
                // break;
            }
        }
    // }
    close(fd);

    return 0;
}