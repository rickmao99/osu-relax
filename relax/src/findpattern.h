#pragma once

DWORD FindPattern(HANDLE process, unsigned char pattern[]) {
	const size_t signature_size = sizeof(pattern);
	const size_t read_size = 4096;
	bool hit = false;

	unsigned char chunk[read_size];

	for (size_t i = 0; i < INT_MAX; i += read_size - signature_size) {
		ReadProcessMemory(process, LPCVOID(i), &chunk, read_size, NULL);

		for (size_t a = 0; a < read_size; a++) {
			hit = true;

			for (size_t j = 0; j < signature_size && hit; j++) {
				if (chunk[a + j] != pattern[j]) {
					hit = false;
				}
			}

			if (hit) {
				return i + a;
			}
		}
	}

	return NULL;
}