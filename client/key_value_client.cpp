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
#include<sstream>


void msg(const char *msg){
    std::cout<<msg<<std::endl;
}

void die(const char *msg){
    int err = errno;
    std::cout<<err<<" "<<msg<<std::endl;
    abort();
}

void print(const char * buf, size_t n){
    // std::cout<<"Reading the data"<<std::endl;
    for(int i=0; i<(int)n; ++i){
        std::cout<<buf[i];
    }
    std::cout<<std::endl;
}

ssize_t read_full(int connfd,char *buf, ssize_t n){
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

    // std::cout<<"Number of bytes read:"<<sizeof(buf)<<std::endl;

    return 0;
}

ssize_t write_all(int connfd, const char *buf, ssize_t n){
    std::cout<<buf<<std::endl;
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

uint32_t k_max_msg = 4096;
static int32_t write_req(int connfd, std::vector<std::string> &request){
    
    // std::cout<<"Hello"<<std::endl;
    uint32_t len=4;
    for(std::string temp:request){
        len += 4+temp.size();
    }
    if(4+len>k_max_msg){
        msg("Message too long");
        return -1;
    }
    uint32_t nstr=request.size();
    char wbuf[k_max_msg+4];
    memcpy(&wbuf[0], (uint8_t *)&len, 4);
    memcpy(&wbuf[4], (uint8_t *)&nstr, 4);

    // std::cout<<"Hi"<<std::endl;
    // std::cout<<"Length of string:"<<len<<" No of strings:"<<nstr<<std::endl;

    size_t curr=8;
    for(std::string temp:request){
        uint32_t size=temp.size();
        memcpy(&wbuf[curr],(uint8_t *)&size, 4);
        memcpy(&wbuf[curr+4], temp.data(), size);
        curr += 4+size;
    }
    // printf(wbuf,len);

    return write_all(connfd, wbuf, len+4);
}

static int32_t read_res(int connfd){

    char rbuf[k_max_msg+4];  //declare read buffer
    uint32_t len=0;     //length of the response
    errno=0;   //clear the previous error
    int rv = read_full(connfd,rbuf, 4);
    if(rv<0){
        std::cerr<<strerror(errno)<<std::endl;
        return -1;
    }
    memcpy(&len, rbuf, 4);
    // std::cout<<"Size of the response:"<<len<<std::endl;

    rv = read_full(connfd, &rbuf[4], len);
    if(rv<0){
        std::cerr<<strerror(errno)<<std::endl;
        return -1;
    }
    uint32_t rescode=0;
    memcpy(&rescode,&rbuf[4],4);

    std::cout<<"Response state:"<<rescode<<std::endl;
    if(rbuf[8]){
        std::cout<<"Response:";
        print(&rbuf[8],len-4);
    }
    

    // std::cout<<rbuf.data()<<std::endl;
    // print(rbuf.data()+4, len);
    


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
    while(true){

        // std::vector<std::vector<std::string>> query={{"set", "name","krishna"}};
        // query.push_back({"get", "name"});
        // query.push_back({"set", "age","20"});
        // query.push_back({"get", "age"});
        // query.push_back({"n", "sir"});

        std::cout<<"Enter the get (or) set (or) del command (or) q to quit:";

        std::string line;
        std::getline(std::cin, line);
        std::stringstream ss(line);
        std::vector<std::string> req;
        std::string word;

        while(ss >> word){
            if(word=="q"){
                break;
            }
            req.push_back(word);
        }
        
        // std::cout<<"size of the query:"<<txt.size()<<std::endl;
        // for(std::vector<std::string> &req:query){
            int err=write_req(fd,req);
            if(err<0){
                std::cerr<<strerror(errno)<<std::endl;
                // break;
            }
        // }
        // for(int i=0; i<(int)query.size(); i++){
            err = read_res(fd);
            if(err<0){
                std::cerr<<strerror(errno)<<std::endl;
                // break;
            }
        // }
    }
    close(fd);

    return 0;
}