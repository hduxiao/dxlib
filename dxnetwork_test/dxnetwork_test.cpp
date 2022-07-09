#ifdef _WIN32
#include <Windows.h>
#elif __APPLE__
#include <dlfcn.h>
#define GetProcAddress dlsym
#define FreeLibrary dlclose
#endif
#include <thread>
#include "../dxnetwork/interface.h"

void test_listen(i_dxnetwork_transfer* pTransfer)
{
	int listen_result = 0;
	pTransfer->listen("192.168.0.100", 5127, listen_result);
}

void test_connect(i_dxnetwork_transfer* pTransfer)
{
	int connect_result = 0;
	pTransfer->connect("192.168.0.100", 5127, connect_result);
}

int main()
{
	i_dxnetwork_object* pObject = nullptr;
	i_dxnetwork_factory* pFactory = nullptr;
	i_dxnetwork_transfer* pTransfer = nullptr;

	auto dxnetwork_dll =
#ifdef _WIN32
            LoadLibraryW(L"dxnetwork");
#elif __APPLE__
            dlopen("../../dxnetwork/cmake-build-debug/libdxnetwork.dylib", RTLD_LOCAL|RTLD_LAZY);
#endif
	if (dxnetwork_dll)
	{
		auto cdf_func_address = GetProcAddress(dxnetwork_dll, "create_dxnetwork_factory");
		auto cdf_func =
			reinterpret_cast<decltype(&create_dxnetwork_factory)>(cdf_func_address);
		if (cdf_func)
		{
			cdf_func(&pFactory);
		}
	}

	if (pFactory)
	{
		pFactory->create_dxnetwork_object(DXNETWORK_TRANSFER, &pObject);
		pTransfer = static_cast<i_dxnetwork_transfer*>(pObject);
	}

	if (pTransfer)
	{
		std::thread th_listen(test_listen, pTransfer);
//		std::thread th_connect(test_connect, pTransfer);

//		th_connect.join();
		th_listen.join();

		delete pTransfer;
		delete pFactory;
	};
}
