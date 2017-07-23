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
// Vertex class
// 
// ------------------------------------------------------
// Sylvain Lefebvre - 2006-04-28
// ------------------------------------------------------

#pragma once

#include <LibSL/LibSL.common.h>

#include <LibSL/Math/Tuple.h>
#include <vector>

namespace LibSL {
  namespace Math {

    typedef Tuple<half,  2> v2h;
    typedef Tuple<float, 2> v2f;
    typedef Tuple<double,2> v2d;
    typedef Tuple<int   ,2> v2i;
    typedef Tuple<uint  ,2> v2u;
    typedef Tuple<char  ,2> v2c;
    typedef Tuple<uchar ,2> v2b;
    typedef Tuple<short ,2> v2s;

    typedef Tuple<half,  3> v3h;
    typedef Tuple<float, 3> v3f;
    typedef Tuple<double,3> v3d;
    typedef Tuple<int   ,3> v3i;
    typedef Tuple<uint  ,3> v3u;
    typedef Tuple<char  ,3> v3c;
    typedef Tuple<uchar ,3> v3b;
    typedef Tuple<short ,3> v3s;
    
    typedef Tuple<half,  4> v4h;
    typedef Tuple<float, 4> v4f;
    typedef Tuple<double,4> v4d;
    typedef Tuple<int   ,4> v4i;
    typedef Tuple<uint  ,4> v4u;
    typedef Tuple<char  ,4> v4c;
    typedef Tuple<uchar ,4> v4b;
    typedef Tuple<short ,4> v4s;

    //! constructors

    LIBSL_DLL v2h V2H(half x,half y);
    LIBSL_DLL v2f V2F(float x,float y);
    LIBSL_DLL v2d V2D(double x,double y);
    LIBSL_DLL v2i V2I(int x,int y);
    LIBSL_DLL v2u V2U(uint x,uint y);
    LIBSL_DLL v2c V2C(char x,char y);
    LIBSL_DLL v2b V2B(uchar x,uchar y);
    LIBSL_DLL v2s V2S(short x,short y);

    LIBSL_DLL v3h V3H(half x,half y,half z);
    LIBSL_DLL v3f V3F(float x,float y,float z);
    LIBSL_DLL v3d V3D(double x,double y,double z);
    LIBSL_DLL v3i V3I(int x,int y,int z);
    LIBSL_DLL v3u V3U(uint x,uint y,uint z);
    LIBSL_DLL v3c V3C(char x,char y,char z);
    LIBSL_DLL v3b V3B(uchar x,uchar y,uchar z);
    LIBSL_DLL v3s V3S(short x,short y,short z);

    LIBSL_DLL v4h V4H(half x,half y,half z,half w);
    LIBSL_DLL v4f V4F(float x,float y,float z,float w);
    LIBSL_DLL v4d V4D(double x,double y,double z,double w);
    LIBSL_DLL v4i V4I(int x,int y,int z,int w);
    LIBSL_DLL v4u V4U(uint x,uint y,uint z,uint w);
    LIBSL_DLL v4c V4C(char x,char y,char z,char w);
    LIBSL_DLL v4b V4B(uchar x,uchar y,uchar z,uchar w);
    LIBSL_DLL v4s V4S(short x,short y,short z,short w);

    //! vertex operators

    template<class T_Type> Tuple<T_Type,3> cross
      (const Tuple<T_Type,3>& t0,const Tuple<T_Type,3>& t1)
    {
      Tuple<T_Type,3> r;
      r[0]=t0[1]*t1[2] - t0[2]*t1[1];
      r[1]=t0[2]*t1[0] - t0[0]*t1[2];
      r[2]=t0[0]*t1[1] - t0[1]*t1[0];
      return (r);
    } 

    template<class T_Type> std::pair<Tuple<T_Type,3>,Tuple<T_Type,3> > frame
      (const Tuple<T_Type,3>& n)
    {
      Tuple<T_Type,3> u,v;
      u[0] = -n[1];
      u[1] =  n[0];
      u[2] =  0;
      if (sqLength(u) < 1e-5) {
        u[0] =  n[2];
        u[1] =  0;
        u[2] = -n[0];
        if (sqLength(u) < 1e-5) {
          throw LibSL::Errors::Fatal("LibSL::Math::Vector::frame - cannot create frame!");
        }
      }
      u = normalize(u);
      v = cross(n,u);
      v = normalize(v);
      return (std::make_pair(u,v));
    } 

  } // namespace LibSL::Math
} // namespace LibSL
