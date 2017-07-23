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
#include "LibSL.precompiled.h"
// ------------------------------------------------------

#include <LibSL/Errors/Errors.h>
#include <LibSL/CppHelpers/CppHelpers.h>

#include <LibSL/System/System.h>

using namespace LibSL::Errors;

// ------------------------------------------------------

#define NAMESPACE LibSL::System

// ------------------------------------------------------

#ifndef WIN32
# include <sys/time.h>
# include <sys/stat.h>
# include <sys/types.h>
#include <fcntl.h>
#include <dirent.h>
#include <unistd.h>
#else
# include <windows.h>
# include <psapi.h>
# include <shlwapi.h>
#endif

// ------------------------------------------------------

void NAMESPACE::File::__fopen_s(FILE **pf,const char *path, const char *mode)
{
#ifdef WIN32
  fopen_s(pf,path,mode);
#else
  FILE *f = fopen(path,mode);
  *pf     = f;
#endif
}

// ------------------------------------------------------

bool NAMESPACE::File::exists(const char *fname)
{
#ifdef WIN32
  return (PathFileExistsA(fname) != 0);
#else
  FILE *f = NULL;
  f = fopen(fname,"rb");
  if (f != NULL) {
    fclose(f);
    return (true);
  } else {
    return (false);
  }
#endif
}

// ------------------------------------------------------

long NAMESPACE::File::size(const char *path)
{
  FILE *f = NULL;
  fopen_s(&f,path,"rb");
  sl_assert(f != NULL);
  fseek(f,0,SEEK_END);
  long fsize = ftell(f);
  fclose(f);
  return fsize;
}

// ------------------------------------------------------

