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
// LibSL::System
// ------------------------------------------------------
//
// Tools for platform independance (Windows/Linux)
//
// ------------------------------------------------------
// Sylvain Lefebvre - 2006-17-03
// ------------------------------------------------------

#pragma once

// ------------------------------------------------------

#include <cstdio>
#include <vector>

#include <LibSL/LibSL.common.h>

// ------------------------------------------------------

#include <LibSL/System/Types.h>
using namespace LibSL::System::Types;

namespace LibSL  {
  namespace System {

    namespace Process {
      LIBSL_DLL void sleep(uint msec);
    } //namespace LibSL::System::Process

    namespace Application {
      LIBSL_DLL const char *executablePath();
    }

    namespace File {

      typedef struct {
        unsigned long  dwLowDateTime;
        unsigned long  dwHighDateTime;
      } t_FileTime;

      LIBSL_DLL void __fopen_s(FILE **pf,const char *path, const char *mode);

      LIBSL_DLL bool        exists         (const char *path);
      LIBSL_DLL long        size           (const char *path);
      LIBSL_DLL void        listFiles      (const char *path,std::vector<std::string>&);
      LIBSL_DLL void        listDirectories(const char *path,std::vector<std::string>&);
      LIBSL_DLL void        createDirectory(const char *path);
      LIBSL_DLL const char *adaptPath      (const char *path);
      LIBSL_DLL t_FileTime  timestamp      (const char *path);


    } //namespace LibSL::System::File

    namespace Time {

      typedef long long      t_time;

      LIBSL_DLL t_time    milliseconds();
      LIBSL_DLL double    microseconds();
      LIBSL_DLL long long ticks();

      class LIBSL_DLL Timer
      {
      protected:
        const char *m_Name;
        t_time      m_TmStart;
        t_time      m_TmAccum;
        bool        m_Active;
        void   display();
      public:
        Timer(const char *name="[Timer] ",bool autostart=true);
        ~Timer();
        void   start();
        t_time stop();
        t_time elapsed();
      };

      class LIBSL_DLL Every
      {
      protected:
        t_time m_TmStart;
        t_time m_Delay;
      public:
        Every(uint msec);
        bool   expired();
        bool   expired(t_time& _elapsed);
        t_time delay();
      };

      class LIBSL_DLL Timeout
      {
      protected:
        t_time m_TmStart;
        t_time m_Delay;
      public:
        Timeout(uint msec);
        void restart();
        bool expired();
      };

      class LIBSL_DLL Elapsed
      {
      protected:
        t_time m_TmLast;
      public:
        Elapsed();
        t_time elapsed();
      };

      class LIBSL_DLL Timings
      {
      protected:
        const char *m_Name;
        t_time      m_TmStart;
        t_time      m_TmLast;
      public:
        Timings(const char *name="[Timings] ");
        ~Timings() {}
        t_time measure(const char *name);
      };

    } //namespace LibSL::System::Time

  } //namespace LibSL::System
} //namespace LibSL

// ------------------------------------------------------

bool operator<(const LibSL::System::File::t_FileTime& a, const LibSL::System::File::t_FileTime& b);

// ------------------------------------------------------

#define LIBSL_BUTTON_DOWN   1
#define LIBSL_BUTTON_UP     2

#define LIBSL_LEFT_BUTTON   1
#define LIBSL_MIDDLE_BUTTON 2
#define LIBSL_RIGHT_BUTTON  4
#define LIBSL_WHEEL_UP      8
#define LIBSL_WHEEL_DOWN   16

#ifdef WIN32
#define SC_F1       59
#define SC_CTRL     29
#define SC_SHIFT    42
#endif

// ------------------------------------------------------

#ifdef WIN32
// ------------------------------------------------------
/// Windows depedent definitions
#define LIBSL_PRINTF_ATTR
// ------------------------------------------------------
#else

// ------------------------------------------------------
/// Linux dependent definitions
#define sprintf_s snprintf
#define vsprintf_s vsnprintf
#define vsprintf_s vsnprintf
#define fopen_s(F,P,M) LibSL::System::File::__fopen_s(F,P,M)
#define LIBSL_PRINTF_ATTR __attribute__ ((format (printf, 2, 3)))
// ------------------------------------------------------

#endif

// ------------------------------------------------------

