#pragma once

#ifdef _WIN32
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN             // 从 Windows 头文件中排除极少使用的内容
// Windows 头文件
#include <windows.h>
#include <winsock2.h>
using socklen_t = int;
#elif __APPLE__
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#define SOCKET int
#define SOCKADDR sockaddr
#define INVALID_SOCKET (-1)
#define SD_SEND SHUT_WR
#define closesocket close
#endif

#include <iostream>
#include <filesystem>

#ifdef _WIN32
#pragma comment (lib, "ws2_32.lib")  //加载 ws2_32.dll
#endif
