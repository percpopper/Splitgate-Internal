#pragma once
#include "Engine.h"

int Width;
int Height;

/*
	Splitgate Internal

	Credit to guttir14 https://github.com/guttir14/CheatIt I pasted his Engine.cpp/h and Util. Shits bussin thanks
*/

void PostRender(UGameViewportClient* UGameViewportClient, Canvas* canvas)
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

		PlayerController->GetViewportSize(Width, Height);

		APawn* MyPlayer = PlayerController->AcknowledgedPawn;
		if (!MyPlayer) break;

		// bTearOff (Ghetto dead check, if you from the hood too you already know about these)
		// https://docs.unrealengine.com/4.26/en-US/API/Runtime/Engine/GameFramework/AActor/TearOff/
		// There is also this you can use instead. Function PortalWars.PortalWarsCharacter.IsDead

		BYTE IsDead = MyPlayer->bTearOff;
		if (IsDead != 0x18) break;

		APlayerState* PlayerState = MyPlayer->PlayerState;
		if (!PlayerState) break;

		BYTE MyTeamNum = PlayerState->TeamNum;

		AGun* MyGun = MyPlayer->CurrentWeapon;

		if (MyGun) {
			MyGun->recoilConfig.horizontalRecoilAmount = 0.f;
			MyGun->recoilConfig.recoilKick = 0.f;
			MyGun->recoilConfig.recoilRiseTime = 0.f;
			MyGun->recoilConfig.recoilTotalTime = 0.f;
			MyGun->recoilConfig.verticalRecoilAmount = 0.f;
			MyGun->recoilConfig.visualRecoil = 0.f;
		}

		for (auto i = 0; i < Actors.Num(); i++) {

			if (!Actors.IsValidIndex(i)) break;

			AActor* Actor = Actors[i];

			if (!Actor || Actor == MyPlayer) continue;

			if (Actor->IsA(EnemyClass)) {

				APawn* Pawn = Actor->Instigator;

				BYTE IsDead = Pawn->bTearOff;
				if (IsDead != 0x18) continue;

				USkeletalMeshComponent* Mesh = Pawn->Mesh;
				if (!Mesh) continue;

				APlayerState* State = Pawn->PlayerState;

				if (State->TeamNum == MyTeamNum) continue;

				FVector2D rootPos2D = GetBone(Mesh, BoneFNames::Root, PlayerController);
				if (!rootPos2D.X && !rootPos2D.Y) continue;

				FLinearColor Color = { 1.f, 1.f, 1.f, 1.f };

				if (PlayerController->LineOfSightTo(Pawn)) Color = { 1.f, 0.f, 0.f, 1.f };

				canvas->K2_DrawText(State->PlayerNamePrivate, rootPos2D, FVector2D{ 1.0f, 1.0f }, FLinearColor{ 1.f, 1.f, 1.f, 1.f }, 1.0f, FLinearColor{ 0, 0, 0, 0 }, FVector2D{ 0, 0 }, true, false, false, FLinearColor{ 0, 0, 0, 0 });
					
				canvas->K2_DrawLine(rootPos2D, FVector2D{ (float)Width / 2.f , (float)Height }, 1, Color);
			}
		}
	} while (false);

	OPostRender(UGameViewportClient, canvas);
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

	UGameViewportClient* ViewPortClient = LocalPlayer->ViewportClient;
	if (!ViewPortClient) return FALSE;

	void** ViewPortClientVTable = ViewPortClient->VFTable;
	if (!ViewPortClientVTable) return FALSE;

	DWORD protecc;
	VirtualProtect(&ViewPortClientVTable[100], 8, PAGE_EXECUTE_READWRITE, &protecc);
	OPostRender = reinterpret_cast<decltype(OPostRender)>(ViewPortClientVTable[100]);
	ViewPortClientVTable[100] = &PostRender;
	VirtualProtect(&ViewPortClientVTable[100], 8, protecc, 0);

    return TRUE;
}

