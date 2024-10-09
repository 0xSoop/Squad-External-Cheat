#include "uiaccses.h"
#include <iostream>
#include <dwmapi.h>
#include "include/imgui.h"
#include "include/imgui_impl_dx9.h"
#include "include/imgui_impl_win32.h"
#include "include/imgui_internal.h"
#include "hooks/imports.hpp"
#include <d3dx9.h>
#include "driver.h"
#include <iomanip>
#include <thread>
#include "structs.h"
#include <sstream>
#include <thread>
#include <mutex>
#include <algorithm>
#include <bitset>
using namespace std;


IDirect3D9Ex* p_object = NULL;
IDirect3DDevice9Ex* p_device = NULL;
D3DPRESENT_PARAMETERS p_params = { NULL };
MSG messager = { NULL };
HWND my_wnd = NULL;
HWND game_wnd = NULL;
inline int width = GetSystemMetrics(SM_CXSCREEN);
inline int height = GetSystemMetrics(SM_CYSCREEN);




HRESULT directx_init()
{
	// Debug logging
	std::cout << "Initializing DirectX..." << std::endl;

	// Create the Direct3D object
	if (FAILED(Direct3DCreate9Ex(D3D_SDK_VERSION, &p_object)))
	{
		std::cerr << "Failed to create Direct3D object: " << GetLastError() << std::endl;
		return E_FAIL; // Return an error code instead of exiting
	}
	std::cout << "Direct3D object created successfully." << std::endl;

	// Zero out the presentation parameters
	ZeroMemory(&p_params, sizeof(p_params));
	p_params.Windowed = TRUE;
	p_params.SwapEffect = D3DSWAPEFFECT_DISCARD;
	p_params.hDeviceWindow = my_wnd;
	p_params.MultiSampleQuality = D3DMULTISAMPLE_NONE;
	p_params.BackBufferFormat = D3DFMT_A8R8G8B8;
	p_params.BackBufferWidth = width;
	p_params.BackBufferHeight = height;
	p_params.EnableAutoDepthStencil = TRUE;
	p_params.AutoDepthStencilFormat = D3DFMT_D16;
	p_params.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;

	// Create the Direct3D device
	if (FAILED(p_object->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, my_wnd,
		D3DCREATE_HARDWARE_VERTEXPROCESSING, &p_params, 0, &p_device)))
	{
		std::cerr << "Failed to create Direct3D device: " << GetLastError() << std::endl;
		p_object->Release();
		return E_FAIL; // Return an error code
	}
	std::cout << "Direct3D device created successfully." << std::endl;

	// Initialize ImGui
	ImGui::CreateContext();
	ImGui_ImplWin32_Init(my_wnd);
	ImGui_ImplDX9_Init(p_device);
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.IniFilename = nullptr; // Disable ImGui INI file saving

	// Customize ImGui styles
	ImGuiStyle* style = &ImGui::GetStyle();
	style->WindowBorderSize = 1;
	style->FrameBorderSize = 1;
	style->WindowTitleAlign = { 0.5f, 0.5f };

	// Set ImGui colors
	style->Colors[ImGuiCol_BorderShadow] = ImColor(0, 0, 0, 0);
	style->Colors[ImGuiCol_Border] = ImColor(8, 8, 8, 250);
	style->Colors[ImGuiCol_TitleBg] = ImColor(38, 38, 38, 250);
	style->Colors[ImGuiCol_TitleBgActive] = ImColor(38, 38, 38, 250);
	style->Colors[ImGuiCol_WindowBg] = ImColor(38, 38, 38, 250);
	style->Colors[ImGuiCol_FrameBg] = ImColor(38, 38, 38, 245);
	style->Colors[ImGuiCol_FrameBgActive] = ImColor(38, 38, 38, 245);
	style->Colors[ImGuiCol_FrameBgHovered] = ImColor(38, 38, 38, 245);
	style->Colors[ImGuiCol_Button] = ImColor(38, 38, 38, 245);
	style->Colors[ImGuiCol_ButtonActive] = ImColor(38, 38, 38, 245);
	style->Colors[ImGuiCol_ButtonHovered] = ImColor(38, 38, 38, 245);
	style->Colors[ImGuiCol_SliderGrab] = ImColor(68, 68, 68, 250);
	style->Colors[ImGuiCol_SliderGrabActive] = ImColor(68, 68, 68, 250);
	style->Colors[ImGuiCol_CheckMark] = ImColor(68, 68, 68, 250);
	style->Colors[ImGuiCol_Header] = ImColor(68, 68, 68, 250);
	style->Colors[ImGuiCol_HeaderActive] = ImColor(0, 0, 0, 0);
	style->Colors[ImGuiCol_HeaderHovered] = ImColor(68, 68, 68, 250);
	style->Colors[ImGuiCol_PopupBg] = ImColor(38, 38, 38, 250);

	// Clean up Direct3D object after initializing ImGui
	p_object->Release();
	std::cout << "DirectX initialization completed successfully." << std::endl;

	return S_OK; // Return success
}
bool menuopen = true;

constexpr auto OFFSET_GOBJECTS = 0x71cf4c0;
constexpr auto OFFSET_APPENDSTRING = 0x21a1760;
constexpr auto OFFSET_GNAMES = 0x7192f40;
constexpr auto OFFSET_GWORLD = 0x7313ca0;


//niggers
constexpr auto OFFSET_CustomTimeDilation = 0x098; //AActor - CustomTimeDilation
constexpr auto OFFSET_DisplayName = 0x270; //ASQEquipableItem -> AActor - DisplayName








std::string FNameToString(FName& fname)
{
	enum { NAME_SIZE = 1024 };
	char name[NAME_SIZE] = { 0 };

	const unsigned int chunkOffset = fname.ComparisonIndex >> 16;
	const unsigned short nameOffset = fname.ComparisonIndex;

	int64_t namePoolChunk = d.readv<uint64_t>(d.BaseAddress + OFFSET_GNAMES + 8 * (chunkOffset + 2)) + 2 * nameOffset;
	const auto nameLength = d.readv<uint16_t>(namePoolChunk) >> 6;

	d.readvm((namePoolChunk + 2), reinterpret_cast<uint64_t>(name), nameLength <= NAME_SIZE ? nameLength : NAME_SIZE);


	std::string finalName = std::string(name);
	return finalName.empty() ? "null" : finalName;
}


void driver_start() {
	SPOOF_FUNC
		d._processid = d.getprocessid(_(L"SquadGame.exe"));
	d.initdriver(d._processid);
	d.BaseAddress = d.get_base_address();
	printf("baseAddress %p\n", d.BaseAddress);

}

struct PrefixFilter {
	std::vector<std::string> prefixes;

	// Function to check if a given name starts with any of the prefixes
	bool Matches(const std::string& name) const {
		for (const auto& prefix : prefixes) {
			if (name.rfind(prefix, 0) == 0) { // Check if the name starts with the prefix
				return true;
			}
		}
		return false;
	}
};


