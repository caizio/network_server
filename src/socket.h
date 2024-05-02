#ifndef __CAIZI_SOCKET_H__
#define __CAIZI_SOCKET_H__

#include <memory>
#include <sys/types.h>
#include <sys/socket.h>
#include <openssl/err.h>
#include <openssl/ssl.h>

#include "noncopyable.h"
#include "address.h"


namespace caizi{

class Socket: public std::enable_shared_from_this<Socket>, Noncopyable{
public:
    typedef std::shared_ptr<Socket> ptr;
    typedef std::weak_ptr<Socket> wptr;

    enum Type{
        TCP = SOCK_STREAM,
        UDP = SOCK_DGRAM
    };

    enum Family {
        IPv4 = AF_INET,
        IPv6 = AF_INET6,
        Unix = AF_UNIX
    };

    Socket(int family, int type, int protocol = 0);
    ~Socket();

    static Socket::ptr createTCP(caizi::Address::ptr address);
    static Socket::ptr createUDP(caizi::Address::ptr address);
    // 创建IPV4套接字
    static Socket::ptr CreateTCPSocket();
    static Socket::ptr CreateUDPSocket();
    // 创建IPV6套接字
    static Socket::ptr CreateTCPSocket6();
    static Socket::ptr CreateUDPSocket6();

    int64_t getSendTimeout();
    void setSendTimeout(int64_t timeout);
    int64_t getRecvTimeout();
    void setRecvTimeout(int64_t timeout);
    
    bool getOption(int level, int option, void* result, socklen_t* length);
    template<typename T>
    bool getOption(int level, int option, T& result){
        socklen_t length = sizeof(T);
        return getOption(level, option, &result, &length);
    }
    bool setOption(int level, int option, void* result, socklen_t length);
    template<typename T>
    bool setOption(int level, int option, const T& result){
        return setOption(level, option, &result, sizeof(T));
    }

    virtual Socket::ptr accept();
    virtual bool bind(const Address::ptr addr);
    virtual bool connect(const Address::ptr addr, uint64_t timeout_ms = -1);
    virtual bool reconnect(uint64_t timeout_ms = -1);
    virtual bool listen(int backlog = SOMAXCONN);
    virtual bool close();

    virtual int send(const void* buf, size_t size, int flags = 0);
    virtual int send(const iovec* buf, size_t size, int flags = 0);
    virtual int sendTo(const void* buf, size_t size, const Address::ptr addr, int flags = 0);
    virtual int sendTo(const iovec* buf, size_t size, const Address::ptr addr, int flags = 0);

    virtual int recv(void* buf, size_t size, int flags = 0);
    virtual int recv(iovec* buf, size_t size, int flags = 0);
    virtual int recvFrom(void* buf, size_t size, Address::ptr addr, int flags = 0);
    virtual int recvFrom(iovec* buf, size_t size, Address::ptr addr, int flags = 0);

    Address::ptr getRemoteAddress();
    Address::ptr getLocalAddress();
    int getSocket() const;
    int getFamily() const;
    int getType() const;
    int getProtocol() const;
    bool isConnect() const;
    bool isValid() const;
    int getError();

    bool cancelRead() const;
    bool cancelWrite() const;
    bool cancelAccept() const;
    bool cancelAll() const;

    virtual std::ostream& dump(std::ostream& os) const;
    virtual std::string toString() const;

protected:
    void initSock();
    void newSock();
    virtual bool init(int sock);

protected:
    int m_sock;
    int m_family;
    int m_type;
    int m_protocol;
    bool m_isConnect;
    Address::ptr m_localAddress;
    Address::ptr m_remoteAddress;
};


// SSL套接字，待实现
class SSLSocket : public Socket{
};



}




#endif