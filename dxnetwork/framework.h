#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN             // 从 Windows 头文件中排除极少使用的内容
// Windows 头文件
#include <windows.h>
#include <winsock2.h>

#include <iostream>
#include <filesystem>

#pragma comment (lib, "ws2_32.lib")  //加载 ws2_32.dll
