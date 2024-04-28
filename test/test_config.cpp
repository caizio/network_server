#include "config.h"
#include "log.h"
#include "yaml-cpp/yaml.h"
#include <cstdint>
#include <iostream>
#include <list>
#include <map>
#include <ostream>
#include <vector>

caizi::ConfigVar<int>::ptr int_port = caizi::Config::Lookup("port", (int)8080, "system port");

void test_yaml(){
    YAML::Node config = YAML::LoadFile("../test/test_config.yaml");
    std::cout << config.size() << std::endl;
    // std::cout << config["port"].as<int>() << std::endl;
    // std::cout << config["name"].as<std::string>() << std::endl;
}


int main(){

    LOG_DEBUG(GET_ROOT_LOGGER(),std::to_string(int_port->getValue())); 
    std::cout << std::endl;

    test_yaml();

    // config_system_port->addListener(
    //     [](const int& old_value, const int& new_value) {
    //         LOG_FMT_DEBUG(
    //             GET_ROOT_LOGGER(),
    //             "配置项 system.port 的值被修改，从 %d 到 %d",
    //             old_value, new_value);
    //     });
    // TEST_ConfigVarToString();
    // TEST_GetConfigVarValue();
    // TEST_loadConfig("./test_config.yml");
    // TEST_ConfigVarToString();
    // TEST_GetConfigVarValue();
    // TEST_nonexistentConfig();

    // YAML::Node node;
    // auto str = node["node"] ? node["node"].as<std::string>() : "";
    // std::cout << str << std::endl;


    return 0;
}
