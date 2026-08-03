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
#include<poll.h>
#include<vector>
#include<map>
#include<fcntl.h>


void msg(const char *msg){
    std::cout<<msg<<std::endl;
}

void die(const char *msg){
    int err = errno;
    std::cout<<err<<" "<<msg<<std::endl;
    abort();
}

struct Conn {
    int fd=-1;

    bool want_read = false;
    bool want_write = false;
    bool want_close = false;

    std::vector<uint8_t> incoming;
    std::vector<uint8_t> outgoing;
};

struct Response{
    uint32_t status=0;
    std::vector<uint8_t> data;
};

//blocking to non blocking
static void fd_set_nb(int fd){
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) | O_NONBLOCK);
}

static void print(const int fd, const uint8_t * buf, size_t n){
    std::cout<<"Client "<<fd <<" says:";
    for(int i=0; i<(int)n; ++i){
        std::cout<<buf[i];
    }
    std::cout<<std::endl;
}

Conn* handle_accept(int fd){
    struct sockaddr_in client_addr = {};
    socklen_t addrlen = sizeof(client_addr);
    int connfd = accept(fd, (struct sockaddr *)&client_addr, &addrlen);
    
    if(connfd<0){
        return NULL;
    }

    //set the fd connection to non blocking
    fd_set_nb(connfd);

    uint32_t ip = client_addr.sin_addr.s_addr;
    std::cout<<"Connected to a new client:";
    std::cout<<(ip & 255)<<"."<<((ip >> 8) & 255)<<"."<<((ip >> 16) & 255)<<"."<<(ip >> 24)<<":"<<client_addr.sin_port<<std::endl;

    Conn* conn = new Conn();
    conn->fd = connfd;
    conn->want_read = true;  //read the 1st request

    return conn;
}

static void buf_append(std::vector<uint8_t> &buf, const uint8_t* data, size_t n){
    buf.insert(buf.end(), data, data+n);
}

static void buf_consume(std::vector<uint8_t> &buf,  size_t n){
    buf.erase(buf.begin(), buf.begin()+n);
}

static bool read_32(const uint8_t *&curr, uint32_t &size, const uint8_t *end){
    if(curr+4>end){
        return false;
    }
    // std::cout<<curr<<std::endl;
    memcpy(&size, curr, 4);
    curr += 4;
    return true;
}

static bool read_str(const uint8_t *&curr,const uint8_t *end, uint32_t len, std::string &out){
    if((curr+len)>end){
        return false;
    }
    out.assign(curr, curr+len);
    curr += len;
    return true;
}

enum {
    RES_OK=0,
    RES_NX=1,
    RES_ERR=2,
};

const uint32_t k_max_msg = 4096;
static int32_t parse_request(const uint8_t *data, size_t size, std::vector<std::string> &cmd){
    const uint8_t *end=data+size;
    uint32_t nstr=0;

    if(!read_32(data, nstr, end)){
        return -1;
    }
    if(nstr>k_max_msg){
        return -1;
    }
    // std::cout<<"No of strings:"<<nstr<<std::endl;

    while(cmd.size()<nstr){
        uint32_t len=0;
        if(!read_32(data, len, end)){
            return -1;
        }
        cmd.push_back(std::string());
        if(!read_str(data, end, len, cmd.back())){
            return -1;
        }
        // std::cout<<"Lenght:"<<len<<" string:"<<cmd.back()<<std::endl;
    }
    
    if(data!=end){
        msg("Traling garbage!");
        return -1; //trailing garbage
    }
    // if(data!=end){
    //     std::cout<<"data!=end"<<std::endl;
    // }
    // else if(data==end){
    //     std::cout<<"data==end"<<std::endl;
    // }
    return 0;
}

std::map<std::string,std::string> get_data;
static void do_request(std::vector<std::string> &cmd, Response &resp){
    // std::cout<<"Printing the commands:";
    // for(std::string s:cmd){
    //     std::cout<<s<<" ";
    // }std::cout<<std::endl;
    if(cmd.size()==2 && cmd[0]=="get"){
        auto it = get_data.find(cmd[1]);
        if(it==get_data.end()){
            std::cout<<"No GET"<<std::endl;
            resp.status = RES_NX;
            return;
        }
        std::string val=it->second;
        resp.data.assign(val.begin(), val.end());
        std::cout<<"Requested GET:"<<val<<std::endl;
    }
    else if(cmd.size()==3 && cmd[0]=="set"){
        get_data[cmd[1]].swap(cmd[2]);
    }
    else if(cmd.size()==2 && cmd[0]=="del"){
        get_data.erase(cmd[0]);
    }
    else {
        resp.status=RES_ERR;
        std::string val="Bad Request";
        resp.data.assign(val.begin(),val.end());
        // std::
    }
}

static void make_response(Response &resp, std::vector<uint8_t> &out){
    uint32_t len = 4+(uint32_t)resp.data.size();
    buf_append(out, (uint8_t *)&len, 4);
    buf_append(out, (uint8_t *)&resp.status, 4);
    buf_append(out, resp.data.data(), resp.data.size());
}

