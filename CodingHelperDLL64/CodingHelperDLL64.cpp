// CodingHelperDLL64.cpp : DLL ���� ���α׷��� ���� ������ �Լ��� �����մϴ�.
//
#include "stdafx.h"
#include <windows.h>
#include <iostream>
#include <stdio.h>
#include <vector>
#include <thread>
#include "atlstr.h"

#pragma warning(disable: 4996)
#define CONVERT_MAX_SIZE 10
#define MACRO_MAX_SIZE 6

struct ConvertStruct {
	TCHAR pOrigin_vec[CONVERT_MAX_SIZE][32];
	TCHAR pConverted_array[CONVERT_MAX_SIZE][128];
};

struct MacroStruct {
	//�Էµ� ���ڿ��� ���� �������� ���� �޸��� ũ�⸦ ������ �� ������....
	TCHAR macroText[MACRO_MAX_SIZE][256];
	int size[MACRO_MAX_SIZE];
	int execute_index = -1;
	int convert_index = -1;
	bool isPressButton;
	bool activateSwitch = false;
	bool executeSwitch = false;
	bool convertSwitch = false;
};



using namespace std;
void CopyToClipboard(TCHAR *text, int size);
void PasteClipboardText();
void WriteBackupDataToSharedMemory(TCHAR* backupData, int backupDataSize);
void UpdateInfo();
int SearchWord(wstring enteredKey);
void RequestConvertText(int matched_index);

ConvertStruct *pConvertStruct;