ImVec2 GetScreenResolution()
{
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);
	return ImVec2(static_cast<float>(screenWidth), static_cast<float>(screenHeight));
}
inline bool filterEnabled = true;
auto draw_line(int x1, int y1, int x2, int y2, ImU32 color) -> void {
	// Snap the points to a grid
	const int snapSize = 10; // Adjust the snapping increment as needed
	x1 = (x1 / snapSize) * snapSize;
	y1 = (y1 / snapSize) * snapSize;
	x2 = (x2 / snapSize) * snapSize;
	y2 = (y2 / snapSize) * snapSize;

	// Create ImVec2 points
	ImVec2 point1(static_cast<float>(x1), static_cast<float>(y1));
	ImVec2 point2(static_cast<float>(x2), static_cast<float>(y2));

	// Draw the line using ImGui
	ImGui::GetForegroundDrawList()->AddLine(point1, point2, color);
}
enum bones : int {
	//	Root = 0,
	Bip01 = 1,
	Bip01_Pelvis = 2,
	Bip01_Spine = 3,
	Bip01_Spine1 = 4,
	Bip01_Spine2 = 5,
	Bip01_Neck = 6,
	Bip01_Head = 7,
	Bip01_HeadNub = 8,
	Bip01_REye = 9,
	Bip01_REyeNub = 10,
	Bip01_LEye = 11,
	Bip01_LEyeNub = 12,
	Bip01_MJaw = 13,
	Bip01_MJawNub = 14,
	Bip01_MBottomLip = 15,
	Bip01_MBottomLipNub = 16,
	Bip01_MTongue = 17,
	Bip01_MTongueNub = 18,
	Bip01_LMouthBottom = 19,
	Bip01_LMouthBottomNub = 20,
	Bip01_RMouthBottom = 21,
	Bip01_RMouthBottomNub = 22,
	Bip01_RMasseter = 23,
	Bip01_RMasseterNub = 24,
	Bip01_LMasseter = 25,
	Bip01_LMasseterNub = 26,
	Bip01_MUpperLip = 27,
	Bip01_MUpperLipNub = 28,
	Bip01_RCaninus = 29,
	Bip01_RCaninusNub = 30,
	Bip01_LCaninus = 31,
	Bip01_LCaninusNub = 32,
	Bip01_REyeBlinkBottom = 33,
	Bip01_REyeBlinkBottomNub = 34,
	Bip01_LEyeBlinkBottom = 35,
	Bip01_LEyeBlinkBottomNub = 36,
	Bip01_RUpperlip = 37,
	Bip01_RUpperlipNub = 38,
	Bip01_LUpperlip = 39,
	Bip01_LUpperlipNub = 40,
	Bip01_RMouthCorner = 41,
	Bip01_RMouthCornerNub = 42,
	Bip01_LMouthCorner = 43,
	Bip01_LMouthCornerNub = 44,
	Bip01_RCheek = 45,
	Bip01_RCheekNub = 46,
	Bip01_LCheek = 47,
	Bip01_LCheekNub = 48,
	Bip01_REyeBlinkTop = 49,
	Bip01_REyeBlinkTopNub = 50,
	Bip01_LEyeBlinkTop = 51,
	Bip01_LEyeBlinkTopNub = 52,
	Bip01_RInnerEyebrow = 53,
	Bip01_RInnerEyebrowNub = 54,
	Bip01_LInnerEyebrow = 55,
	Bip01_LInnerEyebrowNub = 56,
	Bip01_MMiddleEyebrow = 57,
	Bip01_MMiddleEyebrowNub = 58,
	Bip01_ROuterEyebrow = 59,
	Bip01_ROuterEyebrowNub = 60,
	Bip01_LOuterEyebrow = 61,
	Bip01_LOuterEyebrowNub = 62,
	Bip01_MNose = 63,
	Bip01_MNoseNub = 64,
	Bip01_R_Clavicle = 65,
	Bip01_R_UpperArm = 66,
	Bip01_R_Forearm = 67,
	Bip01_R_Hand = 68,
	Bip01_R_Finger0 = 69,
	Bip01_R_Finger01 = 70,
	Bip01_R_Finger02 = 71,
	Bip01_R_Finger0Nub = 72,
	Bip01_R_Finger1 = 73,
	Bip01_R_Finger11 = 74,
	Bip01_R_Finger12 = 75,
	Bip01_R_Finger1Nub = 76,
	Bip01_R_Finger2 = 77,
	Bip01_R_Finger21 = 78,
	Bip01_R_Finger22 = 79,
	Bip01_R_Finger2Nub = 80,
	Bip01_R_Finger3 = 81,
	Bip01_R_Finger31 = 82,
	Bip01_R_Finger32 = 83,
	Bip01_R_Finger3Nub = 84,
	Bip01_R_Finger4 = 85,
	Bip01_R_Finger41 = 86,
	Bip01_R_Finger42 = 87,
	Bip01_R_Finger4Nub = 88,
	Bip01_R_ForeTwist = 89,
	Bip01_R_ForeTwist1 = 90,
	Bip01_R_ForeTwist2 = 91,
	Bip01_L_Clavicle = 92,
	Bip01_L_UpperArm = 93,
	Bip01_L_Forearm = 94,
	Bip01_L_Hand = 95,
	Bip01_L_Finger0 = 96,
	Bip01_L_Finger01 = 97,
	Bip01_L_Finger02 = 98,
	Bip01_L_Finger0Nub = 99,
	Bip01_L_Finger1 = 100,
	Bip01_L_Finger11 = 101,
	Bip01_L_Finger12 = 102,
	Bip01_L_Finger1Nub = 103,
	Bip01_L_Finger2 = 104,
	Bip01_L_Finger21 = 105,
	Bip01_L_Finger22 = 106,
	Bip01_L_Finger2Nub = 107,
	Bip01_L_Finger3 = 108,
	Bip01_L_Finger31 = 109,
	Bip01_L_Finger32 = 110,
	Bip01_L_Finger3Nub = 111,
	Bip01_L_Finger4 = 112,
	Bip01_L_Finger41 = 113,
	Bip01_L_Finger42 = 114,
	Bip01_L_Finger4Nub = 115,
	Bip01_L_ForeTwist = 116,
	Bip01_L_ForeTwist1 = 117,
	Bip01_L_ForeTwist2 = 118,
	Bip01_IK_Weapon_Root = 119,
	Bip01_IK_Weapon = 120,
	Bip01_CameraBone = 121,
	Bip01_Weapon1 = 122,
	Bip01_IK_Weapon_L_Hand = 123,
	Bip01_IK_Weapon_R_Hand = 124,
	Bip01_L_Thigh = 125,
	Bip01_L_Calf = 126,
	Bip01_L_Foot = 127,
	Bip01_L_Toe0 = 128,
	Bip01_L_Toe0Nub = 129,
	Bip01_R_Thigh = 130,
	Bip01_R_Calf = 131,
	Bip01_R_Foot = 132,
	Bip01_R_Toe0 = 133,
	Bip01_R_Toe0Nub = 134,
	Bip01_IK_Hand_Root = 135,
	Bip01_IK_L_Hand = 136,
	Bip01_IK_R_Hand = 137,
	IK_Feet_Root = 138,
	IK_Left_Foot = 139,
	IK_Right_Foot = 140
};

enum bones2
{
	Root = 0,
	pelvis = 3,
	upperarm_l = 93,
	hand_l = 118,
	neck_01 = 6,
	head = 7,
	upperarm_r = 66,
	hand_r = 91,
	thigh_l = 125,
	calf_l = 126,
	foot_l = 139,
	thigh_r = 130,
	calf_r = 131,
	foot_r = 140,
	ik_hand_l = 116,
	ik_hand_r = 89,
};

bones targetBone = bones::Bip01_Head;


typedef struct _EntityList
{
	uintptr_t actor_pawn;
	uintptr_t actor_mesh;
	uintptr_t actor_state;
	Vector3 actor_pos;
	int actor_id;
	string actor_name;

	string bot_name;
	Vector3 bot_pos;
	int bot_id;

	uintptr_t item_pawn;
	Vector3 item_pos;
	int item_id;
	string item_name;

	string Ships_name;
	Vector3 Ships_pos;
	int Ships_id;
}EntityList;


FTransform GetBoneIndex(DWORD_PTR mesh, int index)
{
	DWORD_PTR bonearray = d.readv<DWORD_PTR>(mesh + 0x4b0);
	if (!bonearray)
		bonearray = d.readv<DWORD_PTR>(mesh + 0x4b0 + 0x10);
	return d.readv<FTransform>(bonearray + (index * 0x30));
}

Vector3 GetBoneWithRotation(DWORD_PTR mesh, int id)
{
	FTransform bone = GetBoneIndex(mesh, id);
	FTransform ComponentToWorld = d.readv<FTransform>(mesh + 0x1c0);
	//auto ComponentToWorld2 = d.readv<uint64_t>(mesh + 0x0158);

	D3DMATRIX Matrix;
	Matrix = MatrixMultiplication(bone.ToMatrixWithScale(), ComponentToWorld.ToMatrixWithScale());
	return Vector3(Matrix._41, Matrix._42, Matrix._43);
}



static PrefixFilter filter = { {_("BP_Soldier")} };
void cache_basic() {
	SPOOF_FUNC
		cache::gworld = d.read<uint64_t>(d.BaseAddress + OFFSET_GWORLD);
	cache::PersistentLevel = d.read<uint64_t>(cache::gworld + 0x30);
	cache::owninggameinstance = d.read<uint64_t>(cache::gworld + 0x180);
	cache::LocalPlayers = d.readv<TArray>(cache::owninggameinstance + 0x38);
	cache::localplayer = d.read<uint64_t>(cache::LocalPlayers.GetAddress());
	cache::PlayerController = d.read<uint64_t>(cache::localplayer + 0x30);
	cache::playerPawn = d.read<uint64_t>(cache::PlayerController + 0x258);
	cache::Playerstate = d.read<uint64_t>(cache::PlayerController + 0x230);
	cache::localteamid = d.readv<int32_t>(cache::Playerstate + 0x400);
	cache::Mesh = d.read<uint64_t>(cache::playerPawn + 0x288);
	cache::player_camera_manager = d.read<uint64_t>(cache::PlayerController + 0x2C0);




	std::this_thread::sleep_for(std::chrono::milliseconds(300));
}


auto DrawLine(const ImVec2& aPoint1, const ImVec2 aPoint2, ImU32 aColor, const FLOAT aLineWidth) -> VOID
{
	SPOOF_FUNC
		ImGui::GetForegroundDrawList()->AddLine(aPoint1, aPoint2, aColor, aLineWidth);
}
auto DrawBox(float x, float y, float w, float h, ImColor color) -> VOID
{
	SPOOF_FUNC
		DrawLine(ImVec2(x, y), ImVec2(x + w, y), color, 1.3f); // top 
	DrawLine(ImVec2(x, y - 1.3f), ImVec2(x, y + h + 1.4f), color, 1.3f); // left
	DrawLine(ImVec2(x + w, y - 1.3f), ImVec2(x + w, y + h + 1.4f), color, 1.3f);  // right
	DrawLine(ImVec2(x, y + h), ImVec2(x + w, y + h), color, 1.3f);   // bottom 
}
ImFont* Verdana, * DefaultFont;
auto DrawOutlinedText(ImFont* pFont, const std::string& text, const ImVec2& pos, float size, ImU32 color, bool center) -> VOID
{
	SPOOF_FUNC
		ImGui::PushFont(Verdana);
	std::stringstream stream(text);
	std::string line;

	float y = 0.0f;
	int index = 0;

	while (std::getline(stream, line))
	{
		ImVec2 textSize = pFont->CalcTextSizeA(size, FLT_MAX, 0.0f, line.c_str());

		if (center)
		{
			ImGui::GetForegroundDrawList()->AddText(pFont, size, ImVec2((pos.x - textSize.x / 2.0f) + 1, (pos.y + textSize.y * index) + 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), line.c_str());
			ImGui::GetForegroundDrawList()->AddText(pFont, size, ImVec2((pos.x - textSize.x / 2.0f) - 1, (pos.y + textSize.y * index) - 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), line.c_str());
			ImGui::GetForegroundDrawList()->AddText(pFont, size, ImVec2(pos.x - textSize.x / 2.0f, pos.y + textSize.y * index), ImGui::GetColorU32(color), line.c_str());
		}
		else
		{//
			ImGui::GetForegroundDrawList()->AddText(pFont, size, ImVec2((pos.x) + 1, (pos.y + textSize.y * index) + 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), line.c_str());
			ImGui::GetForegroundDrawList()->AddText(pFont, size, ImVec2((pos.x) - 1, (pos.y + textSize.y * index) - 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), line.c_str());
			ImGui::GetForegroundDrawList()->AddText(pFont, size, ImVec2(pos.x, pos.y + textSize.y * index), ImGui::GetColorU32(color), line.c_str());
		}

		y = pos.y + textSize.y * (index + 1);
		index++;
	}
	ImGui::PopFont();
}

