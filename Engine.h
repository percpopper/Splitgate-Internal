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
	void ProcessEvent(void* fn, void* parms);
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
	inline FString()
	{
	};

	FString(const wchar_t* other)
	{
		Max = Count = *other ? std::wcslen(other) + 1 : 0;

		if (Count)
		{
			Data = const_cast<wchar_t*>(other);
		}
	};

	inline bool IsValid() const
	{
		return Data != nullptr;
	}

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

struct FRecoilData {
	float recoilRiseTime; 
	float recoilTotalTime;
	float verticalRecoilAmount; 
	float horizontalRecoilAmount; 
	float recoilKick; 
	float visualRecoil;
};

// Class Engine.Actor 
struct AActor : UObject {
	char pad_0000[0x30]; // 0x28 (0x30) 
	BYTE bTearOff; // 0x58 (0x01)
	char pad_0001[0xBF]; // 0x59 (0xBF) 
	class APawn* Instigator; // 0x118 (0x08) 
	char pad_0002[0x10]; // 0x120 (0x10)
	class USceneComponent* RootComponent; // 0x130 (0x08)
};

// Class Engine.Controller 
struct AController : AActor {
	char pad_0000[0xF0]; // 0x138 (0xF0)
	class APlayerState* PlayerState; // 0x228(0x08)
	bool LineOfSightTo(AActor* Other);
	void SetControlRotation(FRotator NewRotation);
};

// Class Engine.PlayerController
struct APlayerController : AController {
	char pad_0000[0x68]; // 0x230 (0x68)
	class UPlayer* Player; // 0x298 (0x08)
	class APawn* AcknowledgedPawn; // 0x2a0 (0x08)
	bool ProjectWorldLocationToScreen(FVector& WorldLocation, FVector2D& ScreenLocation);
	void GetViewportSize(INT& X, INT& Y);
};

// Class Engine.Pawn 
struct APawn : AActor {
	char pad_0000[0x108]; // 0x138 (0x108)
	class APlayerState* PlayerState; // 0x240 (0x08)
	char pad_0001[0x10]; // 0x248 (0x10)
	class AController* Controller; // 0x258 (0x08)
	char pad_0002[0x20];// 0x260 (0x20)
	class USkeletalMeshComponent* Mesh; // 0x280 (0x08) 
	char pad_0003[0x25C];// 0x288 (0x25C)
	float Health; // 0x4E4 (0x04)
	char pad_0004[0x2F8];// 0x4E8 (0x2F8)
	class AGun* CurrentWeapon; // 0x7E0 (0x08)
};

// Class PortalWars.Gun
struct AGun {
	char pad_0000[0x2FC]; // 0x0 (0x2FC)
	int32_t CurrentAmmo; // 0x2FC(0x04)
	int32_t CurrentAmmoInClip; // 0x300(0x04)
	char pad_0002[0x14]; // 0x304 (0x14)
	float EquipTime; // 0x318(0x04)
	char pad_0003[0x24]; // 0x31C (0x24)
	FRecoilData recoilConfig; // 0x340(0x18)
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
	class UGameViewportClient* ViewportClient; // 0x70 (0x08)
};

// Class Engine.PlayerState
struct APlayerState {
	char pad_0000[0x280]; // 0x0 (0x280)
	class APawn* PawnPrivate; // 0x280 (0x08)
	char pad_0001[0x78]; // 0x228 (0x78)
	FString PlayerNamePrivate; // 0x300 (0x10)
	char pad_0002[0x28]; // 0x310 (0x28)
	BYTE TeamNum; // 0x338 (0x01)
};

// Class Engine.SkinnedMeshComponent
struct USkeletalMeshComponent : UObject {
	char pad_0000[0x00]; // 0x28
	FName GetBoneName(INT BoneIndex);
	FVector GetBoneMatrix(INT index);
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

// Class Engine.Canvas
struct Canvas : UObject {
	char pad_0000[0x00]; // 0x28
	void K2_DrawLine(FVector2D ScreenPositionA, FVector2D ScreenPositionB, FLOAT Thickness, FLinearColor Color);
	void K2_DrawText(FString RenderText, FVector2D ScreenPosition, FVector2D Scale, FLinearColor RenderColor, float Kerning, FLinearColor ShadowColor, FVector2D ShadowOffset, bool bCentreX, bool bCentreY, bool bOutlined, FLinearColor OutlineColor);
};

// Class Engine.GameViewportClient
struct UGameViewportClient : UObject {
	char pad_0000[0x00]; // 0x28 
};

enum BoneFNames {
	Root = 0,
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
	knee_target_l = 68,
	knee_target_r = 69,
	RHS_ik_hand_gun = 70,
};

extern FNamePool* NamePoolData;
extern TUObjectArray* ObjObjects;
extern UWorld* WRLD;
extern UObject* WorldToScreenUFunc;
extern UObject* GetViewportSizeUFunc;
extern UObject* GetBoneNameUFunc;
extern UObject* K2_DrawLineUFunc;
extern UObject* K2_DrawTextUFunc;
extern UObject* SetControlRotationUFunc;
extern UObject* LineOfSightToUFunc;
extern UObject* EnemyClass;
extern uintptr_t GetBoneMatrixF;
extern void(*OPostRender)(UGameViewportClient* UGameViewportClient, Canvas* Canvas);

bool EngineInit();
FVector2D GetBone(USkeletalMeshComponent* Mesh, INT index, APlayerController* PlayerController);
void GetAllBoneNames(USkeletalMeshComponent* Mesh);
