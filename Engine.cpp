#include "engine.h"
#include "Util.h"
#include <Psapi.h>

/*
	guttir14 https://github.com/guttir14/CheatIt
*/

std::string FNameEntry::String()
{
	if (bIsWide) { return std::string(); }
	return { AnsiName, Len };
}

FNameEntry* FNamePool::GetEntry(FNameEntryHandle handle) const
{
	return reinterpret_cast<FNameEntry*>(Blocks[handle.Block] + 2 * static_cast<uint64_t>(handle.Offset));
}

std::string FName::GetName()
{
	auto entry = NamePoolData->GetEntry(Index);
	auto name = entry->String();
	if (Number > 0)
	{
		name += '_' + std::to_string(Number);
	}
	auto pos = name.rfind('/');
	if (pos != std::string::npos)
	{
		name = name.substr(pos + 1);
	}
	return name;
}

std::string UObject::GetName()
{
	return NamePrivate.GetName();
}

std::string UObject::GetFullName()
{
	std::string name;
	for (auto outer = OuterPrivate; outer; outer = outer->OuterPrivate) { name = outer->GetName() + "." + name; }
	name = ClassPrivate->GetName() + " " + name + this->GetName();
	return name;
}

bool UObject::IsA(void* cmp)
{
	for (auto super = ClassPrivate; super; super = static_cast<UClass*>(super->SuperStruct)) { if (super == cmp) { return true; } }
	return false;
}

UObject* TUObjectArray::GetObjectPtr(uint32_t id) const
{
	if (id >= NumElements) return nullptr;
	uint64_t chunkIndex = id / 65536;
	if (chunkIndex >= NumChunks) return nullptr;
	auto chunk = Objects[chunkIndex];
	if (!chunk) return nullptr;
	uint32_t withinChunkIndex = id % 65536 * 24;
	auto item = *reinterpret_cast<UObject**>(chunk + withinChunkIndex);
	return item;
}

UObject* TUObjectArray::FindObject(const char* name) const
{
	for (auto i = 0u; i < NumElements; i++)
	{
		auto object = GetObjectPtr(i);
		if (object && object->GetFullName() == name) { return object; }
	}
	return nullptr;
}

void ProcessEvent(UObject* Object, void* UFunction, void* Parameters)
{
	auto VTable = *reinterpret_cast<void***>(Object);
	reinterpret_cast<void(*)(void*, void*, void*)>(VTable[68])(Object, UFunction, Parameters);
}

FNamePool* NamePoolData = nullptr;
TUObjectArray* ObjObjects = nullptr;
UWorld* WRLD = nullptr;
UObject* WorldToScreenUFunc;
UObject* GetViewportSizeUFunc;
UObject* K2_DrawLineUFunc;
UObject* LineOfSightToUFunc;
UObject* EnemyClass;
uintptr_t GetBoneMatrixF;
void(*OPostRender)(PVOID UGameViewportClient, PVOID Canvas) = nullptr;

UObject* PortalWarsCharacter()
{
	if (!EnemyClass)
		EnemyClass = ObjObjects->FindObject("Class PortalWars.PortalWarsCharacter");

	return EnemyClass;
}

bool ProjectWorldLocationToScreen(APlayerController* PlayerController, FVector& WorldLocation, FVector2D& ScreenLocation)
{
	struct {
		FVector WorldLocation;
		FVector2D ScreenLocation;
		bool bPlayerViewportRelative;
		bool ReturnValue;
	} Parameters;

	Parameters.WorldLocation = WorldLocation;
	Parameters.ScreenLocation = ScreenLocation;
	Parameters.bPlayerViewportRelative = FALSE;

	ProcessEvent(PlayerController, WorldToScreenUFunc, &Parameters);

	ScreenLocation = Parameters.ScreenLocation;

	return Parameters.ReturnValue;
}

bool LineOfSightTo(AController* Controller, AActor* Other)
{
	struct {
		AActor* Other;
		FVector ViewPoint;
		bool bAlternateChecks;
		bool ReturnValue;
	} Parameters;

	Parameters.Other = Other;
	Parameters.ViewPoint = FVector{ 0, 0, 0 };
	Parameters.bAlternateChecks = FALSE;

	ProcessEvent(Controller, LineOfSightToUFunc, &Parameters);

	return Parameters.ReturnValue;
}

