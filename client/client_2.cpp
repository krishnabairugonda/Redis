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

uint32_t k_max_msg = 20550;
static int32_t query(int connfd, const char* text, const ssize_t n){
    
    //write to the server
    char wbuf[k_max_msg+4];
    uint32_t len = n;

    // std::cout<<"Length of the request is:"<<len<<std::endl<<std::endl;
    

    if(len>k_max_msg){
        msg("Request msg is too long");
        return -1;
    }
    memcpy(wbuf, &len, 4);
    memcpy(wbuf+4, text, len);
    errno=0;
    std::cout<<"Sending length:"<<len<<std::endl<<std::endl;
    int32_t err = write_all(connfd, wbuf, len+4);
    if(err<0){
        std::cerr<<strerror(errno)<<std::endl;    
        return -1;
    }

    //read from the server
    char rbuf[k_max_msg+4];
    err = read_full(connfd, rbuf, 4);
    if(err<0){
        std::cerr<<strerror(errno)<<std::endl;    
        return -1;
    }

    err = read_full(connfd, rbuf+4, len);
    if(err<0){
        std::cerr<<strerror(errno)<<std::endl;    
        return -1;
    }

    std::cout<<"Server says:"<<std::string(&rbuf[4], len)<<std::endl;

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
        // std::string txt = "is a preprocessor macro that is used for error indication and reporting errors that occur during a function call. It contains error codes so if a call to a function fails somehow then the errno is set to a value that corresponds to the error is a preprocessor macro that is used for error indication and reporting errors that occur during a function call. It contains error codes so if a call to a function fails somehow then the errno is set to a value that corresponds to the error is a preprocessor macro that is used for error indication and reporting errors that occur during a function call. It contains error codes so if a call to a function fails somehow then the errno is set to a value that corresponds to the error is a preprocessor macro that is used for error indication and reporting errors that occur during a function call. It contains error codes so if a call to a function fails somehow then the errno is set to a value that corresponds to the error is a preprocessor macro that is used for error indication and reporting errors that occur during a function call. It contains error codes so if a call to a function fails somehow then the errno is set to a value that corresponds to the error is a preprocessor macro that is used for error indication and reporting errors that occur during a function call. It contains error codes so if a call to a function fails somehow then the errno is set to a value that corresponds to the error is a preprocessor macro that is used for error indication and reporting errors that occur during a function call. It contains error codes so if a call to a function fails somehow then the errno is set to a value that corresponds to the error is a preprocessor macro that is used for error indication and reporting errors that occur during a function call. It contains error codes so if a call to a function fails somehow then the errno is set to a value that corresponds to the error is a preprocessor macro that is used for error indication and reporting errors that occur during a function call. It contains error codes so if a call to a function fails somehow then the errno is set to a value that corresponds to the error is a preprocessor macro that is used for error indication and reporting errors that occur during a function call. It contains error codes so if a call to a function fails somehow then the errno is set to a value that corresponds to the error is a preprocessor macro that is used for error indication and reporting errors that occur during a function call. It contains error codes so if a call to a function fails somehow then the errno is set to a value that corresponds to the error is a preprocessor macro that is used for error indication and reporting errors that occur during a function call. It contains error codes so if a call to a function fails somehow then the errno is set to a value that corresponds to the error is a preprocessor macro that is used for error indication and reporting errors that occur during a function call. It contains error codes so if a call to a function fails somehow then the errno is set to a value that corresponds to the error is a preprocessor macro that is used for error indication and reporting errors that occur during a function call. It contains error codes so if a call to a function fails somehow then the errno is set to a value that corresponds to the erroris a preprocessor macro that is used for error indication and reporting errors that occur during a function call. It contains error codes so if a call to a function fails somehow then the errno is set to a value that corresponds to the error is a preprocessor macro that is used for error indication and reporting errors that occur during a function call. It contains error codes so if a call to a function fails somehow then the errno is set to a value that corresponds to the error is a preprocessor macro that is used for error indication and reporting errors that occur during a function call. It contains error codes so if a call to a function fails somehow then the errno is set to a value that corresponds to the error is a preprocessor macro that is used for error indication and reporting errors that occur during a function call. It contains error codes so if a call to a function fails somehow then the errno is set to a value that corresponds to the error is a preprocessor macro that is used for error indication and reporting errors that occur during a function call. It contains error codes so if a call to a function fails somehow then the errno is set to a value that corresponds to the error is a preprocessor macro that is used for error indication and reporting errors that occur during a function call. It contains error codes so if a call to a function fails somehow then the errno is set to a value that corresponds to the error is a preprocessor macro that is used for error indication and reporting errors that occur during a function call. It contains error codes so if a call to a function fails somehow then the errno is set to a value that corresponds to the error is a preprocessor macro that is used for error indication and reporting errors that occur during a function call. It contains error codes so if a call to a function fails somehow then the errno is set to a value that corresponds to the error is a preprocessor macro that is used for error indication and reporting errors that occur during a function call. It contains error codes so if a call to a function fails somehow then the errno is set to a value that corresponds to the error is a preprocessor macro that is used for error indication and reporting errors that occur during a function call. It contains error codes so if a call to a function fails somehow then the errno is set to a value that corresponds to the error is a preprocessor macro that is used for error indication and reporting errors that occur during a function call. It contains error codes so if a call to a function fails somehow then the errno is set to a value that corresponds to the error is a preprocessor macro that is used for error indication and reporting errors that occur during a function call. It contains error codes so if a call to a function fails somehow then the errno is set to a value that corresponds to the error is a preprocessor macro that is used for error indication and reporting errors that occur during a function call. It contains error codes so if a call to a function fails somehow then the errno is set to a value that corresponds to the error is a preprocessor macro that is used for error indication and reporting errors that occur during a function call. It contains error codes so if a call to a function fails somehow then the errno is set to a value that corresponds to the erroris a preprocessor macro that is used for error indication and reporting errors that occur during a function call. It contains error codes so if a call to a function fails somehow then the errno is set to a value that corresponds to the error is a preprocessor macro that is used for error indication and reporting errors that occur during a function call. It contains error codes so if a call to a function fails somehow then the errno is set to a value that corresponds to the error is a preprocessor macro that is used for error indication and reporting errors that occur during a function call. It contains error codes so if a call to a function fails somehow then the errno is set to a value that corresponds to the error is a preprocessor macro that is used for error indication and reporting errors that occur during a function call. It contains error codes so if a call to a function fails somehow then the errno is set to a value that corresponds to the error is a preprocessor macro that is used for error indication and reporting errors that occur during a function call. It contains error codes so if a call to a function fails somehow then the errno is set to a value that corresponds to the error is a preprocessor macro that is used for error indication and reporting errors that occur during a function call. It contains error codes so if a call to a function fails somehow then the errno is set to a value that corresponds to the error is a preprocessor macro that is used for error indication and reporting errors that occur during a function call. It contains error codes so if a call to a function fails somehow then the errno is set to a value that corresponds to the error is a preprocessor macro that is used for error indication and reporting errors that occur during a function call. It contains error codes so if a call to a function fails somehow then the errno is set to a value that corresponds to the error is a preprocessor macro that is used for error indication and reporting errors that occur during a function call. It contains error codes so if a call to a function fails somehow then the errno is set to a value that corresponds to the error is a preprocessor macro that is used for error indication and reporting errors that occur during a function call. It contains error codes so if a call to a function fails somehow then the errno is set to a value that corresponds to the error is a preprocessor macro that is used for error indication and reporting errors that occur during a function call. It contains error codes so if a call to a function fails somehow then the errno is set to a value that corresponds to the error is a preprocessor macro that is used for error indication and reporting errors that occur during a function call. It contains error codes so if a call to a function fails somehow then the errno is set to a value that corresponds to the error is a preprocessor macro that is used for error indication and reporting errors that occur during a function call. It contains error codes so if a call to a function fails somehow then the errno is set to a value that corresponds to the error is a preprocessor macro that is used for error indication and reporting errors that occur during a function call. It contains error codes so if a call to a function fails somehow then the errno is set to a value that corresponds to the erroris a preprocessor macro that is used for error indication and reporting errors that occur during a function call. It contains error codes so if a call to a function fails somehow then the errno is set to a value that corresponds to the error is a preprocessor macro that is used for error indication and reporting errors that occur during a function call. It contains error codes so if a call to a function fails somehow then the errno is set to a value that corresponds to the error is a preprocessor macro that is used for error indication and reporting errors that occur during a function call. It contains error codes so if a call to a function fails somehow then the errno is set to a value that corresponds to the error is a preprocessor macro that is used for error indication and reporting errors that occur during a function call. It contains error codes so if a call to a function fails somehow then the errno is set to a value that corresponds to the error is a preprocessor macro that is used for error indication and reporting errors that occur during a function call. It contains error codes so if a call to a function fails somehow then the errno is set to a value that corresponds to the error is a preprocessor macro that is used for error indication and reporting errors that occur during a function call. It contains error codes so if a call to a function fails somehow then the errno is set to a value that corresponds to the error is a preprocessor macro that is used for error indication and reporting errors that occur during a function call. It contains error codes so if a call to a function fails somehow then the errno is set to a value that corresponds to the error is a preprocessor macro that is used for error indication and reporting errors that occur during a function call. It contains error codes so if a call to a function fails somehow then the errno is set to a value that corresponds to the error is a preprocessor macro that is used for error indication and reporting errors that occur during a function call. It contains error codes so if a call to a function fails somehow then the errno is set to a value that corresponds to the error is a preprocessor macro that is used for error indication and reporting errors that occur during a function call. It contains error codes so if a call to a function fails somehow then the errno is set to a value that corresponds to the error is a preprocessor macro that is used for error indication and reporting errors that occur during a function call. It contains error codes so if a call to a function fails somehow then the errno is set to a value that corresponds to the error is a preprocessor macro that is used for error indication and reporting errors that occur during a function call. It contains error codes so if a call to a function fails somehow then the errno is set to a value that corresponds to the error is a preprocessor macro that is used for error indication and reporting errors that occur during a function call. It contains error codes so if a call to a function fails somehow then the errno is set to a value that corresponds to the error is a preprocessor macro that is used for error indication and reporting errors that occur during a function call. It contains error codes so if a call to a function fails somehow then the errno is set to a value that corresponds to the erroris a preprocessor macro that is used for error indication and reporting errors that occur during a function call. It contains error codes so if a call to a function fails somehow then the errno is set to a value that corresponds to the error is a preprocessor macro that is used for error indication and reporting errors that occur during a function call. It contains error codes so if a call to a function fails somehow then the errno is set to a value that corresponds to the error is a preprocessor macro that is used for error indication and reporting errors that occur during a function call. It contains error codes so if a call to a function fails somehow then the errno is set to a value that corresponds to the error is a preprocessor macro that is used for error indication and reporting errors that occur during a function call. It contains error codes so if a call to a function fails somehow then the errno is set to a value that corresponds to the error is a preprocessor macro that is used for error indication and reporting errors that occur during a function call. It contains error codes so if a call to a function fails somehow then the errno is set to a value that corresponds to the error is a preprocessor macro that is used for error indication and reporting errors that occur during a function call. It contains error codes so if a call to a function fails somehow then the errno is set to a value that corresponds to the error is a preprocessor macro that is used for error indication and reporting errors that occur during a function call. It contains error codes so if a call to a function fails somehow then the errno is set to a value that corresponds to the error is a preprocessor macro that is used for error indication and reporting errors that occur during a function call. It contains error codes so if a call to a function fails somehow then the errno is set to a value that corresponds to the error is a preprocessor macro that is used for error indication and reporting errors that occur during a function call. It contains error codes so if a call to a function fails somehow then the errno is set to a value that corresponds to the error is a preprocessor macro that is used for error indication and reporting errors that occur during a function call. It contains error codes so if a call to a function fails somehow then the errno is set to a value that corresponds to the error is a preprocessor macro that is used for error indication and reporting errors that occur during a function call. It contains error codes so if a call to a function fails somehow then the errno is set to a value that corresponds to the error is a preprocessor macro that is used for error indication and reporting errors that occur during a function call. It contains error codes so if a call to a function fails somehow then the errno is set to a value that corresponds to the error is a preprocessor macro that is used for error indication and reporting errors that occur during a function call. It contains error codes so if a call to a function fails somehow then the errno is set to a value that corresponds to the error is a preprocessor macro that is used for error indication and reporting errors that occur during a function call. It contains error codes so if a call to a function fails somehow then the errno is set to a value that corresponds to the erroris a preprocessor macro that is used for error indication and reporting errors that occur during a function call. It contains error codes so if a call to a function fails somehow then the errno is set to a value that corresponds to the error is a preprocessor macro that is used for error indication and reporting errors that occur during a function call. It contains error codes so if a call to a function fails somehow then the errno is set to a value that corresponds to the error is a preprocessor macro that is used for error indication and reporting errors that occur during a function call. It contains error codes so if a call to a function fails somehow then the errno is set to a value that corresponds to the error is a preprocessor macro that is used for error indication and reporting errors that occur during a function call. It contains error codes so if a call to a function fails somehow then the errno is set to a value that corresponds to the error is a preprocessor macro that is used for error indication and reporting errors that occur during a function call. It contains error codes so if a call to a function fails somehow then the errno is set to a value that corresponds to the error is a preprocessor macro that is used for error indication and reporting errors that occur during a function call. It contains error codes so if a call to a function fails somehow then the errno is set to a value that corresponds to the error is a preprocessor macro that is used for error indication and reporting errors that occur during a function call. It contains error codes so if a call to a function fails somehow then the errno is set to a value that corresponds to the error is a preprocessor macro that is used for error indication and reporting errors that occur during a function call. It contains error codes so if a call to a function fails somehow then the errno is set to a value that corresponds to the error is a preprocessor macro that is used for error indication and reporting errors that occur during a function call. It contains error codes so if a call to a function fails somehow then the errno is set to a value that corresponds to the error is a preprocessor macro that is used for error indication and reporting errors that occur during a function call. It contains error codes so if a call to a function fails somehow then the errno is set to a value that corresponds to the error is a preprocessor macro that is used for error indication and reporting errors that occur during a function call. It contains error codes so if a call to a function fails somehow then the errno is set to a value that corresponds to the error is a preprocessor macro that is used for error indication and reporting errors that occur during a function call. It contains error codes so if a call to a function fails somehow then the errno is set to a value that corresponds to the error is a preprocessor macro that is used for error indication and reporting errors that occur during a function call. It contains error codes so if a call to a function fails somehow then the errno is set to a value that corresponds to the error is a preprocessor macro that is used for error indication and reporting errors that occur during a function call. It contains error codes so if a call to a function fails somehow then the errno is set to a value that corresponds to the error";
        // txt += '\n';
        std::cout<<txt.size()<<std::endl<<std::endl;
        int err = query(fd, txt.c_str(), txt.size());
        if(err<0){
            std::cerr<<strerror(errno)<<std::endl;
            // break;
            
        }
        
    }
    close(fd);

    return 0;
}