static bool try_one_request(Conn* conn){

    if(conn->incoming.size()<4){
        return false;
    }
    uint32_t len=0;
    // print(conn->fd, conn->incoming.data(), 4);
    memcpy(&len,conn->incoming.data(),4);
    // std::cout<<"Length of the string:"<<len<<std::endl;
    if(len>k_max_msg){
        msg("Msg too long");
        return false;
    }
    if(4+len>conn->incoming.size()){
        msg("Full data not arrived");
        return false;
    }

    uint8_t *request=&conn->incoming[4];
    // print(conn->fd,request,len);
    // std::cout<<"Length of strings:"<<len<<std::endl;
    std::vector<std::string> cmd;
    if(parse_request(request, len, cmd)<0){
        conn->want_close=true;
        return false;
    }


    Response resp;
    do_request(cmd,resp);
    make_response(resp,conn->outgoing);

    // std::cout<<"Size of the incoming:"<<conn->incoming.size()<<std::endl;
    // std::cout<<"Before consume:";
    // print(conn->fd, conn->incoming.data(), len+4);
    buf_consume(conn->incoming,4+len);


    // std::cout<<"After consume:";
    // print(conn->fd, conn->incoming.data(), len+4);
    // std::cout<<"Size of the incoming:"<<conn->incoming.size()<<std::endl;

    return true;  //success
}

static void handle_write(Conn* conn){

    assert(conn->outgoing.size()>0);
    ssize_t rv = write(conn->fd, conn->outgoing.data(), conn->outgoing.size());
    if(rv<0){
        std::cout<<strerror(errno)<<std::endl;
        conn->want_close = true;
        return;
    }
    //remove written data from the outgoing
    std::cout<<"Written "<<rv<<" bytes"<<std::endl;
    buf_consume(conn->outgoing, (size_t)rv);

    if(conn->outgoing.size()==0){
        conn->want_read = true;
        conn->want_write = false;
    }
}

static void handle_read(Conn* conn){
    uint8_t buf[64 * 1024];
    errno=0;
    ssize_t rv = read(conn->fd, buf, sizeof(buf)-1);
    if(rv<0 && errno==EAGAIN){
        std::cerr<<"Error:"<<strerror(errno)<<std::endl;
        return;
    }
    if(rv<0){
        conn->want_close = true;
        std::cerr<<"Error:"<<strerror(errno)<<std::endl;
        return;
    }
    if(rv==0){
        conn->want_close=true;
        if(conn->incoming.size()==0){
            return;
        }
        else {
            std::cout<<"Unexpected EOF"<<std::endl;
        }
    }

    buf_append(conn->incoming, buf, (size_t)rv);
    // print(conn->fd, buf+4, (size_t)rv-4);

    while(try_one_request(conn)){}
    // try_one_request(conn);

    if(conn->outgoing.size()>0){
        // print(conn->fd, conn->outgoing.data(), conn->outgoing.size());
        conn->want_read=false;
        conn->want_write=true;
        return handle_write(conn);
    }
}

int main() {

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd<0){
        die("socket()");
    }
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
    //set the listen fd to nonblocking mode
    fd_set_nb(fd);

    //listen
    rv = listen(fd, SOMAXCONN);
    if(rv){
        die("listen()");
    }

    std::vector<Conn*> fd2conn;
    std::vector<struct pollfd> poll_arg;
    while(true){
        //accept
        poll_arg.clear();
        //put the listening sockets in the first position
        struct pollfd pfd = {fd, POLLIN, 0};
        poll_arg.push_back(pfd);

        // std::cout<<"New client:"<<fd<<std::endl;
        for(Conn* conn:fd2conn){
            if(!conn){
                continue;
            }
            struct pollfd pfd = {conn->fd, POLLERR, 0};
            
            if(conn->want_read){
                pfd.events |= POLLIN;
            }
            if(conn->want_write){
                pfd.events |= POLLOUT;
            }
            poll_arg.push_back(pfd);
        }
        
        //wait for readiness
        int rv = poll(poll_arg.data(), (nfds_t)poll_arg.size(), -1);

        if(rv<0 && errno==EINTR){
            continue;
        }
        if(rv<0){
            die("poll()");
        }

        //handle the listeining socket
        if(poll_arg[0].revents){
            if(Conn* conn = handle_accept(fd)){
                if(fd2conn.size() <= (size_t)conn->fd){
                    fd2conn.resize(conn->fd+1);
                }
                assert(!fd2conn[conn->fd]);
                fd2conn[conn->fd] = conn;
            }
        }

        //handle connection sockets
        for(size_t i=1; i<poll_arg.size(); ++i){
            uint32_t ready = poll_arg[i].revents;
            Conn* conn = fd2conn[poll_arg[i].fd];
            if(ready==0){
                continue;
            }
            if(ready & POLLIN){
                assert(conn->want_read);
                handle_read(conn);
            }
            if(ready & POLLOUT){
                assert(conn->want_write);
                handle_write(conn);
            }
            //terminate the connection
            if(ready & POLLERR || conn->want_close){
                std::cout<<"Closing the connection:"<<conn->fd<<std::endl;
                (void)close(conn->fd);
                fd2conn[conn->fd]=NULL;
                delete conn;
            }
        }
    }

    return 0;
}