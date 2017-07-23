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
// LibSL::Math
// ------------------------------------------------------
//
// Defines standard math functions
//
// ------------------------------------------------------
// Sylvain Lefebvre - 2006-03-23
// ------------------------------------------------------

#pragma once

#include <LibSL/LibSL.common.h>

#include <cmath>
#include <cstdlib>

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace LibSL {
  namespace Math {

    template <typename T_Type1,typename T_Type2> inline  T_Type1 min(T_Type1 a,T_Type2 b)
    {
      return ( a < T_Type1(b) ? a : T_Type1(b));
    }
    template <typename T_Type1,typename T_Type2> inline T_Type1 max(T_Type1 a,T_Type2 b)
    {
      return ( a > T_Type1(b) ? a : T_Type1(b));
    }
    template <typename T_Type> inline T_Type floor(T_Type v)
    {
      return ::floor(v);
    }
    template <typename T_Type> inline T_Type log(T_Type v)
    {
      return ::log(v);
    }
    template <typename T_Type> inline T_Type exp(T_Type v)
    {
      return ::exp(v);
    }
    template <typename T_Type> inline T_Type sqrt(T_Type v)
    {
      return ::sqrt(v);
    }
    template <typename T_Type> inline T_Type abs(T_Type v)
    {
      return ( v > 0 ? v : -v);
    }
    template <typename T_Type> inline T_Type frac(T_Type v)
    {
      return (v-int(v));
    }
    template <typename T_Type> inline T_Type sign(T_Type v)
    {
      return ( (v > 0) ? T_Type(1)
                       : ( (v < 0) ? T_Type(-1)
                                   : T_Type( 0) ) );
    }
    template <typename T_Type> inline T_Type clamp(T_Type v,T_Type clampMin,T_Type clampMax)
    {
      return (LibSL::Math::min(clampMax,LibSL::Math::max(clampMin,v)));
    }
    template <typename T_Type> inline T_Type log2(T_Type v)  {return ::log(v)/::log(T_Type(2));}
    template <typename T_Type> inline T_Type log10(T_Type v) {return ::log(v)/::log(T_Type(10));}
    template <typename T_Type> inline T_Type cot(T_Type v)   {return T_Type(1)/::tan(v);}

    template <typename T_Type> inline T_Type round(T_Type v) {return T_Type(::floor(T_Type(v + 0.5)));}

    template <typename T_Type> inline T_Type mod2pi(T_Type v) {
      T_Type vmod    = T_Type(v - int(v/(M_PI*2.0))*M_PI*2.0);
      if (vmod < 0)  { vmod += M_PI*2.0; }
      return (vmod);
    }

    LIBSL_DLL float rnd();   // random number in [0,1]
    LIBSL_DLL float srnd();  // random number in [-1,1]

  } // namespace LibSL::Math
} // namespace LibSL
