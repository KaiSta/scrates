#include "uarng.h"
#include <Windows.h>
#include <cryptopp\cryptlib.h>
#include <cryptopp\hex.h>
#include <cryptopp\randpool.h>
#include <string>
#include <windowsx.h>
#include <cryptopp\osrng.h>


UARNG::UARNG()
{
}

static CryptoPP::SecByteBlock seed(32);

std::string seed_to_string()
{
	CryptoPP::HexEncoder encoder;
	std::string lseed;
	encoder.Put(seed, seed.size());
	encoder.MessageEnd();
	auto s = encoder.MaxRetrievable();
	if (s)
	{
		lseed.resize(s);
		encoder.Get((byte*)lseed.data(), lseed.size());
	}
	return lseed;
}

LRESULT CALLBACK wnd_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static HWND ok_button;
	static HWND current_seed;

	
	//encoder.Put(random_num_, random_num_.size());
	//encoder.MessageEnd();

	switch (msg)
	{
	case WM_MOUSEMOVE:
	{
						 auto x = GET_X_LPARAM(lParam);
						 auto y = GET_Y_LPARAM(lParam);
						 seed[x%seed.size()] += y;				 
						 SetWindowText(current_seed, seed_to_string().c_str());
	}
		break;
	case WM_LBUTTONDOWN:
	{
						   for (size_t i = 0; i < seed.size(); ++i)
						   {
							   if (i % 2)
								   seed[i] += 1;
							   else
								   seed[i] -= 1;
						   }
						   SetWindowText(current_seed, seed_to_string().c_str());
	}
		break;
	case WM_RBUTTONDOWN:
	{
						   for (size_t i = 0; i < seed.size(); ++i)
						   {
							   if (i % 2)
								   seed[i] -= 1;
							   else
								   seed[i] += 1;
						   }
						   SetWindowText(current_seed, seed_to_string().c_str());
	}
		break;
	case WM_COMMAND:
	{
					   if (lParam == (LPARAM)ok_button)
					   {
						   if (HIWORD(wParam) == BN_CLICKED)
						   {
							   SendMessage(hwnd, WM_CLOSE, 0, 0);
						   }
					   }
					   return 0;
	}
		break;
	case WM_CREATE:
	{
					  ok_button = CreateWindow("button", "OK",
						  WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
						  (360/2)-60, (200/2)+30,100,25,	  
						  hwnd, nullptr,
						GetModuleHandle(nullptr), nullptr);

					  current_seed = CreateWindowEx(WS_EX_CLIENTEDGE,
						  "edit", seed_to_string().c_str(), WS_CHILD | WS_VISIBLE | ES_READONLY,
						  (360 / 2) - 170, 10, 320, 25,
						  hwnd, nullptr, GetModuleHandle(nullptr), nullptr);
					  return 0;
	}
		break;
	case WM_SIZE:
	{
					//MoveWindow(ok_button, LOWORD(lParam) / 2 - 50,
						//HIWORD(lParam) - 30, 100, 25, TRUE);
					return 0;
	}
		break;
	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

bool UARNG::generate(CryptoPP::SecByteBlock& s)
{
	CryptoPP::AutoSeededRandomPool prng;
	prng.GenerateBlock(seed, seed.size());

	HINSTANCE hinstance = GetModuleHandle(nullptr);
	const char classname[] = "uarng";
	WNDCLASSEX wc;
	HWND hwnd;
	MSG msg;

	wc.cbSize      = sizeof(WNDCLASSEX);
	wc.style       = 0;
	wc.lpfnWndProc = wnd_proc;
	wc.cbClsExtra  = 0;
	wc.cbWndExtra  = 0;
	wc.hInstance   = hinstance;
	wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = classname;
	wc.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);

	if (!RegisterClassEx(&wc))
	{
		MessageBox(nullptr, "window registration failed", "error",
			MB_ICONEXCLAMATION | MB_OK);
		return false;
	}

	hwnd = CreateWindowEx(WS_EX_CLIENTEDGE, classname,
		"User-assisted random number generator",
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
		360, 200, nullptr, nullptr, hinstance, nullptr);
	
	if (hwnd == nullptr)
	{
		MessageBox(nullptr, "window creation failed", "error",
			MB_ICONEXCLAMATION | MB_OK);
		return false;
	}

	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);

	while (GetMessage(&msg, nullptr, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	s = seed;
	return true;
}


UARNG::~UARNG()
{
}
