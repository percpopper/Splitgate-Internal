#pragma once
#include "Engine.h"

/*
	Splitgate Internal

	It is nothing special at all & not that helpful either, that being said please do not bully me about this please thanks please im fragile i may cri.

	Regards, RiceCum1.

	https://github.com/guttir14/CheatIt I pasted his Engine.cpp/h and Util. Shits bussin thanks guttir14

*/

int Width;
int Height;

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

// https://github.com/lguilhermee/Discord-DX11-Overlay-Hook/blob/bf1fd7055d8a6815468dc204310f3dfc7b1eccdc/Helper/Helper.cpp#L12 Pattern scanner from here
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

// I'll switch to ProcessEvent if Biden kills all these Talibans. You have my promise!

namespace UE4 {

	UWorld* WRLD;
	
	void(*OPostRender)(PVOID UGameViewportClient, PVOID Canvas) = nullptr;

	uintptr_t W2S_ADDRESS = NULL;
	uintptr_t FREE_ADDRESS = NULL;
	uintptr_t BONEMATRIX_ADDRESS = NULL;
	uintptr_t K2DRAWLINE_ADDRESS = NULL;
	uintptr_t LINEOFSIGHTTO_ADDRESS = NULL;
	uintptr_t GETVIEWPORTSIZE_ADDRESS = NULL;

	UObject* EnemyClass;

	UObject* PortalWarsCharacter()
	{
		if (!EnemyClass)
			EnemyClass = ObjObjects->FindObject("Class PortalWars.PortalWarsCharacter");

		return EnemyClass;
	}

	VOID FMemory_Free(PVOID Original) {
		return reinterpret_cast<VOID(*)(PVOID)>(FREE_ADDRESS)(Original);
	}

