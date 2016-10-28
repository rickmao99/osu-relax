#pragma once

#include <tlhelp32.h>
#include "relax.h"

inline const DWORD get_process_id() {
	// store process ID in here to return later.
	DWORD process_id = NULL;

	// iterate process list for "osu!.exe"
	HANDLE process_list = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	PROCESSENTRY32 entry = {0};
	entry.dwSize = sizeof PROCESSENTRY32;

	if (Process32First(process_list, &entry)) {
		while (Process32Next(process_list, &entry)) {
			if (_wcsicmp(entry.szExeFile, L"osu!.exe") == 0) {
				process_id = entry.th32ProcessID;
			}
		}
	}

	CloseHandle(process_list);

	return process_id;
};

inline const DWORD find_time_address() {
	// scan process memory for array of bytes.
	DWORD time_ptr = FindPattern(game_process, PBYTE(TIME_SIGNATURE)) + 7;
	DWORD time_address = NULL;

	if (!ReadProcessMemory(game_process, LPCVOID(time_ptr), &time_address, sizeof DWORD, nullptr)) {
		return false;
	}

	return time_address;
};

inline const int32_t get_elapsed_time() {
	// read and return the elapsed time in the current beatmap.
	int32_t current_time = NULL;

	if (!ReadProcessMemory(game_process, LPCVOID(time_address), &current_time, sizeof int32_t, nullptr)) {
		return false;
	}

	return current_time;
};

inline void set_key_pressed(char key, bool pressed) {
	// send a key press input.
	INPUT key_press = {0};
	key_press.type = INPUT_KEYBOARD;
	key_press.ki.wVk = VkKeyScanEx(key, GetKeyboardLayout(NULL)) & 0xFF;
	key_press.ki.wScan = 0;
	key_press.ki.dwExtraInfo = 0;
	key_press.ki.dwFlags = (pressed ? 0 : KEYEVENTF_KEYUP);
	SendInput(1, &key_press, sizeof INPUT);
}