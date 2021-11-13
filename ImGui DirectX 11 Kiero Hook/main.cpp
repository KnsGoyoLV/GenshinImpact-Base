#include "includes.h"
#include "main.h"
/* Game: Genshin Impact.exe
Version : 2.0v
Date: 2021 - 11 - 13
Author : DrError#4949

This script does blah blah blah
*/
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

Present oPresent;
HWND window = NULL;
WNDPROC oWndProc;
ID3D11Device* pDevice = NULL;
ID3D11DeviceContext* pContext = NULL;
ID3D11RenderTargetView* mainRenderTargetView;

void InitImGui()
{
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX11_Init(pDevice, pContext);
}

LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	if (true && ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
		return true;

	return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}

bool init = false;
HRESULT __stdcall hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
	if (!init)
	{
		if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)& pDevice)))
		{
			pDevice->GetImmediateContext(&pContext);
			DXGI_SWAP_CHAIN_DESC sd;
			pSwapChain->GetDesc(&sd);
			window = sd.OutputWindow;
			ID3D11Texture2D* pBackBuffer;
			pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)& pBackBuffer);
			pDevice->CreateRenderTargetView(pBackBuffer, NULL, &mainRenderTargetView);
			pBackBuffer->Release();
			oWndProc = (WNDPROC)SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)WndProc);
			InitImGui();
			init = true;
		}

		else
			return oPresent(pSwapChain, SyncInterval, Flags);
	}

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Genshin Impact Simple base (Ready to be Pasted XD )");//Menu starts
	{
		
			ImGui::Checkbox("NOCD E", &settings::NoCDE);//Check box for our Function (simple shit)
			ImGui::Checkbox("Inf Stamina", &settings::InfStamina);//Check box for our Function (simple shit)
			ImGui::Checkbox("Enemy Rank", &settings::EnemyRank);//Check box for our Function (simple shit)
			

	}






	ImGui::End();
	ImGui::Render();//Menu Ends

	pContext->OMSetRenderTargets(1, &mainRenderTargetView, NULL);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	return oPresent(pSwapChain, SyncInterval, Flags);
}

DWORD WINAPI HackThread(HMODULE hModule)
{
	//Create Console
	AllocConsole();
	FILE* f;
	freopen_s(&f, "CONOUT$", "w", stdout);
	std::cout << "Initialized...";
	//calling it with NULL also gives you the address of the .exe module
	uintptr_t moduleBase = (uintptr_t)GetModuleHandle("UserAssembly");
	uintptr_t UnityBase = (uintptr_t)GetModuleHandle("UnityPlayer");



	while (true)
	{
		

		if (GetAsyncKeyState(VK_NUMPAD1) & 1) //Numpad 1 On&Off
			settings::NoCDE = !settings::NoCDE;
		if (GetAsyncKeyState(VK_NUMPAD2) & 1) //Numpad 1 On&Off
			settings::InfStamina = !settings::InfStamina;


		//Memory Patching 
		//Original Bytes |  0F 11 43 20   | movups [rbx+20],xmm0 | Moving 11 To 10 =  movups xmm0,[rbx+20] | 
		if (settings::NoCDE)
		{
			mem::Patch((BYTE*)(moduleBase + 0x29CCC46), (BYTE*)"\x10", 1); // Patching one single Byte | 0F 10* 43 20 | 
		}
		else
		{
			//50F 11* 43 20  | THE ORIGINAL STACK |
			mem::Patch((BYTE*)(moduleBase + 0x29CCC46), (BYTE*)"\x11", 1);
		}


		//Memory Patching 
		//Original Bytes |  0F 11 04 C1   | movups [rbx+rax+8],xmm0 | Moving 11 To 10 =  movups xmm0,[rbx+rax+8] | 
		if (settings::InfStamina)
		{
			mem::Patch((BYTE*)(moduleBase + 0x74A45DD), (BYTE*)"\x10", 1); // Patching one single Byte | 0F 10* 04 C1  | 
		}
		else
		{
			//0F 11 04 C1   | THE ORIGINAL STACK |
			mem::Patch((BYTE*)(moduleBase + 0x74A45DD), (BYTE*)"\x11", 1);
		}
		//Memory Patching 
		//Original Bytes |  87 E1 00 00   | xchg exc,esp | Moving 87 To 84 =  test cl,ah | 
		if (settings::EnemyRank)
		{
			mem::Patch((BYTE*)(moduleBase + 0x3556638), (BYTE*)"\x84", 1); // Patching one single Byte |  87* E1 00 00 | 
		}
		else
		{
			
			mem::Patch((BYTE*)(moduleBase + 0x3556638), (BYTE*)"\x87", 1);
		}


		Sleep(5);
	}

	fclose(f);
	FreeConsole();
	FreeLibraryAndExitThread(hModule, 0);
	return 0;
}




DWORD WINAPI MainThread(LPVOID lpReserved)
{
	bool init_hook = false;
	do
	{
		if (kiero::init(kiero::RenderType::D3D11) == kiero::Status::Success)
		{
			kiero::bind(8, (void**)& oPresent, hkPresent);
			init_hook = true;
		}
	} while (!init_hook);
	return TRUE;
}

BOOL WINAPI DllMain(HMODULE hMod, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hMod);
		CreateThread(nullptr, 0, MainThread, hMod, 0, nullptr); // Rendering menu starting etc. 
		CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)HackThread, hMod, 0, nullptr)); //Hacking stuff goes here
		break;
	case DLL_PROCESS_DETACH:
		kiero::shutdown();
		break;
	}
	return TRUE;
}