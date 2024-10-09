#pragma once
#include <Windows.h>




struct TArray
{
    uintptr_t Array;    // Pointer to the start of the array in memory
    uint32_t Count;     // Number of valid elements in the array
    uint32_t MaxCount;  // Allocated size for the array

    uintptr_t operator[](uint32_t index) const
    {
        if (Array && index < Count) {
            // Reads the address at the specified index (assuming 8-byte pointers)
            return d.read<uintptr_t>(Array + (index * sizeof(uintptr_t)));
        }
        return 0;
    }

    uint32_t Size() const {
        return Count;
    }



    bool IsValid() const
    {
        constexpr uint32_t MAX_ARRAY_COUNT = 1000000;
        if (!Array)
            return false;

        if (Count > MaxCount)
            return false;

        if (MaxCount > MAX_ARRAY_COUNT)
            return false;

        return true;
    }

    uintptr_t GetAddress() const
    {
        return Array;
    }
};






float aimbotFov = 100.0f;  // Initial FOV value
float aimbotSmoothness = 2.0f;
const char* boneOptions[] = { "Head", "Neck", "Chest", "Pelvis", "Left Hand", "Right Hand", "Left Foot", "Right Foot" };
static int selectedBoneIndex = 0;
bool enableAimbot = true;
bool enableGodModeAndInvisibility = false;
bool enableInfiniteStamina = false;
bool enableSpeedHack = false;
bool drawAimbotFovCircle = true;
bool drawClosestTarget = true;
bool ignoreteam = true;
bool ignoredead = true;
bool boxRainbowMode = false;
bool displayCorneredBox = true;
bool displayBones = true;
bool displayHealth = true;
bool displayDistance = true;
bool displaySnaplines = false;
bool displayBox = false;
bool displayHeadDot = true;

namespace cache
{

    uint64_t PlayerController;
    uint64_t gworld;
    uint64_t PersistentLevel; //0xd8
    uint64_t owninggameinstance;
    TArray LocalPlayers;
    uint64_t LocalPlayers2;
    uint64_t localplayer;
    uint64_t playerPawn;
    uint64_t Playerstate;
    int32_t localteamid;
    uint64_t Mesh;
    uint64_t player_camera_manager;
    TArray Actors;
    uint64_t Actors2;


}