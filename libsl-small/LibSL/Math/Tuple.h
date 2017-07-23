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
// LibSL::Math::Tuple
// ------------------------------------------------------
//
// Tuple class
//
// ------------------------------------------------------
// Sylvain Lefebvre - 2006-03-07
// ------------------------------------------------------

#pragma once

#include <LibSL/CppHelpers/CppHelpers.h>
#include <LibSL/Math/Math.h>

#include <LibSL/System/Types.h>

#include <cmath>
#include <iostream>

namespace LibSL {
  namespace Math {

    template<uint Test> class StaticCheck;
    template<> class StaticCheck<1> {};

    template<typename T_Type, int T_N>
    class Tuple
    {
    protected:

      T_Type m_Values[T_N];

    public:

      typedef T_Type t_Element;
      enum           {e_Size = T_N};

      Tuple()  { }

      Tuple(const Tuple<T_Type,T_N>& tup) {
        ForIndex(i,T_N) {
          m_Values[i]=tup[i];
        }
      }

      template <typename T_Type2,int T_N2>
      explicit Tuple(const Tuple<T_Type2,T_N2>& tup) {
        ForIndex(i,( T_N < T_N2 ? T_N : T_N2) ) {
          m_Values[i] = T_Type(tup[i]);
        }
        ForRange(i, (T_N < T_N2 ? T_N : T_N2), T_N-1) {
          m_Values[i] = T_Type(0);
        }
      }

      Tuple(const Tuple<T_Type,T_N-1>& tup,const T_Type& v) {
        ForIndex(i,T_N-1) {
          m_Values[i]=tup[i];
        }
        m_Values[T_N-1]=v;
      }

      Tuple(const T_Type& v,const Tuple<T_Type,T_N-1>& tup) {
        m_Values[0]=v;
        ForIndex(i,T_N-1) {
          m_Values[i+1]=tup[i];
        }
      }

      Tuple(const T_Type& v0,const T_Type& v1)
      {
        StaticCheck<T_N >= 2?1:0> ERROR_Tuple_constructor_dimension; (void)ERROR_Tuple_constructor_dimension;
        m_Values[0] = v0;
        m_Values[1] = v1;
        ForRange(i,2,T_N-1) {
          m_Values[i]=T_Type(0);
        }
      }

      Tuple(const T_Type& v0,const T_Type& v1,const T_Type& v2)
      {
        StaticCheck<T_N >= 3?1:0> ERROR_Tuple_constructor_dimension; (void)ERROR_Tuple_constructor_dimension;
        m_Values[0] = v0;
        m_Values[1] = v1;
        m_Values[2] = v2;
        ForRange(i,3,T_N-1) {
          m_Values[i]=T_Type(0);
        }
      }

      Tuple(const T_Type& v0,const T_Type& v1,const T_Type& v2,const T_Type& v3)
      {
        StaticCheck<T_N >= 4?1:0> ERROR_Tuple_constructor_dimension; (void)ERROR_Tuple_constructor_dimension;
        m_Values[0] = v0;
        m_Values[1] = v1;
        m_Values[2] = v2;
        m_Values[3] = v3;
        ForRange(i,4,T_N-1) {
          m_Values[i]=T_Type(0);
        }
      }

      Tuple(const T_Type& v)
      {
        ForIndex(i,T_N) {
          m_Values[i]=v;
        }
      }

      void fill(const T_Type& v)
      {
        ForIndex(i,T_N) {
          m_Values[i]=v;
        }
      }

      Tuple<T_Type,T_N>& operator=(const Tuple<T_Type,T_N>& tup)
      {
        ForIndex(i,T_N) {
          m_Values[i]=tup[i];
        }
        return *this;
      }

      Tuple<T_Type,T_N>& operator=(const T_Type& v)
      {
        ForIndex(i,T_N) {
          m_Values[i]=v;
        }
        return *this;
      }

      ///! Comparisons

      bool operator == (const Tuple<T_Type,T_N>& tup) const {
        ForIndex(i,T_N) {
          if (m_Values[i]!=tup[i])
            return false;
        }
        return true;
      }

