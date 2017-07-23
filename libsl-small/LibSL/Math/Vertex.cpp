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
#include "LibSL.precompiled.h"
// ------------------------------------------------------

#include "Vertex.h"
using namespace LibSL::Math;

// ------------------------------------------------------

#define NAMESPACE LibSL::Math

// ------------------------------------------------------

v2h NAMESPACE::V2H(half x,half y)
{v2h v; v[0]=x; v[1]=y; return (v);}

v2f NAMESPACE::V2F(float x,float y)           
{v2f v; v[0]=x; v[1]=y; return (v);}

v2d NAMESPACE::V2D(double x,double y)
{v2d v; v[0]=x; v[1]=y; return (v);}

v2i NAMESPACE::V2I(int x,int y)
{v2i v; v[0]=x; v[1]=y; return (v);}

v2u NAMESPACE::V2U(uint x,uint y)
{v2u v; v[0]=x; v[1]=y; return (v);}

v2c NAMESPACE::V2C(char x,char y)
{v2c v; v[0]=x; v[1]=y; return (v);}

v2b NAMESPACE::V2B(uchar x,uchar y)
{v2b v; v[0]=x; v[1]=y; return (v);}

v2s NAMESPACE::V2S(short x,short y)
{v2s v; v[0]=x; v[1]=y; return (v);}


v3h NAMESPACE::V3H(half x,half y,half z)
{v3h v; v[0]=x; v[1]=y; v[2]=z; return (v);}

v3f NAMESPACE::V3F(float x,float y,float z)
{v3f v; v[0]=x; v[1]=y; v[2]=z; return (v);}

v3d NAMESPACE::V3D(double x,double y,double z)
{v3d v; v[0]=x; v[1]=y; v[2]=z; return (v);}

v3i NAMESPACE::V3I(int x,int y,int z)
{v3i v; v[0]=x; v[1]=y; v[2]=z; return (v);}

v3u NAMESPACE::V3U(uint x,uint y,uint z)
{v3u v; v[0]=x; v[1]=y; v[2]=z; return (v);}

v3c NAMESPACE::V3C(char x,char y,char z)
{v3c v; v[0]=x; v[1]=y; v[2]=z; return (v);}

v3b NAMESPACE::V3B(uchar x,uchar y,uchar z)
{v3b v; v[0]=x; v[1]=y; v[2]=z; return (v);}

v3s NAMESPACE::V3S(short x,short y,short z)
{v3s v; v[0]=x; v[1]=y; v[2]=z; return (v);}


v4h NAMESPACE::V4H(half x,half y,half z,half w)
{v4h v; v[0]=x; v[1]=y; v[2]=z; v[3]=w; return (v);}

v4f NAMESPACE::V4F(float x,float y,float z,float w)
{v4f v; v[0]=x; v[1]=y; v[2]=z; v[3]=w; return (v);}

v4d NAMESPACE::V4D(double x,double y,double z,double w)
{v4d v; v[0]=x; v[1]=y; v[2]=z; v[3]=w; return (v);}

v4i NAMESPACE::V4I(int x,int y,int z,int w)
{v4i v; v[0]=x; v[1]=y; v[2]=z; v[3]=w; return (v);}

v4u NAMESPACE::V4U(uint x,uint y,uint z,uint w)
{v4u v; v[0]=x; v[1]=y; v[2]=z; v[3]=w; return (v);}

v4c NAMESPACE::V4C(char x,char y,char z,char w)
{v4c v; v[0]=x; v[1]=y; v[2]=z; v[3]=w; return (v);}

v4b NAMESPACE::V4B(uchar x,uchar y,uchar z,uchar w)
{v4b v; v[0]=x; v[1]=y; v[2]=z; v[3]=w; return (v);}

v4s NAMESPACE::V4S(short x,short y,short z,short w)
{v4s v; v[0]=x; v[1]=y; v[2]=z; v[3]=w; return (v);}

// ------------------------------------------------------