__declspec(dllexport) LRESULT CALLBACK GetMsgProc(INT nCode, WPARAM wp, LPARAM lp) {
	if (((MSG*)lp)->message == (long)WM_SYSCHAR) {
		DWORD str = GetLastError();
		HANDLE hMemoryMap;
		LPBYTE pMemoryMap;		
		HANDLE hClipboard;
		TCHAR* backupData;
		int backupDataSize;
		
		
		//Ŭ�����忡 ����Ǿ��ִ� �ؽ�Ʈ�� ����Ѵ�.
		OpenClipboard(NULL);
		hClipboard = GetClipboardData(CF_UNICODETEXT);
		TCHAR* clipboardData = (TCHAR*)GlobalLock(hClipboard);
		if (clipboardData == NULL) {
			clipboardData = L"";
			backupData = L"";
			backupDataSize = 0;
		}
		else {
			backupDataSize = wcslen(clipboardData) + 1;
			backupData = (TCHAR*)malloc(sizeof(TCHAR)*(backupDataSize));
			memset(backupData, '\0', backupDataSize*sizeof(TCHAR));
			memcpy(backupData, clipboardData, sizeof(TCHAR) * (backupDataSize));
			GlobalUnlock(hClipboard);
			CloseClipboard();
		}
		//���� �޸𸮸� open 
		hMemoryMap = OpenFileMapping(FILE_MAP_READ | FILE_MAP_WRITE, NULL, L"shared_memory");
		if (!hMemoryMap) {
			//MessageBox(NULL, L"shared memory open failed", L"oops", MB_OK);
			return 0;
		}

		pMemoryMap = (LPBYTE)MapViewOfFile(hMemoryMap, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
		if (!pMemoryMap) {
			MessageBox(NULL, L"maping failed", L"oops", MB_OK);
			return 0;
		}

		MacroStruct* pMacroStruct = (MacroStruct*)pMemoryMap;
		DWORD vk = ((MSG*)lp)->wParam;

		if (((MSG*)lp)->wParam == 0x60) {
			pMacroStruct->activateSwitch = true;
			return 0;
		}
		
		else if (((MSG*)lp)->wParam == 0x31) {
			TCHAR* text = pMacroStruct->macroText[0];
			CopyToClipboard(text, pMacroStruct->size[0]); //Ŭ�����忡 �ý�Ʈ�� �����Ѵ�.
			//PasteClipboardText();
		}
		
		else if(((MSG*)lp)->wParam == 0x32){
			TCHAR* text = pMacroStruct->macroText[1];
			CopyToClipboard(text, pMacroStruct->size[1]); //Ŭ�����忡 �ý�Ʈ�� �����Ѵ�.
			//PasteClipboardText();
		}

		else if (((MSG*)lp)->wParam == 0x33) {
			TCHAR* text = pMacroStruct->macroText[2];
			CopyToClipboard(text, pMacroStruct->size[2]); //Ŭ�����忡 �ý�Ʈ�� �����Ѵ�.
			//PasteClipboardText();
		}

		else if (((MSG*)lp)->wParam == 0x34) {
			TCHAR* text = pMacroStruct->macroText[3];
			CopyToClipboard(text, pMacroStruct->size[3]); //Ŭ�����忡 �ý�Ʈ�� �����Ѵ�.
			//PasteClipboardText();
		}

		else if (((MSG*)lp)->wParam == 0x35) {
			TCHAR* text = pMacroStruct->macroText[4];
			CopyToClipboard(text, pMacroStruct->size[4]); //Ŭ�����忡 �ý�Ʈ�� �����Ѵ�.
			//PasteClipboardText();
		}

		else if (((MSG*)lp)->wParam == 0x36) {
			TCHAR* text = pMacroStruct->macroText[5];
			CopyToClipboard(text, pMacroStruct->size[5]); //Ŭ�����忡 �ý�Ʈ�� �����Ѵ�.
			//PasteClipboardText();
		}
		
		else if (((MSG*)lp)->wParam == 0x38) {
			pMacroStruct->executeSwitch = true;
			pMacroStruct->execute_index = 0;
			return 0;
		}

		else if (((MSG*)lp)->wParam == 0x39) {
			pMacroStruct->executeSwitch = true;
			pMacroStruct->execute_index = 1;
			return 0;
		}

		else if (((MSG*)lp)->wParam == 0x30) {
			pMacroStruct->executeSwitch = true;
			pMacroStruct->execute_index = 2;
			return 0;
		}

		if (backupDataSize == 0)
			return 0;
		WriteBackupDataToSharedMemory(backupData, backupDataSize);
		pMacroStruct->isPressButton = true;
		
		if (pMemoryMap) {
			UnmapViewOfFile(pMemoryMap);
		}
		if (hMemoryMap) {
			CloseHandle(hMemoryMap);
		}
	}

	if (((MSG*)lp)->message == (long)WM_CHAR) {
		static bool flag = true;
		static wstring enteredKey = L"";
		int matched_index;
		int max_size = 0;
		int vec_size = 0;
		if (flag) {      //ù ������ ���
			UpdateInfo();
			flag = false;
		}
		
		/*vec_size = pOrigin_vec->size();
		for (int i = 0; i < vec_size; i++) {
			if()
		}*/
		enteredKey += (TCHAR)(((MSG*)lp)->wParam);
		
		matched_index = SearchWord(enteredKey);
		if (matched_index != -1) {
			RequestConvertText(matched_index);
			enteredKey = L"";
		}
		if (enteredKey.length() >= 32)
			enteredKey = enteredKey.substr(1);
 	}
	
	
	return TRUE;
}

void CopyToClipboard(TCHAR *text, int size) {
	HGLOBAL hglobal = GlobalAlloc(GHND | GMEM_SHARE, (size + 1) * sizeof(TCHAR));
	LPTSTR pGlobal = (LPTSTR)GlobalLock(hglobal);
	memcpy(pGlobal, text, (size + 1) * sizeof(TCHAR));
	GlobalUnlock(hglobal);

	OpenClipboard(NULL);
	EmptyClipboard();
	SetClipboardData(CF_UNICODETEXT, hglobal);
	CloseClipboard();
}


void PasteClipboardText() {
	//set up
	INPUT ip;
	ip.type = INPUT_KEYBOARD;
	ip.ki.wScan = 0;
	ip.ki.time = 0;
	ip.ki.dwExtraInfo = 0;

	//ALT UP
	ip.ki.wVk = VK_MENU;
	ip.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &ip, sizeof(INPUT));

	//CTRL DOWN
	ip.ki.wVk = VK_CONTROL;
	ip.ki.dwFlags = 0;
	SendInput(1, &ip, sizeof(INPUT));

	//V Down
	ip.ki.wVk = 0x56;
	ip.ki.dwFlags = 0;
	SendInput(1, &ip, sizeof(INPUT));

	//V UP
	ip.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &ip, sizeof(INPUT));

	//CTRL UP
	ip.ki.wVk = VK_CONTROL;
	ip.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &ip, sizeof(INPUT));

	MSG Message;

	while (!PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&Message);
		DispatchMessage(&Message);
		break;
	}

}

