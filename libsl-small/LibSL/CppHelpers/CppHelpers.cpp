/* --------------------------------------------------------------------
Author: Sylvain Lefebvre    sylvain.lefebvre@sophia.inria.fr

Simple Library for Graphics (LibSL)

This software is a computer program whose purpose is to offer a set of
tools to simplify programming real-time computer graphics applications
under OpenGL and DirectX.

This software is governed by the CeCILL-C license under French law and
abiding by the rules of distribution of free software.  You can  use,
modify and/ or redistribute the software under the terms of the CeCILL-C
license as circulated by CEA, CNRS and INRIA at the following URL
"http://www.cecill.info".

As a counterpart to the access to the source code and  rights to copy,
modify and redistribute granted by the license, users are provided only
with a limited warranty  and the software's author,  the holder of the
economic rights,  and the successive licensors  have only  limited
liability.

In this respect, the user's attention is drawn to the risks associated
with loading,  using,  modifying and/or developing or reproducing the
software by the user in light of its specific status of free software,
that may mean  that it is complicated to manipulate,  and  that  also
therefore means  that it is reserved for developers  and  experienced
professionals having in-depth computer knowledge. Users are therefore
encouraged to load and test the software's suitability as regards their
requirements in conditions enabling the security of their systems and/or
data to be ensured and,  more generally, to use and operate it in the
same conditions as regards security.

The fact that you are presently reading this means that you have had
knowledge of the CeCILL-C license and that you accept its terms.
-------------------------------------------------------------------- */
// ------------------------------------------------------
// Sylvain Lefebvre - 2006-21-02
// ------------------------------------------------------
#include "LibSL.precompiled.h"
// ------------------------------------------------------

#include "CppHelpers.h"

#include <cstdarg>
#include <cstdio>
#include <stack>

#include <LibSL/System/System.h>

// ------------------------------------------------------

#define NAMESPACE LibSL::CppHelpers

using namespace LibSL::System;
using namespace LibSL::System::Time;

// ------------------------------------------------------

#define TEMP_STR_SIZE 4096

#ifdef _DEBUG
void sl_assert_func(bool b, const char *expr, const char *file, int line)
{
  if (!b) { 
    fprintf(stderr, "**ASSERT FAILED** %s, file %s, line %d\n", expr, file, line);
    exit(-1);
  }
}
#endif

// ------------------------------------------------------

static char    s_StrSingle[TEMP_STR_SIZE];
static wchar_t s_StrUnicode[TEMP_STR_SIZE];

// ------------------------------------------------------

const char *NAMESPACE::sprint(const char *msg,...)
{
  va_list args;
  va_start(args, msg);

  vsprintf_s(s_StrSingle,TEMP_STR_SIZE,msg,args);

  va_end(args);
  return (s_StrSingle);
}

// ------------------------------------------------------

#ifdef WIN32 // TODO: the following must be moved in System/Platform

#include <windows.h>

const char    *NAMESPACE::toChar(const wchar_t *wstr)
{
  WideCharToMultiByte( CP_ACP, 0, wstr, -1, s_StrSingle, TEMP_STR_SIZE, NULL, NULL );
  return s_StrSingle;
}

// ------------------------------------------------------

const wchar_t *NAMESPACE::toUnicode(const char *str)
{
  MultiByteToWideChar( CP_ACP, 0, str, -1, s_StrUnicode, TEMP_STR_SIZE);
  return s_StrUnicode;
}

#endif

// ------------------------------------------------------

NAMESPACE::ibinstream& NAMESPACE::operator >> (NAMESPACE::ibinstream& s,std::string& str)
{
  size_t l=0;
  s >> l;
  str=std::string(l,' ');
  ForIndex(c,l) {
    s >> str[c];
  }
  return s;
}

// ------------------------------------------------------

NAMESPACE::obinstream& NAMESPACE::operator << (NAMESPACE::obinstream& s,const std::string& str)
{
  size_t l=str.length();
  s << l;
  ForIndex(c,l) {
    s << str[c];
  }
  return s;
}

// ------------------------------------------------------
// ------------------------------------------------------
// ------------------------------------------------------

#define PROGRESS_BAR_LENGTH 20

static uint   s_ProgressBarMax         = 1;
static uint   s_ProgressBarValue       = 0;
static uint   s_ProgressBarCursorLast  = 0;
static t_time s_ProgressBarTimeLast    = 0;

// ------------------------------------------------------

void NAMESPACE::Console::progressBarInit(uint max)
{
  s_ProgressBarMax         = max;
  s_ProgressBarValue       = 0;
  s_ProgressBarCursorLast  = 0;
  s_ProgressBarTimeLast    = System::Time::milliseconds();
  std::cerr << '[';
}

