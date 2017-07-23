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
// LibSL::Memory::Pointer
// ------------------------------------------------------
//
// Implements a 'smart' pointer driven by policies
//  - Access
//  - Check
//  - Copy
// ------------------------------------------------------
// Sylvain Lefebvre - 2006-02-20
// ------------------------------------------------------

#pragma once

// ------------------------------------------------------

#include <LibSL/Errors/Errors.h>
#include <LibSL/CppHelpers/CppHelpers.h>

#include <LibSL/System/Types.h>
using namespace LibSL::System::Types;

// #include <atomic>

// ------------------------------------------------------

namespace LibSL  {
  namespace Memory {
    namespace Pointer {

      // ===============
      // Check policies

      class CheckNop
      {
      public:
        static void checkValid(void *) {}
      };

      class CheckValid
      {
      public:
        static void checkValid(void *p) {if (p==NULL) LIBSL_FATAL_ERROR("Invalid pointer !");}
      };

      // ===============
      // Transfer policies

      template <typename T_Type> class TransferAddress
      {
      public:
        TransferAddress()         {}
        TransferAddress(T_Type *) {}
        void transferFrom(
          T_Type                       **dst,
          const TransferAddress<T_Type> *policy_src,
          T_Type                        *src)     { *dst=src; }
        template <typename T_Type2>
        void transferCastFrom(
          T_Type                        **dst,
          const TransferAddress<T_Type2> *policy_src,
          T_Type2                        *src)     { *dst=dynamic_cast<T_Type*>(src); }
        void release(T_Type **ptr)               {  }
      };

      template <typename T_Type> class TransferClone
      {
      public:
        TransferClone()         {}
        TransferClone(T_Type *) {}
        void transferFrom(
          T_Type                     **dst,
          const TransferClone<T_Type> *policy_src,
          T_Type                      *src)        { if ( src != NULL) {*dst=new T_Type(*src);   } else {*dst=NULL;} }
        template <typename T_Type2>
        void transferCastFrom(
          T_Type                     **dst,
          const TransferClone<T_Type2> *policy_src,
          T_Type2                      *src)       { if ( src != NULL) {*dst=new T_Type(*src);   } else {*dst=NULL;} }
        void release(T_Type **ptr)                 { if (*ptr != NULL) {delete (*ptr); *ptr=NULL;} }
      };

      template <typename T_Type,typename T_CounterType,uint T_CounterMaxValue> class TransferRefCount
      {
      public:

//        std::atomic<T_CounterType> *m_Counter;
        T_CounterType *m_Counter;

        TransferRefCount()
        {
          m_Counter = NULL;
        }

        TransferRefCount(T_Type *ptr)
        {
          if (ptr != NULL) {
            // allocate a new counter initialized to 1
            // m_Counter = new std::atomic<T_CounterType>(1);
            m_Counter = new T_CounterType(1);
          } else {
            m_Counter = NULL;
          }
        }

        void transferFrom(
          T_Type                         **dst,
          const TransferRefCount<T_Type,T_CounterType,T_CounterMaxValue>  *policy_src,
          T_Type                          *src)
        {
          *dst = src;
          if (policy_src->m_Counter == NULL) {
            sl_assert(src == NULL);
          }
          m_Counter = policy_src->m_Counter;
          if (m_Counter != NULL) {
            if ((*m_Counter) == T_CounterMaxValue) {
              throw LibSL::Errors::Fatal("TransferRefCount - instance counter reached its maximum value!");
            }
            // increment the counter
            (*m_Counter)++;
          }
        }

        template <typename T_Type2>
        void transferCastFrom(
          T_Type                         **dst,
          const TransferRefCount<T_Type2,T_CounterType,T_CounterMaxValue> *policy_src,
          T_Type2                         *src)
        {
          *dst = dynamic_cast<T_Type*>(src); // NOTE: forced to use dynamic_cast in case of virtual
                                             // inheritance. Possibly add a policy for this.
          if (policy_src->m_Counter == NULL) {
            sl_assert(src == NULL);
          }
          m_Counter = policy_src->m_Counter;
          if (m_Counter != NULL) {
            if ((*m_Counter) == T_CounterMaxValue) {
              throw LibSL::Errors::Fatal("TransferRefCount - instance counter reached its maximum value!");
            }
            // increment the counter
            (*m_Counter)++;
          }
        }

        void release(T_Type **ptr)
        {
          if (m_Counter == NULL) {
            //std::cerr << "RELEASE, m_Counter == NULL, *ptr = " << uint(*ptr) << std::endl;
            sl_assert((*ptr) == NULL);
          } else {
            // decrement the counter
            (*m_Counter)--;
            // null test
            if ((*m_Counter) == 0) {
              delete (m_Counter);
              m_Counter = NULL;
              // std::cerr << "RELEASE " << uint(*ptr) << std::endl;
              sl_assert(  ptr  != NULL );
              sl_assert((*ptr) != NULL );
              delete (*ptr);
              (*ptr)    = NULL;
            }
          }
        }

        uint refCount() const {if (m_Counter==NULL) return (0); else return (*m_Counter);}

        /*
        void debug()
        {
        std::cerr
        << " counter = " << uint(m_Counter)
        << '(' << (m_Counter!=NULL?(*m_Counter):-1) << ')'
        << std::endl;
        }
        */
      };