      bool operator != (const Tuple<T_Type,T_N>& tup) const {
        ForIndex(i,T_N) {
          if (m_Values[i]!=tup[i])
            return true;
        }
        return false;
      }

      bool operator < (const Tuple<T_Type,T_N>& tup) const {
        ForIndex(i,T_N) {
          if (m_Values[i] < tup[i]) {
            return true;
          } else if (m_Values[i] > tup[i]) {
            return false;
          }
        }
        return false;
      }

      bool allLt(const Tuple<T_Type,T_N>& tup) const {
        ForIndex(i,T_N) {
          if (m_Values[i] >= tup[i]) {
            return false;
          }
        }
        return true;
      }

      bool allLteq(const Tuple<T_Type,T_N>& tup) const {
        ForIndex(i,T_N) {
          if (m_Values[i] > tup[i]) {
            return false;
          }
        }
        return true;
      }

      bool oneLt(const Tuple<T_Type,T_N>& tup) const {
        ForIndex(i,T_N) {
          if (m_Values[i] < tup[i]) {
            return true;
          }
        }
        return false;
      }

      bool oneLteq(const Tuple<T_Type,T_N>& tup) const {
        ForIndex(i,T_N) {
          if (m_Values[i] <= tup[i]) {
            return true;
          }
        }
        return false;
      }

      bool allGt(const Tuple<T_Type,T_N>& tup) const {
        ForIndex(i,T_N) {
          if (m_Values[i] <= tup[i]) {
            return false;
          }
        }
        return true;
      }

      bool allGteq(const Tuple<T_Type,T_N>& tup) const {
        ForIndex(i,T_N) {
          if (m_Values[i] < tup[i]) {
            return false;
          }
        }
        return true;
      }

      bool oneGt(const Tuple<T_Type,T_N>& tup) const {
        ForIndex(i,T_N) {
          if (m_Values[i] > tup[i]) {
            return true;
          }
        }
        return false;
      }

      bool oneGteq(const Tuple<T_Type,T_N>& tup) const {
        ForIndex(i,T_N) {
          if (m_Values[i] >= tup[i]) {
            return true;
          }
        }
        return false;
      }

      ///! Accessors

      const T_Type& operator[](int i) const {
        sl_dbg_assert((i>=0) && (i<T_N));
        return m_Values[i];
      }

      T_Type& operator[](int i) {
        sl_dbg_assert((i>=0) && (i<T_N));
        return m_Values[i];
      }

      template<int T_N2>
      Tuple<T_Type,T_N2> extract(uint p=0) const
      {
        Tuple<T_Type,T_N2> r;
        sl_dbg_assert(p+T_N2 <= T_N);
        ForIndex(n,T_N2) {
          r[n] = m_Values[p+n];
        }
        return (r);
      }

      ///! Unary operators

      Tuple<T_Type,T_N>&
        operator += (const Tuple<T_Type,T_N>& t)
      {
        ForIndex(i,T_N) {
          m_Values[i] += t[i];
        }
        return (*this);
      }

      Tuple<T_Type,T_N>&
        operator -= (const Tuple<T_Type,T_N>& t)
      {
        ForIndex(i,T_N) {
          m_Values[i] -= t[i];
        }
        return (*this);
      }

    };

    // Tuple operators (add,sub,dot,length,dist)

    // add

    template<class T_Type, int T_N> Tuple<T_Type,T_N>
    operator + (const Tuple<T_Type,T_N>& t0,const Tuple<T_Type,T_N>& t1)
    {
      Tuple<T_Type,T_N> r;
      ForIndex(i,T_N) {
        r[i]=t0[i]+t1[i];
      }
      return (r);
    }

    template<class T_Type, int T_N> Tuple<T_Type,T_N>
    add(const Tuple<T_Type,T_N>& t0,const Tuple<T_Type,T_N>& t1)
    {
      Tuple<T_Type,T_N> r;
      ForIndex(i,T_N) {
        r[i]=t0[i]+t1[i];
      }
      return (r);
    }

    template<class T_Type, int T_N> void addEq(Tuple<T_Type,T_N>& t0,const Tuple<T_Type,T_N>& t1)
    {
      ForIndex(i,T_N) {
        t0[i]+=t1[i];
      }
    }

