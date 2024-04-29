#ifndef __ADDRESS_H__
#define __ADDRESS_H__

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>

namespace caizi{

class Address{
public:
    typedef std::shared_ptr<Address> ptr;
    virtual ~Address();

    static Address::ptr create(const sockaddr* addr, socklen_t addrlen);
    static Address::ptr create(const std::string& ip, uint16_t port);

    int getFamily() const;
    virtual const sockaddr* getAddr() const = 0;
    virtual socklen_t getAddrLen() const = 0;

    // 将地址信息插入到输出流
    virtual std::ostream& insert(const std::ostream& os) const;
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

    const sockaddr* getAddr() const override;
    IPAddress::ptr broadcastAddress(uint32_t prefix_len) const override;
    IPAddress::ptr networkAddress(uint32_t prefix_len) const override;
    IPAddress::ptr subnetMask(uint32_t prefix_len) const override;

    uint32_t get_port() const override;

private:
    sockaddr_in m_addr;
};

}

#endif