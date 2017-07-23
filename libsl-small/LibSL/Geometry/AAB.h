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
// LibSL::Geometry::AAB
// ------------------------------------------------------
//
// Axis Aligned Box class
//
// ------------------------------------------------------
// Sylvain Lefebvre - 2006-11-15
// ------------------------------------------------------

#pragma once

#include <LibSL/Errors/Errors.h>
#include <LibSL/CppHelpers/CppHelpers.h>
#include <LibSL/Math/Tuple.h>
#include <LibSL/Math/Math.h>
//#include <LibSL/Geometry/Plane.h>
//#include <LibSL/Math/Matrix4x4.h>

#include <limits>

#undef max
#undef min

namespace LibSL {
  namespace Geometry {

    /*!

    \class AAB
    \brief Axis Aligned Box

    */
    template <uint T_NumDim,typename T_Type = float>
    class AAB
    {
    public:

      enum {e_NumDim      = T_NumDim};

      typedef LibSL::Math::Tuple<T_Type, T_NumDim> t_Vertex;

      t_Vertex   m_Mins;
      t_Vertex   m_Maxs;

    public:

      //! Build an empty AAB
      AAB()
      {
        m_Mins = std::numeric_limits<T_Type>::max();
        m_Maxs = -std::numeric_limits<T_Type>::max();
      }

      AAB(const AAB& aab)
      {
        m_Mins = aab.m_Mins;
        m_Maxs = aab.m_Maxs;
      }

      AAB(const t_Vertex& mins,const t_Vertex& maxs)
      {
        sl_assert(mins.allLteq(maxs));
        m_Mins = mins;
        m_Maxs = maxs;
      }

      //! Add a point to the AAB
      void addPoint(const t_Vertex& p)
      {
        m_Mins = tupleMin(m_Mins,p);
        m_Maxs = tupleMax(m_Maxs,p);
      }

      //! Merge with another AAB
      void mergeWith(const AAB& aab)
      {
        m_Mins = tupleMin(m_Mins,aab.m_Mins);
        m_Maxs = tupleMax(m_Maxs,aab.m_Maxs);
      }

      //! Intersect with another AAB
      void intersectWith(const AAB& aab)
      {
        if (intersect(aab)) {
          m_Mins = tupleMax(m_Mins, aab.m_Mins);
          m_Maxs = tupleMin(m_Maxs, aab.m_Maxs);
        } else {
          m_Mins = std::numeric_limits<T_Type>::max();
          m_Maxs = std::numeric_limits<T_Type>::min();
        }
      }

      //! Test intersection between AAB
      //  (inclusion is an intersection,
      //   contact is an intersection)
      bool intersect(const AAB& aab) const
      {
        if (aab.m_Mins.oneGt(m_Maxs)) {
          return (false);
        } else {
          if (aab.m_Maxs.oneLt(m_Mins)) {
            return (false);
          } else {
            return (true);
          }
        }
      }

      //! Test whether box is completly inside aab
      //! (contact is not inside)
      bool inside(const AAB& aab) const
      {
        if (m_Mins.allGt(aab.m_Mins) && m_Maxs.allLt(aab.m_Maxs)) {
          return (true);
        } else {
          return (false);
        }
      }

      //! Test whether the box encloses a point
      //! (contact with sides is treated asymetrically to allow for tiling)
      bool enclose(const t_Vertex& p) const
      {
        if (p.allGteq(m_Mins) && p.allLt(m_Maxs)) {
          return (true);
        } else {
          return (false);
        }
      }

      //! Test whether the box contains a point
      //! (contains is not asymetric, contrary to enclose)
      bool contain(const t_Vertex& p) const
      {
        if (p.allGteq(m_Mins) && p.allLteq(m_Maxs)) {
          return (true);
        } else {
          return (false);
        }
      }

      //! Create a new box enlarged with a margin of 'size'
      AAB enlarge(const t_Vertex& size) const
      {
        AAB box;
        box.minCorner() = m_Mins - size;
        box.maxCorner() = m_Maxs + size;
        return (box);
      }

      //! Create a new box enlarged with a margin of 'size' on the min side
      AAB enlargeOneSide(const t_Vertex& size)
      {
        AAB box;
        box.minCorner() = m_Mins - size;
        box.maxCorner() = m_Maxs;
        return (box);
      }

      //! Is bounding box empty?
      bool empty() const
      {
        ForIndex(d, T_NumDim) {
          if (m_Maxs[d] <= m_Mins[d]) return true;
        }
        return false;
      }

      //! Min corner
      const t_Vertex& minCorner() const {return (m_Mins);}
      t_Vertex&       minCorner()       {return (m_Mins);}

      //! Max corner
      const t_Vertex& maxCorner() const {return (m_Maxs);}
      t_Vertex&       maxCorner()       {return (m_Maxs);}

      //! Center
      t_Vertex        center()    const {return T_Type(0.5f)*(m_Mins + m_Maxs);}

      //! Extent
      t_Vertex        extent()    const {return (m_Maxs - m_Mins);}

      //! Volume
      T_Type          volume()
      {
        t_Vertex ex = this->extent();
        return ex[0] * ex[1] * ex[2];
      }

    };

    typedef AAB<2> AASquare;
    typedef AAB<3> AABox;

    //! Combine two another AABs
    template <uint T_NumDim, typename T_Type>
    AAB<T_NumDim, T_Type> combine(const AAB<T_NumDim, T_Type>& aab0, const AAB<T_NumDim, T_Type>& aab1)
    {
      AAB<T_NumDim, T_Type> r;
      r = aab0;
      r.mergeWith(aab1);
      return r;
    }


  } //namespace LibSL::Geometry
} //namespace LibSL

// ------------------------------------------------------