    // sub

    template<class T_Type, int T_N> Tuple<T_Type,T_N>
    operator - (const Tuple<T_Type,T_N>& t0,const Tuple<T_Type,T_N>& t1)
    {
      Tuple<T_Type,T_N> r;
      ForIndex(i,T_N) {
        r[i]=t0[i]-t1[i];
      }
      return (r);
    }

    template<class T_Type, int T_N> Tuple<T_Type,T_N>
    sub(const Tuple<T_Type,T_N>& t0,const Tuple<T_Type,T_N>& t1)
    {
      Tuple<T_Type,T_N> r;
      ForIndex(i,T_N) {
        r[i]=t0[i]-t1[i];
      }
      return (r);
    }

    template<class T_Type, int T_N> void
      subEq(Tuple<T_Type,T_N>& t0,const Tuple<T_Type,T_N>& t1)
    {
      ForIndex(i,T_N) {
        t0[i]-=t1[i];
      }
    }

    // negate

    template<class T_Type, int T_N> Tuple<T_Type,T_N>
    operator - (const Tuple<T_Type,T_N>& t)
    {
      Tuple<T_Type,T_N> r;
      ForIndex(i,T_N) {
        r[i]=-t[i];
      }
      return (r);
    }

    template<class T_Type, int T_N> Tuple<T_Type,T_N>
    negate(const Tuple<T_Type,T_N>& t)
    {
      Tuple<T_Type,T_N> r;
      ForIndex(i,T_N) {
        r[i]=-t[i];
      }
      return (r);
    }

    template<class T_Type, int T_N> void
    negateEq(Tuple<T_Type,T_N>& t)
    {
      ForIndex(i,T_N) {
        t[i]=-t[i];
      }
    }

    // dot

    template<class T_Type, int T_N> T_Type
      dot(const Tuple<T_Type,T_N>& t0,const Tuple<T_Type,T_N>& t1)
    {
      T_Type r(0);
      ForIndex(i,T_N) {
        r += t0[i]*t1[i];
      }
      return (r);
    }

    // mul tuple,tuple

    template<class T_Type, int T_N> Tuple<T_Type,T_N>
    operator * (const Tuple<T_Type,T_N>& t0,const Tuple<T_Type,T_N>& t1)
    {
      Tuple<T_Type,T_N> r;
      ForIndex(i,T_N) {
        r[i]=t0[i]*t1[i];
      }
      return (r);
    }

    template<class T_Type, int T_N> Tuple<T_Type,T_N>
    mul(const Tuple<T_Type,T_N>& t0,const Tuple<T_Type,T_N>& t1)
    {
      Tuple<T_Type,T_N> r;
      ForIndex(i,T_N) {
        r[i]=t0[i]*t1[i];
      }
      return (r);
    }

    template<class T_Type, int T_N> void
      mulEq(Tuple<T_Type,T_N>& t0,const Tuple<T_Type,T_N>& t1)
    {
      ForIndex(i,T_N) {
        t0[i]*=t1[i];
      }
    }

    // mul tuple,scalar

    template<class T_Type, int T_N> Tuple<T_Type,T_N>
    operator * (const Tuple<T_Type,T_N>& t,const T_Type& s)
    {
      Tuple<T_Type,T_N> r;
      ForIndex(i,T_N) {
        r[i]=t[i]*s;
      }
      return (r);
    }

    template<class T_Type, int T_N> Tuple<T_Type,T_N>
    mul(const Tuple<T_Type,T_N>& t,const T_Type& s)
    {
      Tuple<T_Type,T_N> r;
      ForIndex(i,T_N) {
        r[i]=t[i]*s;
      }
      return (r);
    }

    template<class T_Type, int T_N> void
      mulEq(Tuple<T_Type,T_N>& t,const T_Type& s)
    {
      ForIndex(i,T_N) {
        t[i]*=s;
      }
    }

    // mul scalar,tuple

    template<class T_Type, int T_N> Tuple<T_Type,T_N>
    operator * (const T_Type& s,const Tuple<T_Type,T_N>& t)
    {
      return (t*s);
    }

