#pragma once
#include "pch.h"
#include "Common/StepTimer.h"
#include "EyeLogger.h"



namespace EyeEngine
{

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

class GameWindow
{
public:
	GameWindow(HINSTANCE hInstance);
	~GameWindow() = default;
protected:

	GameWindow(const GameWindow& rhs) = delete;
	GameWindow& operator=(const GameWindow& rhs) = delete;
	

public:

	static GameWindow*	GetWindow();

	HINSTANCE			AppInst()const;
	HWND				MainWnd()const;

	int Run();

	virtual bool Initialize();
	virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

protected:

	static GameWindow* m_window;

	HINSTANCE m_hAppInst = nullptr; // application instance handle
	HWND      m_hMainWnd = nullptr; // main window handle
	
};

}// namespace EyeEngine