void DrawHealth(Vector3 screenPosition, int Health, ImU32 color, const FCameraCacheEntry& CameraCache, Vector3 headPos, Vector3 footPos) {
#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif
	Vector3 cameraPosition = CameraCache.POV.Location;
	float distance = (cameraPosition - headPos).Length();
	ImVec2 headScreen = ImVec2(ProjectWorldToScreen(headPos).x, ProjectWorldToScreen(headPos).y);
	ImVec2 footScreen = ImVec2(ProjectWorldToScreen(footPos).x, ProjectWorldToScreen(footPos).y);
	float barHeight = std::abs(headScreen.y - footScreen.y);
	float barWidth = 8.0f;
	float padding = barWidth * 0.25f;
	float borderThickness = 2.0f;
	Health = std::max(0, std::min(Health, 100));
	float healthPercent = Health / 100.0f;
	ImDrawList* drawList = ImGui::GetForegroundDrawList();
	ImVec2 barPos(screenPosition.x - barWidth - 12.0f, headScreen.y);
	ImU32 backgroundColor = IM_COL32(30, 30, 30, 150);
	ImU32 borderColor = IM_COL32(255, 255, 255, 180);
	ImU32 gradientStartColor = IM_COL32(0, 180, 0, 255);
	ImU32 gradientMiddleColor = IM_COL32(255, 200, 0, 255);
	ImU32 gradientEndColor = IM_COL32(200, 0, 0, 255);
	ImU32 currentColor = healthPercent > 0.5f ? ImLerp(gradientMiddleColor, gradientStartColor, (healthPercent - 0.5f) * 2)
		: ImLerp(gradientEndColor, gradientMiddleColor, healthPercent * 2);
	drawList->AddRectFilled(barPos, ImVec2(barPos.x + barWidth, barPos.y + barHeight), backgroundColor, 5.0f);
	drawList->AddRectFilled(
		ImVec2(barPos.x, barPos.y + barHeight * (1.0f - healthPercent)),
		ImVec2(barPos.x + barWidth, barPos.y + barHeight),
		currentColor, 5.0f);
	drawList->AddRect(barPos, ImVec2(barPos.x + barWidth, barPos.y + barHeight), borderColor, 5.0f, 0, borderThickness);
	std::string healthText = std::to_string(Health) + "%";
	ImVec2 textSize = ImGui::CalcTextSize(healthText.c_str());
	ImVec2 textPos(barPos.x + barWidth / 2 - textSize.x / 2, barPos.y - textSize.y - padding);
	drawList->AddText(textPos, IM_COL32(255, 255, 255, 255), healthText.c_str());
	drawList->AddRectFilled(ImVec2(barPos.x + 2, barPos.y + 2), ImVec2(barPos.x + barWidth + 2, barPos.y + barHeight + 2), IM_COL32(0, 0, 0, 60), 5.0f);
}


void RenderPlayerInfo(Vector3 screenPosition, int distance, ImU32 color) {
	// Render is dead status based on checkbox
	SPOOF_FUNC

		// Render distance based on checkbox
		if (displayDistance) {
			std::string distanceText = _("Distance: [") + std::to_string(distance) + _("]");
			ImGui::GetForegroundDrawList()->AddText(ImVec2(screenPosition.x, screenPosition.y + 5), color, distanceText.c_str());
		}
}

ImVec2 screenResolution = GetScreenResolution();

ImVec2 screenCenter = ImVec2(screenResolution.x / 2.0f, screenResolution.y / 2.0f);


void move_to(float x, float y)
{
	float center_x = GetSystemMetrics(SM_CXSCREEN) / 2;
	float center_y = GetSystemMetrics(SM_CYSCREEN) / 2;


	float target_x = 0.f;
	float target_y = 0.f;

	if (x != 0.f)
	{
		if (x > center_x)
		{
			target_x = -(center_x - x);
			target_x /= aimbotSmoothness;
			if (target_x + center_x > center_x * 2.f) target_x = 0.f;
		}

		if (x < center_x)
		{
			target_x = x - center_x;
			target_x /= aimbotSmoothness;
			if (target_x + center_x < 0.f) target_x = 0.f;
		}
	}

	if (y != 0.f)
	{
		if (y > center_y)
		{
			target_y = -(center_y - y);
			target_y /= aimbotSmoothness;
			if (target_y + center_y > center_y * 2.f) target_y = 0.f;
		}

		if (y < center_y)
		{
			target_y = y - center_y;
			target_y /= aimbotSmoothness;
			if (target_y + center_y < 0.f) target_y = 0.f;
		}
	}
	d.move_mouse(target_x, target_y, NULL);
}
float GetDistance(ImVec2 point1, ImVec2 point2)
{
	return sqrt(pow(point1.x - point2.x, 2) + pow(point1.y - point2.y, 2));
}

uintptr_t lockedTarget = 0;   // Store the currently locked target actor
bool isLockedOn = false;

float CalculateDistance(Vector3 actorWorldPos, Vector3 cameraPosition)
{
	Vector3 distanceVec = actorWorldPos - cameraPosition;
	return sqrtf(distanceVec.x * distanceVec.x + distanceVec.y * distanceVec.y + distanceVec.z * distanceVec.z);
}



auto aimbot_ya_heard()
{
	while (true)
	{
		SPOOF_FUNC

			// Check if RMB is pressed to activate the aimbot
			bool isAimbotActive = (GetAsyncKeyState(VK_RBUTTON) & 0x8000) && enableAimbot;

		if (isAimbotActive && !isLockedOn)  // If RMB is pressed but no target is locked
		{
			uintptr_t closestTarget = 0;
			float minDistance = FLT_MAX; // Initialize with maximum possible value

			for (int i = 0; i < cache::Actors.Size(); i++)
			{
				auto CurrentActor = cache::Actors[i];
				auto CameraCache = d.readv<FCameraCacheEntry>(cache::player_camera_manager + 0x1AF0);
				auto trust = d.readv<FName>(CurrentActor + 0x18);
				std::string brain = FNameToString(trust);

				bool shouldRender = !filterEnabled || filter.Matches(brain);

				if (shouldRender)
				{

					auto actorPawn = d.read<uintptr_t>(cache::Actors2 + i * 0x8);
					auto actorState = d.read<uint64_t>(actorPawn + 0x0248);
					auto actorid = d.readv<int32_t>(actorState + 0x400);
					auto actor_health = d.readv<float>(actorPawn + 0x1df8);
					auto actorRootComponent = d.read<uint64_t>(CurrentActor + 0x138);
					Vector3 actorWorldPos = d.readv<Vector3>(actorRootComponent + 0x11c);
					if (cache::playerPawn == actorPawn || actor_health <= 0.0f) {
						continue;
					}

					if (ignoreteam && actorid == cache::localteamid) {
						continue;
					}

					auto actorMesh = d.read<uint64_t>(actorPawn + 0x288);
					if (!actorMesh) {
						continue;
					}

					// Get the actor's head position in world space and project it to screen space
					Vector3 vHeadBone = ProjectWorldToScreen(GetBoneWithRotation(actorMesh, bones2::head));

					// Calculate distance between head bone position and screen center (FOV check)
					ImVec2 headBoneScreenPos(vHeadBone.x, vHeadBone.y);
					float distanceToCenter = GetDistance(headBoneScreenPos, screenCenter);

					// Get 3D distance between camera and actor for prioritizing closest target
					float worldDistance = CalculateDistance(actorWorldPos, CameraCache.POV.Location);

					// If the target is within FOV and is closer than the current closest target, lock onto it
					if (distanceToCenter <= aimbotFov && worldDistance < minDistance)
					{
						closestTarget = actorPawn;
						minDistance = worldDistance;
					}
				}
			}

			// Lock onto the closest target found
			if (closestTarget != 0)
			{
				lockedTarget = closestTarget;
				isLockedOn = true;
			}
		}
		else if (isLockedOn)  // If a target is already locked
		{
			// Read the actorMesh again for the locked target
			auto actorMesh = d.read<uint64_t>(lockedTarget + 0x288);
			if (!actorMesh) {
				isLockedOn = false;  // If target is invalid, unlock
				continue;
			}

			auto actor_health = d.readv<float>(lockedTarget + 0x1df8);
			if (actor_health <= 0.0f) {
				isLockedOn = false;  // Unlock if target is dead
				continue;
			}

			// Get the head bone position in screen space for the locked target
			Vector3 vTargetBonePos = ProjectWorldToScreen(GetBoneWithRotation(actorMesh, targetBone));
			move_to(vTargetBonePos.x, vTargetBonePos.y);  // Move to the locked target's head position

			// Unlock target if RMB is released
			if (!(GetAsyncKeyState(VK_RBUTTON) & 0x8000))
			{
				isLockedOn = false;
				lockedTarget = 0;
			}
		}

		// Small delay to avoid excessive CPU usage
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}
}