void GetViewportSize(APlayerController* PlayerController, INT& X, INT& Y)
{
	struct {
		INT X;
		INT Y;
	} Parameters;

	Parameters.X = X;
	Parameters.Y = Y;

	ProcessEvent(PlayerController, GetViewportSizeUFunc, &Parameters);

	X = Parameters.X;
	Y = Parameters.Y;
}

void K2_DrawLine(PVOID Canvas, FVector2D ScreenPositionA, FVector2D ScreenPositionB, FLOAT Thickness, FLinearColor Color)
{
	struct {
		FVector2D ScreenPositionA;
		FVector2D ScreenPositionB;
		FLOAT Thickness;
		FLinearColor Color;
	} Parameters;

	Parameters.ScreenPositionA = ScreenPositionA;
	Parameters.ScreenPositionB = ScreenPositionB;
	Parameters.Thickness = Thickness;
	Parameters.Color = Color;

	ProcessEvent((UObject*)Canvas, K2_DrawLineUFunc, &Parameters);
}

FVector USkinnedMeshComponent_GetBoneMatrix(USkeletalMeshComponent* mesh, INT index) {

	auto GetBoneMatrix = reinterpret_cast<FMatrix * (*)(USkeletalMeshComponent*, FMatrix*, INT)>(GetBoneMatrixF);

	FMatrix matrix;
	GetBoneMatrix(mesh, &matrix, index);

	return FVector({ matrix.M[3][0], matrix.M[3][1], matrix.M[3][2] });
}

bool EngineInit()
{
	auto main = GetModuleHandleA(nullptr);

	static byte objSig[] = { 0x48, 0x8B, 0x05, 0x00, 0x00, 0x00, 0x00, 0x48, 0x8B, 0x0C, 0xC8, 0x48, 0x8D, 0x04, 0xD1, 0xEB };
	ObjObjects = reinterpret_cast<decltype(ObjObjects)>(FindPointer(main, objSig, sizeof(objSig), 0));
	if (!ObjObjects) return false;

	static byte poolSig[] = { 0x48, 0x8D, 0x0D, 0x00, 0x00, 0x00, 0x00, 0xE8, 0x00, 0x00, 0x00, 0x00, 0xC6, 0x05, 0x00, 0x00, 0x00, 0x00, 0x01, 0x0F, 0x10, 0x03, 0x4C, 0x8D, 0x44, 0x24, 0x20, 0x48, 0x8B, 0xC8 };
	NamePoolData = reinterpret_cast<decltype(NamePoolData)>(FindPointer(main, poolSig, sizeof(poolSig), 0));
	if (!NamePoolData) return false;

	static byte worldSig[] = { 0x48, 0x8B, 0x1D, 0x00, 0x00, 0x00, 0x00, 0x48, 0x85, 0xDB, 0x74, 0x3B, 0x41, 0xB0, 0x01, 0x33, 0xD2, 0x48, 0x8B, 0xCB, 0xE8  };
	WRLD = reinterpret_cast<decltype(WRLD)>(FindPointer(main, worldSig, sizeof(worldSig), 0));
	if (!WRLD) return false;

	static byte getbonematrixSig[] = { 0x48, 0x8B, 0xC4, 0x55, 0x53, 0x56, 0x57, 0x41, 0x54, 0x41, 0x56, 0x41, 0x57, 0x48, 0x8D, 0x68, 0xA1, 0x48, 0x81, 0xEC, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x29, 0x78, 0xB8, 0x33, 0xF6, 0x44, 0x0F, 0x29, 0x40 };
	MODULEINFO info;
	if (K32GetModuleInformation(GetCurrentProcess(), main, &info, sizeof(MODULEINFO))) {
		auto base = static_cast<byte*>(info.lpBaseOfDll);
		GetBoneMatrixF = reinterpret_cast<decltype(GetBoneMatrixF)>(FindSignature(base, base + info.SizeOfImage - 1, getbonematrixSig, sizeof(getbonematrixSig)));
		if (!GetBoneMatrixF) return false;
	}

	WorldToScreenUFunc = ObjObjects->FindObject("Function Engine.PlayerController.ProjectWorldLocationToScreen");

	GetViewportSizeUFunc = ObjObjects->FindObject("Function Engine.PlayerController.GetViewportSize");

	K2_DrawLineUFunc = ObjObjects->FindObject("Function Engine.Canvas.K2_DrawLine");

	LineOfSightToUFunc = ObjObjects->FindObject("Function Engine.Controller.LineOfSightTo");

	return true;
}
