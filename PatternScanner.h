#pragma once
#include <common\IDebugLog.h>
#include <Psapi.h>
#include <vector>
#include <unordered_map>
#pragma comment(lib, "Psapi.lib")
using std::vector;
using std::unordered_map;
using std::string;
using std::pair;

struct MemoryRegion {
public:
	void Set(uintptr_t b, size_t s) {
		base = b;
		size = s;
	}
	MemoryRegion() {};
	MemoryRegion(uintptr_t b, size_t s) {
		Set(b, s);
	}
	uintptr_t base = -1;
	size_t size = -1;
};

bool CompareBytes(BYTE* data, vector<BYTE> pattern, bool useMask, BYTE mask) {
	for (vector<BYTE>::iterator it = pattern.begin(); it != pattern.end(); *data++, it++) {
		if (*it == mask && useMask)
			continue;
		if (*data != *it)
			return false;
	}
	return true;
}

namespace PatternScanner {
	void GetSkyrimMemoryRegion(MemoryRegion* mr) {
		HMODULE handle = GetModuleHandle(NULL);
		MODULEINFO mi{};
		GetModuleInformation(GetCurrentProcess(), handle, &mi, sizeof(mi));
		mr->Set((uintptr_t)handle, mi.SizeOfImage);
	}

	unordered_map<string, uintptr_t> PatternScanInternal(MemoryRegion* r, unordered_map<string, vector<BYTE>> pattern, bool useMask = false, BYTE mask = '\x00') {
		unordered_map<string, uintptr_t> ret;
		size_t len = pattern.size();
		BYTE* end = (BYTE*)r->base + r->size - len;
		for (BYTE* addr = (BYTE*)r->base; addr < end; addr++) {
			for (auto it = pattern.begin(); it != pattern.end(); ++it) {
				if (CompareBytes(addr, it->second, useMask, mask))
					ret.insert(pair<string, uintptr_t>(it->first, (uintptr_t)addr));
			}
		}
		return ret;
	}
}