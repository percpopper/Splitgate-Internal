#pragma once
#include "UwU.h"

/*
	Splitgate Internal

	It is nothing special at all & not that helpful either, that being said please do not bully me about this please thanks please im fragile i may cri.

	Regards, RiceCum1.
*/

void PostRender(PVOID UGameViewportClient, PVOID Canvas)
{
	UWorld* World = (UWorld*)(*(uintptr_t*)(UE4::WRLD));
	if (!World) return UE4::OPostRender(UGameViewportClient, Canvas);

	UGameInstance* OwningGameInstance = World->OwningGameInstance;
	if (!OwningGameInstance) return UE4::OPostRender(UGameViewportClient, Canvas);

	ULevel* PersistentLevel = World->PersistentLevel;
	if (!PersistentLevel) return UE4::OPostRender(UGameViewportClient, Canvas);

	TArray<AActor*> Actors = PersistentLevel->Actors;

	TArray<UPlayer*> LocalPlayers = OwningGameInstance->LocalPlayers;

	UPlayer* LocalPlayer = LocalPlayers[0];
	if (!LocalPlayer) return UE4::OPostRender(UGameViewportClient, Canvas);

	APlayerController* PlayerController = LocalPlayer->PlayerController;
	if (!PlayerController) return UE4::OPostRender(UGameViewportClient, Canvas);

	APawn* MyPlayer = PlayerController->AcknowledgedPawn;
	if (!MyPlayer) return UE4::OPostRender(UGameViewportClient, Canvas);

	// bTearOff (Ghetto dead check, if you from the hood too you already know about these) https://docs.unrealengine.com/4.26/en-US/API/Runtime/Engine/GameFramework/AActor/TearOff/
	BYTE IsDead = *(BYTE*)((uintptr_t)MyPlayer + 0x58);
	if (IsDead != 0x18) return UE4::OPostRender(UGameViewportClient, Canvas);

	APlayerState* PlayerState = MyPlayer->PlayerState;
	if (!PlayerState) return UE4::OPostRender(UGameViewportClient, Canvas);

	BYTE MyTeamNum = PlayerState->TeamNum;

	for (auto i = 0; i < Actors.Num(); i++) {

		if (!Actors.IsValidIndex(i)) break;

		AActor* Actor = Actors[i];

		if (Actor == MyPlayer || !Actor) continue;

		// Lag calling this with so many objects bad, replace with someting to filter actor faster if you want. 
		auto ObjectName = UE4::UKismetSystemLibrary_GetObjectName(Actor);
		if (!ObjectName.data()) continue;

		// Class PortalWars.PortalWarsCharacter
		if (ObjectName.find("PortalWarsCharacter_BP_C") != std::string::npos || ObjectName.find("PortalWarsBot_BP_C") != std::string::npos) {

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

				UE4::UCanvas_K2_DrawLine(Canvas, rootPos2D, FVector2D{ Width / 2 , Height }, 1, Color);
			}


			// etc etc do your thing...
		}
	}

	UE4::OPostRender(UGameViewportClient, Canvas);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    if (ul_reason_for_call != DLL_PROCESS_ATTACH) return FALSE;

	DisableThreadLibraryCalls(hModule);

	// You might want get these using GetWindowRect or something so you can use in windowed mode
	Width = GetSystemMetrics(SM_CXSCREEN);

	Height = GetSystemMetrics(SM_CYSCREEN);

	if (!UE4::FindAddresses()) return FALSE;

	UWorld* World = (UWorld*)(*(uintptr_t*)(UE4::WRLD));
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

