#pragma once
#include <Windows.h>
#include <iostream>
#include <vector>

float Width;
float Height;

template<class T>
struct TArray
{
	friend struct FString;

public:
	inline int Num() const
	{
		return Count;
	};

	inline T& operator[](int i)
	{
		return Data[i];
	};

	inline bool IsValidIndex(int i) const
	{
		return i < Num();
	}

private:
	T* Data;
	int32_t Count;
	int32_t Max;
};

struct FString : private TArray<wchar_t>
{
public:
	inline const wchar_t* c_str() const
	{
		return Data;
	}

	std::string ToString() const
	{
		auto length = std::wcslen(Data);

		std::string str(length, '\0');

		std::use_facet<std::ctype<wchar_t>>(std::locale()).narrow(Data, Data + length, '?', &str[0]);

		return str;
	}

};

struct FLinearColor
{
	float R;
	float G;
	float B;
	float A;
};

struct FMatrix {
	float M[4][4];
};

struct FVector2D
{
	float X;
	float Y;
};

struct FVector {
	float X;
	float Y;
	float Z;
};

struct FRotator {
	float Pitch;
	float Yaw;
	float Roll;
};

// Thanks for posting them Hinnie saved everyone time :)
enum BoneFNames
{
	root = 0,
	pelvis = 1,
	spine_01 = 2,
	spine_02 = 3,
	spine_03 = 4,
	clavicle_l = 5,
	upperarm_l = 6,
	lowerarm_l = 7,
	hand_l = 8,
	index_01_l = 9,
	index_02_l = 10,
	index_03_l = 11,
	middle_01_l = 12,
	middle_02_l = 13,
	middle_03_l = 14,
	pinky_01_l = 15,
	pinky_02_l = 16,
	pinky_03_l = 17,
	ring_01_l = 18,
	ring_02_l = 19,
	ring_03_l = 20,
	thumb_01_l = 21,
	thumb_02_l = 22,
	thumb_03_l = 23,
	lowerarm_twist_01_l = 24,
	upperarm_twist_01_l = 25,
	clavicle_r = 26,
	upperarm_r = 27,
	lowerarm_r = 28,
	hand_r = 29,
	index_01_r = 30,
	index_02_r = 31,
	index_03_r = 32,
	middle_01_r = 33,
	middle_02_r = 34,
	middle_03_r = 35,
	pinky_01_r = 36,
	pinky_02_r = 37,
	pinky_03_r = 38,
	ring_01_r = 39,
	ring_02_r = 40,
	ring_03_r = 41,
	thumb_01_r = 42,
	thumb_02_r = 43,
	thumb_03_r = 44,
	lowerarm_twist_01_r = 45,
	upperarm_twist_01_r = 46,
	neck_01 = 47,
	head = 48,
	thigh_l = 49,
	calf_l = 50,
	calf_twist_01_l = 51,
	foot_l = 52,
	ball_l = 53,
	thigh_twist_01_l = 54,
	thigh_r = 55,
	calf_r = 56,
	calf_twist_01_r = 57,
	foot_r = 58,
	ball_r = 59,
	thigh_twist_01_r = 60,
	ik_foot_root = 61,
	ik_foot_l = 62,
	ik_foot_r = 63,
	ik_hand_root = 64,
	ik_hand_gun = 65,
	ik_hand_l = 66,
	ik_hand_r = 67,
	vb_knee_target_l = 68,
	vb_knee_target_r = 69,
	vb_rhs_ik_hand_gun = 70,
};

// Class Engine.Actor 
class AActor
{
public:
	char pad_0000[0x118]; // 0x00 (0x118)
	class APawn* Instigator; // 0x118 (0x08) 
	char pad_0001[0x10]; // 0x120 (0x10)
	class USceneComponent* RootComponent; // 0x130 (0x08)
};

// Class Engine.Controller 
class AController : public AActor
{
public:
	char pad_0000[0xF0]; // 0x138 (0xF0)
	class APlayerState* PlayerState; // 0x228(0x08)
};

// Class Engine.PlayerController
class APlayerController : public AController
{
public:
	char pad_0000[0x68]; // 0x230 (0x68)
	class UPlayer* Player; // 0x298 (0x08)
	class APawn* AcknowledgedPawn; // 0x2a0 (0x08)
};

// Class Engine.Pawn 
class APawn : public AActor
{
public:
	char pad_0099[0x108]; // 0x138 (0x108)
	class APlayerState* PlayerState; // 0x240 (0x08)
	char pad_0001[0x10]; // 0x248 (0x10)
	class AController* Controller; // 0x258 (0x08)
	char pad_0002[0x20];// 0x260 (0x20)
	class USkeletalMeshComponent* Mesh; // 0x280 (0x08) 
};

