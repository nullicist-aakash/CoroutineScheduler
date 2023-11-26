export module network.dns;
import <string>;
import <vector>;
import network.types;

export std::vector<IP> get_dns_response(std::string_view hostname);