    template<class T_Type, int T_N> Tuple<T_Type,T_N>
    mul(const T_Type& s,const Tuple<T_Type,T_N>& t)
    {
      return (mul(t,s));
    }

    template<class T_Type, int T_N> void
      mulEq(const T_Type& s,Tuple<T_Type,T_N>& t)
    {
      mulEq(t,s);
    }

    // div tuple,tuple

    template<class T_Type, int T_N> Tuple<T_Type,T_N>
    operator / (const Tuple<T_Type,T_N>& t0,const Tuple<T_Type,T_N>& t1)
    {
      Tuple<T_Type,T_N> r;
      ForIndex(i,T_N) {
        r[i]=t0[i]/t1[i];
      }
      return (r);
    }

    template<class T_Type, int T_N> Tuple<T_Type,T_N>
    div(const Tuple<T_Type,T_N>& t0,const Tuple<T_Type,T_N>& t1)
    {
      Tuple<T_Type,T_N> r;
      ForIndex(i,T_N) {
        r[i]=t0[i]/t1[i];
      }
      return (r);
    }

    template<class T_Type, int T_N> void
      divEq(Tuple<T_Type,T_N>& t0,const Tuple<T_Type,T_N>& t1)
    {
      ForIndex(i,T_N) {
        t0[i]/=t1[i];
      }
    }

    // div tuple,scalar

    template<class T_Type, int T_N> Tuple<T_Type,T_N>
    operator / (const Tuple<T_Type,T_N>& t,const T_Type& s)
    {
      Tuple<T_Type,T_N> r;
      ForIndex(i,T_N) {
        r[i]=t[i]/s;
      }
      return (r);
    }

    template<class T_Type, int T_N> Tuple<T_Type,T_N>
    div(const Tuple<T_Type,T_N>& t,const T_Type& s)
    {
      Tuple<T_Type,T_N> r;
      ForIndex(i,T_N) {
        r[i]=t[i]/s;
      }
      return (r);
    }

    template<class T_Type, int T_N> void
      divEq(Tuple<T_Type,T_N>& t,const T_Type& s)
    {
      sl_dbg_assert(abs(s) > 0);
      ForIndex(i,T_N) {
        t[i]/=s;
      }
    }

    // div scalar,tuple

    template<class T_Type, int T_N> Tuple<T_Type,T_N>
    operator / (const T_Type& s,const Tuple<T_Type,T_N>& t)
    {
      Tuple<T_Type,T_N> r;
      ForIndex(i,T_N) {
        r[i]=s/t[i];
      }
      return (r);
    }

    template<class T_Type, int T_N> Tuple<T_Type,T_N>
    div(const T_Type& s,const Tuple<T_Type,T_N>& t)
    {
      Tuple<T_Type,T_N> r;
      ForIndex(i,T_N) {
        r[i]=s/t[i];
      }
      return (r);
    }

    // length (norm L2)

    template<class T_Type, int T_N>
    T_Type sqLength(const Tuple<T_Type,T_N>& t)
    {
      T_Type sqlength = 0;
      ForIndex(i,T_N) {
        sqlength += t[i]*t[i];
      }
      return (sqlength);
    }

    template<class T_Type, int T_N>
    float length(const Tuple<T_Type,T_N>& t)
    {
      return (sqrt((float)sqLength(t)));
    }

    // normalize

    template<class T_Type, int T_N> Tuple<T_Type,T_N>
    normalize(const Tuple<T_Type,T_N>& t)
    {
      T_Type len = length(t);
      sl_dbg_assert(len > 0);
      Tuple<T_Type,T_N> r;
      r = t / len;
      return (r);
    }

    template<class T_Type, int T_N> void
      normalizeEq(Tuple<T_Type,T_N>& t)
    {
      divEq(t,length(t));
    }

    template<class T_Type, int T_N> Tuple<T_Type,T_N>
    normalize_safe(const Tuple<T_Type,T_N>& t)
    {
      T_Type len = length(t);
      Tuple<T_Type,T_N> r = 0;
      if (len > 1e-9) {
        r = t / len;
      }
      return (r);
    }

