#ifndef __ADDRESS_H__
#define __ADDRESS_H__

#include <iostream>
#include <ostream>
#include <string>
#include <vector>
#include <memory>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/un.h>

namespace caizi{

class Address{
public:
    typedef std::shared_ptr<Address> ptr;
    virtual ~Address();

    static Address::ptr create(const sockaddr* addr, socklen_t addrlen);
    static Address::ptr create(const std::string& ip, uint16_t port);

    // 协议簇
    int getFamily() const;
    // 针对常量成员函数和非常量成员函数获取地址
    virtual const sockaddr* getAddr() const = 0;
    virtual sockaddr* getAddr() = 0;
    virtual socklen_t getAddrLen() const = 0;

    // 将地址信息插入到输出流
    virtual std::ostream& insert(std::ostream& os) const;
    std::string toString() const;

    // 用于地址的比较
    bool operator<(const Address& rhs) const;
    bool operator==(const Address& rhs) const;
    bool operator!=(const Address& rhs) const;
};

class IPAddress : public Address{
public:
    typedef std::shared_ptr<IPAddress> ptr;

    // 获取IP地址的广播地址
    virtual IPAddress::ptr broadcastAddress(uint32_t prefix_len) const = 0;
    // 获取ip地址的网段
    virtual IPAddress::ptr networkAddress(uint32_t prefix_len) const = 0;
    // 获取子网掩码地址
    virtual IPAddress::ptr subnetMask(uint32_t prefix_len) const = 0;
    virtual uint32_t get_port() const = 0;
    virtual void set_port(uint16_t) = 0;
};

// IPV4 address
class IPv4Address : public IPAddress{
public:
    typedef std::shared_ptr<IPv4Address> ptr;
    static IPv4Address::ptr create(const char* address, uint16_t port = 0);
    IPv4Address(const sockaddr_in& address);
    IPv4Address(uint32_t address = INADDR_ANY, uint16_t port = 0);
    ~IPv4Address();
    
    const sockaddr* getAddr() const override;
    sockaddr* getAddr() override;
    socklen_t getAddrLen() const override;
    IPAddress::ptr broadcastAddress(uint32_t prefix_len) const override;
    IPAddress::ptr networkAddress(uint32_t prefix_len) const override;
    IPAddress::ptr subnetMask(uint32_t prefix_len) const override;

    uint32_t get_port() const override;
    void set_port(uint16_t) override;

    std::ostream& insert(std::ostream& os) const override;
private:
    sockaddr_in m_addr;
};

class IPv6Address : public IPAddress{
public:
    typedef std::shared_ptr<IPv6Address> ptr;
    static IPv4Address::ptr create(const char* address, uint16_t port = 0);
    IPv6Address();
    IPv6Address(const sockaddr_in6& address);
    IPv6Address(const uint8_t address[16], uint16_t port = 0);

    const sockaddr* getAddr() const override;
    sockaddr* getAddr() override;
    socklen_t getAddrLen() const override;
    std::ostream& insert(std::ostream& os) const override;

    IPAddress::ptr broadcastAddress(uint32_t prefix_len) const override;
    IPAddress::ptr networkAddress(uint32_t prefix_len) const override;
    IPAddress::ptr subnetMask(uint32_t prefix_len) const override;

    uint32_t get_port() const override;
    void set_port(uint16_t) override;
private:
    sockaddr_in6 m_addr;
};

// 待实现
class UnixAddress : public Address{
public:
    typedef std::shared_ptr<UnixAddress> ptr;
    UnixAddress(const std::string& path);

    const sockaddr* getAddr() const override;
    socklen_t getAddrLen() const override;
    std::ostream& insert(std::ostream& os) const override;
private:
    struct sockaddr_un m_addr;
    socklen_t m_length;
};

// 待实现
class UnknownAddress: public Address{
public:
    typedef std::shared_ptr<UnknownAddress> ptr;
    UnknownAddress(int family);
    UnknownAddress(const sockaddr& addr);
    const sockaddr* getAddr() const override;
    sockaddr* getAddr() override;
    socklen_t getAddrLen() const override;
    std::ostream& insert(std::ostream& os) const override;
private:
    sockaddr m_addr;    
};

}

#endif