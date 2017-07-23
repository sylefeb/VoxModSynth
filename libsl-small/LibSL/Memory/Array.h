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
// LibSL::Memory::Array
// ------------------------------------------------------
//
// Simple Array alloator with bound checking capabilities
//
// ------------------------------------------------------
// Sylvain Lefebvre - 2006-02-20
// ------------------------------------------------------

#pragma once

// ------------------------------------------------------

#include <LibSL/Errors/Errors.h>
#include <LibSL/CppHelpers/CppHelpers.h>
#include <LibSL/System/Types.h>
//#include <LibSL/Memory/TraceLeaks.h>
#include <LibSL/Math/Tuple.h>
using namespace LibSL::System::Types;

#include <vector>
#ifdef max
#undef max
#endif
#ifdef min
#undef min
#endif

// ------------------------------------------------------

namespace LibSL  {
  namespace Memory {
    namespace Array {

      // ===============
      // Init policies

      template <typename T_Value> class InitNop
      {
      public:
        static void initValue(T_Value *) {}
      };

      template <typename T_Value> class InitZero
      {
      public:
        static void initValue(T_Value *v) {*v=0;}
      };

      // ===============
      // Check policies

      class CheckNop
      {
      public:
        enum {PerformCheck = 0};
        static inline void checkAllocation(const void *)  {}
        static inline void checkPointer(const void *)     {}
        static inline void checkAccess(int ,uint)         {}
        static inline void checkEmpty(const void *)       {}
      };

      class CheckAll
      {
      public:
        enum {PerformCheck = 1};
        static inline void checkAllocation(const void *p) {if (p==NULL) LIBSL_FATAL_ERROR("Memory::Array - allocation failed");}
        static inline void checkPointer(const void *p)    {if (p==NULL) LIBSL_FATAL_ERROR("Memory::Array - access violation");}
        static inline void checkAccess(int n,uint num)    {if (n < 0 || n >= int(num)) LIBSL_FATAL_ERROR("Memory::Array - access out of bounds");}
        static inline void checkEmpty(const void *p)      {if (p!=NULL) LIBSL_FATAL_ERROR("Memory::Array - array already initialized");}
      };

      // ===============
      /// Access policies

      class Clamp
      {
      public:
        static uint access(const uint c, const uint size)
        { return LibSL::Math::max(int(0),LibSL::Math::min(int(size)-1,c));}
      };

      class Wrap
      {
      public:
        static uint access(const uint c, const uint size)
        {
          uint ct = (c+size) % size;
          return ct;
        }
      };

      // ===============
      // Array class
      //   Configurable through above policies

      template <
        typename T_Type,
        template <typename> class P_Init=InitNop,
#ifdef LIBSL_RELEASE
        class P_Check=CheckNop
#else
        class P_Check=CheckAll
#endif
      >
      class Array // : public LibSL::Memory::TraceLeaks::LeakProbe<Array<T_Type,P_Init,P_Check> >
      {
      private:

        T_Type *m_Data;
        uint    m_Size;
        uint    m_AllocSize;

      public:

        typedef T_Type t_Element;

        enum {e_NumDim = 1};

      public:

        Array(void)
        {
          m_Size=0;
          m_AllocSize=0;
          m_Data=NULL;
        }

        Array(uint size)
        {
          m_Size=0;
          m_AllocSize=0;
          m_Data=NULL;
          allocate(size);
        }

        Array(const std::vector<T_Type>& vec)
        {
          m_Size=0;
          m_AllocSize=0;
          m_Data=NULL;
          allocate(vec.size());
          ForIndex(n,vec.size()) {
            if (P_Check::PerformCheck) P_Check::checkAccess(n,m_Size);
            m_Data[n]=vec[n];
          }
        }

        ~Array(void)
        {
          if (m_Data != NULL) {
            delete[] (m_Data);
          }
        }

        /// Erase the array
        void erase()
        {
          if (m_Data != NULL) {
            delete[] (m_Data);
            m_Data      = NULL;
            m_AllocSize = 0;
            m_Size      = 0;
          }
        }

        /// Allocate the array
        void allocate(uint size_to_allocate)
        {
          erase();
          if (P_Check::PerformCheck) P_Check::checkEmpty(m_Data);
          m_AllocSize = size_to_allocate;
          m_Size      = size_to_allocate;
          m_Data      = new T_Type[m_AllocSize];
          if (P_Check::PerformCheck) P_Check::checkAllocation(m_Data);
          // init array
          ForIndex(n,int(m_AllocSize)) {
            if (P_Check::PerformCheck) P_Check::checkAccess(n,m_AllocSize);
            P_Init<T_Type>::initValue(&(m_Data[n]));
          }
        }