void WriteBackupDataToSharedMemory(TCHAR* backupData, int backupDataSize) {
	HANDLE hCreateMemoryMap = NULL;
	HANDLE hOpenMemoryMap = NULL;
	LPBYTE pCreateMemoryMap = NULL;
	LPBYTE pOpenMemoryMap = NULL;
	long long sharedMemorySize;

	//���� �޸𸮸� open 
	hOpenMemoryMap = OpenFileMapping(FILE_MAP_READ | FILE_MAP_WRITE, NULL, L"clipboard");
	UnmapViewOfFile(hOpenMemoryMap);

	/*
	//���� �����޸��� ũ�Ⱑ �����ϰ��� �ϴ� �����޸��� ũ�⺸�� ���� ��� ���� ����, Ŭ ���� ���� �������� ����.	
	if (hOpenMemoryMap) {
		pOpenMemoryMap = (LPBYTE)MapViewOfFile(hOpenMemoryMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);
		TCHAR* temp = (TCHAR*)pOpenMemoryMap;
		sharedMemorySize = wcslen(temp);

		if (backupDataSize > sharedMemorySize) {
			UnmapViewOfFile(hOpenMemoryMap);
		}
		else {
			memset(pOpenMemoryMap, '\0', sharedMemorySize);   //TCHAR�� ���ڴ� 2����Ʈ
			memcpy(pOpenMemoryMap, backupData, sizeof(TCHAR)*backupDataSize);
			return;
		}	
		
	}
	*/
	
	//���� ���� Ŀ�� ������Ʈ ����
	hCreateMemoryMap = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(TCHAR)*backupDataSize, L"clipboard");
	if (!hCreateMemoryMap) {
		MessageBox(NULL, L"shared memory create failed", L"oops", MB_OK);
		return;
	}

	//���μ����� ������ ���� ���� ������Ʈ�� ����
	pCreateMemoryMap = (LPBYTE)MapViewOfFile(hCreateMemoryMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	if (!pCreateMemoryMap) {
		MessageBox(NULL, L"maping failed", L"clipboard backup", MB_OK);
		return;
	}
	TCHAR* pSharedMemory = (TCHAR*)pCreateMemoryMap;
	memcpy(pSharedMemory, backupData, sizeof(TCHAR)*backupDataSize);
	
}

void UpdateInfo() {
	int origin_size;
	HANDLE hMemoryMap;
	LPBYTE pMemoryMap;
	
	//���� �޸𸮸� open 
	hMemoryMap = OpenFileMapping(FILE_MAP_READ | FILE_MAP_WRITE, NULL, L"translate");
	if (!hMemoryMap) {
		MessageBox(NULL, L"shared memory open failed", L"oops", MB_OK);
		return ;
	}

	pMemoryMap = (LPBYTE)MapViewOfFile(hMemoryMap, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
	if (!pMemoryMap) {
		MessageBox(NULL, L"maping failed", L"oops", MB_OK);
		return ;
	}
	
	
	pConvertStruct = (ConvertStruct*)pMemoryMap;
	
}

int SearchWord(wstring enteredKey) {
	for (int i = 0; i < CONVERT_MAX_SIZE; i++) {
		wstring origin(pConvertStruct->pOrigin_vec[i]);
		if (pConvertStruct->pOrigin_vec[i][0] == NULL)
			break;
		if (enteredKey.find(origin)!=wstring::npos)
			return i;
	}

	/*if (enteredKey.length() <= 1)
		return -1;
	return SearchWord(enteredKey.substr(1));*/
	return -1;
}

void RequestConvertText(int matched_index) {
	HANDLE hMemoryMap;
	LPBYTE pMemoryMap;
	HANDLE hClipboard;
	TCHAR* backupData;
	int backupDataSize;

	//������ Ŭ�����忡 �ִ� �ؽ�Ʈ�� ����Ѵ�.
	OpenClipboard(NULL);            
	hClipboard = GetClipboardData(CF_UNICODETEXT);
	TCHAR* clipboardData = (TCHAR*)GlobalLock(hClipboard);
	if (clipboardData == NULL) {
		clipboardData = L"";
		backupData = L"";
		backupDataSize = 0;
	}
	else {
		backupDataSize = wcslen(clipboardData) + 1;
		backupData = (TCHAR*)malloc(sizeof(TCHAR)*(backupDataSize));
		memset(backupData, '\0', backupDataSize*sizeof(TCHAR));
		memcpy(backupData, clipboardData, sizeof(TCHAR) * (backupDataSize));
		GlobalUnlock(hClipboard);
		CloseClipboard();
	}
	
	//�����޸𸮿� ����� �ؽ�Ʈ�� write
	WriteBackupDataToSharedMemory(backupData, backupDataSize);

	//Ŭ�����忡 ġȯ�� �ý�Ʈ�� �����Ѵ�.
	TCHAR* tchr = pConvertStruct->pConverted_array[matched_index];
	CopyToClipboard(tchr, wcslen(pConvertStruct->pConverted_array[matched_index]));


	//���� �޸𸮸� open 
	hMemoryMap = OpenFileMapping(FILE_MAP_READ | FILE_MAP_WRITE, NULL, L"shared_memory");
	if (!hMemoryMap) {
		//MessageBox(NULL, L"shared memory open failed", L"oops", MB_OK);
		return ;
	}

	pMemoryMap = (LPBYTE)MapViewOfFile(hMemoryMap, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
	if (!pMemoryMap) {
		MessageBox(NULL, L"maping failed", L"oops", MB_OK);
		return ;
	}
	
	MacroStruct* pMacroStruct = (MacroStruct*)pMemoryMap;

	pMacroStruct->isPressButton = true;
	pMacroStruct->convertSwitch = true;
	pMacroStruct->convert_index = matched_index;
}
 