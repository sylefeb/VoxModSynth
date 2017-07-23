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
// LibSL::CppHelpers
// ------------------------------------------------------
//
// C++ language syntax helpers
//
// ------------------------------------------------------
// Sylvain Lefebvre - 2006-03-23
// ------------------------------------------------------

#pragma once

#include <LibSL/LibSL.common.h>
#include <LibSL/Errors/Errors.h>

#include <fstream>
#include <cstdio>
#include <cstdlib>

// ------------------------------------------------------

#define ForIndex(I,N)          for (int I=0;I<int(N);I++)
#define ForRange(I,A,B)        for (int I=int(A);I<=int(B);I++)
#define ForRangeReverse(I,A,B) for (int I=int(A);I>=int(B);I--)

// ------------------------------------------------------

#ifdef _DEBUG
void sl_assert_func(bool b, const char *expr, const char *file, int line);
#else
inline void sl_assert_func(bool b, const char *expr, const char *file, int line)
{
  if (!b) {
    fprintf(stderr, "**ASSERT FAILED** %s, file %s, line %d\n", expr, file, line);
    exit(-1);
  }
}
#endif

// #define sl_assert(B) if (!(B)) LIBSL_FATAL_ERROR("**ASSERT FAILED** " #B)
// #define sl_assert(B) if (!(B)) { fprintf(stderr,"**ASSERT FAILED** " #B ", file %s, line %d\n",  __FILE__,__LINE__); exit (-1); }
#define sl_assert(B) sl_assert_func(B,#B,__FILE__,__LINE__);

#ifdef LIBSL_RELEASE
#  define sl_dbg_assert(B)
#else
#  define sl_dbg_assert(B) if (!(B)) LIBSL_FATAL_ERROR("**ASSERT FAILED** " #B)
#endif

#define LIBSL_DISABLE_COPY(Class) \
      Class(const Class&)         \
      {throw LibSL::Errors::Fatal(#Class " - copy disallowed");}

#ifndef LIBSL_SAFE_DELETE
#  define LIBSL_SAFE_DELETE(p)  {if (p != NULL) {delete (p);   p = NULL;} }
#endif

#ifndef LIBSL_SAFE_DELETE_ARRAY
#  define LIBSL_SAFE_DELETE_ARRAY(p)  {if (p != NULL) {delete[](p);   p = NULL;} }
#endif

#define LIBSL_DBGPRT(V) std::cerr << #V << ": " << V << std::endl;

#define LIBSL_BEGIN  { try {

#define LIBSL_END    } catch (LibSL::Errors::Fatal& __LibSL_f) { LIBSL_BOUNCE_ERROR(__LibSL_f); } }

#define LIBSL_TRACE  (std::cerr << '[' << __FILE__ << "] line " << __LINE__ << std::endl);
#define LIBSL_TRACE_COUT  (std::cout << '[' << __FILE__ << "] line " << __LINE__ << std::endl);

// ------------------------------------------------------

// JD: Removed because of cylcic dependencies between includes, which causes an
// error when including, for instance, <LibSL/Math/Matrix4x4.h>, alone in a
// single .cpp file (Tuple.h includes CppHelpers.h, which includes Vertex.h,
// but Vertex.h needs Tuple.h as well).
//#include <LibSL/Math/Vertex.h>

// ------------------------------------------------------

namespace LibSL {
  namespace CppHelpers {

    //! text stream helpers
    LIBSL_DLL const char    *sprint(const char *msg, ...);

    //! unicode helpers
    LIBSL_DLL const char    *toChar(const wchar_t *);
    LIBSL_DLL const wchar_t *toUnicode(const char *);

    namespace Console {
      //! progress bar
      LIBSL_DLL void progressBarInit(uint max);
      LIBSL_DLL void progressBarUpdate(uint cur);
      LIBSL_DLL void progressBarUpdate();
      LIBSL_DLL void progressBarEnd();
      //! progress text
      LIBSL_DLL void progressTextInit(uint max);
      LIBSL_DLL void progressTextUpdate(uint cur);
      LIBSL_DLL void progressTextUpdate();
      LIBSL_DLL void progressTextEnd();
      //! processing marker
      LIBSL_DLL void processingInit();
      LIBSL_DLL void processingUpdate();
      LIBSL_DLL void processingEnd();
      //! cursor control
      LIBSL_DLL void cursorGotoPreviousLineStart();
      LIBSL_DLL void pushCursor();
      LIBSL_DLL void popCursor(bool clearLines = false);
      //! clear screen
      LIBSL_DLL void clear();
      //! print colored text
      std::ostream& normal(std::ostream& s);
      std::ostream& bold(std::ostream& s);
      std::ostream& black(std::ostream& s);
      std::ostream& red(std::ostream& s);
      std::ostream& green(std::ostream& s);
      std::ostream& yellow(std::ostream& s);
      std::ostream& blue(std::ostream& s);
      std::ostream& magenta(std::ostream& s);
      std::ostream& cyan(std::ostream& s);
      std::ostream& white(std::ostream& s);
      std::ostream& gray(std::ostream& s);
    } // namespace LibSL::CppHelpers::Console

    //! print arg as bits
    template <typename T_Type>
    void printBits(T_Type v)
    {
      ForIndex(b, sizeof(T_Type) * 8) {
        if (v & (1 << (sizeof(T_Type) * 8 - 1 - b))) {
          std::cerr << '1';
        } else {
          std::cerr << '0';
        }
      }
    }

    //! check if number is power of two (only integers!!)
    template <typename T_Type>
    bool isPowerOf2(T_Type x)
    {
      return ((x) && !(x & (x - 1)));
    }

    //! count bits (only integers!!)
    template <typename T_Type>
    bool numBits(T_Type x)
    {
      int b = 0;
      while (x > 0) { b++; x = x >> 1; }
      return b;
    }

    //! binary streams
    class ibinstream : public std::ifstream
    {
    public:
      ibinstream(const char *n, std::ios::openmode m) : std::ifstream(n, m) {}
    };
    class obinstream : public std::ofstream
    {
    public:
      obinstream(const char *n, std::ios::openmode m) : std::ofstream(n, m) {}
    };

    //! generic binary stream operators
    template <typename T>
    ibinstream& operator >> (ibinstream& s, T& t)
    {
      s.read(reinterpret_cast<char *>(&t), sizeof(T)); return s;
    }
    template <typename T>
    obinstream& operator << (obinstream& s, const T& t)
    {
      s.write(reinterpret_cast<const char *>(&t), sizeof(T)); return s;
    }

    //! string operators
    ibinstream& operator >> (ibinstream& s, std::string& str);
    obinstream& operator << (obinstream& s, const std::string& str);

    std::string& replaceAll(std::string& str, const std::string& from, const std::string& to);

    //! print size as MB/KB/bytes
    class printByteSize
    {
    private:
      uint m_Sz;
    public:
      printByteSize(uint sz) : m_Sz(sz) {}
      std::ostream &operator()(std::ostream &) const;
    };
    std::ostream& operator << (std::ostream& s, const printByteSize& pbs);

  } //namespace LibSL::CppHelpers
} //namespace LibSL

// ------------------------------------------------------
