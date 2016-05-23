#ifndef PROXY_UTILS_H
#define PROXY_UTILS_H

void throw_error(std::string msg);
std::string epoll_event_to_str(uint32_t &event);
#endif //PROXY_UTILS_H
