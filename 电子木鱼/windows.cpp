namespace
{
#include <SFML/Window/WindowHandle.hpp>
#if defined(SFML_SYSTEM_WINDOWS)

#include <Windows.h>
	extern int Register_Touch(sf::WindowHandle hwnd)
	{
		int ret = RegisterTouchWindow((HWND)hwnd, 0);
		if (ret == 0) return 1;
		else return 0;
	}

	extern int Event_Touch()
	{
		//GetTouchInputInfo();
	}

#else
	extern int Register_Touch(sf::WindowHandle hwnd)
	{
		//·Çwindows£¬·µ»Ø-1
		return -1;
	}
#endif
}
