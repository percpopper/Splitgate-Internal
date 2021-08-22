#pragma once
#include <cstdint>
#include <Windows.h>

/*
	guttir14 https://github.com/guttir14/CheatIt
*/

bool Compare(byte* data, byte* sig, uint64_t size);
byte* FindSignature(byte* start, byte* end, byte* sig, uint64_t size);
void* FindPointer(HMODULE mod, byte* sig, uint64_t size, int addition);