// Class Engine.Level
class ULevel
{
public:
	char pad_0000[0x98]; // 0x00 (0x98)
	TArray<AActor*> Actors; // 0x98 (0x10)
	char pad_0001[0x10]; // 0xA8 (0x10)
	class UWorld* OwningWorld; // 0xB8 (0x08)
};

// Class Engine.GameInstance
class UGameInstance
{
public:
	char pad_0000[0x38]; // 0x0 (0x38)
	TArray<class UPlayer*> LocalPlayers; // 0x38(0x10)
};

// Class Engine.Player
class UPlayer
{
public:
	char pad_0000[0x30]; // 0x0 (0x30)
	class APlayerController* PlayerController; // 0x30(0x08)
	char pad_0001[0x38]; // 0x38 (0x38)
	uintptr_t ViewportClient; // 0x70 (0x08)
};

// Class Engine.PlayerState
class APlayerState
{
public:
	char pad_0000[0x280]; // 0x0 (0x280)
	class APawn* PawnPrivate; // 0x280 (0x08)
	char pad_0001[0xB0]; // 0x288 (0xB0)
	BYTE TeamNum; // 0x338 (0x01)
};

// Class Engine.SkinnedMeshComponent
class USkeletalMeshComponent
{
public:
	char pad_0000[0x69420]; // 0x0 (0x69420)
};

// Class Engine.SceneComponent
class USceneComponent
{
public:
	char pad_0000[0x11c]; // 0x0 (0x11c)
	struct FVector RelativeLocation; // 0x011C(0x0C)
	struct FRotator RelativeRotation; // 0x0128(0x0C)
};

// Class Engine.World 
class UWorld
{
public:
	char pad_0000[0x30]; // 0x0 (0x30)
	class ULevel* PersistentLevel; // 0x30(0x08)
	char pad_0001[0x148]; // 0x38 (0x148)
	class UGameInstance* OwningGameInstance; // 0x180 (0x08)
};

namespace Utilities {

	/*
		Thank you, lguilhermee. https://github.com/lguilhermee/Discord-DX11-Overlay-Hook/blob/bf1fd7055d8a6815468dc204310f3dfc7b1eccdc/Helper/Helper.cpp#L12
	*/

	uintptr_t PatternScan(uintptr_t moduleAdress, const char* signature)
	{
		static auto patternToByte = [](const char* pattern)
		{
			auto       bytes = std::vector<int>{};
			const auto start = const_cast<char*>(pattern);
			const auto end = const_cast<char*>(pattern) + strlen(pattern);

			for (auto current = start; current < end; ++current)
			{
				if (*current == '?')
				{
					++current;

					if (*current == '?')
						++current;

					bytes.push_back(-1);
				}
				else { bytes.push_back(strtoul(current, &current, 16)); }
			}
			return bytes;
		};

		const auto dosHeader = (PIMAGE_DOS_HEADER)moduleAdress;
		const auto ntHeaders = (PIMAGE_NT_HEADERS)((std::uint8_t*)moduleAdress + dosHeader->e_lfanew);

		const auto sizeOfImage = ntHeaders->OptionalHeader.SizeOfImage;
		auto       patternBytes = patternToByte(signature);
		const auto scanBytes = reinterpret_cast<std::uint8_t*>(moduleAdress);

		const auto dd = patternBytes.size();
		const auto d = patternBytes.data();

		for (auto i = 0ul; i < sizeOfImage - dd; ++i)
		{
			bool found = true;
			for (auto j = 0ul; j < dd; ++j)
			{
				if (scanBytes[i + j] != d[j] && d[j] != -1)
				{
					found = false;
					break;
				}
			}
			if (found) { return reinterpret_cast<uintptr_t>(&scanBytes[i]); }
		}
		return NULL;
	}

}

// Ill switch to ProcessEvent if Biden kills all these Talibans. You have my promise.
namespace UE4 {

	UWorld* WRLD; 

	void(*OPostRender)(PVOID UGameViewportClient, PVOID Canvas) = nullptr;

	uintptr_t W2S_ADDRESS = NULL;
	uintptr_t GETOBJECTNAMES_ADDRESS = NULL;
	uintptr_t FREE_ADDRESS = NULL;
	uintptr_t BONEMATRIX_ADDRESS = NULL;
	uintptr_t K2DRAWLINE_ADDRESS = NULL;
	uintptr_t LINEOFSIGHTTO_ADDRESS = NULL;

	VOID FMemory_Free(PVOID Original) {
		return reinterpret_cast<VOID(*)(PVOID)>(FREE_ADDRESS)(Original);
	}