      template <typename T_Type>
      class TransferRefCountUInt : public TransferRefCount<T_Type,uint,(1u<<31)>
      {
      public:
        TransferRefCountUInt()            : TransferRefCount<T_Type,uint,(1u<<31)>() {}
        TransferRefCountUInt(T_Type *ptr) : TransferRefCount<T_Type,uint,(1u<<31)>(ptr) {}
      };

      template <typename T_Type>
      class TransferRefCountUShort : public TransferRefCount<T_Type,ushort,65535>
      {
      public:
        TransferRefCountUShort()            : TransferRefCount<T_Type,ushort,65535>() {}
        TransferRefCountUShort(T_Type *ptr) : TransferRefCount<T_Type,ushort,65535>(ptr) {}
      };

      template <typename T_Type>
      class TransferRefCountUChar : public TransferRefCount<T_Type,uchar,255>
      {
      public:
        TransferRefCountUChar()            : TransferRefCount<T_Type,uchar,255>() {}
        TransferRefCountUChar(T_Type *ptr) : TransferRefCount<T_Type,uchar,255>(ptr) {}
      };

      /*!

      Pointer class configurable with policies defined in LibSL::Memory::Pointer

      */
      template <
        typename T_Type,
        class    P_Check,
        template <typename> class P_Transfer>
      class Pointer : public P_Transfer<T_Type>
      {

      public:

        typedef T_Type  t_Type;
        typedef T_Type* t_RawPointer;
        typedef T_Type& t_Reference;

        typedef P_Transfer<T_Type> t_Transfer;

      private:

        t_RawPointer m_RawPointer;

      protected:

        // specialized classes will define the behaviour of the constructor from raw pointer
        Pointer(const t_RawPointer& raw) : t_Transfer(raw)
        {
          m_RawPointer = raw;
        }

      public:

        Pointer() : t_Transfer()
        {
          m_RawPointer = NULL;
        }

        Pointer(const Pointer& ptr) : t_Transfer()
        {
          m_RawPointer = NULL;
          t_Transfer::transferFrom(
            &m_RawPointer,
            static_cast<const t_Transfer *>(&ptr),
            ptr.m_RawPointer);
        }

        // copy between different pointers of compatible types
        template <typename T_Type2,class P_Check2>
        explicit Pointer(const Pointer<T_Type2,P_Check2,P_Transfer>& ptr) : t_Transfer()
        {
          m_RawPointer = NULL;
          t_Transfer::transferCastFrom(
            &m_RawPointer,
            static_cast<const P_Transfer<T_Type2> *>(&ptr),
            ptr.raw());
        }

        ~Pointer()
        {
          /*std::cerr << "~Pointer\n";*/
          t_Transfer::release(&m_RawPointer);
        }

        const Pointer& operator = (const Pointer& ptr)
        {
          t_Transfer::release(&m_RawPointer);
          t_Transfer::transferFrom(
            &m_RawPointer,
            static_cast<const t_Transfer *>(&ptr),
            ptr.m_RawPointer);
          return (*this);
        }

        t_RawPointer operator->()        { P_Check::checkValid(m_RawPointer); return (m_RawPointer); }
        t_RawPointer operator->()  const { P_Check::checkValid(m_RawPointer); return (m_RawPointer); }

        t_Reference  operator*()         { P_Check::checkValid(m_RawPointer); return (*m_RawPointer); }
        t_Reference  operator*()   const { P_Check::checkValid(m_RawPointer); return (*m_RawPointer); }