void DrawChams(uint64_t actorMesh, ImColor Color, const FCameraCacheEntry& CameraCache)
{
	// Define main bones for visualization
	const std::vector<int> mainBones = {
		bones::Bip01_Head, bones::Bip01_Neck, bones::Bip01_Spine, bones::Bip01_Spine1, bones::Bip01_Spine2,
		bones::Bip01_R_UpperArm, bones::Bip01_R_Forearm, bones::Bip01_R_Hand,
		bones::Bip01_L_UpperArm, bones::Bip01_L_Forearm, bones::Bip01_L_Hand,
		bones::Bip01_R_Thigh, bones::Bip01_R_Calf, bones::Bip01_R_Foot,
		bones::Bip01_L_Thigh, bones::Bip01_L_Calf, bones::Bip01_L_Foot
	};

	// Calculate distance and thickness
	Vector3 cameraPosition = CameraCache.POV.Location;
	Vector3 headPos = GetBoneWithRotation(actorMesh, bones::Bip01_Head);
	float distance = (cameraPosition - headPos).Length();
	float minThickness = 2.0f;
	float maxThickness = 6.0f;
	float scalingFactor = 500.0f;
	float thickness = std::clamp(maxThickness - (distance / scalingFactor), minThickness, maxThickness);

	// Draw bones with the specified color and thickness
	for (size_t i = 1; i < mainBones.size(); i++)
	{
		Vector3 startPos = ProjectWorldToScreen(GetBoneWithRotation(actorMesh, mainBones[i - 1]));
		Vector3 endPos = ProjectWorldToScreen(GetBoneWithRotation(actorMesh, mainBones[i]));

		// Draw the line between bones to form a chams-like effect
		DrawLine(ImVec2(startPos.x, startPos.y), ImVec2(endPos.x, endPos.y), Color, thickness);
	}
}




void DrawPlayerBones(uint64_t actorMesh, Vector3 headPos, ImColor Color, const FCameraCacheEntry& CameraCache)
{
	Vector3 cameraPosition = CameraCache.POV.Location;
	float distance = (cameraPosition - headPos).Length();
	float minThickness = 0.5f;
	float maxThickness = 3.0f;
	float scalingFactor = 200.0f;
	float thickness = std::clamp(maxThickness - (distance / scalingFactor), minThickness, maxThickness);

	// Define main body bones using the 'bones' enum
	Vector3 vHead = ProjectWorldToScreen(GetBoneWithRotation(actorMesh, bones::Bip01_Head));
	Vector3 vNeck = ProjectWorldToScreen(GetBoneWithRotation(actorMesh, bones::Bip01_Neck));
	Vector3 vPelvis = ProjectWorldToScreen(GetBoneWithRotation(actorMesh, bones::Bip01_Pelvis));
	Vector3 vSpine1 = ProjectWorldToScreen(GetBoneWithRotation(actorMesh, bones::Bip01_Spine));
	Vector3 vSpine2 = ProjectWorldToScreen(GetBoneWithRotation(actorMesh, bones::Bip01_Spine1));
	Vector3 vSpine3 = ProjectWorldToScreen(GetBoneWithRotation(actorMesh, bones::Bip01_Spine2));

	// Arm bones
	Vector3 vClavicleLeft = ProjectWorldToScreen(GetBoneWithRotation(actorMesh, bones::Bip01_L_Clavicle));
	Vector3 vUpperArmLeft = ProjectWorldToScreen(GetBoneWithRotation(actorMesh, bones::Bip01_L_UpperArm));
	Vector3 vForearmLeft = ProjectWorldToScreen(GetBoneWithRotation(actorMesh, bones::Bip01_L_Forearm));
	Vector3 vHandLeft = ProjectWorldToScreen(GetBoneWithRotation(actorMesh, bones::Bip01_L_Hand));

	Vector3 vClavicleRight = ProjectWorldToScreen(GetBoneWithRotation(actorMesh, bones::Bip01_R_Clavicle));
	Vector3 vUpperArmRight = ProjectWorldToScreen(GetBoneWithRotation(actorMesh, bones::Bip01_R_UpperArm));
	Vector3 vForearmRight = ProjectWorldToScreen(GetBoneWithRotation(actorMesh, bones::Bip01_R_Forearm));
	Vector3 vHandRight = ProjectWorldToScreen(GetBoneWithRotation(actorMesh, bones::Bip01_R_Hand));

	// Leg bones
	Vector3 vThighLeft = ProjectWorldToScreen(GetBoneWithRotation(actorMesh, bones::Bip01_L_Thigh));
	Vector3 vCalfLeft = ProjectWorldToScreen(GetBoneWithRotation(actorMesh, bones::Bip01_L_Calf));
	Vector3 vFootLeft = ProjectWorldToScreen(GetBoneWithRotation(actorMesh, bones::Bip01_L_Foot));

	Vector3 vThighRight = ProjectWorldToScreen(GetBoneWithRotation(actorMesh, bones::Bip01_R_Thigh));
	Vector3 vCalfRight = ProjectWorldToScreen(GetBoneWithRotation(actorMesh, bones::Bip01_R_Calf));
	Vector3 vFootRight = ProjectWorldToScreen(GetBoneWithRotation(actorMesh, bones::Bip01_R_Foot));

	// Finger bones for left hand
	Vector3 vFinger0_L = ProjectWorldToScreen(GetBoneWithRotation(actorMesh, bones::Bip01_L_Finger0));
	Vector3 vFinger01_L = ProjectWorldToScreen(GetBoneWithRotation(actorMesh, bones::Bip01_L_Finger01));
	Vector3 vFinger02_L = ProjectWorldToScreen(GetBoneWithRotation(actorMesh, bones::Bip01_L_Finger02));

	Vector3 vFinger1_L = ProjectWorldToScreen(GetBoneWithRotation(actorMesh, bones::Bip01_L_Finger1));
	Vector3 vFinger11_L = ProjectWorldToScreen(GetBoneWithRotation(actorMesh, bones::Bip01_L_Finger11));
	Vector3 vFinger12_L = ProjectWorldToScreen(GetBoneWithRotation(actorMesh, bones::Bip01_L_Finger12));

	Vector3 vFinger2_L = ProjectWorldToScreen(GetBoneWithRotation(actorMesh, bones::Bip01_L_Finger2));
	Vector3 vFinger21_L = ProjectWorldToScreen(GetBoneWithRotation(actorMesh, bones::Bip01_L_Finger21));
	Vector3 vFinger22_L = ProjectWorldToScreen(GetBoneWithRotation(actorMesh, bones::Bip01_L_Finger22));

	Vector3 vFinger3_L = ProjectWorldToScreen(GetBoneWithRotation(actorMesh, bones::Bip01_L_Finger3));
	Vector3 vFinger31_L = ProjectWorldToScreen(GetBoneWithRotation(actorMesh, bones::Bip01_L_Finger31));
	Vector3 vFinger32_L = ProjectWorldToScreen(GetBoneWithRotation(actorMesh, bones::Bip01_L_Finger32));

	Vector3 vFinger4_L = ProjectWorldToScreen(GetBoneWithRotation(actorMesh, bones::Bip01_L_Finger4));
	Vector3 vFinger41_L = ProjectWorldToScreen(GetBoneWithRotation(actorMesh, bones::Bip01_L_Finger41));
	Vector3 vFinger42_L = ProjectWorldToScreen(GetBoneWithRotation(actorMesh, bones::Bip01_L_Finger42));

	// Finger bones for right hand
	Vector3 vFinger0_R = ProjectWorldToScreen(GetBoneWithRotation(actorMesh, bones::Bip01_R_Finger0));
	Vector3 vFinger01_R = ProjectWorldToScreen(GetBoneWithRotation(actorMesh, bones::Bip01_R_Finger01));
	Vector3 vFinger02_R = ProjectWorldToScreen(GetBoneWithRotation(actorMesh, bones::Bip01_R_Finger02));

	Vector3 vFinger1_R = ProjectWorldToScreen(GetBoneWithRotation(actorMesh, bones::Bip01_R_Finger1));
	Vector3 vFinger11_R = ProjectWorldToScreen(GetBoneWithRotation(actorMesh, bones::Bip01_R_Finger11));
	Vector3 vFinger12_R = ProjectWorldToScreen(GetBoneWithRotation(actorMesh, bones::Bip01_R_Finger12));

	Vector3 vFinger2_R = ProjectWorldToScreen(GetBoneWithRotation(actorMesh, bones::Bip01_R_Finger2));
	Vector3 vFinger21_R = ProjectWorldToScreen(GetBoneWithRotation(actorMesh, bones::Bip01_R_Finger21));
	Vector3 vFinger22_R = ProjectWorldToScreen(GetBoneWithRotation(actorMesh, bones::Bip01_R_Finger22));

	Vector3 vFinger3_R = ProjectWorldToScreen(GetBoneWithRotation(actorMesh, bones::Bip01_R_Finger3));
	Vector3 vFinger31_R = ProjectWorldToScreen(GetBoneWithRotation(actorMesh, bones::Bip01_R_Finger31));
	Vector3 vFinger32_R = ProjectWorldToScreen(GetBoneWithRotation(actorMesh, bones::Bip01_R_Finger32));

	Vector3 vFinger4_R = ProjectWorldToScreen(GetBoneWithRotation(actorMesh, bones::Bip01_R_Finger4));
	Vector3 vFinger41_R = ProjectWorldToScreen(GetBoneWithRotation(actorMesh, bones::Bip01_R_Finger41));
	Vector3 vFinger42_R = ProjectWorldToScreen(GetBoneWithRotation(actorMesh, bones::Bip01_R_Finger42));

	// Draw central spine
	DrawLine(ImVec2(vPelvis.x, vPelvis.y), ImVec2(vSpine1.x, vSpine1.y), Color, thickness);
	DrawLine(ImVec2(vSpine1.x, vSpine1.y), ImVec2(vSpine2.x, vSpine2.y), Color, thickness);
	DrawLine(ImVec2(vSpine2.x, vSpine2.y), ImVec2(vSpine3.x, vSpine3.y), Color, thickness);
	DrawLine(ImVec2(vSpine3.x, vSpine3.y), ImVec2(vNeck.x, vNeck.y), Color, thickness);
	DrawLine(ImVec2(vNeck.x, vNeck.y), ImVec2(vHead.x, vHead.y), Color, thickness);

	// Draw arms and hands
	DrawLine(ImVec2(vNeck.x, vNeck.y), ImVec2(vClavicleLeft.x, vClavicleLeft.y), Color, thickness);
	DrawLine(ImVec2(vClavicleLeft.x, vClavicleLeft.y), ImVec2(vUpperArmLeft.x, vUpperArmLeft.y), Color, thickness);
	DrawLine(ImVec2(vUpperArmLeft.x, vUpperArmLeft.y), ImVec2(vForearmLeft.x, vForearmLeft.y), Color, thickness);
	DrawLine(ImVec2(vForearmLeft.x, vForearmLeft.y), ImVec2(vHandLeft.x, vHandLeft.y), Color, thickness);

	DrawLine(ImVec2(vNeck.x, vNeck.y), ImVec2(vClavicleRight.x, vClavicleRight.y), Color, thickness);
	DrawLine(ImVec2(vClavicleRight.x, vClavicleRight.y), ImVec2(vUpperArmRight.x, vUpperArmRight.y), Color, thickness);
	DrawLine(ImVec2(vUpperArmRight.x, vUpperArmRight.y), ImVec2(vForearmRight.x, vForearmRight.y), Color, thickness);
	DrawLine(ImVec2(vForearmRight.x, vForearmRight.y), ImVec2(vHandRight.x, vHandRight.y), Color, thickness);

	// Draw fingers (left hand)
	DrawLine(ImVec2(vHandLeft.x, vHandLeft.y), ImVec2(vFinger0_L.x, vFinger0_L.y), Color, thickness);
	DrawLine(ImVec2(vFinger0_L.x, vFinger0_L.y), ImVec2(vFinger01_L.x, vFinger01_L.y), Color, thickness);
	DrawLine(ImVec2(vFinger01_L.x, vFinger01_L.y), ImVec2(vFinger02_L.x, vFinger02_L.y), Color, thickness);

	DrawLine(ImVec2(vHandLeft.x, vHandLeft.y), ImVec2(vFinger1_L.x, vFinger1_L.y), Color, thickness);
	DrawLine(ImVec2(vFinger1_L.x, vFinger1_L.y), ImVec2(vFinger11_L.x, vFinger11_L.y), Color, thickness);
	DrawLine(ImVec2(vFinger11_L.x, vFinger11_L.y), ImVec2(vFinger12_L.x, vFinger12_L.y), Color, thickness);

	DrawLine(ImVec2(vHandLeft.x, vHandLeft.y), ImVec2(vFinger2_L.x, vFinger2_L.y), Color, thickness);
	DrawLine(ImVec2(vFinger2_L.x, vFinger2_L.y), ImVec2(vFinger21_L.x, vFinger21_L.y), Color, thickness);
	DrawLine(ImVec2(vFinger21_L.x, vFinger21_L.y), ImVec2(vFinger22_L.x, vFinger22_L.y), Color, thickness);

	DrawLine(ImVec2(vHandLeft.x, vHandLeft.y), ImVec2(vFinger3_L.x, vFinger3_L.y), Color, thickness);
	DrawLine(ImVec2(vFinger3_L.x, vFinger3_L.y), ImVec2(vFinger31_L.x, vFinger31_L.y), Color, thickness);
	DrawLine(ImVec2(vFinger31_L.x, vFinger31_L.y), ImVec2(vFinger32_L.x, vFinger32_L.y), Color, thickness);

	DrawLine(ImVec2(vHandLeft.x, vHandLeft.y), ImVec2(vFinger4_L.x, vFinger4_L.y), Color, thickness);
	DrawLine(ImVec2(vFinger4_L.x, vFinger4_L.y), ImVec2(vFinger41_L.x, vFinger41_L.y), Color, thickness);
	DrawLine(ImVec2(vFinger41_L.x, vFinger41_L.y), ImVec2(vFinger42_L.x, vFinger42_L.y), Color, thickness);

	// Draw fingers (right hand)
	DrawLine(ImVec2(vHandRight.x, vHandRight.y), ImVec2(vFinger0_R.x, vFinger0_R.y), Color, thickness);
	DrawLine(ImVec2(vFinger0_R.x, vFinger0_R.y), ImVec2(vFinger01_R.x, vFinger01_R.y), Color, thickness);
	DrawLine(ImVec2(vFinger01_R.x, vFinger01_R.y), ImVec2(vFinger02_R.x, vFinger02_R.y), Color, thickness);

	DrawLine(ImVec2(vHandRight.x, vHandRight.y), ImVec2(vFinger1_R.x, vFinger1_R.y), Color, thickness);
	DrawLine(ImVec2(vFinger1_R.x, vFinger1_R.y), ImVec2(vFinger11_R.x, vFinger11_R.y), Color, thickness);
	DrawLine(ImVec2(vFinger11_R.x, vFinger11_R.y), ImVec2(vFinger12_R.x, vFinger12_R.y), Color, thickness);

	DrawLine(ImVec2(vHandRight.x, vHandRight.y), ImVec2(vFinger2_R.x, vFinger2_R.y), Color, thickness);
	DrawLine(ImVec2(vFinger2_R.x, vFinger2_R.y), ImVec2(vFinger21_R.x, vFinger21_R.y), Color, thickness);
	DrawLine(ImVec2(vFinger21_R.x, vFinger21_R.y), ImVec2(vFinger22_R.x, vFinger22_R.y), Color, thickness);

	DrawLine(ImVec2(vHandRight.x, vHandRight.y), ImVec2(vFinger3_R.x, vFinger3_R.y), Color, thickness);
	DrawLine(ImVec2(vFinger3_R.x, vFinger3_R.y), ImVec2(vFinger31_R.x, vFinger31_R.y), Color, thickness);
	DrawLine(ImVec2(vFinger31_R.x, vFinger31_R.y), ImVec2(vFinger32_R.x, vFinger32_R.y), Color, thickness);

	DrawLine(ImVec2(vHandRight.x, vHandRight.y), ImVec2(vFinger4_R.x, vFinger4_R.y), Color, thickness);
	DrawLine(ImVec2(vFinger4_R.x, vFinger4_R.y), ImVec2(vFinger41_R.x, vFinger41_R.y), Color, thickness);
	DrawLine(ImVec2(vFinger41_R.x, vFinger41_R.y), ImVec2(vFinger42_R.x, vFinger42_R.y), Color, thickness);

	// Draw legs
	DrawLine(ImVec2(vPelvis.x, vPelvis.y), ImVec2(vThighLeft.x, vThighLeft.y), Color, thickness);
	DrawLine(ImVec2(vThighLeft.x, vThighLeft.y), ImVec2(vCalfLeft.x, vCalfLeft.y), Color, thickness);
	DrawLine(ImVec2(vCalfLeft.x, vCalfLeft.y), ImVec2(vFootLeft.x, vFootLeft.y), Color, thickness);

	DrawLine(ImVec2(vPelvis.x, vPelvis.y), ImVec2(vThighRight.x, vThighRight.y), Color, thickness);
	DrawLine(ImVec2(vThighRight.x, vThighRight.y), ImVec2(vCalfRight.x, vCalfRight.y), Color, thickness);
	DrawLine(ImVec2(vCalfRight.x, vCalfRight.y), ImVec2(vFootRight.x, vFootRight.y), Color, thickness);
}

