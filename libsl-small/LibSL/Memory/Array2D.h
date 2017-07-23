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
// LibSL::Memory::Array2D
// ------------------------------------------------------
//
// 2D Array built upon Array
//
// Access is done by get(x,y)/set(x,y) or array[A2D(x,y)]
//
// Memory layout is the same as an image:
// => Array2D are stored in row major order
// (x / column is the most coherent index.    )
// (an efficient loop must do for (y) for (x) )
// (see also ForArray2D(array,x,y)            )
//
// ------------------------------------------------------
// Sylvain Lefebvre - 2006-03-07
// ------------------------------------------------------

#pragma once

// ------------------------------------------------------

#include <LibSL/Errors/Errors.h>
#include <LibSL/CppHelpers/CppHelpers.h>
#include <LibSL/System/Types.h>
#include <LibSL/Math/Tuple.h>
using namespace LibSL::System::Types;

#include "LibSL/Memory/Array.h"

// ------------------------------------------------------

namespace LibSL  {
  namespace Memory {
    namespace Array {

      /// Array2D class
      ///   Configurable through Array policies

      template <
        typename T_Type,
        template <typename> class P_Init=InitNop,
#ifdef LIBSL_RELEASE
        class P_Check=CheckNop
#else
        class P_Check=CheckAll
#endif
      >
      class Array2D
        // : public LibSL::Memory::TraceLeaks::LeakProbe<Array2D<T_Type,P_Init,P_Check> >
      {
      private:

        Array<T_Type,P_Init,P_Check> m_Array;
        uint                         m_YSize;
        uint                         m_XSize;

      public:

        typedef T_Type t_Element;

        enum {e_NumDim = 2};

      public:

        Array2D(void)
        {
          m_XSize=0;
          m_YSize=0;
        }

        Array2D(uint xsize, uint ysize)
        {
          m_XSize=xsize;
          m_YSize=ysize;
          m_Array.allocate(m_YSize*m_XSize);
        }

        Array2D(const LibSL::Math::Tuple<uint,2>& sizes)
        {
          m_XSize=sizes[0];
          m_YSize=sizes[1];
          m_Array.allocate(m_YSize*m_XSize);
        }

        ~Array2D(void)
        {
        }

        void allocate(uint xsize, uint ysize)
        {
          m_XSize=xsize;
          m_YSize=ysize;
          m_Array.allocate(m_YSize*m_XSize);
        }

        /// Copy (constructor)
        Array2D(const Array2D& a)
        {
          m_XSize=a.m_XSize;
          m_YSize=a.m_YSize;
          m_Array=a.m_Array;
        }

        /// Copy (affectation)
        const Array2D& operator = (const Array2D& a)
        {
          m_XSize=a.m_XSize;
          m_YSize=a.m_YSize;
          m_Array=a.m_Array;
          return (*this);
        }

        /// Fill array with a given value
        void fill(const T_Type& value_to_fill_with)
        {
          m_Array.fill(value_to_fill_with);
        }

        /// Read only access
        const T_Type& get(uint x, uint y) const
        {
          if (P_Check::PerformCheck) P_Check::checkAccess(x,m_XSize);
          if (P_Check::PerformCheck) P_Check::checkAccess(y,m_YSize);
          return (m_Array[y*m_XSize + x]);
        }

        /// Read/write access
        T_Type& set(uint x, uint y)
        {
          if (P_Check::PerformCheck) P_Check::checkAccess(x,m_XSize);
          if (P_Check::PerformCheck) P_Check::checkAccess(y,m_YSize);
          return (m_Array[y*m_XSize + x]);
        }

        /// Read only access
        const T_Type& at(uint x, uint y) const
        {
          if (P_Check::PerformCheck) P_Check::checkAccess(x,m_XSize);
          if (P_Check::PerformCheck) P_Check::checkAccess(y,m_YSize);
          return (m_Array[y*m_XSize + x]);
        }

        /// Read/write access
        T_Type& at(uint x, uint y)
        {
          if (P_Check::PerformCheck) P_Check::checkAccess(x,m_XSize);
          if (P_Check::PerformCheck) P_Check::checkAccess(y,m_YSize);
          return (m_Array[y*m_XSize + x]);
        }

        /// [] operator with Tuple
        const T_Type& operator[](const LibSL::Math::Tuple<uint,2>& access) const
        {
          return at(access[0],access[1]);
        }

        T_Type& operator[](const LibSL::Math::Tuple<uint,2>& access)
        {
          return at(access[0],access[1]);
        }

        /// 2D Array xsize
        uint xsize() const
        {
          return (m_XSize);
        }

        /// 2D Array ysize
        uint ysize() const
        {
          return (m_YSize);
        }

        /// Array size as a tuple
        LibSL::Math::Tuple<uint,2> sizeTuple() const
        {
          return (LibSL::Math::Pair(m_XSize,m_YSize));
        }

        /// empty?
        bool empty() const
        {
          return (m_Array.empty());
        }

        /// Erase
        void erase()
        {
          m_Array.erase();
          m_XSize=0;
          m_YSize=0;
        }

        /// Raw pointer
        const T_Type *raw() const {return (m_Array.raw());}
        T_Type       *raw()       {return (m_Array.raw());}

        /// Array size of data
        uint sizeOfData() const
        {
          return (m_Array.sizeOfData());
        }

        /// Access with Policy
        template <class T_AccessPolicy>
        const T_Type& at(uint i, uint j) const {
          const uint x = T_AccessPolicy::access(i,m_XSize);
          const uint y = T_AccessPolicy::access(j,m_YSize);
          return m_Array[y*m_XSize + x];
        }
        template <class T_AccessPolicy>
        T_Type& at(uint i, uint j)       {
          const uint x = T_AccessPolicy::access(i,m_XSize);
          const uint y = T_AccessPolicy::access(j,m_YSize);
          return m_Array[y*m_XSize + x];
        }

      };


#ifdef WIN32 // This is due to an incompatibility btw Visual C and g++ // TODO FIXME
      // assumes WIN32 means Visual C++

      /*!

      FastArray build from Array with InitNop and CheckNop policies

      */
      template <class T_Type>
      class FastArray2D : public Array2D<T_Type,InitNop,CheckNop>
      {
      public:
        FastArray2D()										 : Array2D()     { }
        FastArray2D(uint xsize, uint ysize)					 : Array2D(xsize, ysize) { }
        FastArray2D(const LibSL::Math::Tuple<uint,2>& sizes) : Array2D(sizes)  { }
      };

#else

	/*!

      FastArray build from Array with InitNop and CheckNop policies

      */
      template <class T_Type>
      class FastArray2D : public Array2D<T_Type,InitNop,CheckNop>
      {
      private:
	typedef Array2D<T_Type,InitNop,CheckNop> t_Base;
      public:
        FastArray2D()					     : t_Base()     { }
        FastArray2D(uint xsize, uint ysize)		     : t_Base(xsize, ysize) { }
        FastArray2D(const LibSL::Math::Tuple<uint,2>& sizes) : t_Base(sizes)  { }
      };

#endif


    } // namespace LibSL::Memory::Array
  } // namespace LibSL::Memory
} // namespace LibSL

// ----------------------------------------------------

#define ForArray2D(T,I,J) for (int J=0;J<(int)T.ysize();J++) for (int I=0;I<(int)T.xsize();I++)
#define A2D(x,y)          LibSL::Math::Pair(x,y)

// ----------------------------------------------------
