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
// LibSL::Memory::Array3D
// ------------------------------------------------------
//
// 3D Array built upon Array
// 
// Access is done by get(x,y,z)/set(x,y,z) or array[A3D(x,y,z)]
//
// Memory layout is the same as an image: x is the least
// varying (or most coherent) index.
// => Array3D are stored in row major order
//
// ------------------------------------------------------
// Sylvain Lefebvre - 2006-05-18
// ------------------------------------------------------

#pragma once

// ------------------------------------------------------

#include <LibSL/Errors/Errors.h>
#include <LibSL/CppHelpers/CppHelpers.h>
#include <LibSL/System/Types.h>
#include <LibSL/Math/Vertex.h>

using namespace LibSL::System::Types;

#include "LibSL/Memory/Array.h"

// ------------------------------------------------------

namespace LibSL  {
  namespace Memory {
    namespace Array {

      /// helper class for 2D array access thorugh operator []
      typedef LibSL::Math::Tuple<uint,3> Access3D;

      /// Array3D class
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
      class Array3D 
        // : public LibSL::Memory::TraceLeaks::LeakProbe<Array2D<T_Type,P_Init,P_Check> >
      {
      private:

        Array<T_Type,P_Init,P_Check> m_Array;
        uint                         m_XSize;
        uint                         m_YSize;
        uint                         m_ZSize;

      public:

        typedef T_Type t_Element;

        enum {e_NumDim = 3};
      
      public:

        Array3D(void) 
        {
          m_XSize=0;
          m_YSize=0;
          m_ZSize=0;
        }

        Array3D(uint xsize,uint ysize,uint zsize) 
        {
          m_XSize=xsize;
          m_YSize=ysize;
          m_ZSize=zsize;
          m_Array.allocate(m_ZSize*m_YSize*m_XSize);
        }

        Array3D(const LibSL::Math::Tuple<uint,3>& sizes)
        {
          m_XSize=sizes[0];
          m_YSize=sizes[1];
          m_ZSize=sizes[2];
          m_Array.allocate(m_ZSize*m_YSize*m_XSize);
        }

        ~Array3D(void)
        {
        }

        void allocate(uint xsize,uint ysize,uint zsize) 
        {
          m_XSize=xsize;
          m_YSize=ysize;
          m_ZSize=zsize;
          m_Array.allocate(m_ZSize*m_YSize*m_XSize);
        }

        /// Copy (constructor)
        Array3D(const Array3D& a)
        {
          m_XSize=a.m_XSize;
          m_YSize=a.m_YSize;
          m_ZSize=a.m_ZSize;
          m_Array=a.m_Array;
        }

        /// Copy (affectation)
        const Array3D& operator = (const Array3D& a)
        {
          m_XSize=a.m_XSize;
          m_YSize=a.m_YSize;
          m_ZSize=a.m_ZSize;
          m_Array=a.m_Array;
          return (*this);
        }

        /// Fill array with a given value
        void fill(T_Type value_to_fill_with)
        {
          m_Array.fill(value_to_fill_with);
        }

        /// Read only access
        const T_Type& get(uint x,uint y,uint z) const
        {
          if (P_Check::PerformCheck) P_Check::checkAccess(x,m_XSize);
          if (P_Check::PerformCheck) P_Check::checkAccess(y,m_YSize);
          if (P_Check::PerformCheck) P_Check::checkAccess(z,m_ZSize);
          return (m_Array[(z*m_YSize+y)*m_XSize + x]);
        }

        /// Read only access
        const T_Type& at(uint x,uint y,uint z) const
        {
          if (P_Check::PerformCheck) P_Check::checkAccess(x,m_XSize);
          if (P_Check::PerformCheck) P_Check::checkAccess(y,m_YSize);
          if (P_Check::PerformCheck) P_Check::checkAccess(z,m_ZSize);
          return (m_Array[(z*m_YSize+y)*m_XSize + x]);
        }

        /// Read/write access
        T_Type& set(uint x,uint y,uint z)
        {
          if (P_Check::PerformCheck) P_Check::checkAccess(x,m_XSize);
          if (P_Check::PerformCheck) P_Check::checkAccess(y,m_YSize);
          if (P_Check::PerformCheck) P_Check::checkAccess(z,m_ZSize);
          return (m_Array[(z*m_YSize+y)*m_XSize + x]);
        }

        /// Read/write access
        T_Type& at(uint x,uint y,uint z)
        {
          if (P_Check::PerformCheck) P_Check::checkAccess(x,m_XSize);
          if (P_Check::PerformCheck) P_Check::checkAccess(y,m_YSize);
          if (P_Check::PerformCheck) P_Check::checkAccess(z,m_ZSize);
          return (m_Array[(z*m_YSize+y)*m_XSize + x]);
        }

        /// [] operator with Tuple
        const T_Type& operator[](const LibSL::Math::Tuple<uint,3>& access) const
        {
          return at(access[0],access[1],access[2]);
        }

        T_Type& operator[](const LibSL::Math::Tuple<uint,3>& access)
        {
          return at(access[0],access[1],access[2]);
        }

        /// 3D Array xsize
        uint xsize() const 
        {
          return (m_XSize);
        }

        /// 3D Array ysize
        uint ysize() const 
        {
          return (m_YSize);
        }

        /// 3D Array zsize
        uint zsize() const 
        {
          return (m_ZSize);
        }

        /// Array size as a tuple
        LibSL::Math::Tuple<uint,3> sizeTuple() const 
        {
          return (LibSL::Math::Triple(m_XSize,m_YSize,m_ZSize));
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
          m_ZSize=0;
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
        const T_Type& at(uint i, uint j,uint k) const {
          const uint x = T_AccessPolicy::access(i,m_XSize);
          const uint y = T_AccessPolicy::access(j,m_YSize);
          const uint z = T_AccessPolicy::access(k,m_ZSize);
          return m_Array[(z*m_YSize+y)*m_XSize + x];
        }
        template <class T_AccessPolicy>
        T_Type& at(uint i, uint j,uint k)       {
          const uint x = T_AccessPolicy::access(i,m_XSize);
          const uint y = T_AccessPolicy::access(j,m_YSize);
          const uint z = T_AccessPolicy::access(k,m_ZSize);
          return m_Array[(z*m_YSize+y)*m_XSize + x];
        }

      };

    } // namespace LibSL::Memory::Array
  } // namespace LibSL::Memory
} // namespace LibSL

// ----------------------------------------------------

#define ForArray3D(T,I,J,K) for (int K=0;K<T.zsize();K++) for (int J=0;J<T.ysize();J++) for (int I=0;I<T.xsize();I++)
#define A3D(x,y,z)          LibSL::Math::Triple(x,y,z)

// ----------------------------------------------------
