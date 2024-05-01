#include "address.h"
#include <sstream>
#include "endiant.h"
#include "log.h"
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

namespace caizi{

static caizi::Logger::ptr g_logger = CAIZI_GET_LOGGER("address");


// 创建mask
template<class T>
static T CreateMask(uint32_t bits){
    return (1 << (sizeof(T) * 8 - bits)) - 1;
}

/*
    Address基类
*/
Address::ptr Address::create(const sockaddr* addr, socklen_t addrlen){
    if(addr == nullptr){
        return nullptr;
    }

    Address::ptr result;
    switch(addr->sa_family){
        case AF_INET:
            result.reset(new IPv4Address(*(sockaddr_in*)(addr)));
            break;
        case AF_INET6:
            result.reset(new IPv6Address(*(sockaddr_in6*)(addr)));
            break;
        default:
            result.reset(new UnknownAddress(*addr));
            break;
    }
    return result;
}


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

IPAddress::ptr IPAddress::create(const char* address, uint16_t port){
    addrinfo hints, *results;
    memset(&hints, 0, sizeof(addrinfo));

    hints.ai_family = AF_UNSPEC;
    hints.ai_flags = AI_NUMERICHOST;
    // hints是用于解析地址的配置信息，result为结果
    int ret = getaddrinfo(address, nullptr, &hints, &results);
    if(ret){
        LOG_FMT_DEBUG(g_logger, "IPAddress::Create:%s, %d, 创建失败！已返回空指针",address, port);
        return nullptr;
    }
    
    try{
        IPAddress::ptr res = std::dynamic_pointer_cast<IPAddress>(
                            Address::create(results->ai_addr, results->ai_addrlen));
        if(res){
            res->set_port(port);
        }
        freeaddrinfo(results);
        return res;
    }catch(...){
        freeaddrinfo(results);
        return nullptr;
    }
    return nullptr;
}

/*
    IPv4Address
*/
IPv4Address::ptr IPv4Address::create(const char* address, uint16_t port){
    IPv4Address::ptr rt(new IPv4Address());
    rt->m_addr.sin_family = AF_INET;
    rt->m_addr.sin_port = port;
    int result = inet_pton(AF_INET, address, &rt->m_addr.sin_addr);
    if(result <= 0){
        LOG_FMT_DEBUG(g_logger, "IPv4Address::create:%s, %d, 创建IPv4失败！已返回空指针",address,result);
        return nullptr;
    }
    return rt;
}

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

// IPv6地址有零压缩机制
// 每一块多个前导的0可以省略，一个块全为0可以简写为一个0，多个块为连续0的话可以简写为:: (地址表示中仅能有一个::)
// URL中IPv6地址必须加[]
// 待完善
std::ostream& IPv6Address::insert(std::ostream& os) const{
    uint16_t* addr = (uint16_t*)m_addr.sin6_addr.s6_addr;
    os << "[";
    bool used_zero = false;
    for(int i = 0; i < 8; i++){
        if(addr[i] == 0 && !used_zero){
            continue;
        }
        if(i && addr[i - 1] == 0 && !used_zero) {
            os << ":";
            used_zero = true;
        }
        if(i) {
            os << ":";
        }
        if(i) os << ":";
        os << std::hex << (int)byteswapOnLitterEndian(addr[i]) << std::dec;
    }

    os << "]:" << byteswapOnLitterEndian(m_addr.sin6_port);
    return os;
}

IPAddress::ptr IPv6Address::broadcastAddress(uint32_t prefix_len) const{
    return nullptr;
}
IPAddress::ptr IPv6Address::networkAddress(uint32_t prefix_len) const{
    return nullptr;
}
IPAddress::ptr IPv6Address::subnetMask(uint32_t prefix_len) const{
    return nullptr;
}

uint32_t IPv6Address::get_port() const{
    return byteswapOnLitterEndian(m_addr.sin6_port);
}
void IPv6Address::set_port(uint16_t port){
    m_addr.sin6_port = byteswapOnLitterEndian(port);
}

/*
    UnixAddress地址
*/
UnixAddress::UnixAddress(const std::string& path){

}

const sockaddr* UnixAddress::getAddr() const{
    return (sockaddr*)&m_addr;
};
socklen_t UnixAddress::getAddrLen() const{
    return m_length;
};
std::ostream& UnixAddress::insert(std::ostream& os) const{
    return os;
};

/*
    未知地址
*/
UnknownAddress::UnknownAddress(int family){};
UnknownAddress::UnknownAddress(const sockaddr& addr){};
const sockaddr* UnknownAddress::getAddr() const{
    return nullptr;
};
sockaddr* UnknownAddress::getAddr() {
    return nullptr;
};
socklen_t UnknownAddress::getAddrLen() const{
    return 0;
};
std::ostream& UnknownAddress::insert(std::ostream& os) const{
    return os;
};

}