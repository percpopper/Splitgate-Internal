#pragma once
#include "UwU.h"

/*
	Splitgate Internal

	It is nothing special at all & not that helpful either, that being said please do not bully me about this please thanks please im fragile i may cri.

	Regards, RiceCum1.

	https://github.com/lguilhermee/Discord-DX11-Overlay-Hook/blob/bf1fd7055d8a6815468dc204310f3dfc7b1eccdc/Helper/Helper.cpp#L12 Pattern scanner from here
	
	https://github.com/guttir14/CheatIt I pasted his Engine.cpp/h and Util. Shits bussin thanks guttir14

*/

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

			if (!Actor) continue;

			UObject* Nigga = (UObject*)Actor;

			if (Nigga->IsA(UE4::PortalWarsCharacter())) {

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