        /// Copy (constructor)
        Array(const Array& a)
        {
          m_Size      = 0;
          m_AllocSize = 0;
          m_Data      = NULL;
          if (a.size() > 0) {
            allocate(a.size());
            ForIndex (n,m_Size) {
              if (P_Check::PerformCheck) P_Check::checkAccess(n,m_Size);
              m_Data[n] = a[n];
            }
          }
        }

        /// Copy (affectation)
        const Array& operator = (const Array& a)
        {
          if (a.size() > m_AllocSize) {    // do not erase if size is sufficient
            erase();
          }
          if (!a.empty()) {           // skip if 'a' is empty
            if (empty()) {            // only allocate if necessary
              allocate(a.size());
            } else {
              m_Size = a.size();      // make size equal, keep allocated memory
            }
            ForIndex (n,m_Size) {
              if (P_Check::PerformCheck) P_Check::checkAccess(n,m_Size);
              m_Data[n] = a[n];
            }
          }
          return (*this);
        }

        /// Fill array with a given value
        void fill(const T_Type& value_to_fill_with)
        {
          for (uint n=0;n<m_Size;n++) {
            if (P_Check::PerformCheck) P_Check::checkAccess(n,m_Size);
            m_Data[n] = value_to_fill_with;
          }
        }

        /// Resize array
        void truncate(uint new_size)
        {
          sl_assert(new_size <= m_AllocSize);
          m_Size = new_size;
        }

        /// Read only access
        const T_Type& operator [](uint n) const
        {
          if (P_Check::PerformCheck) P_Check::checkAccess(n,m_Size);
          return (m_Data[n]);
        }

        /// Read/write access
        T_Type& operator [](uint n)
        {
          if (P_Check::PerformCheck) P_Check::checkAccess(n,m_Size);
          return (m_Data[n]);
        }

        /// [] operator with Tuple
        const T_Type& operator[](const LibSL::Math::Tuple<uint,1>& access) const
        {
          return (*this)[access[0]];
        }

        T_Type& operator[](const LibSL::Math::Tuple<uint,1>& access)
        {
          return (*this)[access[0]];
        }

        /// Array size
        uint size() const
        {
          return (m_Size);
        }

        /// Array allocated size
        uint allocatedSize() const
        {
          return (m_AllocSize);
        }

        /// Array size as a tuple
        LibSL::Math::Tuple<uint,1> sizeTuple() const
        {
          return (LibSL::Math::Single(m_Size));
        }

        /// Empty?
        bool empty() const
        {
          return (m_Size == 0);
        }

        /// Raw pointer
        const T_Type *raw() const {return (m_Data);}
        T_Type       *raw()       {return (m_Data);}

        /// STL Style iterators
        const T_Type *begin() const {return (m_Data);}
        T_Type *begin()             {return (m_Data);}

        const T_Type *end() const {return (m_Data + m_Size);}
        T_Type *end()             {return (m_Data + m_Size);}

        /// Array size of
        uint sizeOfData() const
        {
          return (m_Size * sizeof(t_Element));
        }

      };


#ifdef WIN32 // This is due to an incompatibility btw Visual C and g++ // TODO FIXME
      // assumes WIN32 means Visual C++

      /*!

      FastArray build from Array with InitNop and CheckNop policies

      */
      template <class T_Type>
      class FastArray : public Array<T_Type,InitNop,CheckNop>
      {
      public:
        FastArray()                               : Array()     { }
        FastArray(uint size)                      : Array(size) { }
        FastArray(const std::vector<T_Type>& vec) : Array(vec)  { }
      };
#else

      /*!

      FastArray build from Array with InitNop and CheckNop policies

      */
      template <class T_Type>
      class FastArray : public Array<T_Type,InitNop,CheckNop>
      {
      protected:
	typedef Array<T_Type,InitNop,CheckNop> t_Base;
      public:
        FastArray()                               : t_Base()     { }
        FastArray(uint size)                      : t_Base(size) { }
        FastArray(const std::vector<T_Type>& vec) : t_Base(vec)  { }
      };

#endif

    } // namespace LibSL::Memory::Array
  } // namespace LibSL::Memory
} // namespace LibSL

// ----------------------------------------------------

#define ForArray(T,I)  for (uint I=0;I<T.size();I++)

// ------------------------------------------------------