	FVector USkinnedMeshComponent_GetBoneMatrix(USkeletalMeshComponent* mesh, INT index) {

		auto GetBoneMatrix = reinterpret_cast<FMatrix * (*)(USkeletalMeshComponent*, FMatrix*, INT)>(BONEMATRIX_ADDRESS);

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

	VOID APlayerController_GetViewportSize(APlayerController* Controller, int* X, int* Y) {
		return reinterpret_cast<VOID(*)(APlayerController*, int*, int*)>(GETVIEWPORTSIZE_ADDRESS)(Controller, X, Y);
	}

	BOOL FindAddresses() {

		uintptr_t BaseAddress = (uintptr_t)GetModuleHandleA(NULL);

		uintptr_t World = PatternScan(BaseAddress, "48 8B 1D ? ? ? ? 48 85 DB 74 3B 41 B0 01 33 D2 48 8B CB E8");
		if (!World) return FALSE;
		int32_t Offset = *(int32_t*)(World + 3); WRLD = (UWorld*)(World + Offset + 7);

		FREE_ADDRESS = PatternScan(BaseAddress, "48 85 C9 ? 2E 53 48");
		if (!FREE_ADDRESS) return FALSE;

		W2S_ADDRESS = PatternScan(BaseAddress, "48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 18 57 48 81 EC 20 01 00 00 41 0F B6 E9");
		if (!W2S_ADDRESS) return FALSE;

		BONEMATRIX_ADDRESS = PatternScan(BaseAddress, "48 8B C4 55 53 56 57 41 54 41 56 41 57 48 8D 68 A1 48 81 EC ? ? ? ? 0F 29 78 B8 33 F6 44 0F 29 40");
		if (!BONEMATRIX_ADDRESS) return FALSE;

		LINEOFSIGHTTO_ADDRESS = PatternScan(BaseAddress, "40 55 53 56 57 48 8D 6C 24 ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 45 E0 49 8B F8 48 8B DA 48 8B F1 48 85 D2 75 07 32 C0 E9");
		if (!LINEOFSIGHTTO_ADDRESS) return FALSE;

		K2DRAWLINE_ADDRESS = PatternScan(BaseAddress, "4C 8B DC 48 81 EC ? ? ? ? 4C 89 44 24 ? 66 48 0F 6E D2 F3 0F 10 64 24 ? F3 0F 10 6C 24 ? 0F 28 C4");
		if (!K2DRAWLINE_ADDRESS) return FALSE;

		GETVIEWPORTSIZE_ADDRESS = PatternScan(BaseAddress, "48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 20 33 C0 49 8B F8 89 02 48 8B F2 41 89");
		if (!GETVIEWPORTSIZE_ADDRESS) return FALSE;

		return TRUE;
	}
}

void PostRender(PVOID UGameViewportClient, PVOID Canvas)
{
	do {
		UWorld* World = *(UWorld**)(UE4::WRLD);
		if (!World) break;

		UGameInstance* OwningGameInstance = World->OwningGameInstance;
		if (!OwningGameInstance) break;

		ULevel* PersistentLevel = World->PersistentLevel;
		if (!PersistentLevel) break;

		TArray<AActor*> Actors = PersistentLevel->Actors;

		TArray<UPlayer*> LocalPlayers = OwningGameInstance->LocalPlayers;

		UPlayer* LocalPlayer = LocalPlayers[0];
		if (!LocalPlayer) break;

		APlayerController* PlayerController = LocalPlayer->PlayerController;
		if (!PlayerController) break;

		UE4::APlayerController_GetViewportSize(PlayerController, &Width, &Height);

		APawn* MyPlayer = PlayerController->AcknowledgedPawn;
		if (!MyPlayer) break;

		// bTearOff (Ghetto dead check, if you from the hood too you already know about these)
		// https://docs.unrealengine.com/4.26/en-US/API/Runtime/Engine/GameFramework/AActor/TearOff/

		BYTE IsDead = *(BYTE*)((uintptr_t)MyPlayer + 0x58);
		if (IsDead != 0x18) break;

		APlayerState* PlayerState = MyPlayer->PlayerState;
		if (!PlayerState) break;

		BYTE MyTeamNum = PlayerState->TeamNum;

		for (auto i = 0; i < Actors.Num(); i++) {

			if (!Actors.IsValidIndex(i)) break;

			AActor* Actor = Actors[i];

			if (!Actor || Actor == MyPlayer) continue;

			if (Actor->IsA(UE4::PortalWarsCharacter())) {

				APawn* Pawn = Actor->Instigator;

				BYTE IsDead = *(BYTE*)((uintptr_t)Pawn + 0x58);
				if (IsDead != 0x18) continue;

				USkeletalMeshComponent* Mesh = Pawn->Mesh;
				if (!Mesh) continue;

				APlayerState* State = Pawn->PlayerState;

				if (State->TeamNum == MyTeamNum) continue;

				FVector rootPos = UE4::USkinnedMeshComponent_GetBoneMatrix(Mesh, BoneFNames::root);

				FVector2D rootPos2D;

				if (UE4::APlayerController_ProjectWorldLocationToScreen(PlayerController, rootPos, &rootPos2D)) {

					if (!rootPos2D.X && !rootPos2D.Y) continue;

					FLinearColor Color = { 1.f,1.f,1.f,1.f };

					if (UE4::AController_LineOfSightTo(PlayerController, Pawn)) Color = { 1.f,0.f,0.f,1.f };

					UE4::UCanvas_K2_DrawLine(Canvas, rootPos2D, FVector2D{ (float)(Width / 2) , (float)Height }, 1, Color);
				}
				// etc etc do your thing...
			}
		}
	} while (false);

	UE4::OPostRender(UGameViewportClient, Canvas);
}


BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	if (ul_reason_for_call != DLL_PROCESS_ATTACH) return FALSE;

	DisableThreadLibraryCalls(hModule);

	if (!EngineInit()) return FALSE; 
	
	if (!UE4::FindAddresses()) return FALSE;

	UWorld* World = *(UWorld**)(UE4::WRLD);
	if (!World) return FALSE;

	UGameInstance* OwningGameInstance = World->OwningGameInstance;
	if (!OwningGameInstance) return FALSE;

	TArray<UPlayer*>LocalPlayers = OwningGameInstance->LocalPlayers;

	UPlayer* LocalPlayer = LocalPlayers[0];
	if (!LocalPlayer) return FALSE;

	uintptr_t ViewPortClient = LocalPlayer->ViewportClient;
	if (!ViewPortClient) return FALSE;

	void** ViewPortClientVTable = *(void***)(ViewPortClient);
	if (!ViewPortClientVTable) return FALSE;

	DWORD protecc;
	VirtualProtect(&ViewPortClientVTable[100], 8, PAGE_EXECUTE_READWRITE, &protecc);
	UE4::OPostRender = reinterpret_cast<decltype(UE4::OPostRender)>(ViewPortClientVTable[100]);
	ViewPortClientVTable[100] = &PostRender;
	VirtualProtect(&ViewPortClientVTable[100], 8, protecc, 0);

    return TRUE;
}

