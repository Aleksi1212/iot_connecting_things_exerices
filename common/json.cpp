#include <json.hpp>
#include <vector>
#include <algorithm>

static void trim_white_space(char *str)
{
    int read = 0, write = 0;
    while (str[read] != '\0') {
        if (str[read] != ' ') {
            str[write++] = str[read];
        }
        str[read++];
    }
    str[write] = '\0';
}

bool parse_json_str(char *str, std::map<std::string, std::string> &obj)
{
    trim_white_space(str);

    int idx = 0;
    if (str[idx] != '{') return false;

    std::string curr_item;

    std::vector<std::string> keys;
    std::vector<std::string> vals;

    bool key = true;
    while(str[idx] != '\0') {
        if (str[idx] == ':' || str[idx] == ',' || str[idx] == '}') {
            if (key) keys.push_back(curr_item);
            else vals.push_back(curr_item);
            curr_item = "";
        }
        else if (str[idx] != '\"' && str[idx] != '{') curr_item += str[idx];

        if (str[idx] == '{' || str[idx] == ',') key = true;
        if (str[idx] == ':') key = false;

        idx++;
    }

    for (int i = 0; i < keys.size(); ++i) {
        std::transform(keys[i].begin(), keys[i].end(), keys[i].begin(), ::toupper);
        std::transform(vals[i].begin(), vals[i].end(), vals[i].begin(), ::toupper);

        if (keys[i] != "LED" && keys[i] != "CMD") return false;
        obj.insert({ keys[i], vals[i] });
    }
    return true;
}