	std::string UKismetSystemLibrary_GetObjectName(AActor* Object) {

		auto GetObjectName = reinterpret_cast<FString* (*)(FString*, AActor*)>(GETOBJECTNAMES_ADDRESS);

		FString Name;
		GetObjectName(&Name, Object);

		auto result_str = Name.ToString();

		if (Name.c_str())
			FMemory_Free((PVOID)Name.c_str());

		return result_str;
	}

	FVector USkinnedMeshComponent_GetBoneMatrix(USkeletalMeshComponent* mesh, INT index) {

		auto GetBoneMatrix = reinterpret_cast<FMatrix* (*)(USkeletalMeshComponent*, FMatrix*, INT)>(BONEMATRIX_ADDRESS);

		FMatrix matrix;
		GetBoneMatrix(mesh, &matrix, index);

		return FVector({ matrix.M[3][0], matrix.M[3][1], matrix.M[3][2] });
	}

	BOOL APlayerController_ProjectWorldLocationToScreen(APlayerController* Controller, FVector& WorldPosition, FVector2D* ScreenPosition) {
		return reinterpret_cast<CHAR(*)(APlayerController*, FVector, FVector2D*, CHAR)>(W2S_ADDRESS)(Controller, WorldPosition, ScreenPosition, 0);
	}

	BOOL AController_LineOfSightTo(APlayerController* Controller, AActor* Actor) {
		return reinterpret_cast<BOOL(*)(APlayerController*, AActor*, FVector)>(LINEOFSIGHTTO_ADDRESS)(Controller, Actor, FVector({ 0, 0, 0 }));
	}

	VOID UCanvas_K2_DrawLine(PVOID Canvas, FVector2D ScreenPositionA, FVector2D ScreenPositionB, FLOAT Thickness, FLinearColor Color) {
		return reinterpret_cast<VOID(*)(PVOID, FVector2D, FVector2D, FLOAT, FLinearColor)>(K2DRAWLINE_ADDRESS)(Canvas, ScreenPositionA, ScreenPositionB, Thickness, Color);
	}

	BOOL FindAddresses() {

		uintptr_t BaseAddress = (uintptr_t)GetModuleHandleA(NULL);

		uintptr_t World = Utilities::PatternScan(BaseAddress, "48 8B 1D ? ? ? ? 48 85 DB 74 3B 41 B0 01 33 D2 48 8B CB E8");
		if (!World) return FALSE;
		int32_t Offset = *(int32_t*)(World + 3); WRLD = (UWorld*)(World + Offset + 7);

		FREE_ADDRESS = Utilities::PatternScan(BaseAddress, "48 85 C9 ? 2E 53 48");
		if (!FREE_ADDRESS) return FALSE;

		GETOBJECTNAMES_ADDRESS = Utilities::PatternScan(BaseAddress, "40 53 48 83 EC 20 48 8B D9 48 85 D2 75 45 33 C0 48 89 01 48 89 41 08 8D 50 05 E8 ? ? ? ? 8B 53 08 8D 42 05 89 43 08 3B 43 0C 7E 08 48 8B CB E8 ? ? ? ? 48 8B 0B 48 8D 15 ? ? ? ? 41 B8 ? ? ? ? E8 ? ? ? ? 48 8B C3 48 83 C4 20 5B C3 48 8B 42 18");
		if (!GETOBJECTNAMES_ADDRESS) return FALSE;

		W2S_ADDRESS = Utilities::PatternScan(BaseAddress, "48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 18 57 48 81 EC 20 01 00 00 41 0F B6 E9");
		if (!W2S_ADDRESS) return FALSE;

		BONEMATRIX_ADDRESS = Utilities::PatternScan(BaseAddress, "48 8B C4 55 53 56 57 41 54 41 56 41 57 48 8D 68 A1 48 81 EC ? ? ? ? 0F 29 78 B8 33 F6 44 0F 29 40");
		if (!BONEMATRIX_ADDRESS) return FALSE;

		LINEOFSIGHTTO_ADDRESS = Utilities::PatternScan(BaseAddress, "40 55 53 56 57 48 8D 6C 24 ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 45 E0 49 8B F8 48 8B DA 48 8B F1 48 85 D2 75 07 32 C0 E9");
		if (!LINEOFSIGHTTO_ADDRESS) return FALSE;

		K2DRAWLINE_ADDRESS = Utilities::PatternScan(BaseAddress, "4C 8B DC 48 81 EC ? ? ? ? 4C 89 44 24 ? 66 48 0F 6E D2 F3 0F 10 64 24 ? F3 0F 10 6C 24 ? 0F 28 C4");
		if (!K2DRAWLINE_ADDRESS) return FALSE;

		return TRUE;
	}
}

