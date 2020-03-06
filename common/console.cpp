#include "console.h"
#ifdef _WIN32
#include <windows.h>
#include "Convert.h"
#endif

#if defined(LINUX) || defined(MACOS)
#include <stdio.h>
#include <termios.h>

bool guiStdoutIsConsole()
{
	static int in = -1;
	if (in < 0)
	{
		struct termios t;
		in = (tcgetattr(1, &t) == 0);
	}
	return in;
}
#endif

void guiSetConsoleTitle(const char* utf8title) //platform-dependent implementations
{
#ifdef _WIN32
	SetConsoleTitleW(Convert::utf8ToUtf16(utf8title).c_str());
#else
#if defined(LINUX) || defined(MACOS)
	if (guiStdoutIsConsole())
	{
		printf("\033]0;%s\007", utf8title);
		fflush(stdout);
	}
#endif
#endif
}

ConsoleColorMode console_color_mode = ConsoleColorNone;

void guiSetConsoleUTF8()
{
#ifdef _WIN32
	SetConsoleOutputCP(CP_UTF8);
	//    SetConsoleCP(CP_UTF8); //set input code page.
	//  ^^ Z ta linia po wpisaniu gdziekolwiek polskiego znaczka, fgets() zwraca NULL co powoduje ze konsola sie zamyka.
	//    	Uzycie fgetws() nie wiadomo czemu od razu wychodzi (nie wczytujac nic z konsoli) zwracajac jakies losowe znaczki - zmiana w stdiofile.h ktora testowalem: char *Vgets(char *s, int size) { wchar_t tab[1000];  wchar_t *x = fgetws(tab, 1000, file); if (x == NULL) return NULL; strcpy(s, Convert::wstrTOstr(tab).c_str()); return s; }
	//  ^^ Bez tej linijki polskie znaki sa kodowane 1-bajtowo jako jakas strona kodowa (pojawiaja sie w konsoli ale frams takie znaki ignoruje jesli sa poczatkiem polecenia?)
	//  ogolnie wszyscy narzekaja na konsole win... https://alfps.wordpress.com/2011/11/22/unicode-part-1-windows-console-io-approaches/
#endif
}