void DrawCorneredBox(float X, float Y, float W, float H, const ImU32& color, float thickness)
{
	auto vList = ImGui::GetForegroundDrawList();
	ImU32 col;
	if (boxRainbowMode)
	{
		static float hue = 0.0f;
		hue += 0.00005f;
		if (hue > 1.0f) hue = 0.0f;
		float r, g, b;
		ImGui::ColorConvertHSVtoRGB(hue, 1.0f, 1.0f, r, g, b);
		col = IM_COL32((int)(r * 255), (int)(g * 255), (int)(b * 255), 255);
	}
	else
	{
		col = color;
	}
	float lineW = (W / 3);
	float lineH = (H / 3);
	vList->AddLine(ImVec2(X, Y), ImVec2(X, Y + lineH), col, thickness);
	vList->AddLine(ImVec2(X, Y), ImVec2(X + lineW, Y), col, thickness);
	vList->AddLine(ImVec2(X + W, Y), ImVec2(X + W - lineW, Y), col, thickness);
	vList->AddLine(ImVec2(X + W, Y), ImVec2(X + W, Y + lineH), col, thickness);
	vList->AddLine(ImVec2(X, Y + H), ImVec2(X + lineW, Y + H), col, thickness);
	vList->AddLine(ImVec2(X, Y + H), ImVec2(X, Y + H - lineH), col, thickness);
	vList->AddLine(ImVec2(X + W, Y + H), ImVec2(X + W - lineW, Y + H), col, thickness);
	vList->AddLine(ImVec2(X + W, Y + H), ImVec2(X + W, Y + H - lineH), col, thickness);
}
void DrawHealth(float X, float Y, float W, float H, int Health, const ImU32& color) {
	ImDrawList* drawList = ImGui::GetForegroundDrawList();
	float barHeight = H;
	float barWidth = 4.0f;
	float padding = 4.0f;
	Health = std::max(0, std::min(Health, 100));
	float healthPercent = Health / 100.0f;
	ImVec2 barPos(X - barWidth - padding, Y);
	ImU32 backgroundColor = IM_COL32(30, 30, 30, 150);
	ImU32 healthColor = ImLerp(IM_COL32(200, 0, 0, 255), IM_COL32(0, 255, 0, 255), healthPercent);
	drawList->AddRectFilled(barPos, ImVec2(barPos.x + barWidth, barPos.y + barHeight), backgroundColor, 5.0f);
	drawList->AddRectFilled(
		ImVec2(barPos.x, barPos.y + barHeight * (1.0f - healthPercent)),
		ImVec2(barPos.x + barWidth, barPos.y + barHeight),
		healthColor, 5.0f);
	drawList->AddLine(ImVec2(barPos.x, barPos.y), ImVec2(barPos.x, barPos.y + barHeight), IM_COL32(255, 255, 255, 180), 0.5f);
	drawList->AddLine(ImVec2(barPos.x + barWidth, barPos.y), ImVec2(barPos.x + barWidth, barPos.y + barHeight), IM_COL32(255, 255, 255, 180), 0.5f);
	drawList->AddLine(ImVec2(barPos.x, barPos.y), ImVec2(barPos.x + barWidth, barPos.y), IM_COL32(255, 255, 255, 180), 0.5f);
	drawList->AddLine(ImVec2(barPos.x, barPos.y + barHeight), ImVec2(barPos.x + barWidth, barPos.y + barHeight), IM_COL32(255, 255, 255, 180), 0.5f);
	std::string healthText = std::to_string(Health) + "%";
	ImVec2 textSize = ImGui::CalcTextSize(healthText.c_str());
	ImVec2 textPos(barPos.x + barWidth / 2 - textSize.x / 2, barPos.y - textSize.y - padding);
	drawList->AddText(textPos, IM_COL32(255, 255, 255, 255), healthText.c_str());
}

