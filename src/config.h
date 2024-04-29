#include <memory>
#include <string>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <map>
#include <yaml-cpp/yaml.h>

#include "thread.h"

namespace caizi{

// 配置项基类
class ConfigVarBase{
public:
    typedef std::shared_ptr<ConfigVarBase> ptr;

    ConfigVarBase(const std::string &name, const std::string &description):
        m_name(name),m_description(description){};
    virtual ~ConfigVarBase() = default;

    const std::string& getName() const { return m_name; }
    const std::string& getDesccription() const { return m_description; }

    virtual std::string toString() const = 0;
    virtual bool fromString(const std::string &val) = 0;
protected:
    std::string m_name;
    std::string m_description;
};

template <typename Source, typename Target>
class LexicalCast{
public:
    Target operator()(const Source& source){
        return boost::lexical_cast<Target>(source);
    }
};

/*  
    通用型配置项类模板，继承自 ConfigVarBase，用于管理各种类型的配置项。
    包含了配置项的具体值和相关操作方法，如获取值、设置值、转换为字符串等。
*/
template<
    class T
    // class FromStringFN = LexicalCast<std::string, T>>
>
class ConfigVar : public ConfigVarBase{
public:
    typedef std::shared_ptr<ConfigVar> ptr;

    ConfigVar(const std::string &name, const T &value, const std::string &descriptopm):
        ConfigVarBase(name, descriptopm), m_value(value){};

    void setValue(const T value){
        WriteScopeLock lock(&m_mutex);
        m_value = value;
    }
    T getValue() const{ 
        ReadScopeLock lock(&m_mutex);
        return m_value;
    };

    std::string toString() const override{
        try{
            return boost::lexical_cast<std::string>(m_value);
        }catch(std::exception &e){
            std::cerr << "ConfogVal::toString exception " 
                << e.what()
                << " "
                << typeid(m_value).name()
                << std::endl;
        }
        return "<error>";
    };

    bool fromString(const std::string &val) override{
        try{
            m_value = boost::lexical_cast<T>(val);
        }catch(std::exception &e){
                std::cerr << "ConfogVal::fromString exception " 
                << e.what()
                << " "
                << val
                << std::endl;
        }
        return false;
    }

private:
    T m_value;
    mutable RWLock m_mutex;
};

// 配置项的管理类
class Config{
public:
    typedef std::map<std::string, ConfigVarBase::ptr> ConfigVarMap;

    // 寻找配置项
    static ConfigVarBase::ptr Lookup(const std::string &name){
        ConfigVarMap &m_data = getData();
        auto iter = m_data.find(name);
        if(iter == m_data.end()){
            return nullptr;
        }
        return iter->second;
    }

    // 返回指定类型T的配置项
    template<class T>
    static typename ConfigVar<T>::ptr Lookup(const std::string &name){
        ReadScopeLock lock(&getRWlock());
        ConfigVarBase::ptr base_ptr = Lookup(name);
        if(!base_ptr) return nullptr;
        auto ptr = std::dynamic_pointer_cast<ConfigVar<T>>(base_ptr);
        if(!ptr){
            std::cerr << "Config::Lookup<T>，无法转化到类型T" << std::endl;
            throw std::bad_cast();
        }
        return ptr;
    }

    // 更新配置项
    template<class T>
    static typename ConfigVar<T>::ptr Lookup(const std::string &name, const T&value, const std::string &description = ""){
        auto temp = Lookup<T>(name);
        if(temp) return temp;
        if(name.find_first_of("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789._") == std::string::npos){
            std::cerr << "Config:Loopup exception, 参数只能以数字、点、下划线为开头" << std::endl;
        }
        auto v = std::make_shared<ConfigVar<T>>(name, value, description);
        WriteScopeLock lock(&getRWlock());
        getData()[name] = v;
        return v;
    }

    // 从YAML::Node中载入配置
    static void LoadFromYAML(const YAML::Node &root){
        std::vector<std::pair<std::string, YAML::Node>> nodes_list;
        TraversalNode(root,"",nodes_list);
        for(const auto &node:nodes_list){
            std::string key = node.first;
            if(key.empty()) continue;
            std::transform(key.begin(), key.end(), key.begin(), ::tolower);
            auto var = Lookup(key);
            if(var){
                std::stringstream ss;
                ss << node.second;
                var->fromString(ss.str());
            }
        }
    }

private:
    // 解析YAML::Node对象，存到列表中并返回
    static void TraversalNode(const YAML::Node &node, const std::string &name, 
                            std::vector<std::pair<std::string, YAML::Node>> &output){
        // 将 YAML::Node 存入 output
        auto output_iter = std::find_if(output.begin(),output.end(),
            [&name](const std::pair<std::string, YAML::Node>& item) {
                return item.first == name;
            });
        if (output_iter != output.end()){
            output_iter->second = node;
        }else{
            output.push_back(std::make_pair(name, node));
        }
        // 当 YAML::Node 为映射型节点，使用迭代器遍历
        if (node.IsMap()){
            for (auto iter = node.begin(); iter != node.end(); ++iter){
                TraversalNode(
                    iter->second,
                    name.empty() ? iter->first.Scalar()
                                 : name + "." + iter->first.Scalar(),
                    output);
            }
        }
        // 当 YAML::Node 为序列型节点，使用下标遍历
        if (node.IsSequence()){
            for (size_t i = 0; i < node.size(); ++i){
                TraversalNode(node[i], name + "." + std::to_string(i), output);
            }
        }
    }

    static ConfigVarMap& getData(){
        static ConfigVarMap m_data;
        return m_data;
    }

    static RWLock& getRWlock(){
        static RWLock m_lock;
        return m_lock;
    }
};

std::ostream& operator<<(std::ostream & out, const ConfigVarBase &cvb);

};
