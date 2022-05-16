#include <napi.h>

#include <windows.h>
#include <thread>

#include <iostream>
#include <fstream>

static WNDPROC origProcessHandler;
static Napi::ThreadSafeFunction tsfn;

static bool flag = false;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_QUERYENDSESSION:
		tsfn.NonBlockingCall([=](Napi::Env env, Napi::Function jsCallback)
							 { jsCallback.Call(env.Global(), {Napi::Number::New(env, message)}); });
		MSG msg;
		BOOL bRet;
		while ((bRet = GetMessage(&msg, NULL, 0, 0)) != 0)
		{
			if (flag)
			{
				break;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		flag = false;
		tsfn.Release();
		tsfn = NULL;
		ShutdownBlockReasonDestroy(hWnd);
		return 1;
		break;
	default:
		return CallWindowProc(origProcessHandler, hWnd, message, wParam, lParam);
		break;
	}

	return 0;
}

void Start(const Napi::CallbackInfo &info)
{
	Napi::Env env = info.Env();

	int64_t hWndMain = info[0].As<Napi::Number>().Int64Value();
	if (!hWndMain)
	{
		Napi::TypeError::New(env, "Invalid window handle").ThrowAsJavaScriptException();
		return;
	}

	if (!info[1].IsFunction())
	{
		Napi::TypeError::New(env, "Second params should be Callback method").ThrowAsJavaScriptException();
		return;
	}

	ShutdownBlockReasonCreate((HWND)hWndMain, L"Timer stops. Please wait...");
	SetProcessShutdownParameters(0x290, 0);

	tsfn = Napi::ThreadSafeFunction::New(
		env,
		info[1].As<Napi::Function>(),
		"Window Events",
		0,
		1);
	origProcessHandler = (WNDPROC)SetWindowLongPtr((HWND)hWndMain, GWLP_WNDPROC, (LONG_PTR)WndProc);
}

void Stop(const Napi::CallbackInfo &info)
{
	flag = true;
}
Napi::Object Init(Napi::Env env, Napi::Object exports)
{
	exports.Set(Napi::String::New(env, "start"), Napi::Function::New(env, Start));
	exports.Set(Napi::String::New(env, "stop"), Napi::Function::New(env, Stop));
	return exports;
}

NODE_API_MODULE(winEvent, Init)