    // distance (norm L2)

    template<class T_Type, int T_N> T_Type
      sqDistance(const Tuple<T_Type,T_N>& t0,const Tuple<T_Type,T_N>& t1)
    {
      T_Type sqdist=0;
      ForIndex(i,T_N) {
        T_Type d = (t0[i]-t1[i]);
        sqdist += (d*d);
      }
      return (sqdist);
    }

    template<class T_Type, int T_N> T_Type
      distance(const Tuple<T_Type,T_N>& t0,const Tuple<T_Type,T_N>& t1)
    {
      return (sqrt(sqDistance(t0,t1)));
    }

    // component wise min

    template<class T_Type, int T_N> Tuple<T_Type,T_N>
    tupleMin(const Tuple<T_Type,T_N>& t0,const Tuple<T_Type,T_N>& t1)
    {
      Tuple<T_Type,T_N> m;
      ForIndex(i,T_N) {
        m[i]=LibSL::Math::min(t0[i],t1[i]);
      }
      return (m);
    }

    // component wise max

    template<class T_Type, int T_N> Tuple<T_Type,T_N>
    tupleMax(const Tuple<T_Type,T_N>& t0,const Tuple<T_Type,T_N>& t1)
    {
      Tuple<T_Type,T_N> m;
      ForIndex(i,T_N) {
        m[i]=LibSL::Math::max(t0[i],t1[i]);
      }
      return (m);
    }

    // component wise abs

    template<class T_Type, int T_N> Tuple<T_Type,T_N>
    tupleAbs(const Tuple<T_Type,T_N>& t)
    {
      Tuple<T_Type,T_N> a;
      ForIndex(i,T_N) {
        a[i]=LibSL::Math::abs(t[i]);
      }
      return (a);
    }

    // min of all

    template<class T_Type, int T_N> T_Type
    tupleMin(const Tuple<T_Type,T_N>& t)
    {
      T_Type m=t[0];
      ForIndex(i,T_N-1) {
        m=LibSL::Math::min(m,t[i+1]);
      }
      return (m);
    }

    // max of all

    template<class T_Type, int T_N> T_Type
    tupleMax(const Tuple<T_Type,T_N>& t)
    {
      T_Type m=t[0];
      ForIndex(i,T_N-1) {
        m=LibSL::Math::max(m,t[i+1]);
      }
      return (m);
    }

    // component wise frac

    template<class T_Type, int T_N> Tuple<T_Type,T_N>
    frac(const Tuple<T_Type,T_N>& t)
    {
      Tuple<T_Type,T_N> r;
      ForIndex(i,T_N) {
        r[i]=LibSL::Math::frac(t[i]);
      }
      return (r);
    }

    // component wise modulo

    template<class T_Type, int T_N> Tuple<T_Type,T_N>
    operator % (const Tuple<T_Type,T_N>& t,const T_Type& m)
    {
      Tuple<T_Type,T_N> r;
      ForIndex(i,T_N) {
        r[i]=t[i] % m;
      }
      return (r);
    }

    // component wise clamp

    template<class T_Type, int T_N> Tuple<T_Type,T_N>
    clamp(const Tuple<T_Type,T_N>& t,const Tuple<T_Type,T_N>& clampMin,const Tuple<T_Type,T_N>& clampMax)
    {
      Tuple<T_Type,T_N> r;
      ForIndex(i,T_N) {
        r[i] = LibSL::Math::clamp(t[i],clampMin[i],clampMax[i]);
      }
      return (r);
    }

    // component wise abs

    template<class T_Type, int T_N>
    Tuple<T_Type,T_N> abs(const Tuple<T_Type,T_N>& t)
    {
      Tuple<T_Type,T_N> r;
      ForIndex(i,T_N) {
        r[i] = LibSL::Math::abs(t[i]);
      }
      return (r);
    }

    // component wise log2