// ------------------------------------------------------

void NAMESPACE::Console::progressBarUpdate(uint cur)
{
  uint cursor    = cur * PROGRESS_BAR_LENGTH / s_ProgressBarMax;
  while (cursor >= s_ProgressBarCursorLast) {
    std::cerr << '.';
    s_ProgressBarCursorLast ++;
  }
}

// ------------------------------------------------------

void NAMESPACE::Console::progressBarUpdate()
{
  s_ProgressBarValue ++;
  progressBarUpdate(s_ProgressBarValue);
}

// ------------------------------------------------------

void NAMESPACE::Console::progressBarEnd()
{
  std::cerr << "]\n";
}

// ------------------------------------------------------
// ------------------------------------------------------
// ------------------------------------------------------

static uint   s_ProgressTextMax         = 1;
static uint   s_ProgressTextValue       = 0;
static float  s_ProgressTextPercentLast = 0;
static t_time s_ProgressTextTimeStart   = 0;
static t_time s_ProgressTextTimeLast    = 0;

#ifdef WIN32
CONSOLE_SCREEN_BUFFER_INFO s_ProgressTextCursorNfo;
#endif

// ------------------------------------------------------

void NAMESPACE::Console::progressTextInit(uint max)
{
  s_ProgressTextMax         = max;
  s_ProgressTextValue       = 0;
  s_ProgressTextPercentLast = 0;
  s_ProgressTextTimeLast    = System::Time::milliseconds();
  s_ProgressTextTimeStart   = System::Time::milliseconds();
#ifndef WIN32 // use VT100
  std::cout << char(27) << "[s" << std::flush;;
#else
  GetConsoleScreenBufferInfo(GetStdHandle(STD_ERROR_HANDLE),&s_ProgressTextCursorNfo);
#endif
}

// ------------------------------------------------------

void NAMESPACE::Console::progressTextUpdate(uint cur)
{
  float percent  = cur * 100.0f / float(s_ProgressTextMax);
  float numsteps = percent - s_ProgressTextPercentLast;
  t_time   now   = System::Time::milliseconds();
  if (numsteps > 0.1f && now - s_ProgressTextTimeLast > 100) {
    s_ProgressTextPercentLast = percent;
    float  msPerStep          = float(now - s_ProgressTextTimeLast) / float(numsteps);
    float  left               = (100 - percent) * msPerStep;
    s_ProgressTextTimeLast    = now;
    uint tm = uint(left);
    uint h  = ( tm/(1000*60*60));
    uint m  = ((tm/(1000*60)) % 60);
    uint s  = ((tm/1000)      % 60);
    uint ms = tm % 1000;
#ifndef WIN32 // use VT100
    std::cout << char(27) << "[u";
    std::cout << char(27) << "[K";
    std::cout << char(27) << "[s";
    std::cout << sprint("%.2f%%\t (%c[32m%02d:%02d:%02d%c[37m left) ",percent,27,h,m,s,27) << std::flush;
#else
    SetConsoleCursorPosition(GetStdHandle(STD_ERROR_HANDLE),s_ProgressTextCursorNfo.dwCursorPosition);
    std::cerr << sprint("%.2f%%\t (",percent);
    SetConsoleTextAttribute(GetStdHandle(STD_ERROR_HANDLE), FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    std::cerr << sprint("%02d:%02d:%02d",h,m,s);
    SetConsoleTextAttribute(GetStdHandle(STD_ERROR_HANDLE), s_ProgressTextCursorNfo.wAttributes);
    std::cerr << sprint(" left)");
#endif
  }
}

// ------------------------------------------------------

void NAMESPACE::Console::progressTextUpdate()
{
  s_ProgressTextValue ++;
  progressTextUpdate(s_ProgressTextValue);
}

// ------------------------------------------------------

void NAMESPACE::Console::progressTextEnd()
{
  uint tm = uint(System::Time::milliseconds() - s_ProgressTextTimeStart);
  uint h  = ( tm/(1000*60*60));
  uint m  = ((tm/(1000*60)) % 60);
  uint s  = ((tm/1000)      % 60);
  uint ms = tm % 1000;
#ifndef WIN32 // use VT100
  std::cout << char(27) << "[u";
  std::cout << char(27) << "[K";
  std::cout << sprint("done in\t %c[36m%02d:%02d:%02d%c[37m.\n",27,h,m,s,27) << std::flush;;
#else
  SetConsoleCursorPosition(GetStdHandle(STD_ERROR_HANDLE),s_ProgressTextCursorNfo.dwCursorPosition);
  std::cerr << sprint("done in ");
  SetConsoleTextAttribute(GetStdHandle(STD_ERROR_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
  std::cerr << sprint("%02d:%02d:%02d",h,m,s);
  SetConsoleTextAttribute(GetStdHandle(STD_ERROR_HANDLE), s_ProgressTextCursorNfo.wAttributes);
  //std::cerr << sprint(".                     \n");
  std::cerr << sprint(".                     ");
#endif
}

// ------------------------------------------------------
// ------------------------------------------------------
// ------------------------------------------------------

#ifdef WIN32

std::stack<CONSOLE_SCREEN_BUFFER_INFO> s_CursorNfoStack;

void NAMESPACE::Console::pushCursor()
{
  CONSOLE_SCREEN_BUFFER_INFO nfo;
  GetConsoleScreenBufferInfo(GetStdHandle(STD_ERROR_HANDLE),&nfo);
  s_CursorNfoStack.push(nfo);
}

void NAMESPACE::Console::popCursor(bool clearLines)
{
  sl_assert(!s_CursorNfoStack.empty());
  if (clearLines) {
    int nLines = 0;
    CONSOLE_SCREEN_BUFFER_INFO current;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_ERROR_HANDLE),&current);
    nLines = 1 + current.dwCursorPosition.Y - s_CursorNfoStack.top().dwCursorPosition.Y;
    SetConsoleCursorPosition(GetStdHandle(STD_ERROR_HANDLE),s_CursorNfoStack.top().dwCursorPosition);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_ERROR_HANDLE), &csbi );
    int s  = s_CursorNfoStack.top().dwCursorPosition.X;
    ForIndex(l,nLines) {
      ForRange(i,s,csbi.dwSize.X-2) { std::cerr << " "; }
      s    = 0;
      std::cerr << std::endl;
    }
  }
  SetConsoleCursorPosition(GetStdHandle(STD_ERROR_HANDLE),s_CursorNfoStack.top().dwCursorPosition);
  s_CursorNfoStack.pop();
}