void DrawFOVCircle(ImDrawList* drawList, ImVec2 screenCenter, float aimbotFov, ImU32 color)
{
	// Number of segments for the jagged look (less segments = more jagged)
	int numSegments = 80; // Adjust this number for more or less jaggedness

	// Draw the jagged circle with reduced segments
	drawList->AddCircle(
		screenCenter,    // Center of the circle
		aimbotFov,       // Radius of the circle (FOV size)
		color,           // Color of the circle (RGBA format)
		numSegments,     // Lower number of segments for jagged look
		2.0f             // Thickness of the circle's line
	);
}


struct InitialWeaponData {
	bool bInfiniteAmmo;   // 0x0000 - 1 byte
	bool bInfiniteMags;   // 0x0001 - 1 byte
};


DWORD offset_last_submit_time = 0x2b4; // AServerStatReplicator -> NumRelevantDeletedActors
DWORD offset_last_render_time = 0x2bc; // AServerStatReplicator -> NumReplicatedActors



void DrawBox(float X, float Y, float W, float H, const ImU32& color, float thickness) {
	ImDrawList* drawList = ImGui::GetForegroundDrawList();
	drawList->AddRect(ImVec2(X, Y), ImVec2(X + W, Y + H), color, 0.0f, 0, thickness);
}

Vector3 calculate_new_rotation(Vector3& zaz, Vector3& daz)
{
	Vector3 dalte = zaz - daz;
	Vector3 ongle;
	float hpm = sqrtf(dalte.x * dalte.x + dalte.y * dalte.y);
	ongle.y = atan(dalte.y / dalte.x) * 57.295779513082f;
	ongle.x = (atan(dalte.z / hpm) * 57.295779513082f) * -1.f;

	if (dalte.x >= 0.f) ongle.y += 180.f;

	return ongle;
}

void silent_aim(uintptr_t target_mesh)
{
	Vector3 vHead = ProjectWorldToScreen(GetBoneWithRotation(target_mesh, bones::Bip01_Head));

	auto CameraCache = d.readv<FCameraCacheEntry>(cache::player_camera_manager + 0x1AF0);
	uintptr_t view_yaw_min = 0x27FC;
	uintptr_t view_yaw_max = 0x2800;
	uintptr_t aim_pitch_min = 0x27F4;
	uintptr_t aim_pitch_max = 0x27F8;
	if (vHead.x != 0 || vHead.y != 0)
	{
		Vector3 new_rotation = calculate_new_rotation(CameraCache.POV.Location, vHead);
		static float og_pitch_min = d.read<float>(cache::player_camera_manager + view_yaw_min);
		static float og_pitch_max = d.read<float>(cache::player_camera_manager + view_yaw_max);

		d.write<float>(d.read(cache::playerPawn + 0x150) + aim_pitch_min, new_rotation.x);
		d.write<float>(d.read(cache::playerPawn + 0x150) + aim_pitch_max, new_rotation.x);

		d.write<float>(cache::player_camera_manager + view_yaw_min, new_rotation.y);
		d.write<float>(cache::player_camera_manager + view_yaw_max, new_rotation.y);

		Sleep(5);

		d.write<float>(cache::player_camera_manager + view_yaw_min, og_pitch_min);
		d.write<float>(cache::player_camera_manager + view_yaw_max, og_pitch_max);
	}
}



bool is_visible(uintptr_t mesh)
{
	float last_sumbit_time = d.readv<float>(mesh + offset_last_submit_time);
	float last_render_time_on_screen = d.readv<float>(mesh + offset_last_render_time);

	return last_render_time_on_screen + 0.06f >= last_sumbit_time;
}

void RenderDistanceText(Vector3 actorWorldPos, Vector3 cameraPosition, bool displayDistance, ImVec2 TopBox, ImVec2 BottomBox, float CornerWidth)
{
	Vector3 distanceVec = actorWorldPos - cameraPosition;
	float distance = sqrtf(distanceVec.x * distanceVec.x + distanceVec.y * distanceVec.y + distanceVec.z * distanceVec.z);
	distance = distance / 100.0f;
	std::string distanceText = "[" + std::to_string(static_cast<int>(distance)) + "m]";
	float boxCenterX = TopBox.x - (CornerWidth / 2);
	ImVec2 textPosition(boxCenterX + (CornerWidth / 2), BottomBox.y + 5);
	ImDrawList* drawList = ImGui::GetForegroundDrawList();
	ImVec2 textSize = ImGui::CalcTextSize(distanceText.c_str());
	textPosition.x -= textSize.x / 2;
	ImU32 textColor;
	if (distance < 20.0f)
		textColor = IM_COL32(0, 255, 0, 255);
	else if (distance >= 20.0f && distance < 50.0f)
		textColor = IM_COL32(0, 200, 0, 255);
	else if (distance >= 50.0f && distance < 150.0f)
		textColor = IM_COL32(255, 255, 0, 255);
	else
		textColor = IM_COL32(255, 0, 0, 255);
	drawList->AddText(textPosition, textColor, distanceText.c_str());
}


void ReadInitialWeaponData(uintptr_t baseAddress) {
	// Create a variable to hold the initial part of the structure
	InitialWeaponData initialData = {};

	// Calculate the address of the WeaponConfig starting point (assuming baseAddress is the correct pointer)
	uintptr_t weaponConfigAddress = baseAddress + 0x0620;

	// Read the initial 2 bytes from the structure into the InitialWeaponData variable
	if (d.readvm(weaponConfigAddress, reinterpret_cast<uintptr_t>(&initialData), sizeof(InitialWeaponData))) {
		std::cout << "Successfully read initial WeaponConfig data!" << std::endl;
		std::cout << "bInfiniteAmmo: " << (initialData.bInfiniteAmmo ? "True" : "False") << std::endl;
		std::cout << "bInfiniteMags: " << (initialData.bInfiniteMags ? "True" : "False") << std::endl;
	}
	else {
		std::cout << "Failed to read initial WeaponConfig data from memory!" << std::endl;
	}
}


void ReadBitfields(uintptr_t baseAddress) {
	// Calculate the address of the bitfield
	uintptr_t bitfieldAddress = baseAddress + 0x1760;

	// Read the entire byte that contains the bitfields
	uint8_t bitfieldValue = d.read<uint8_t>(bitfieldAddress);

	// Print the raw byte value in binary form for visualization
	std::cout << "Bitfield raw value (binary): " << std::bitset<8>(bitfieldValue) << std::endl;

	// Read each bit by masking with the appropriate bit position
	bool bStopADSWhenSprintPressed = bitfieldValue & (1 << 0);
	bool bStopADSWhenPronePressed = bitfieldValue & (1 << 1);
	bool bStopADSWhenUnpronePressed = bitfieldValue & (1 << 2);
	bool bStopSprintWhenADSPressed = bitfieldValue & (1 << 3);
	bool bStopUsingPrimaryItemWhenPronePressed = bitfieldValue & (1 << 4);
	bool bStopUsingAltItemWhenPronePressed = bitfieldValue & (1 << 5);
	bool bStopUsingPrimaryItemWhenUnpronePressed = bitfieldValue & (1 << 6);
	bool bStopUsingAltItemWhenUnpronePressed = bitfieldValue & (1 << 7);

	// Print the values of each bitfield
	std::cout << "bStopADSWhenSprintPressed: " << (bStopADSWhenSprintPressed ? "True" : "False") << std::endl;
	std::cout << "bStopADSWhenPronePressed: " << (bStopADSWhenPronePressed ? "True" : "False") << std::endl;
	std::cout << "bStopADSWhenUnpronePressed: " << (bStopADSWhenUnpronePressed ? "True" : "False") << std::endl;
	std::cout << "bStopSprintWhenADSPressed: " << (bStopSprintWhenADSPressed ? "True" : "False") << std::endl;
	std::cout << "bStopUsingPrimaryItemWhenPronePressed: " << (bStopUsingPrimaryItemWhenPronePressed ? "True" : "False") << std::endl;
	std::cout << "bStopUsingAltItemWhenPronePressed: " << (bStopUsingAltItemWhenPronePressed ? "True" : "False") << std::endl;
	std::cout << "bStopUsingPrimaryItemWhenUnpronePressed: " << (bStopUsingPrimaryItemWhenUnpronePressed ? "True" : "False") << std::endl;
	std::cout << "bStopUsingAltItemWhenUnpronePressed: " << (bStopUsingAltItemWhenUnpronePressed ? "True" : "False") << std::endl;
}