    template <typename T_Type,int T_Num>
    Tuple<T_Type,T_Num> log2(const Tuple<T_Type,T_Num>& t)
    {
      Tuple<T_Type,T_Num> r;
      ForIndex(n,T_Num) {
        if (t[n] == 0) {
          r[n] = 0;
        } else {
          r[n] = log2(t[n]);
        }
      }
      return r;
    }

    // component wise log10

    template <typename T_Type,int T_Num>
    Tuple<T_Type,T_Num> log10(const Tuple<T_Type,T_Num>& t)
    {
      Tuple<T_Type,T_Num> r;
      ForIndex(n,T_Num) {
        if (t[n] == 0) {
          r[n] = 0;
        } else {
          r[n] = log10(t[n]);
        }
      }
      return r;
    }

    // component wise log

    template <typename T_Type,int T_Num>
    Tuple<T_Type,T_Num> log(const Tuple<T_Type,T_Num>& t)
    {
      Tuple<T_Type,T_Num> r;
      ForIndex(n,T_Num) {
        if (t[n] == 0) {
          r[n] = 0;
        } else {
          r[n] = log(t[n]);
        }
      }
      return r;
    }

    // component wise exp

    template <typename T_Type,int T_Num>
    Tuple<T_Type,T_Num> exp(const Tuple<T_Type,T_Num>& t)
    {
      Tuple<T_Type,T_Num> r;
      ForIndex(n,T_Num) {
        r[n] = exp(t[n]);
      }
      return r;
    }

    // component wise sqrt

    template <typename T_Type,int T_Num>
    Tuple<T_Type,T_Num> sqrt(const Tuple<T_Type,T_Num>& t)
    {
      Tuple<T_Type,T_Num> r;
      ForIndex(n,T_Num) {
        r[n] = sqrt(t[n]);
      }
      return r;
    }

    // component wise ceil

    template <typename T_Type,int T_Num>
    Tuple<T_Type,T_Num> ceil(const Tuple<T_Type,T_Num>& t)
    {
      Tuple<T_Type,T_Num> r;
      ForIndex(n,T_Num) {
        r[n] = ::ceil(t[n]);
      }
      return r;
    }

    // component wise floor

    template <typename T_Type,int T_Num>
    Tuple<T_Type,T_Num> floor(const Tuple<T_Type,T_Num>& t)
    {
      Tuple<T_Type,T_Num> r;
      ForIndex(n,T_Num) {
        r[n] = ::floor(t[n]);
      }
      return r;
    }

    // component wise round

    template <typename T_Type,int T_Num>
    Tuple<T_Type,T_Num> round(const Tuple<T_Type,T_Num>& t)
    {
      Tuple<T_Type,T_Num> r;
      ForIndex(n,T_Num) {
        r[n] = ::round(t[n]);
      }
      return r;
    }

    // component wise sign

    template <typename T_Type,int T_Num>
    Tuple<T_Type,T_Num> sign(const Tuple<T_Type,T_Num>& t)
    {
      Tuple<T_Type,T_Num> r;
      ForIndex(n,T_Num) {
        r[n] = Math::sign(t[n]);
      }
      return r;
    }

    // stream

    template<class T_Type, int T_N>
    std::ostream& operator<<(std::ostream& s,const Tuple<T_Type,T_N>& t)
    {
      s << '(';
      ForIndex(i,T_N) { s << t[i]; if (i < T_N-1) s << ','; }
      s << ')';
      return (s);
    }

    template<class T_Type, int T_N>
    std::istream& operator>>(std::istream& s,Tuple<T_Type,T_N>& t)
    {
			char tmp = 0;
			s >> tmp; // (
			for (int i = 0; i < T_N; ++i)
			{
				s >> t [i];
				s >> tmp; //, or )
			}

			return s;
    }

    // iterator (on integers only)

    // inc(i,s) - most varying is i[0]
    //          - use with done(i,s) in loops
    template<class T_Type,int T_N>
    Tuple<T_Type,T_N>& inc(Tuple<T_Type,T_N>& i,const Tuple<T_Type,T_N> s)
    {
      ForIndex(d,T_N) {
        if (i[d] == s[d]-1) {
          // max value reached
          if (d == T_N-1) {
            // last one, make equal to s
            i[d]++;
          } else {
            // zero, next will be incremented
            i[d]=0;
          }
        } else {
          i[d]++;
          // no need to go further
          return i;
        }
      }
      return i;
    }

