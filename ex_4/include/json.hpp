#ifndef JSON_HPP
#define JSON_HPP

#include <map>
#include <iostream>

bool parse_json_str(char *str, std::map<std::string, std::string> &obj);

#endif