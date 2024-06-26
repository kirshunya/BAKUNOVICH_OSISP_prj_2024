//
// Created by nikitabakunovich on 22.03.24.
//

#include "con_funcs.h"

void ErrorCatching(int res, const char* serror){
    if (res == -1) {
        perror(serror);
        exit(EXIT_FAILURE);
    }
}

int Socket(int domain, int type, int protocol) {
    int res = socket(domain, type, protocol);
    ErrorCatching(res, "Socket failure");
    return res;
}

void Bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
    int res = bind(sockfd, addr, addrlen);
    ErrorCatching(res, "Bind failure");
}

void Listen(int sockfd, int clnum) {
    int res = listen(sockfd, clnum);
    ErrorCatching(res, "Listen failure");
}

int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) {
    int res = accept(sockfd, addr, addrlen);
    ErrorCatching(res, "Accept failure");
    return res;
}

void Connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
    int res = connect(sockfd, addr, addrlen);
    ErrorCatching(res, "Connect failure");
}

void Inet_pton(int af, const char *src, void *dst) {
    int res = inet_pton(af, src, dst);
    if (res == 0) {
        printf("inet_pton failed: src does not contain a character"
               " string representing a valid network address in the specified"
               " address family\n");
        exit(EXIT_FAILURE);
    }
    ErrorCatching(res, "inet_pton failure");
}

void Hostname(char* hostname) {
    struct hostent *host_info;
    char **ip_list;
    char *ip_address = NULL;

    host_info = gethostbyname(hostname);
    if (host_info == NULL) {
        printf("Ошибка при получении информации о хосте.\n");
    }

    ip_list = host_info->h_addr_list;
    for (int i = 0; ip_list[i] != NULL; i++) {
        struct in_addr ip;
        memcpy(&ip, ip_list[i], sizeof(ip));
        ip_address = strdup(inet_ntoa(ip));
        printf("Доступные IP адреса для подключения: %s\n", ip_address);
        //break; // Выбираем первый IP-адрес из списка
    }
}

std::string getIPLinux(char *hostname) {
    struct hostent *host_info;
    char **ip_list;
    char *ip_address = NULL;

    host_info = gethostbyname(hostname);
    if (host_info == NULL) {
        printf("Ошибка при получении информации о хосте.\n");
        return NULL;
    }

    ip_list = host_info->h_addr_list;
    for (int i = 0; ip_list[i] != NULL; i++) {
        struct in_addr ip;
        memcpy(&ip, ip_list[i], sizeof(ip));
        ip_address = strdup(inet_ntoa(ip));
        break; // Выбираем первый IP-адрес из списка
    }

    return ip_address;
}

std::string getIPMAC()
{
    std::string ipAddress;
    struct ifaddrs* ifAddrStruct = nullptr;
    struct ifaddrs* ifa = nullptr;
    void* tmpAddrPtr = nullptr;

    getifaddrs(&ifAddrStruct);

    for (ifa = ifAddrStruct; ifa != nullptr; ifa = ifa->ifa_next)
    {
        if (!ifa->ifa_addr)
        {
            continue;
        }

        // IPv4 and interface name "en0"
        if (ifa->ifa_addr->sa_family == AF_INET && strcmp(ifa->ifa_name, "en0") == 0)
        {
            tmpAddrPtr = &reinterpret_cast<struct sockaddr_in*>(ifa->ifa_addr)->sin_addr;
            char addressBuffer[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
            ipAddress = std::string(addressBuffer);
            break;
        }
    }

    if (ifAddrStruct != nullptr)
    {
        freeifaddrs(ifAddrStruct);
    }

    return ipAddress;
}


std::string getIpForOS(char* hostname) {
#   ifdef __APPLE__
    return getIPMAC();
#   elif __linux__
    return getIPLinux(hostname);
#   endif
}