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
// LibSL::Errors
// ------------------------------------------------------
//
// Error management
//
// ------------------------------------------------------
// Sylvain Lefebvre - 2006-25-02
// ------------------------------------------------------

#pragma once

#include <cstdarg>
#include <cstdio>
#include <iostream>
#include <LibSL/System/System.h>

namespace LibSL {

  namespace Errors {

    class Fatal
    {
    public:

      enum { e_MessageBufferSize = 4096 };

    protected:

      char m_Message[e_MessageBufferSize];

      Fatal() {m_Message[0]='\0';}

    public:

      Fatal(const char *msg,...) LIBSL_PRINTF_ATTR
      {
        va_list args;
        va_start(args, msg);
        vsprintf_s(m_Message,e_MessageBufferSize,msg,args);
        va_end(args);
      }

      const char *message() const {return (m_Message);}
    };

  } // namespace LibSL::Errors
} // namespace LibSL

// ------------------------------------------------------

#define LIBSL_NEW_EXCEPTION_TYPE(T)        \
class T : public LibSL::Errors::Fatal      \
{                                          \
public:                                    \
  T(const char *msg,...) LIBSL_PRINTF_ATTR {\
    va_list args; va_start(args, msg);     \
    vsprintf_s(m_Message,LibSL::Errors::Fatal::e_MessageBufferSize,msg,args);   \
    va_end(args);			                     \
  }					                               \
};

#define LIBSL_FATAL_ERROR(M)             throw LibSL::Errors::Fatal(M ", file %s, line %d",__FILE__,__LINE__);
#define LIBSL_FATAL_ERROR_WITH_ARGS(M,A) throw LibSL::Errors::Fatal(M ", file %s, line %d",A,__FILE__,__LINE__);

#define LIBSL_BOUNCE_ERROR(E)            throw LibSL::Errors::Fatal("%s\n  [caller] file %s, line %d",E.message(),__FILE__,__LINE__);
#define LIBSL_CATCH_AND_BOUNCE(B)        { try { B; } catch (LibSL::Errors::Fatal& err) { LIBSL_BOUNCE_ERROR(err); } }
#define LIBSL_CATCH_ANY(B)               { try { B; } catch (LibSL::Errors::Fatal& err) { std::cerr << err.message() << std::endl; } }

// ------------------------------------------------------
