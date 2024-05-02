#include "socket.h"

namespace caizi{
    
    Socket::Socket(int family, int type, int protocol):
                    m_sock(-1),m_family(family),m_type(type),m_protocol(protocol),m_isConnect(false){};
    Socket::~Socket(){
        close();
    }

    Socket::ptr Socket::createTCP(caizi::Address::ptr address){
        Socket::ptr sock(new Socket(address->getFamily(), TCP, 0));
        return sock;
    }
    Socket::ptr Socket::createUDP(caizi::Address::ptr address){
        Socket::ptr sock(new Socket(address->getFamily(), UDP, 0));
        sock->newSock();
        sock->m_isConnect = true;
        return sock;
    }
    // 创建IPV4套接字
    Socket::ptr Socket::CreateTCPSocket(){}
    Socket::ptr Socket::CreateUDPSocket(){}
    // 创建IPV6套接字
    Socket::ptr Socket::CreateTCPSocket6(){}
    Socket::ptr Socket::CreateUDPSocket6(){}

    int64_t Socket::getSendTimeout(){}
    void Socket::setSendTimeout(int64_t timeout){}
    int64_t Socket::getRecvTimeout(){}
    void Socket::setRecvTimeout(int64_t timeout){}
    
    bool Socket::getOption(int level, int option, void* result, socklen_t* length){}
    bool Socket::setOption(int level, int option, void* result, socklen_t length){}

    Address::ptr Socket::getRemoteAddress(){}
    Address::ptr Socket::getLocalAddress(){}
    int Socket::getSocket() const { return m_sock;}
    int Socket::getFamily() const {return m_family;};
    int Socket::getType() const {return m_type;};
    int Socket::getProtocol() const {return m_protocol;};
    bool Socket::isConnect() const {return m_isConnect;};
    bool Socket::isValid() const{}
    int Socket::getError(){}

    bool Socket::cancelRead() const{}
    bool Socket::cancelWrite() const{}
    bool Socket::cancelAccept() const{}
    bool Socket::cancelAll() const{}

    void Socket::initSock(){}
    void Socket::newSock(){
        
    }
}