void esp() {
	SPOOF_FUNC
		cache::Actors = d.readv<TArray>(cache::PersistentLevel + 0x98);
	cache::Actors2 = d.readv<uint64_t>(cache::PersistentLevel + 0x98);

	if (drawAimbotFovCircle)
	{
		ImVec2 screenResolution = GetScreenResolution();
		ImVec2 screenCenter = ImVec2(screenResolution.x / 2.0f, screenResolution.y / 2.0f);
		ImDrawList* drawList = ImGui::GetForegroundDrawList();
		DrawFOVCircle(drawList, screenCenter, aimbotFov, IM_COL32(255, 255, 255, 70));  // Red color with full opacity
	}

	for (int i = 0; i < cache::Actors.Size(); i++) {
		auto CurrentActor = cache::Actors[i];
		auto CameraCache = d.readv<FCameraCacheEntry>(cache::player_camera_manager + 0x1AF0);
		auto trust = d.readv<FName>(CurrentActor + 0x18);
		std::string brain = FNameToString(trust);

		bool shouldRender = !filterEnabled || filter.Matches(brain);

		if (shouldRender) {
			Vector3 Screen;
			auto player_RootComponent = d.read<uint64_t>(cache::PlayerController + 0x138);
			Vector3 player_WorldPos = d.readv<Vector3>(player_RootComponent + 0x11c);
			auto actorPawn = d.read<uintptr_t>(cache::Actors2 + i * 0x8);
			auto actorState = d.read<uint64_t>(actorPawn + 0x0248);
			auto actorid = d.readv<int32_t>(actorState + 0x400);


			auto actor_health = d.readv<float>(actorPawn + 0x1df8);
			//printf("actor_health %.3f" , actor_health);


			auto actorRootComponent = d.read<uint64_t>(CurrentActor + 0x138);
			DWORD actorWorldPoxds = d.readv<DWORD>(actorRootComponent + 0x11c);

			Vector3 actorWorldPos = d.readv<Vector3>(actorRootComponent + 0x11c);
			//	auto isDead = d.readv<bool>(actorState + 0x1df4); // bIsDying : 1

			if (cache::playerPawn == actorPawn) {
				continue;

			}
			if (ignoreteam && actorid == cache::localteamid) {
				continue;
			}




			if (actor_health <= 0.0f) {
				continue;
			}
			auto actorMesh = d.read<uint64_t>(actorPawn + 0x288);
			if (!actorMesh) {
				continue;
			}
			auto bone_pos = GetBoneWithRotation(actorMesh, 0);
			if (WorldToScreenX(actorWorldPos, CameraCache.POV, Screen)) {

				ImU32 color = IM_COL32(255, 255, 255, 255); // Green color
				auto BottomBox = ProjectWorldToScreen(bone_pos);
				auto TopBox = ProjectWorldToScreen(Vector3(GetBoneWithRotation(actorMesh, bones2::head).x, GetBoneWithRotation(actorMesh, bones2::head).y, GetBoneWithRotation(actorMesh, bones2::head).z + 15));
				auto CornerHeight = abs(TopBox.y - BottomBox.y);
				auto CornerWidth = CornerHeight * 0.65;
				if (displayCorneredBox)
				{
					DrawCorneredBox(TopBox.x - (CornerWidth / 2), TopBox.y, CornerWidth, CornerHeight, ImColor(255, 255, 255), 1.5);
				}
				if (displayBones)
				{
					DrawPlayerBones(actorMesh, GetBoneWithRotation(actorMesh, bones2::head), ImColor(255, 255, 255), CameraCache);
				}
				if (displayHealth)
				{
					DrawHealth(TopBox.x - (CornerWidth / 2), TopBox.y, CornerWidth, CornerHeight, actor_health, color);
				}
				if (displayHeadDot)
				{
					ImDrawList* drawList = ImGui::GetForegroundDrawList();
					ImVec2 headPos = ImVec2(TopBox.x, TopBox.y);
					drawList->AddCircleFilled(headPos, 3.0f, IM_COL32(255, 255, 255, 255));
				}
				if (displayDistance)
				{
					ImVec2 TopBox2D = ImVec2(TopBox.x, TopBox.y);
					ImVec2 BottomBox2D = ImVec2(BottomBox.x, BottomBox.y);

					RenderDistanceText(actorWorldPos, CameraCache.POV.Location, displayDistance, TopBox2D, BottomBox2D, CornerWidth);
				}
				if (enableSpeedHack)
				{
					d.write<float>(cache::playerPawn + OFFSET_CustomTimeDilation, 5.f);
				}
				else
				{
					d.write<float>(cache::playerPawn + OFFSET_CustomTimeDilation, 1.f);
				}
				if (displayBox)
				{
					DrawBox(TopBox.x - (CornerWidth / 2), TopBox.y, CornerWidth, CornerHeight, color, 1.5f);
				}
				if (displaySnaplines)
				{
					ImDrawList* drawList = ImGui::GetBackgroundDrawList();
					float screenWidth = GetSystemMetrics(SM_CXSCREEN);
					float screenHeight = GetSystemMetrics(SM_CYSCREEN);
					ImVec2 screenBottomCenter = ImVec2(screenWidth / 2, screenHeight + 30);
					drawList->AddLine(screenBottomCenter, ImVec2(BottomBox.x, BottomBox.y), IM_COL32(255, 255, 255, 255), 1.0f);
				}

			}
		}
	}
}




//	printf("uworld %p\n", gworld);
	//printf("OwningGameInstance %p\n", owninggameinstance);
	//printf("PersistentLevel %p\n", PersistentLevel);
	//printf("localplayers %p\n", LocalPlayers.GetAddress());
	//printf("localplayer size % i\n", LocalPlayers.Size());
	//printf("actors size %p\n", Actors);
	//printf("actors size % i\n", Actors.Size());
	//printf("localplayer %p\n", localplayer);
	//printf("playercontroller %p\n", PlayerController);
	//printf("camera cache %.3f\n", CameraCache.POV.FOV);
struct Snowflake {
	ImVec2 position;  // Position of the snowflake (x, y)
	float speed;      // Falling speed of the snowflake
	float size;       // Size of the snowflake
};

// Snowflake properties
std::vector<Snowflake> snowflakes;
bool initialized = false;  // To check if snowflakes have been initialized
const int maxSnowflakes = 100;  // Maximum number of snowflakes