void NAMESPACE::File::listFiles(const char *path,std::vector<std::string>& _files)
{
  _files.clear();
#ifdef WIN32
  WIN32_FIND_DATAA current;
  std::string wpath = std::string(path) + "*";
  HANDLE find = FindFirstFileA(wpath.c_str(),&current);
  if (find == INVALID_HANDLE_VALUE) {
    DWORD errCode = GetLastError();
    if (errCode == ERROR_FILE_NOT_FOUND) {
      return;
    } else {
      throw LibSL::Errors::Fatal("File::listFiles - cannot list '%s' (Win32 error code: %d)",path,errCode);
    }
  }
  do {
    if (!(current.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
      _files.push_back(std::string( current.cFileName ));
    }
  } while (FindNextFileA(find,&current));
#else
  DIR *dir = opendir(path);
  if (dir == NULL) {
    throw Fatal("File::listFiles - cannot list directory '%s'",path);
  }
  struct dirent *dp = NULL;
  while ((dp = readdir(dir)) != NULL) {
      struct stat s;
      std::string full = path + std::string(dp->d_name);
      stat(full.c_str(),&s);
      if ( ! S_ISDIR(s.st_mode) && S_ISREG(s.st_mode) ) {
         _files.push_back( std::string(dp->d_name) );
      }
  }
  closedir(dir);
#endif
}

// ------------------------------------------------------

void NAMESPACE::File::listDirectories(const char *path,std::vector<std::string>& _dirs)
{
  _dirs.clear();
#ifdef WIN32
  std::string wpath = std::string(path) + "*";
  WIN32_FIND_DATAA current;
  HANDLE find = FindFirstFileA(wpath.c_str(),&current);
  if (find == INVALID_HANDLE_VALUE) {
    throw Fatal("File::listDirectories - cannot list directory (Win32 error code: %d)",GetLastError());
  }
  do {
    if (current.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
      _dirs.push_back(std::string( current.cFileName ));
    }
  } while (FindNextFileA(find,&current));
#else
  DIR *dir = opendir(path);
  if (dir == NULL) {
    throw Fatal("File::listDirectories - cannot list directory '%s'",path);
  }
  struct dirent *dp = NULL;
  while ((dp = readdir(dir)) != NULL) {
      struct stat s;
      std::string full = path + std::string(dp->d_name);
      stat(full.c_str(),&s);
      if ( S_ISDIR(s.st_mode) ) {
         _dirs.push_back( std::string(dp->d_name) );
      }
  }
  closedir(dir);
#endif

}

// ------------------------------------------------------

void    NAMESPACE::File::createDirectory(const char *path)
{
  sl_assert(path != NULL);
#ifdef WIN32
  CreateDirectoryA(path,NULL);
#else
  mkdir(path,S_IRUSR|S_IWUSR|S_IXUSR);
#endif
}

// ------------------------------------------------------

const char *NAMESPACE::File::adaptPath      (const char *path)
{
  const int N = 2048;
  static char buffer[N];
  sl_assert(path != NULL);
  int c = 0;
#ifdef WIN32
  while (path[c] != '\0') {
    buffer[c] = path[c];
    if (buffer[c] == '/') { buffer[c] = '\\'; }
    c++;
    sl_assert(c < N);
  };
  buffer[c] = '\0';
#else
  while (path[c] != '\0') {
    buffer[c] = path[c];
    if (buffer[c] == '\\') { buffer[c] = '/'; }
    c++;
    sl_assert(c < N);
  };
  buffer[c] = '\0';
#endif
  return buffer;
}

// -----------------------------------------------

NAMESPACE::File::t_FileTime NAMESPACE::File::timestamp(const char *path)
{
#ifdef WIN32
  HANDLE f = CreateFileA(path,
    GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
    NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  if (f == NULL) {
    t_FileTime z;
    memset(&z, 0, sizeof(t_FileTime));
    return z;
  }
  FILETIME creation, lastaccess, lastwrite;
  GetFileTime(f, &creation, &lastaccess, &lastwrite);
  CloseHandle(f);
  t_FileTime ftm;
  ftm.dwHighDateTime = lastwrite.dwHighDateTime;
  ftm.dwLowDateTime = lastwrite.dwLowDateTime;
  return ftm;
#else
  struct stat st;
  t_FileTime ftm = {0,0};
  int fh = open(path, O_RDONLY);
  if (fh < 0) return ftm;
  fstat(fh, &st);
  close(fh);
  ftm.dwHighDateTime = 0;
  ftm.dwLowDateTime = st.st_mtime;
  return ftm;
#endif
}

// ------------------------------------------------------

bool operator<(const NAMESPACE::File::t_FileTime& a, const NAMESPACE::File::t_FileTime& b)
{
  return a.dwHighDateTime < b.dwHighDateTime || (a.dwHighDateTime == b.dwHighDateTime && a.dwLowDateTime < b.dwLowDateTime);
}

// ------------------------------------------------------
// ------------------------------------------------------
// ------------------------------------------------------

void NAMESPACE::Process::sleep(uint msec)
{
#ifdef WIN32
  Sleep(msec);
#else
  usleep(msec * 1000);
#endif
}

// ------------------------------------------------------
// ------------------------------------------------------
// ------------------------------------------------------

const char *NAMESPACE::Application::executablePath()
{
  static char appPath[1024]=".";
#ifdef WIN32
  GetModuleFileNameExA(GetCurrentProcess(),NULL,appPath,1024);
  // remove file name
  char *pos = strrchr(appPath,'\\');
  if (pos) {
    *(pos+1) = '\0';
  }
#else
#ifndef __APPLE__
  int length;

  /* /proc/self is a symbolic link to the process-ID subdir
      * of /proc, e.g. /proc/4323 when the pid of the process
      * of this program is 4323.
      *
      * Inside /proc/<pid> there is a symbolic link to the
      * executable that is running as this <pid>.  This symbolic
      * link is called "exe".
      *
      * So if we read the path where the symlink /proc/self/exe
      * points to we have the full path of the executable.
      */


  length = readlink("/proc/self/exe", appPath, sizeof(appPath));

  /* Catch some errors: */
  if (length < 0) {
      fprintf(stderr, "Error resolving symlink /proc/self/exe.\n");
      exit(EXIT_FAILURE);
  }
  if (length >= 1024) {
      fprintf(stderr, "Path too long. Truncated.\n");
      exit(EXIT_FAILURE);
  }

  /*
    The last part is the name of the executable, so walk backwards
    and place a '\0' after the last slash
    */
  for (int i = length - 1; i > 0; --i)
  {
      if (appPath [i] == '/')
      {
          appPath [i+1] = '\0';
          break;
      }
  }

  /* I don't know why, but the string this readlink() function
      * returns is appended with a '@'.
      */
  appPath[length] = '\0';       /* Strip '@' off the end. */
#endif
#endif
  return (appPath);
}

// ------------------------------------------------------
// ------------------------------------------------------
// ------------------------------------------------------

NAMESPACE::Time::t_time NAMESPACE::Time::milliseconds()
{
#ifdef WIN32

  static bool   init = false;
  static t_time freq;
  if (!init) { // TODO / FIXME move into a global timer init ?
    init = true;
    LARGE_INTEGER lfreq;
    sl_assert(QueryPerformanceFrequency(&lfreq) != 0);
    freq = t_time(lfreq.QuadPart);
  }
  LARGE_INTEGER tps;
  QueryPerformanceCounter(&tps);
  return t_time(t_time(tps.QuadPart)*1000/freq);

#else

  struct timeval        now;
  uint                  ticks;
  static struct timeval start;

  static bool           init=false;
  if (!init) { // TODO / FIXME move into a global timer init ?
    gettimeofday(&start, NULL);
    init=true;
  }

  gettimeofday(&now, NULL);
  uint ms=uint((now.tv_sec-start.tv_sec)*1000+(now.tv_usec-start.tv_usec)/1000);
  return (ms);

#endif
}

// ------------------------------------------------------
// ------------------------------------------------------
// ------------------------------------------------------

double NAMESPACE::Time::microseconds()
{
#ifdef WIN32

  static bool          init = false;
  static LARGE_INTEGER freq;
  if (!init) { // TODO / FIXME move into a global timer init ?
    init   = true;
    BOOL b = QueryPerformanceFrequency(&freq);
    sl_assert(b != 0);
  }
  LARGE_INTEGER tps;
  QueryPerformanceCounter(&tps);

  return tps.QuadPart * (1000000.0 / freq.QuadPart);

#else

  sl_assert(false);
  return 0;
  /*
  struct timeval        now;
  uint                  ticks;
  static struct timeval start;

  static bool           init=false;
  if (!init) { // TODO / FIXME move into a global timer init ?
    gettimeofday(&start, NULL);
    init=true;
  }

  gettimeofday(&now, NULL);
  uint ms=uint((now.tv_sec-start.tv_sec)*1000+(now.tv_usec-start.tv_usec)/1000);
  return (ms);
  */
#endif
}

// ------------------------------------------------------
// ------------------------------------------------------
// ------------------------------------------------------

#ifdef WIN32

// icompatibility with x64
/*
__declspec(naked)
unsigned __int64 __cdecl rdtsc(void)
{
   __asm
   {
      rdtsc
      ret       ; valeur de retour dans EDX:EAX
   }
}
*/

#include <intrin.h>
#pragma intrinsic(__rdtsc)

unsigned __int64 rdtsc(void)
{
	return __rdtsc();
}



#endif

long long NAMESPACE::Time::ticks()
{
#ifdef WIN32

  return rdtsc();

#else

  sl_assert(false);
  return 0;

#endif
}

// ------------------------------------------------------
// ------------------------------------------------------
// ------------------------------------------------------

NAMESPACE::Time::Timer::Timer(const char *name, bool autostart)
{
  m_Name    = name;
  m_TmAccum = 0;
  m_Active = false;
  if (autostart) {
    start();
  }
}

// ------------------------------------------------------

NAMESPACE::Time::Timer::~Timer()
{
  stop();
  display();
}

// ------------------------------------------------------

void NAMESPACE::Time::Timer::start()
{
  m_TmStart = milliseconds();
  m_Active  = true;
}

// ------------------------------------------------------

void NAMESPACE::Time::Timer::display()
{
  t_time h = (m_TmAccum / (1000 * 60 * 60));
  t_time m = ((m_TmAccum / (1000 * 60)) % 60);
  t_time s = ((m_TmAccum / 1000) % 60);
  t_time ms = m_TmAccum % 1000;
  if (m_Name) {
    std::cerr << LibSL::CppHelpers::sprint("%s: %2d hours %2d min %2ds %4d ms\n", m_Name, (int)h, (int)m, (int)s, (int)ms);
  }
}

// ------------------------------------------------------

NAMESPACE::Time::t_time NAMESPACE::Time::Timer::stop()
{
  if (m_Active) {
    t_time tm = elapsed();
    m_TmAccum += tm;
    m_Active = false;
    return (tm);
  } else {
    return (0);
  }
}

// ------------------------------------------------------

NAMESPACE::Time::t_time NAMESPACE::Time::Timer::elapsed()
{
  return (milliseconds() - m_TmStart);
}

// ------------------------------------------------------
// ------------------------------------------------------
// ------------------------------------------------------

NAMESPACE::Time::Timings::Timings(const char *name)
{
  m_Name    = name;
  m_TmStart = milliseconds();
  m_TmLast  = m_TmStart;
}

// ------------------------------------------------------


NAMESPACE::Time::t_time NAMESPACE::Time::Timings::measure(const char *name)
{
  t_time now = milliseconds();
  t_time tm  = now - m_TmStart;
  t_time h   = ( tm/(1000*60*60));
  t_time m   = ((tm/(1000*60)) % 60);
  t_time s   = ((tm/1000)      % 60);
  t_time ms  = tm % 1000;
  std::cerr << LibSL::CppHelpers::sprint("%s:(%s) %2d hours %2d min %2ds %4d ms (+ %4d ms)\n",m_Name,name,(int)h,(int)m,(int)s,(int)ms,uint(now - m_TmLast));
  m_TmLast = now;
  return tm;
}

// ------------------------------------------------------
// ------------------------------------------------------
// ------------------------------------------------------

NAMESPACE::Time::Every::Every(uint msec)
{
  m_Delay   = msec;
  m_TmStart = milliseconds();
}

// ------------------------------------------------------

NAMESPACE::Time::t_time NAMESPACE::Time::Every::delay()
{
  return m_Delay;
}

// ------------------------------------------------------

bool NAMESPACE::Time::Every::expired()
{
  t_time now = milliseconds();
  if (now - m_TmStart > m_Delay) {
    m_TmStart = now;
    return (true);
  } else {
    return (false);
  }
}

// ------------------------------------------------------

bool NAMESPACE::Time::Every::expired(t_time& _elapsed)
{
  t_time now = milliseconds();
  _elapsed = now - m_TmStart;
  if (now - m_TmStart > m_Delay) {
    m_TmStart = now;
    return (true);
  } else {
    return (false);
  }
}

// ------------------------------------------------------
// ------------------------------------------------------
// ------------------------------------------------------

NAMESPACE::Time::Elapsed::Elapsed()
{
  m_TmLast = milliseconds();
}

// ------------------------------------------------------

NAMESPACE::Time::t_time NAMESPACE::Time::Elapsed::elapsed()
{
  t_time now = milliseconds();
  t_time e   = (now - m_TmLast);
  if (e > 0) {
    m_TmLast = now;
  }
  return e;
}

// ------------------------------------------------------
// ------------------------------------------------------
// ------------------------------------------------------

NAMESPACE::Time::Timeout::Timeout(uint msec)
{
  m_Delay   = msec;
  m_TmStart = milliseconds();
}

// ------------------------------------------------------

void NAMESPACE::Time::Timeout::restart()
{
  m_TmStart = milliseconds();
}

// ------------------------------------------------------

bool NAMESPACE::Time::Timeout::expired()
{
  t_time now = milliseconds();
  if (now - m_TmStart > m_Delay) {
    return (true);
  } else {
    return (false);
  }
}

// ------------------------------------------------------
// ------------------------------------------------------
// ------------------------------------------------------
