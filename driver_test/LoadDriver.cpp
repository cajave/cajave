// LoadDriver.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <windows.h>
#include <iostream>


BOOL LoadSys(IN WCHAR* pDriverName, IN WCHAR* NewSYSImagePath, DWORD dwStartType);

int main(int argc, WCHAR* argv[]) {

	//if (argc < 2) {
	//	return 1;
	//}

	LoadSys((WCHAR*)L"my_service", 
		(WCHAR*)L"F:\\Projects\\Driver1\\Driver1\\x64\\Debug\\Driver1\\Driver1.sys", 
		SERVICE_DEMAND_START);
}


BOOL LoadSys(IN WCHAR* pDriverName, IN WCHAR* NewSYSImagePath, DWORD dwStartType) {
	BOOL bRet = FALSE;
	DWORD dwRet = 0;
	SC_HANDLE hServiceMgr = NULL;
	SC_HANDLE hServiceDriver = NULL;

	hServiceMgr = OpenSCManagerW(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (hServiceMgr == NULL) {
		SetLastError(1);
		return FALSE;
	}

	hServiceDriver = CreateServiceW(hServiceMgr, 
		pDriverName, 
		pDriverName,
		SERVICE_ALL_ACCESS, 
		SERVICE_KERNEL_DRIVER, 
		dwStartType,
		SERVICE_ERROR_IGNORE, 
		NewSYSImagePath,
		NULL, NULL, NULL, NULL, NULL);
	if (hServiceDriver == NULL) {
		dwRet = GetLastError();
		if ((dwRet != ERROR_IO_PENDING) && (dwRet != ERROR_SERVICE_EXISTS)) {
			SetLastError(2);
			CloseServiceHandle(hServiceMgr);
			return FALSE;
		}

		hServiceDriver = OpenServiceW(hServiceMgr, pDriverName, SERVICE_ALL_ACCESS);
		if (hServiceDriver == NULL) {
			SetLastError(3);
			CloseServiceHandle(hServiceMgr);
			return FALSE;
		}
	}

	bRet = StartServiceW(hServiceDriver, NULL, NULL);
	if (bRet == NULL) {
		dwRet = GetLastError();
		if ((dwRet != ERROR_IO_PENDING) && (dwRet != ERROR_SERVICE_ALREADY_RUNNING)) {
			SetLastError(4);
			CloseServiceHandle(hServiceDriver);
			CloseServiceHandle(hServiceMgr);
			return FALSE;
		} else {
			//if (dwRet == ERROR_IO_PENDING){
			CloseServiceHandle(hServiceDriver);
			CloseServiceHandle(hServiceMgr);
			return FALSE;
			//} else {
			//	CloseServiceHandle(hServiceDriver);
			//	CloseServiceHandle(hServiceMgr);
			//	return FALSE;
			//}
		}
	}
	CloseServiceHandle(hServiceDriver);
	CloseServiceHandle(hServiceMgr);
	return TRUE;
}


// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门提示: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