void render_menu()
{
	// Toggle menu visibility with the Insert key
	if (GetAsyncKeyState(VK_INSERT) & 1) {
		menuopen = !menuopen;

		if (menuopen) {
			SetWindowLong(my_wnd, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TOOLWINDOW);

			// Initialize snowflakes if the menu is opened and they haven't been initialized
			if (!initialized) {
				srand(static_cast<unsigned int>(time(0)));  // Seed the random number generator
				snowflakes.clear();  // Clear any existing snowflakes

				// Create random snowflakes
				for (int i = 0; i < maxSnowflakes; ++i) {
					Snowflake flake;
					flake.position = ImVec2(rand() % 1920, rand() % 1080);  // Random start position within a 1080p screen
					flake.speed = 1.0f + static_cast<float>(rand() % 100) / 100.0f;  // Random speed between 1.0 and 2.0
					flake.size = 3.0f + static_cast<float>(rand() % 100) / 50.0f;     // Random size between 3.0 and 5.0
					snowflakes.push_back(flake);
				}
				initialized = true;  // Mark snowflakes as initialized
			}
		}
		else {
			SetWindowLong(my_wnd, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW);
		}
	}

	// Render the menu if it's open
	if (menuopen)
	{
		// Render a semi-transparent overlay for dimming the background
		ImDrawList* drawList = ImGui::GetBackgroundDrawList();
		drawList->AddRectFilled(ImVec2(0, 0), ImVec2(1920, 1080), IM_COL32(0, 0, 0, 150));  // Black overlay with 150 alpha

		// Set menu window size and attributes
		ImGui::SetNextWindowSize(ImVec2(620, 500));  // Adjusted height for new sections

		// Create the main menu window with "Squad" as its title
		ImGui::Begin(_("Squad"), nullptr, ImGuiWindowFlags_NoResize);

		ImGui::Spacing();

		// General Settings Section
		if (ImGui::CollapsingHeader(_("General Settings"), ImGuiTreeNodeFlags_CollapsingHeader))
		{
			ImGui::Text(_("General SETTINGS:"));
			ImGui::Checkbox(_("Enable Filter"), &filterEnabled);
			ImGui::Checkbox(_("Filter Team"), &ignoreteam);
		}

		// Visual Settings Section
		if (ImGui::CollapsingHeader(_("Visual Settings"), ImGuiTreeNodeFlags_CollapsingHeader))
		{
			ImGui::Text(_("Visual SETTINGS:"));
			ImGui::Checkbox(_("Show Cornered Box"), &displayCorneredBox);
			if (displayCorneredBox)
			{
				ImGui::Checkbox(_("Box Rainbow Mode"), &boxRainbowMode);
			}
			ImGui::Checkbox(_("Show Bones"), &displayBones);
			ImGui::Checkbox(_("Show Health"), &displayHealth);
			ImGui::Checkbox(_("Show Distance"), &displayDistance);
			ImGui::Checkbox(_("Show Snaplines"), &displaySnaplines);
			ImGui::Checkbox(_("Show Box"), &displayBox);
			ImGui::Checkbox(_("Show Head Dot"), &displayHeadDot);
		}

		// Aimbot Settings Section
		if (ImGui::CollapsingHeader(_("Aimbot Settings"), ImGuiTreeNodeFlags_CollapsingHeader))
		{
			ImGui::Text(_("Aimbot SETTINGS:"));
			ImGui::Checkbox(_("Enable Aimbot"), &enableAimbot);
			ImGui::Checkbox("Draw Aimbot FOV Circle", &drawAimbotFovCircle);
			ImGui::InputFloat("Enable Aimbot FOV", &aimbotFov, 10.0f, 10.0f, "%.1f");
			ImGui::InputFloat("Enable Aimbot Smothness", &aimbotSmoothness, 1.0f, 10.0f, " % .1f");
			ImGui::Text("Target Bone:");
			if (ImGui::RadioButton("Head", selectedBoneIndex == 0)) selectedBoneIndex = 0;
			if (ImGui::RadioButton("Neck", selectedBoneIndex == 1)) selectedBoneIndex = 1;
			if (ImGui::RadioButton("Chest", selectedBoneIndex == 2)) selectedBoneIndex = 2;
			if (ImGui::RadioButton("Pelvis", selectedBoneIndex == 3)) selectedBoneIndex = 3;
			if (ImGui::RadioButton("Left Hand", selectedBoneIndex == 4)) selectedBoneIndex = 4;
			if (ImGui::RadioButton("Right Hand", selectedBoneIndex == 5)) selectedBoneIndex = 5;
			if (ImGui::RadioButton("Left Foot", selectedBoneIndex == 6)) selectedBoneIndex = 6;
			if (ImGui::RadioButton("Right Foot", selectedBoneIndex == 7)) selectedBoneIndex = 7;

			// Update the target bone based on the selected index
			switch (selectedBoneIndex)
			{
			case 0: targetBone = bones::Bip01_Head; break;         // Head bone
			case 1: targetBone = bones::Bip01_Neck; break;         // Neck bone
			case 2: targetBone = bones::Bip01_Spine2; break;       // Chest bone (using Spine2)
			case 3: targetBone = bones::Bip01_Pelvis; break;       // Pelvis bone
			case 4: targetBone = bones::Bip01_L_Hand; break;       // Left hand
			case 5: targetBone = bones::Bip01_R_Hand; break;       // Right hand
			case 6: targetBone = bones::Bip01_L_Foot; break;       // Left foot
			case 7: targetBone = bones::Bip01_R_Foot; break;       // Right foot
			default: targetBone = bones::Bip01_Head; break;        // Default to head if none selected
			}
		}

		if (ImGui::CollapsingHeader(_("Miscellaneous Settings"), ImGuiTreeNodeFlags_CollapsingHeader))
		{
			ImGui::Text(_("Miscellaneous SETTINGS:"));
			ImGui::Checkbox(_("Enable SpeedHack"), &enableSpeedHack);
			ImGui::Checkbox(_("Enable Infinite Stamina"), &enableInfiniteStamina);

		}

		ImGui::Spacing();
		ImGui::Separator();

		// Get the window size and set cursor position for bottom-left alignment
		ImVec2 windowSize = ImGui::GetWindowSize();
		ImVec2 cursorPos = ImGui::GetCursorPos();

		// Calculate new cursor position at bottom-left
		float textHeight = ImGui::GetTextLineHeightWithSpacing(); // Get height of one line of text
		ImGui::SetCursorPos(ImVec2(10, windowSize.y - textHeight * 1.2));  // Adjust this value to fine-tune position

		ImGui::Text("Press 'Insert' to toggle menu visibility.");

		ImGui::End();  // End the main menu window
	}

	// Render the snowfall effect when the menu is open
	if (menuopen)
	{
		// Get the draw list to draw directly on the screen
		ImDrawList* drawList = ImGui::GetBackgroundDrawList();
		for (auto& flake : snowflakes) {
			// Draw each snowflake as a circle with its size and position
			drawList->AddCircleFilled(flake.position, flake.size, IM_COL32(255, 255, 255, 200));  // Semi-transparent white

			// Move the snowflake down by its speed
			flake.position.y += flake.speed;

			// Reset the snowflake to the top if it goes out of bounds
			if (flake.position.y > 1080.0f) {  // Assuming a 1080p screen resolution
				flake.position.y = 0.0f;
				flake.position.x = rand() % 1920;  // Random x position within screen width
				flake.speed = 1.0f + static_cast<float>(rand() % 100) / 100.0f;  // Random speed between 1.0 and 2.0
				flake.size = 3.0f + static_cast<float>(rand() % 100) / 50.0f;     // Random size between 3.0 and 5.0
			}
		}
	}
}









WPARAM render_loop() {
	SPOOF_FUNC
		ZeroMemory(&messager, sizeof(MSG));
	std::cout << _("Starting render loop...") << std::endl;

	while (messager.message != WM_QUIT) {
		// Process messages
		if (PeekMessage(&messager, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&messager);
			DispatchMessage(&messager);
		}

		// Handle resizing and updating ImGui
		RECT rc;
		POINT xy = { 0, 0 }; // Set xy to origin for simplicity
		ZeroMemory(&rc, sizeof(RECT));
		rc.left = xy.x;
		rc.top = xy.y;

		ImGuiIO& io = ImGui::GetIO();
		io.DeltaTime = 1.0f / 60.0f; // Set frame time

		// Update mouse position
		POINT p;
		GetCursorPos(&p);
		ScreenToClient(my_wnd, &p); // Convert to client coordinates
		io.MousePos.x = p.x;
		io.MousePos.y = p.y;

		// Handle mouse input
		io.MouseDown[0] = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0; // Left mouse button
		if (io.MouseDown[0]) {
			io.MouseClicked[0] = true;
			io.MouseClickedPos[0] = { io.MousePos.x, io.MousePos.y };
		}

		// Start new ImGui frame
		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		esp(); // Call ESP function to draw player info and bones
		//	cache_basic();
		render_menu(); // Call menu rendering function

		// End ImGui frame
		ImGui::EndFrame();

		// Clear the device
		p_device->SetRenderState(D3DRS_ZENABLE, false);
		p_device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
		p_device->SetRenderState(D3DRS_SCISSORTESTENABLE, false);
		p_device->Clear(0, 0, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);

		// Render the scene
		if (p_device->BeginScene() >= 0) {
			ImGui::Render();
			ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
			p_device->EndScene();
		}

		// Present the rendered frame
		HRESULT result = p_device->Present(0, 0, 0, 0);
		if (result == D3DERR_DEVICELOST && p_device->TestCooperativeLevel() == D3DERR_DEVICENOTRESET) {
			std::cerr << _("Device lost, resetting...") << std::endl;
			ImGui_ImplDX9_InvalidateDeviceObjects();
			if (FAILED(p_device->Reset(&p_params))) {
				std::cerr << _("Failed to reset Direct3D device after loss.") << std::endl;
				continue; // Skip rendering this frame
			}
			ImGui_ImplDX9_CreateDeviceObjects();
		}
	}

	// Cleanup ImGui and Direct3D
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	if (p_device != nullptr) {
		p_device->EndScene();
		p_device->Release();
	}

	if (p_object != nullptr) p_object->Release();
	DestroyWindow(my_wnd);

	std::cout << _("Render loop ended.") << std::endl;
	return messager.wParam;
}
void create_overlay()
{
	SPOOF_FUNC
		// Debug logging
		std::cout << _("Creating overlay window...") << std::endl;

	WNDCLASSEXA wcex = {
		sizeof(WNDCLASSEXA),
		0,
		DefWindowProcA,
		0,
		0,
		0,
		LoadIcon(0, IDI_APPLICATION),
		LoadCursor(0, IDC_ARROW),
		0,
		0,
		_("Medal"),
		LoadIcon(0, IDI_APPLICATION)
	};

	// Register the window class
	ATOM rce = RegisterClassExA(&wcex);
	if (!rce)
	{
		std::cerr << _("Error registering window class: ") << GetLastError() << std::endl;
		return; // Exit if registration fails
	}
	std::cout << _("Window class registered successfully.") << std::endl;

	RECT rect;
	GetWindowRect(GetDesktopWindow(), &rect);

	// Create the overlay window
	my_wnd = gui::create_window_in_band(0, rce, _(L"Medal"), WS_POPUP,
		rect.left, rect.top, rect.right, rect.bottom,
		0, 0, wcex.hInstance, 0, gui::ZBID_UIACCESS);

	if (!my_wnd)
	{
		std::cerr << _("Error creating overlay window: ") << GetLastError() << std::endl;
		return; // Exit if window creation fails
	}
	std::cout << _("Overlay window created successfully.") << std::endl;

	// Set window styles

	SetLayeredWindowAttributes(my_wnd, RGB(0, 0, 0), 255, LWA_ALPHA);

	MARGINS margin = { -1 };
	HRESULT hr = DwmExtendFrameIntoClientArea(my_wnd, &margin);
	if (FAILED(hr))
	{
		std::cerr << _("Error extending frame into client area: ") << std::hex << hr << std::endl;
		return; // Exit if DWM extension fails
	}
	std::cout << _("Frame extended into client area successfully.") << std::endl;

	ShowWindow(my_wnd, SW_SHOW);
	UpdateWindow(my_wnd);
	std::cout << _("Overlay window displayed.") << std::endl;
}
int main() {
	SPOOF_FUNC
		// hide_imports;
		driver_start();

	if (!gui::init())
	{
		printf(_("The gui was not initialized!"));
		Sleep(3000);
		exit(0);
	}


	CreateThread(nullptr, NULL, reinterpret_cast<LPTHREAD_START_ROUTINE>(cache_basic), nullptr, NULL, nullptr);
	CreateThread(nullptr, NULL, reinterpret_cast<LPTHREAD_START_ROUTINE>(aimbot_ya_heard), nullptr, NULL, nullptr);
	create_overlay();
	directx_init();
	render_loop();
}
