#include <Windows.h>
#include <cstdint>
#include <string>
#include <vector>
#include <iostream>

/*
	guttir14 https://github.com/guttir14/CheatIt
*/

struct FNameEntryHandle {
	uint32_t Block = 0;
	uint32_t Offset = 0;

	FNameEntryHandle(uint32_t block, uint32_t offset) : Block(block), Offset(offset) {};
	FNameEntryHandle(uint32_t id) : Block(id >> 16), Offset(id & 65535) {};
	operator uint32_t() const { return (Block << 16 | Offset); }
};

struct FNameEntry {
	uint16_t bIsWide : 1;
	uint16_t LowercaseProbeHash : 5;
	uint16_t Len : 10;
	union
	{
		char AnsiName[1024];
		wchar_t	WideName[1024];
	};

	std::string String();
};

struct FNamePool
{
	BYTE Lock[8];
	uint32_t CurrentBlock;
	uint32_t CurrentByteCursor;
	BYTE* Blocks[8192];

	FNameEntry* GetEntry(FNameEntryHandle handle) const;
};

struct FName {
	uint32_t Index;
	uint32_t Number;

	std::string GetName();
};


struct UObject {
	void** VFTable;
	uint32_t ObjectFlags;
	uint32_t InternalIndex;
	struct UClass* ClassPrivate;
	FName NamePrivate;
	UObject* OuterPrivate;

	std::string GetName();
	std::string GetFullName();
	bool IsA(void* cmp);
};

// Class CoreUObject.Field
// Size: 0x30 (Inherited: 0x28)
struct UField : UObject {
	char UnknownData_28[0x8]; // 0x28(0x08)
};

// Class CoreUObject.Struct
// Size: 0xb0 (Inherited: 0x30)
struct UStruct : UField {
	char pad_30[0x10]; // 0x30(0x10)
	UStruct* SuperStruct; // 0x40(0x8)
	char UnknownData_48[0x68]; // 0x48(0x80)
};

// Class CoreUObject.Class
// Size: 0x230 (Inherited: 0xb0)
struct UClass : UStruct {
	char UnknownData_B0[0x180]; // 0xb0(0x180)
};

struct TUObjectArray {
	BYTE** Objects;
	BYTE* PreAllocatedObjects;
	uint32_t MaxElements;
	uint32_t NumElements;
	uint32_t MaxChunks;
	uint32_t NumChunks;

	UObject* GetObjectPtr(uint32_t id) const;
	UObject* FindObject(const char* name) const;
};

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

// Class Engine.Actor 
struct AActor : UObject {
	char pad_0000[0xF0]; // 0x28 (0xF0) 
	class APawn* Instigator; // 0x118 (0x08) 
	char pad_0001[0x10]; // 0x120 (0x10)
	class USceneComponent* RootComponent; // 0x130 (0x08)
};

// Class Engine.Controller 
struct AController : AActor {
	char pad_0000[0xF0]; // 0x138 (0xF0)
	class APlayerState* PlayerState; // 0x228(0x08)
};

// Class Engine.PlayerController
struct APlayerController : AController {
	char pad_0000[0x68]; // 0x230 (0x68)
	class UPlayer* Player; // 0x298 (0x08)
	class APawn* AcknowledgedPawn; // 0x2a0 (0x08)
};

// Class Engine.Pawn 
struct APawn : AActor {
	char pad_0099[0x108]; // 0x138 (0x108)
	class APlayerState* PlayerState; // 0x240 (0x08)
	char pad_0001[0x10]; // 0x248 (0x10)
	class AController* Controller; // 0x258 (0x08)
	char pad_0002[0x20];// 0x260 (0x20)
	class USkeletalMeshComponent* Mesh; // 0x280 (0x08) 
};

// Class Engine.Level
struct ULevel {
	char pad_0000[0x98]; // 0x00 (0x98)
	TArray<AActor*> Actors; // 0x98 (0x10)
	char pad_0001[0x10]; // 0xA8 (0x10)
	class UWorld* OwningWorld; // 0xB8 (0x08)
};

// Class Engine.GameInstance
struct UGameInstance {
	char pad_0000[0x38]; // 0x0 (0x38)
	TArray<class UPlayer*> LocalPlayers; // 0x38(0x10)
};

// Class Engine.Player
struct UPlayer {
	char pad_0000[0x30]; // 0x0 (0x30)
	class APlayerController* PlayerController; // 0x30(0x08)
	char pad_0001[0x38]; // 0x38 (0x38)
	uintptr_t ViewportClient; // 0x70 (0x08)
};

// Class Engine.PlayerState
struct APlayerState {
	char pad_0000[0x280]; // 0x0 (0x280)
	class APawn* PawnPrivate; // 0x280 (0x08)
	char pad_0001[0xB0]; // 0x288 (0xB0)
	BYTE TeamNum; // 0x338 (0x01)
};

// Class Engine.SkinnedMeshComponent
struct USkeletalMeshComponent {
	char pad_0000[0x69420]; // 0x0 (0x69420)
};

// Class Engine.SceneComponent
struct USceneComponent {
	char pad_0000[0x11C]; // 0x0 (0x11C)
	struct FVector RelativeLocation; // 0x11C(0x0C)
	struct FRotator RelativeRotation; // 0x128(0x0C)
};

// Class Engine.World 
struct UWorld {
	char pad_0000[0x30]; // 0x0 (0x30)
	class ULevel* PersistentLevel; // 0x30(0x08)
	char pad_0001[0x148]; // 0x38 (0x148)
	class UGameInstance* OwningGameInstance; // 0x180 (0x08)
};

extern FNamePool* NamePoolData;
extern TUObjectArray* ObjObjects;
extern UWorld* WRLD;
extern UObject* WorldToScreenUFunc;
extern UObject* GetViewportSizeUFunc;
extern UObject* K2_DrawLineUFunc;
extern UObject* LineOfSightToUFunc;
extern UObject* EnemyClass;
extern uintptr_t GetBoneMatrixF;
extern void(*OPostRender)(PVOID UGameViewportClient, PVOID Canvas);

bool EngineInit();
void ProcessEvent(UObject* Object, void* UFunction, void* Parameters);
UObject* PortalWarsCharacter();
FVector USkinnedMeshComponent_GetBoneMatrix(USkeletalMeshComponent* mesh, INT index);
void K2_DrawLine(PVOID Canvas, FVector2D ScreenPositionA, FVector2D ScreenPositionB, FLOAT Thickness, FLinearColor Color);
void GetViewportSize(APlayerController* PlayerController, INT& X, INT& Y);
bool LineOfSightTo(AController* PlayerController, AActor* Other);
bool ProjectWorldLocationToScreen(APlayerController* PlayerController, FVector& WorldLocation, FVector2D& ScreenLocation);