void NAMESPACE::Console::cursorGotoPreviousLineStart()
{
  CONSOLE_SCREEN_BUFFER_INFO current;
  GetConsoleScreenBufferInfo(GetStdHandle(STD_ERROR_HANDLE),&current);
  current.dwCursorPosition.Y --;
  current.dwCursorPosition.X = 0;
  SetConsoleCursorPosition  (GetStdHandle(STD_ERROR_HANDLE),current.dwCursorPosition);
}

#else

void NAMESPACE::Console::pushCursor()
{
}

void NAMESPACE::Console::popCursor(bool clearLines)
{
}

void NAMESPACE::Console::cursorGotoPreviousLineStart()
{
}

#endif

// ------------------------------------------------------
// ------------------------------------------------------
// ------------------------------------------------------

static uint   s_Processing       = 0;
static t_time s_ProcessingTmLast = 0;

#ifdef WIN32
CONSOLE_SCREEN_BUFFER_INFO s_ProcessingCursorNfo;
#endif

// ------------------------------------------------------

void NAMESPACE::Console::processingInit()
{
  s_Processing       = 0;
  s_ProcessingTmLast = LibSL::System::Time::milliseconds();
#ifdef WIN32
  GetConsoleScreenBufferInfo(GetStdHandle(STD_ERROR_HANDLE),&s_ProcessingCursorNfo);
#endif
  std::cerr << '.';
}

// ------------------------------------------------------

void NAMESPACE::Console::processingUpdate()
{
  static char seq[]={'/','-','\\','|'};
  t_time tm      = LibSL::System::Time::milliseconds();
  t_time elapsed = tm - s_ProcessingTmLast;
  if (elapsed > 100) {
    s_Processing ++;
    s_ProcessingTmLast = tm;
#ifndef WIN32
    std::cerr << char(27) << "[1D";
#else
    SetConsoleCursorPosition(GetStdHandle(STD_ERROR_HANDLE),s_ProcessingCursorNfo.dwCursorPosition);
#endif
    std::cerr << seq[s_Processing&3];
  }
}

// ------------------------------------------------------

void NAMESPACE::Console::processingEnd()
{
#ifndef WIN32
  std::cerr << char(27) << "[1D";
#else
  SetConsoleCursorPosition(GetStdHandle(STD_ERROR_HANDLE),s_ProcessingCursorNfo.dwCursorPosition);
#endif
}

// ------------------------------------------------------

