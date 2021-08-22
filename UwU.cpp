#pragma once
#include "Engine.h"

int Width;
int Height;

/*
	Splitgate Internal

	Credit to guttir14 https://github.com/guttir14/CheatIt I pasted his Engine.cpp/h and Util. Shits bussin thanks
*/

void PostRender(PVOID UGameViewportClient, PVOID Canvas)
{
	do {

		UWorld* World = *(UWorld**)(WRLD);
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

		GetViewportSize(PlayerController, Width, Height);

		APawn* MyPlayer = PlayerController->AcknowledgedPawn;
		if (!MyPlayer) break;

		// bTearOff (Ghetto dead check, if you from the hood too you already know about these)
		// https://docs.unrealengine.com/4.26/en-US/API/Runtime/Engine/GameFramework/AActor/TearOff/
		// There is also this you can use instead. Function PortalWars.PortalWarsCharacter.IsDead

		BYTE IsDead = *(BYTE*)((uintptr_t)MyPlayer + 0x58);
		if (IsDead != 0x18) break;

		APlayerState* PlayerState = MyPlayer->PlayerState;
		if (!PlayerState) break;

		BYTE MyTeamNum = PlayerState->TeamNum;

		for (auto i = 0; i < Actors.Num(); i++) {

			if (!Actors.IsValidIndex(i)) break;

			AActor* Actor = Actors[i];

			if (!Actor || Actor == MyPlayer) continue;

			if (Actor->IsA(PortalWarsCharacter())) {

				APawn* Pawn = Actor->Instigator;

				BYTE IsDead = *(BYTE*)((uintptr_t)Pawn + 0x58);
				if (IsDead != 0x18) continue;

				USkeletalMeshComponent* Mesh = Pawn->Mesh;
				if (!Mesh) continue;

				APlayerState* State = Pawn->PlayerState;

				if (State->TeamNum == MyTeamNum) continue;

				FVector rootPos = USkinnedMeshComponent_GetBoneMatrix(Mesh, 0);

				FVector2D rootPos2D;

				if (ProjectWorldLocationToScreen(PlayerController, rootPos, rootPos2D)) {

					if (!rootPos2D.X && !rootPos2D.Y) continue;

					FLinearColor Color = { 1.f,1.f,1.f,1.f };

					if (LineOfSightTo(PlayerController, Pawn)) Color = { 1.f,0.f,0.f,1.f };
					
					K2_DrawLine(Canvas, rootPos2D, FVector2D{ (float)(Width / 2) , (float)Height }, 1, Color);
				}
			}
		}
	} while (false);

	OPostRender(UGameViewportClient, Canvas);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	if (ul_reason_for_call != DLL_PROCESS_ATTACH) return FALSE;

	if (!EngineInit()) return FALSE; 
	
	UWorld* World = *(UWorld**)(WRLD);
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
	OPostRender = reinterpret_cast<decltype(OPostRender)>(ViewPortClientVTable[100]);
	ViewPortClientVTable[100] = &PostRender;
	VirtualProtect(&ViewPortClientVTable[100], 8, protecc, 0);

    return TRUE;
}

