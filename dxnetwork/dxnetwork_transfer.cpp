#include "pch.h"
#include "dxnetwork_transfer.h"

void dxnetwork_transfer::listen(const char* ip, ushort port, int& result)
{
	SOCKET servSock = socket(AF_INET, SOCK_STREAM, 0);
	sockaddr_in sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));
	sockAddr.sin_family = PF_INET;
	sockAddr.sin_addr.s_addr = inet_addr(ip);
	sockAddr.sin_port = htons(port);
	::bind(servSock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR));
	result = ::listen(servSock, 20);

	while (true)
	{
		SOCKADDR clntAddr{};
		int nSize = sizeof(SOCKADDR);
		SOCKET clntSock = ::accept(servSock, (SOCKADDR*)&clntAddr, &nSize);
		if (clntSock != INVALID_SOCKET) {
			std::cout << "please input the file you want to transfer: " << std::endl;

			std::wstring filePath;
			//std::wcin >> filePath;
			filePath = L"C:\\Users\\hduxi\\Desktop\\syc.mp4";

			FILE* fp = _wfopen(filePath.c_str(), L"rb");
			if (fp == NULL) {
				std::cout << "file load failed!" << std::endl;
				continue;
			}

			int bufferSize = 1024;
			char* buffer_tmp = new char[bufferSize] {};
			std::unique_ptr<char[]> buffer(buffer_tmp);
			auto fileSize = std::filesystem::file_size(filePath);
			llong transferSize = 0;
			std::cout << "file load succeed, file size: "
				<< fileSize / 1024 * 1024
				<< "MB" << std::endl;

			while (true)
			{
				auto dataSize = fread(buffer.get(), 1, bufferSize, fp);
				if (dataSize == 0) {
					fclose(fp);
					break;
				}

				::send(clntSock, buffer.get(), int(dataSize), 0);

				transferSize += dataSize;
				std::cout << "transfer size: " << transferSize / llong(1024 * 1024)
					<< "MB" << std::endl;
			}

			//文件读取完毕，断开输出流，向客户端发送FIN包
			shutdown(clntSock, SD_SEND);

			//阻塞，等待客户端接收完毕
			recv(clntSock, buffer.get(), bufferSize, 0);
			closesocket(clntSock);

			break;
		}
	}
}

void dxnetwork_transfer::connect(const char* ip, ushort port, int& result)
{
	SOCKET sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	sockaddr_in sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));
	sockAddr.sin_family = PF_INET;
	sockAddr.sin_addr.s_addr = inet_addr(ip);
	sockAddr.sin_port = htons(port);

	auto res = ::connect(sock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR));
	if (res != 0) {
		std::cout << "connect failed!\n";
	}

	std::cout << "please input the path of the file you want to save:\n";

	std::wstring fileSavePath;
	//std::wcin >> fileSavePath;
	fileSavePath = L"C:\\Users\\hduxi\\Desktop\\syc2.mp4";

	FILE* fp = _wfopen(fileSavePath.c_str(), L"wb");
	if (fp == NULL) {
		std::cout << "file create failed!" << std::endl;
		return;
	}

	int bufferSize = 1024;
	char* buffer_tmp = new char[bufferSize] {};
	std::unique_ptr<char[]> buffer(buffer_tmp);

	while (true)
	{
		auto dataSize = ::recv(sock, buffer.get(), bufferSize, 0);
		if (dataSize == 0) {
			std::cout << "file transfer finish!\n";
			fclose(fp);
			break;
		}

		fwrite(buffer.get(), 1, dataSize, fp);
	}

	closesocket(sock);
}