        t_Reference  operator<(const Pointer& ptr)  const { return m_RawPointer < ptr.m_RawPointer; }
        t_Reference  operator==(const Pointer& ptr) const { return m_RawPointer == ptr.m_RawPointer; }

        // operator t_RawPointer()            { return (m_RawPointer); }
        // operator T_Type * const ()   const { return (m_RawPointer); }

        t_RawPointer raw()               { return (m_RawPointer); }
        t_RawPointer raw()         const { return (m_RawPointer); }

        bool operator!()         const  { return (m_RawPointer == 0); }
        bool isNull()            const  { return (m_RawPointer == 0); }

        /*
        void debug() { std::cerr
        << uint(this)
        << " ptr = " << uint(m_RawPointer)
        << " "; t_Transfer::debug();
        std::cerr << std::endl; }
        */
      };

      #ifdef WIN32 // This is due to an incompatibility btw Visual C and g++ // TODO FIXME

      /*!

      AutoPtr build from Pointer with CheckValid and TransferRefCount policies

      */


      template <typename T_Type>
      class AutoPtr : public Pointer<T_Type,CheckValid,TransferRefCountUInt>
      {
      public:

        AutoPtr() : Pointer() { }

#ifdef LIBSL_IDKFK // power user only
        AutoPtr(const typename Pointer::t_RawPointer& raw) : Pointer(raw)  { }
#else
        explicit AutoPtr(const typename Pointer::t_RawPointer& raw) : Pointer(raw)  { }
#endif

        AutoPtr(const AutoPtr& ptr) : Pointer(ptr)  { }

        template <typename T_Type2>
        explicit AutoPtr(const AutoPtr<T_Type2>& ptr) : Pointer(ptr) { }

        ~AutoPtr() {  }
      };

      /*!

      SafePtr build from Pointer with CheckValid and TransferAddress policies

      */

      template <class T_Type>
      class SafePtr : public Pointer<T_Type,CheckValid,TransferAddress>
      {
      public:

        SafePtr() : Pointer() { }

        SafePtr(const typename Pointer::t_RawPointer& raw) : Pointer(raw) { }

        SafePtr(const SafePtr& ptr) : Pointer(ptr) { }

        ~SafePtr() { /*std::cerr << "~SafePtr\n";*/ }

        void erase() { if (!Pointer::isNull()) { delete (Pointer::raw()); (*this) = NULL; } }

        operator t_RawPointer()            { return (raw()); }
        operator T_Type * const ()   const { return (raw()); }
      };

#else // g++

      /*!

      AutoPtr build from Pointer with CheckValid and TransferRefCount policies

      */

      template <class T_Type>
      class AutoPtr : public Pointer<T_Type,CheckValid,TransferRefCountUInt>
      {
      private:

        typedef Pointer<T_Type,CheckValid,TransferRefCountUInt> t_Parent;

      public:

        AutoPtr() : t_Parent() { }

#ifdef LIBSL_IDKFK // power user only
        AutoPtr(const typename t_Parent::t_RawPointer& raw) : t_Parent(raw) { }
#else
        explicit AutoPtr(const typename t_Parent::t_RawPointer& raw) : t_Parent(raw) { }
#endif

        AutoPtr(const AutoPtr& ptr) : t_Parent(ptr) { }

        template <typename T_Type2>
        explicit AutoPtr(const AutoPtr<T_Type2>& ptr) : t_Parent(ptr) { }

        ~AutoPtr() { /*std::cerr << "~AutoPtr\n";*/ }
      };

      /*!

      SafePtr build from Pointer with CheckValid and TransferAddress policies

      */

      template <class T_Type>
      class SafePtr : public Pointer<T_Type,CheckValid,TransferAddress>
      {
      private:

        typedef Pointer<T_Type,CheckValid,TransferAddress> t_Parent;

      public:


        SafePtr() : t_Parent() { }

        SafePtr(const typename t_Parent::t_RawPointer& raw) : t_Parent(raw) { }

        SafePtr(const SafePtr& ptr) : t_Parent(ptr) { }

        ~SafePtr() { /*std::cerr << "~SafePtr\n";*/ }

        void erase() { if (!Pointer<T_Type,CheckValid,TransferAddress>::isNull()) { delete (Pointer<T_Type,CheckValid,TransferAddress>::raw()); (*this) = NULL; } }
      };

#endif

    } // namespace LibSL::Memory::Pointer
  } // namespace LibSL::Memory
} // namespace LibSL

// ----------------------------------------------------

