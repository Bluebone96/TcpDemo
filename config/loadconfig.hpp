#include <string>
#include "tinyxml2.h"

struct server_config {
    std::string ip;
    uint32_t port;
};

int8_t load_config(const char* _name, server_config& _server) {
    tinyxml2::XMLDocument doc;
    doc.LoadFile("config.xml");
    tinyxml2::XMLElement *root = doc.FirstChildElement("server_config");
    if (root) {
        tinyxml2::XMLElement *p = root->FirstChildElement(_name);
        if (p) {
            tinyxml2::XMLElement *pp = p->FirstChildElement("ip");
            _server.ip = pp->GetText();
            pp = p->FirstChildElement("port");
            _server.port = pp->Int64Text();
            return 0;
        }
    }
    return -1;
    // doc 析构负责已读取节点的内存释放
}