void NAMESPACE::Console::clear()
{
#ifndef WIN32
  std::cerr << char(27) << "[2J";
#else
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  COORD coordScreen = { 0, 0 };
  DWORD cCharsWritten;
  DWORD dwConSize;

  HANDLE hConsole = GetStdHandle(STD_ERROR_HANDLE);
  GetConsoleScreenBufferInfo( hConsole, &csbi );
  dwConSize = csbi.dwSize.X * csbi.dwSize.Y;
  FillConsoleOutputCharacter( hConsole, (TCHAR) ' ', dwConSize, coordScreen, &cCharsWritten );
  GetConsoleScreenBufferInfo( hConsole, &csbi );
  FillConsoleOutputAttribute( hConsole, csbi.wAttributes, dwConSize, coordScreen, &cCharsWritten );
  SetConsoleCursorPosition( hConsole, coordScreen );
#endif
}

// ------------------------------------------------------
// ------------------------------------------------------
// ------------------------------------------------------

std::ostream &NAMESPACE::printByteSize::operator()(std::ostream &s) const
{
  if (m_Sz >= (1<<20)) {
    s << double(m_Sz)/(1024.0*1024.0) << " MB";
  } else if (m_Sz >= (1<<10)) {
    s << double(m_Sz)/(1024.0) << " KB";
  } else {
    s << m_Sz << " bytes";
  }
  return (s);
}

// ------------------------------------------------------

std::ostream& NAMESPACE::operator << (std::ostream& s, const printByteSize& pbs)
{
  return pbs(s);
}

// ------------------------------------------------------

std::ostream& NAMESPACE::Console::normal(std::ostream& s)
{
#ifdef WIN32
  SetConsoleTextAttribute(GetStdHandle(STD_ERROR_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
#else
  s << char(27) << "[0m";
#endif
  return (s);
}

std::ostream& NAMESPACE::Console::bold(std::ostream& s)
{
#ifdef WIN32
  SetConsoleTextAttribute(GetStdHandle(STD_ERROR_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
#else
  s << char(27) << "[1m";
#endif
  return (s);
}

std::ostream& NAMESPACE::Console::black(std::ostream& s)
{
#ifdef WIN32
  SetConsoleTextAttribute(GetStdHandle(STD_ERROR_HANDLE), FOREGROUND_INTENSITY);
#else
  s << char(27) << "[30m";
#endif
  return (s);
}

std::ostream& NAMESPACE::Console::red(std::ostream& s)
{
#ifdef WIN32
  SetConsoleTextAttribute(GetStdHandle(STD_ERROR_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
#else
  s << char(27) << "[31m";
#endif
  return (s);
}

std::ostream& NAMESPACE::Console::green(std::ostream& s)
{
#ifdef WIN32
  SetConsoleTextAttribute(GetStdHandle(STD_ERROR_HANDLE), FOREGROUND_GREEN | FOREGROUND_INTENSITY);
#else
  s << char(27) << "[32m";
#endif
  return (s);
}

std::ostream& NAMESPACE::Console::yellow(std::ostream& s)
{
#ifdef WIN32
  SetConsoleTextAttribute(GetStdHandle(STD_ERROR_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
#else
  s << char(27) << "[33m";
#endif
  return (s);
}

std::ostream& NAMESPACE::Console::blue(std::ostream& s)
{
#ifdef WIN32
  SetConsoleTextAttribute(GetStdHandle(STD_ERROR_HANDLE), FOREGROUND_BLUE | FOREGROUND_INTENSITY);
#else
  s << char(27) << "[34m";
#endif
  return (s);
}

std::ostream& NAMESPACE::Console::magenta(std::ostream& s)
{
#ifdef WIN32
  SetConsoleTextAttribute(GetStdHandle(STD_ERROR_HANDLE), FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
#else
  s << char(27) << "[35m";
#endif
  return (s);
}

std::ostream& NAMESPACE::Console::cyan(std::ostream& s)
{
#ifdef WIN32
  SetConsoleTextAttribute(GetStdHandle(STD_ERROR_HANDLE), FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
#else
  s << char(27) << "[36m";
#endif
  return (s);
}

std::ostream& NAMESPACE::Console::white(std::ostream& s)
{
#ifdef WIN32
  SetConsoleTextAttribute(GetStdHandle(STD_ERROR_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
#else
  s << char(27) << "[37m";
#endif
  return (s);
}

std::ostream& NAMESPACE::Console::gray(std::ostream& s)
{
#ifdef WIN32
  SetConsoleTextAttribute(GetStdHandle(STD_ERROR_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
#else
  s << char(27) << "[37m";
#endif
  return (s);
}

// ------------------------------------------------------

std::string& NAMESPACE::replaceAll(std::string& str, const std::string& from, const std::string& to)
{
  size_t lookHere = 0;
  size_t foundHere;
  while ((foundHere = str.find(from, lookHere)) != std::string::npos) {
    str.replace(foundHere, from.size(), to);
    lookHere = foundHere + to.size();
  }
  return str;
}

// ------------------------------------------------------
