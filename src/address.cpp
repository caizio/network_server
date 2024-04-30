#include "address.h"
#include <sstream>
#include "endiant.h"
#include "log.h"

namespace caizi{

static caizi::Logger::ptr g_logger = GET_LOGGER("address");


// 创建mask
template<class T>
static T CreateMask(uint32_t bits){
    return (1 << (sizeof(T) * 8 - bits)) - 1;
}

/*
    Address基类
*/
int Address::getFamily() const{
    return getAddr()->sa_family;
}

std::string Address::toString() const{
    std::stringstream ss;
    insert(ss);
    return ss.str();
}

bool Address::operator<(const Address& rhs) const{
    socklen_t minlen = std::min(this->getAddrLen(), rhs.getAddrLen());
    int result = memcmp(this->getAddr(), rhs.getAddr(), minlen);
    if(result < 0){
        return true;
    }else if(result > 0){
        return false;
    }else if(this->getAddrLen() < rhs.getAddrLen()){
        return true;
    }
    return false;
}

bool Address::operator==(const Address& rhs) const{
    return getAddrLen() == rhs.getAddrLen() && memcmp(getAddr(), rhs.getAddr(), getAddrLen()) == 0; 
}

bool Address::operator!=(const Address& rhs) const{
    return !(*this == rhs);
}

/*
    IPv4Address
*/
IPv4Address::IPv4Address(const sockaddr_in& address){
    m_addr = address;
}
IPv4Address::IPv4Address(uint32_t address, uint16_t port){
    m_addr.sin_family = AF_INET;
    m_addr.sin_port = byteswapOnLitterEndian(port);
    m_addr.sin_addr.s_addr = byteswapOnLitterEndian(address);
}
IPv4Address::~IPv4Address(){};

const sockaddr* IPv4Address::getAddr() const{
    return (sockaddr*)&m_addr;
};
sockaddr* IPv4Address::getAddr() {
    return (sockaddr*)&m_addr;
}
socklen_t IPv4Address::getAddrLen() const{
    return sizeof(m_addr);
};

// 广播地址的主机地址在二进制全为1
IPAddress::ptr IPv4Address::broadcastAddress(uint32_t prefix_len) const{
    if(prefix_len > 32){
        return nullptr;
    }
    sockaddr_in r_addr(m_addr);
    r_addr.sin_addr.s_addr |= byteswapOnLitterEndian(CreateMask<uint32_t>(prefix_len));
    return IPv4Address::ptr(new IPv4Address(r_addr));
};

IPAddress::ptr IPv4Address::networkAddress(uint32_t prefix_len) const{
    if(prefix_len > 32){
        return nullptr;
    }
    sockaddr_in r_addr(m_addr);
    r_addr.sin_addr.s_addr &= byteswapOnLitterEndian(CreateMask<uint32_t>(prefix_len));
    return IPv4Address::ptr(new IPv4Address(r_addr));
};

IPAddress::ptr IPv4Address::subnetMask(uint32_t prefix_len) const{
    sockaddr_in submask;
    memset(&submask, 0, sizeof(submask));
    submask.sin_family = AF_INET;
    submask.sin_addr.s_addr = ~byteswapOnLitterEndian(CreateMask<uint32_t>(prefix_len));
    return IPv4Address::ptr(new IPv4Address(submask));
};

uint32_t IPv4Address::get_port() const{
    return byteswapOnLitterEndian(m_addr.sin_port);
};
void IPv4Address::set_port(uint16_t port){
    m_addr.sin_port = byteswapOnLitterEndian(port);
};

// 将IPv4地址信息输入到os中，并返回os
std::ostream& IPv4Address::insert(std::ostream& os) const{
    uint32_t addr = byteswapOnLitterEndian(m_addr.sin_addr.s_addr);
    os  << (addr >> 24) << "." 
        << ((addr >> 16) & 0xff) << "." 
        << ((addr >> 8) & 0xff) << "." 
        << (addr & 0xff)
        << ":" << byteswapOnLitterEndian(m_addr.sin_port); 
    return os;
}

/*
    IPV6 address
*/

IPv6Address::IPv6Address(){
    memset(&m_addr, 0, sizeof(m_addr));
    m_addr.sin6_family = AF_INET6;
}

IPv6Address::IPv6Address(const sockaddr_in6& address){
    m_addr = address;
}
IPv6Address::IPv6Address(const uint8_t address[16], uint16_t port){
    memset(&m_addr, 0, sizeof(m_addr));
    m_addr.sin6_family = AF_INET6; 
    m_addr.sin6_port = byteswapOnLitterEndian(port);
    memcpy(m_addr.sin6_addr.s6_addr, address, 16);
}

const sockaddr* IPv6Address::getAddr() const{
    return (sockaddr*)&m_addr;
}
sockaddr* IPv6Address::getAddr(){
    return (sockaddr*)&m_addr;
}
socklen_t IPv6Address::getAddrLen() const{
    return sizeof(m_addr);
}
std::ostream& IPv6Address::insert(std::ostream& os) const{
    uint16_t* addr = (uint16_t*)m_addr.sin6_addr.s6_addr;
    for(int i = 0; i < 8; i++){

    }
}

IPAddress::ptr IPv6Address::broadcastAddress(uint32_t prefix_len) const{

}
IPAddress::ptr IPv6Address::networkAddress(uint32_t prefix_len) const{

}
IPAddress::ptr IPv6Address::subnetMask(uint32_t prefix_len) const{

}

uint32_t IPv6Address::get_port() const{
    return byteswapOnLitterEndian(m_addr.sin6_port);
}
void IPv6Address::set_port(uint16_t port){
    m_addr.sin6_port = byteswapOnLitterEndian(port);
}


}