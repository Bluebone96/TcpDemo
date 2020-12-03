#include <iostream>
#include "tinyxml2.h"


int main()
{
    try {
        tinyxml2::XMLDocument doc;
        doc.LoadFile("config.xml");
        tinyxml2::XMLElement *root = doc.FirstChildElement("server_config");
        if (root) {
            tinyxml2::XMLElement *p = root->FirstChildElement("gate_server");
            if (p) {
                std::cout << p->Attribute("id") << std::endl;
                tinyxml2::XMLElement *pp = p->FirstChildElement("ip");
                std::cout << pp->GetText() << std::endl;
                pp = p->FirstChildElement("port");
                uint32_t port = pp->Int64Text();

                std::cout << port << std::endl;
                // p->SetAttribute("id", 0);
                // doc.SaveFile("server_config");
            }
        }
    } catch (...) {

    }
    return 0;
}