    // inc(i,s) - most varying is i[0]
    //          - use with done(i,s) in loops
    //          - restart from l instead of 0
    template<class T_Type,int T_N>
    Tuple<T_Type,T_N>& inc(Tuple<T_Type,T_N>& i,const Tuple<T_Type,T_N> l,const Tuple<T_Type,T_N> s)
    {
      ForIndex(d,T_N) {
        if (i[d] == s[d]-1) {
          // max value reached
          if (d == T_N-1) {
            // last one, make equal to s
            i[d]++;
          } else {
            // zero, next will be incremented
            i[d]=l[d];
          }
        } else {
          i[d]++;
          // no need to go further
          return i;
        }
      }
      return i;
    }

    // done(i,s) - true when i reaches s (use with inc(i,s) in loops)
    template<class T_Type,int T_N>
    bool done(Tuple<T_Type,T_N>& i,const Tuple<T_Type,T_N> s)
    {
      return (i[T_N-1]>=s[T_N-1]);
    }

    // product of all components
    template<class T_Type,int T_N>
    T_Type productOfAll(const Tuple<T_Type,T_N>& t)
    {
      T_Type r = T_Type(1);
      ForIndex(n,T_N) {
        r = r * t[n];
      }
      return (r);
    }

    // addressing

    template<int T_N>
    uint addressOf(const Tuple<uint,T_N>& idx,const Tuple<uint,T_N>& size)
    {
      uint addr   = 0;
      uint stride = 1;
      ForIndex(n,T_N) {
        addr   += idx[n]*stride;
        stride *= size[n];
      }
      return (addr);
    }

    template<int T_N>
    Tuple<uint,T_N> indexOf(uint addr,const Tuple<uint,T_N>& size)
    {
      Tuple<uint,T_N> idx = 0;
      ForIndex(n,T_N) {
        idx[n] = addr % size[n];
        addr   = addr / size[n];
      }
      return (idx);
    }

    // default constructors

    template<class T_Type>
    Tuple<T_Type,1> Single(const T_Type& v0)
    {
      Tuple<T_Type,1> t;
      t[0]=v0;
      return (t);
    }

    template<class T_Type>
    Tuple<T_Type,2> Pair(const T_Type& v0,const T_Type& v1)
    {
      Tuple<T_Type,2> t;
      t[0]=v0; t[1]=v1;
      return (t);
    }

    template<class T_Type>
    Tuple<T_Type,3> Triple(const T_Type& v0,const T_Type& v1,const T_Type& v2)
    {
      Tuple<T_Type,3> t;
      t[0]=v0; t[1]=v1; t[2]=v2;
      return (t);
    }

    template<class T_Type>
    Tuple<T_Type,4> Quad(const T_Type& v0,const T_Type& v1,const T_Type& v2,const T_Type& v3)
    {
      Tuple<T_Type,4> t;
      t[0]=v0; t[1]=v1; t[2]=v2; t[3]=v3;
      return (t);
    }

  } //namespace LibSL::Math
} //namespace LibSL

// -----------------------------------------------------------

#define ForTuple(N,I,S)        for (LibSL::Math::Tuple<uint,N> I=0 ; !LibSL::Math::done(I,LibSL::Math::Tuple<uint,N>(S))   ; LibSL::Math::inc(I,LibSL::Math::Tuple<uint,N>(S)))
#define ForTuple_int(N,I,S)    for (LibSL::Math::Tuple<int ,N> I=0 ; !LibSL::Math::done(I,LibSL::Math::Tuple<int ,N>(S))   ; LibSL::Math::inc(I,LibSL::Math::Tuple<int ,N>(S)))
#define ForTupleRange(N,I,L,U) for (LibSL::Math::Tuple<int,N>  I=L ; !LibSL::Math::done(I,U+LibSL::Math::Tuple<int,N>(1)) ; LibSL::Math::inc(I,L,U+LibSL::Math::Tuple<int,N>(1)))

// -----------------------------------------------------------
