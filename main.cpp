// --------------------------------------------------------------
// Implementation in C++ of 3D model synthesis / wave function collapse
// Simple, quite efficient and easily hackable
//
// For detailed usage information see README.md
// - the expected voxel format is '.slab.vox' as exported by MagicaVoxel
// - the output can be directly imported into MagicaVoxel
//   (use MagicaVoxel viewer for larger outputs, as MagicaVoxel clamps to 128^3)
// - palette indices are used as tile ids (labels).
// - palette index 255 is empty, 254 is ground.
// - input files are in subdir exemplars/
// - output is produced in subdir results/
//    results/synthesized.slab.vox is the synthesized labeling
//    results/synthesized_detailed.slab.vox is the output using detailed tiles
//
// For more details on model synthesis:
// - http://graphics.stanford.edu/~pmerrell/
// - https://github.com/mxgmn/WaveFunctionCollapse
// 
// The goal is to keep it short, efficient, and (relatively) clear.
// Shamelessly uses globals.
//
// Enjoy!
//
// Limitations:
// - all labels are currently equiprobable (will be updated soon)
//
// Sylvain Lefebvre @sylefeb
// --------------------------------------------------------------
/*
MIT License
https://opensource.org/licenses/MIT

Copyright 2017, Sylvain Lefebvre

Permission is hereby granted, free of charge, to any person obtaining a copy 
of this software and associated documentation files(the "Software"), to deal 
in the Software without restriction, including without limitation the rights 
to use, copy, modify, merge, publish, distribute, sub-license, and / or sell 
copies of the Software, and to permit persons to whom the Software is furnished 
to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in 
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE 
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN 
THE SOFTWARE.
*/
// --------------------------------------------------------------

#include <LibSL/LibSL.h>

#include <iostream>
#include <ctime>
#include <cmath>
#include <set>
#include <map>
#include <queue>
#include <limits>
#include <cstring>

// --------------------------------------------------------------

using namespace std;

// --------------------------------------------------------------

// volume size to synthesize (sz^3)
const int   sz = 16;

// name of the problem (files in subdirectory exemplars/)
string problem = "towers";
// string problem = "simple";
// string problem = "flat";
// string problem = "blog6";

// name of the tilemap (files in subdirectory exemplars/)
string tilemap = "castle";
// string tilemap = ""; // none

// synthesize a periodic structure? (only makes sense if not using borders!)
const bool  periodic = false;

// --------------------------------------------------------------

// number of labels in problem
int            num_lbls;

// bits to describe axial directions
const uchar    axis_x = 1;
const uchar    axis_y = 2;
const uchar    axis_z = 4;

// for navigating neighbors
const v3i      neighs[6] = { v3i(-1, 0, 0), v3i(1, 0, 0), v3i(0, -1, 0), v3i(0, 1, 0), v3i(0, 0, -1), v3i(0, 0, 1) };
const bool     side[6] = { true, false, true, false, true, false };
const uchar    face[6] = { axis_x, axis_x, axis_y, axis_y, axis_z, axis_z };
const int      n_left = 0;
const int      n_right = 1;
const int      n_top = 2;
const int      n_bottom = 3;
const int      n_below = 4;
const int      n_above = 5;

// constraint bit-field for each label pairs
// (e.g. constraints.at(2,5) = axis_x means label 2 can have 5 on its right)
Array2D<uchar> constraints;
// same as above under a different form allowing for faster checks
// (build from 'constraints' by prepareFastConstraintChecks)
Array< Array<vector<int> > > allowed_by_side;

// information from loaded voxel problem
Array<v3b>               palette; // RGB palette
map<uchar, int>          pal2id;  // palette index to label id
map<int, uchar>          id2pal;  // label id to palette index

/* -------------------------------------------------------- */

// This prepares the small data structure 'allowed_by_side' from 'constraints'
// to allow for a faster check in 'updateConstraintsAtSite'
void prepareFastConstraintChecks()
{
  allowed_by_side.allocate(6);
  ForIndex(n, 6) {
    allowed_by_side[n].allocate(num_lbls);
    ForIndex(l1, num_lbls) {
      bool allowed = false;
      ForIndex(l2, num_lbls) {
        int a = l1; int b = l2;
        if (side[n]) { std::swap(a, b); }
        bool can_be_side_by_side = (constraints.at(a, b) & face[n]);
        if (can_be_side_by_side) {
          allowed_by_side[n][l1].push_back(l2);
        }
      }
    }
  }
}

// --------------------------------------------------------------

// Returns the opposite neighbor, used in 'updateConstraintsAtSite'
inline int oppositeNeighbor(int n)
{
  switch (n)
  {
  case n_left:   return n_right; break;
  case n_right:  return n_left; break;
  case n_top:    return n_bottom;  break;
  case n_bottom: return n_top; break;
  case n_below:  return n_above;  break;
  case n_above:  return n_below; break;
  }
  return -1;
}

// --------------------------------------------------------------

// Tiny class to hold a vector of bools representing choices at a site (voxel)
// encoded as a bit field (an unsigned int holds 32 bits for the first 32 labels)

class Presence
{
private:
  static const int c_MaxLabelFields = 2;        // enough to hold 64 labels, increase if needed
  static const int s_PowNumBits = 5;            // sizeof(uint) * 8 = 32 = 2^5
  static const int s_ModNumBits = (1 << 5) - 1; // 31
  uint m_Values[c_MaxLabelFields];
public:
  Presence() { }
  Presence& operator = (const Presence& p) { memcpy(m_Values, p.m_Values, c_MaxLabelFields * sizeof(uint)); return *this; }
  const bool operator[](int n) const   { return (m_Values[n >> s_PowNumBits] >> (n & s_ModNumBits)) & 1; }
  void       set(int n, bool b) { 
    if (b) { m_Values[n >> s_PowNumBits] |=   1 << (n & s_ModNumBits); } 
    else   { m_Values[n >> s_PowNumBits] &= ~(1 << (n & s_ModNumBits)); } 
  }
  void fill(bool b) { memset(m_Values, b ? 0xFF : 0x00, c_MaxLabelFields * sizeof(uint)); }
};

// --------------------------------------------------------------
// Simple helper functions to manipulate Presence vectors
//
// Note the use of the global 'num_lbls'. Yes, not elegant, but avoids 
// storing the size (which remains the same after loading a given problem)
// in the Presence class.

inline bool isFalse(const Presence& a)
{
  ForIndex(i, num_lbls) { if (a[i] > 0.0f) return false; }
  return true;
}

inline void orEq(Presence& a, const Presence& b)
{
  ForIndex(i, num_lbls) { a.set(i, a[i] || b[i]); }
}

inline void andEq(Presence& a, const Presence& b)
{
  ForIndex(i, num_lbls) { a.set(i, a[i] && b[i]); }
}

/* -------------------------------------------------------- */

// Updates the set of possible labels at a given site (voxel i,j,k), considering the n-th neighbor.
// Returns whether something changed, and whether all labels disappeared due to over-constraints (failed).
// This is a local update used in the global 'propagateConstraints' function below.
void updateConstraintsAtSite(int i, int j, int k, int n, Array3D<Presence>& _S, bool& _changed, bool& _failed)
{
  if (!periodic) {
    if ((i + neighs[n][0] < 0 || i + neighs[n][0] >= _S.xsize())
      || (j + neighs[n][1] < 0 || j + neighs[n][1] >= _S.ysize())
      || (k + neighs[n][2] < 0 || k + neighs[n][2] >= _S.zsize())) {
      // out of domain, nothing changes
      _changed = false;
      _failed = false;
      return;
    }
  }

  _changed = false;
  const Presence& from_neigh = _S.at<Wrap>(i + neighs[n][0], j + neighs[n][1], k + neighs[n][2]);
  ForIndex(l1, num_lbls) {
    if (_S.at(i, j, k)[l1]) {
      bool allowed = false;
      int num = allowed_by_side[n][l1].size();
      ForIndex(t, num) {
        int l2 = allowed_by_side[n][l1][t];
        allowed = allowed || from_neigh[l2];
      }
      if (!allowed) {
        _changed = true;
        _S.at(i, j, k).set(l1,false);
      }
    }
  }

  // is the selection empty?
  if (isFalse(_S.at(i,j,k))) { // yes ...
    _failed = true;
  } else {
    _failed = false;
  }  
}

/* -------------------------------------------------------- */

// Propagates the constraints: this is the major ingredient of model synthesis.
// Initially all labels are present (possible). When some labels are discarded,
// some choices are no longer possible in the neighbors due to the constraints. 
// This function will propagate the change throughout the entire domain.
bool propagateConstraints(int i, int j, int k, Array3D<Presence>& _S)
{
  std::queue<v3i> q;
  q.push(v3i(i, j, k));
  while (!q.empty()) {
    v3i cur = q.front();
    q.pop();
    // update neighbors
    ForIndex(n, 6) {
      v3i ne = v3i(cur[0] + neighs[n][0], cur[1] + neighs[n][1], cur[2] + neighs[n][2]);
      ne[0] = (ne[0] + _S.xsize()) % _S.xsize();
      ne[1] = (ne[1] + _S.ysize()) % _S.ysize();
      ne[2] = (ne[2] + _S.zsize()) % _S.zsize();
      bool changed;
      bool failed;
      updateConstraintsAtSite(ne[0], ne[1], ne[2], oppositeNeighbor(n), _S, changed, failed);
      if (changed) {
        q.push(ne); // changed: add to sites to process
      }
      if (failed) {
        return false; // constraints disagree, fail
      }
    }
  }
  return true;
}

/* -------------------------------------------------------- */

// Initializes the domain with a 'soup' where all labels are possible.
// If lbl_empty is given, an empty border is initialized all around the domain.
bool init_global_soup(Array3D<Presence>& S,int lbl_empty = -1)
{
  // init: global, uniform soup
  ForArray3D(S, i, j, k) {
    S.at(i, j, k).fill(true);
  }
  bool ok = true;
  if (lbl_empty > -1) {
    // border
    ForIndex(k, S.zsize()) {
      ForIndex(i, S.xsize()) {
        S.at(i, 0, k).fill(false);
        S.at(i, 0, k).set(lbl_empty,true);
        S.at(i, S.ysize() - 1, k).fill(false);
        S.at(i, S.ysize() - 1, k).set(lbl_empty,true);
        ok &= propagateConstraints(i, 0, k, S);
        ok &= propagateConstraints(i, S.ysize() - 1, k, S);
      }
      ForIndex(j, S.ysize()) {
        S.at(0, j, k).fill(false);
        S.at(0, j, k).set(lbl_empty,true);
        S.at(S.xsize() - 1, j, k).fill(false);
        S.at(S.xsize() - 1, j, k).set(lbl_empty,true);
        ok &= propagateConstraints(0, j, k, S);
        ok &= propagateConstraints(S.xsize() - 1, j, k, S);
      }
    }
    ForIndex(j, S.xsize()) {
      ForIndex(i, S.xsize()) {
        S.at(i, j, 0).fill(false);
        S.at(i, j, 0).set(lbl_empty,true);
        S.at(i, j, S.zsize() - 1).fill(false);
        S.at(i, j, S.zsize() - 1).set(lbl_empty,true);
        ok &= propagateConstraints(i, j, 0, S);
        ok &= propagateConstraints(i, j, S.zsize() - 1, S);
      }
    }
  }
  return ok; // could fail due to propagation
}

/* -------------------------------------------------------- */

// Initializes the domain with an empty assignment.
// If lbl_ground is given, a ground is created on z == 0
bool init_global_empty(Array3D<Presence>& S, int lbl_empty,int lbl_ground=-1)
{
  if (lbl_ground < 0) lbl_ground = lbl_empty;
  ForArray3D(S, i, j, k) {
    S.at(i, j, k).fill(false);
    if (k > 0) {
      S.at(i, j, k).set(lbl_empty,true);
    } else {
      S.at(i, j, k).set(lbl_ground,true);
    }
  }
  return true;
}

/* -------------------------------------------------------- */

// Resets a sub-domain with an empty soup. The border is preserved and
// constraints are propagated inside.
// Returns true on success, false otherwise (i.e. constraints cannot be resolved).
// The domain is changed, even on failure. Caller is responsible for restoring it.
bool reinit_sub(Array3D<Presence>& S, int lbl_empty, AAB<3, int> sub)
{
  // init: reset subset, propagate constraints from borders
  v3i cri = sub.minCorner();
  v3i cra = sub.maxCorner();
  ForRange(k, cri[2] + 1, cra[2] - 1) {
    ForRange(j, cri[1] + 1, cra[1] - 1) {
      ForRange(i, cri[0] + 1, cra[0] - 1) {
        S.at(i, j, k).fill(true);
      }
    }
  }
  bool ok = true;
  ForRange(k, cri[2], cra[2]) {
    ForRange(i, cri[0], cra[0]) {
      ok &= propagateConstraints(i, cri[1], k, S);
      ok &= propagateConstraints(i, cra[1], k, S);
    }
    ForRange(j, cri[1], cra[1]) {
      ok &= propagateConstraints(cri[0], j, k, S);
      ok &= propagateConstraints(cra[0], j, k, S);
    }
  }
  ForRange(j, cri[1], cra[1]) {
    ForRange(i, cri[0], cra[0]) {
      ok &= propagateConstraints(i, j, cri[2], S);
      ok &= propagateConstraints(i, j, cra[2], S);
    }
  }
  return ok; // failed due to propagation
}

/* -------------------------------------------------------- */

// Counts the number of non empty labels in a sub domain (ignoring border)
int num_solids_sub(Array3D<Presence>& S, int lbl_empty, AAB<3, int> sub)
{
  int num = 0;
  v3i cri = sub.minCorner();
  v3i cra = sub.maxCorner();
  ForRange(k, cri[2] + 1, cra[2] - 1) {
    ForRange(j, cri[1] + 1, cra[1] - 1) {
      ForRange(i, cri[0] + 1, cra[0] - 1) {
        if (!S.at(i, j, k)[lbl_empty]) num++;
      }
    }
  }
  return num;
}

/* -------------------------------------------------------- */

// Main synthesis function
// Performs synthesis within the sub domain given as a box, or the full domain
// if no sub domain is specified.
// Returns true on success, false otherwise (i.e. constraints cannot be resolved).
// The domain is changed, even on failure. Caller is responsible for restoring it.
// After a success _num_solids contains the number of synthesized non empty labels.
bool synthesize(
  Array3D<Presence>& S,
  int lbl_empty, int& _num_solids,
  AAB<3, int> sub = AAB<3, int>())
{
  // buffer for choices
  int choices[1024];
  sl_assert(num_lbls < 1024);
  // box to operate upon
  AAB<3, int> box;
  if (!sub.empty()) {
    box = sub;
  } else {
    box.addPoint(v3i(0, 0, 0));
    box.addPoint(v3i(S.xsize() - 1, S.ysize() - 1, S.zsize() - 1));
  }

  // starting
  int num_choices = 0;
  _num_solids = 0;

  // randomize scanline order
  int order[] = { 0, 1, 2 };
  ForIndex(p, 9) {
    int a = rand() % 3;
    int b = rand() % 3;
    std::swap(order[a],order[b]);
  }
  v3i starts = box.minCorner();
  v3i ends   = box.maxCorner();
  int sign[] = { 1, 1, 1 };
  ForIndex(p, 3) {
    sign[p] = 1 - 2 * (rand() & 1);
  }
  ForIndex(p, 3) {
    if (sign[p] < 0) {
      std::swap(starts[p], ends[p]);
    }
    ends[p] += sign[p];
  }

  // propagate until done or conflict
  v3i cur = starts;
  bool failed = false;
  while (!failed) {

    cur[order[0]] += sign[order[0]];
    if (cur[order[0]] == ends[order[0]]) {
      cur[order[0]] = starts[order[0]];
      cur[order[1]] += sign[order[1]];
      if (cur[order[1]] == ends[order[1]]) {
        cur[order[1]] = starts[order[1]];
        cur[order[2]] += sign[order[2]];
        if (cur[order[2]] == ends[order[2]]) {
          break;
        }
      }
    }

    sl_assert(cur[0] > -1 && cur[0] < S.xsize());
    sl_assert(cur[1] > -1 && cur[1] < S.ysize());
    sl_assert(cur[2] > -1 && cur[2] < S.zsize());

    // which choices do we have here?
    num_choices = 0;
    ForIndex(l, num_lbls) {
      if (S.at(cur[0], cur[1], cur[2])[l]) {
        choices[num_choices++] = l;
      }
    }
    // failure?
    if (num_choices == 0) {
      failed = true;
    }
    // random choice
    int r = rand() % num_choices;
    int c = choices[r];
    S.at(cur[0], cur[1], cur[2]).fill(false);
    S.at(cur[0], cur[1], cur[2]).set(c,true);
    if (c != lbl_empty) {
      _num_solids ++;
    }

    // propagate this change
    bool ok = propagateConstraints(cur[0], cur[1], cur[2], S);
    if (!ok) {
      failed = true;
    }

  } // main update loop

  if (failed) {
    
    // giving up :-(
    return false;

  } else {

    // done!
    return true;
  }

}

/* -------------------------------------------------------- */

// Loads a voxel grid (.slab.vox format as exported by MagicaVoxel).
void loadFromVox(const char *fname,Array3D<uchar>& _voxels,Array<v3b>& _palette)
{
  FILE *f;
  f = fopen(fname, "rb");
  sl_assert(f != NULL);
  long sx, sy, sz;
  fread(&sx, 4, 1, f);
  fread(&sy, 4, 1, f);
  fread(&sz, 4, 1, f);
  _voxels.allocate(sx, sy, sz);
  ForIndex(i, sx) { ForIndex(j, sy) { ForIndex(k, sz) {
        fread(&_voxels.at(i, j, k), sizeof(uchar), 1, f);
  } } }
  _palette.allocate(256);
  fread(_palette.raw(), sizeof(v3b), 256, f);
  fclose(f);
}

/* -------------------------------------------------------- */

// Loads a 3D problem (.slab.vox format as exported by MagicaVoxel).
// Each voxel palette id becomes a label (renumbering is performed).
// When two voxels are neighboring in the exemplar, they are allowed 
// to appear together in the output. (What is observed is allowed,
// everything else is forbidden).
// See README.md for more details.
void load3DProblem(const char *fname)
{
  // read voxels
  Array3D<uchar> grid;
  loadFromVox(fname, grid, palette);
  // build label set
  set<uchar> labels;
  ForArray3D(grid, i, j, k) {
    uchar lbl = grid.at(i, j, k);
    labels.insert(lbl);
  }
  num_lbls = labels.size();
  int id = 0;
  for (uchar l : labels) {
    pal2id[l] = id;
    id2pal[id] = l;
    id++;
  }
  // now construct constraints
  constraints.allocate(num_lbls, num_lbls);
  constraints.fill(0);
  ForArray3D(grid, i, j, k) {
    int id = pal2id[grid.at(i,j,k)];
    ForIndex(n, 6) {
      int lbl = grid.at<Wrap>(i + neighs[n][0], j + neighs[n][1], k + neighs[n][2]);
      sl_assert(pal2id.find(lbl) != pal2id.end());
      int neigh_id = pal2id[lbl];
      if (side[n]) {
        constraints.at(id, neigh_id) |= face[n];
      } else {
        constraints.at(neigh_id, id) |= face[n];
      }
    }
  }
  // prepare table for faster constraint checks
  prepareFastConstraintChecks();
  // ready!
}

/* -------------------------------------------------------- */

// Saves a voxel file (.slab.vox format, can be imported by MagicaVoxel)
void saveAsVox(const char *fname,const Array3D<Presence>& S)
{
  FILE *f;
  f = fopen(fname, "wb");
  sl_assert(f != NULL);
  long sx = S.xsize(), sy = S.ysize(), sz = S.zsize();
  fwrite(&sx, 4, 1, f);
  fwrite(&sy, 4, 1, f);
  fwrite(&sz, 4, 1, f);
  ForIndex(i, sx) {
    ForIndex(j, sy) {
      ForRangeReverse(k, sz-1, 0) {
        int id = -1;
        ForIndex(l, num_lbls) {
          if (S.at(i, j, k)[l]) {
            id = l;
            break;
          }
        }
        sl_assert(id > -1);
        uchar pal = id2pal[id];
        fwrite(&pal, sizeof(uchar), 1, f);
      }
    }
  }
  fwrite(palette.raw(), sizeof(v3b), 256, f);
  fclose(f);
}

/* -------------------------------------------------------- */

// Saves a voxel file (.slab.vox format, can be imported by MagicaVoxel)
// This function takes as input a low res and high res tile map. The low res
// voxel grid locates detailed tiles in the high res grid. For instance,
// if palette index 128 appears at (1,2,3) in low res, and the tile size is
// 8x8x8, the detailed tile for palette index 128 is expected to be at 
// (8,16,24) in the high res voxel grid.
// It is expected the low res and high res grid sizes correspond exactly
// through the tile size. If the low res grid is WxHxD and the tile size 
// is 8x8x8 then the high res grid has to be 8Wx8Hx8D.
void saveAsVoxDetailed(
  const char *flow,
  const char *fdetailed,
  const char *fout,
  const Array3D<Presence>& S)
{
  uchar solid_color = 246; // from MagicaVoxel default palette
  // load high res voxels
  Array3D<uchar> highres;
  loadFromVox(fdetailed, highres, palette);
  // get corresponding low res voxels
  Array3D<uchar> lowres;
  loadFromVox(flow, lowres, palette);
  // find out detailed tiles
  map<uchar, v3i > pal2pos;
  int tx, ty, tz;
  // tile size
  tx = highres.xsize() / lowres.xsize();
  ty = highres.ysize() / lowres.ysize();
  tz = highres.zsize() / lowres.zsize();
  std::cerr << "Tile size: " << tx << ',' << ty << ',' << tz << std::endl;
  // find out detailed tiles: parse low res, check high res for details
  ForIndex(i, lowres.xsize()) { ForIndex(j, lowres.ysize()) { ForIndex(k, lowres.zsize()) {
    uchar pal = lowres.at(i, j, k);
    if (pal < 255) {
      // no detailed tile known?
      if (pal2pos.find(pal) == pal2pos.end()) {
        // check if details exists in high res voxels
        bool has_details = false;
        bool is_empty = true;
        ForIndex(tk, tz) { ForIndex(tj, ty) { ForIndex(ti, tx) {
              uchar v = highres.at(i * tx + tx - 1 - ti, j * ty + ty - 1 - tj, k * tz + tz - 1 - tk);
              if (v == 255) {
                has_details = true;
              } else {
                is_empty = false;
              }
        } } }
        if (has_details && !is_empty) {
          // ok!
          pal2pos[pal] = v3i(i, j, k);
        }
      } // not known
    } // lbl < 255
  } } }
  // output detailed version
  FILE *f = fopen(fout, "wb");
  sl_assert(f != NULL);
  long sx = tx*S.xsize(), sy = ty*S.ysize(), sz = tz*S.zsize();
  fwrite(&sx, 4, 1, f);
  fwrite(&sy, 4, 1, f);
  fwrite(&sz, 4, 1, f);
  // build high res grid
  Array3D<uchar> detailed;
  detailed.allocate(sx, sy, sz);
  detailed.fill(255);
  ForIndex(k, S.zsize()) { ForIndex(j, S.ysize()) { ForIndex(i, S.xsize()) {
    int id = -1;
    ForIndex(l, num_lbls) {
      if (S.at(i, j, k)[l]) {
        id = l;
        break;
      }
    }
    sl_assert(id > -1);
    uchar lbl = id2pal[id];
    if (lbl < 255) {
      // output high res tile
      if (pal2pos.find(lbl) != pal2pos.end()) {
        v3i pos = pal2pos[lbl];
        ForIndex(tk, tz) { ForIndex(tj, ty) { ForIndex(ti, tx) {
              detailed.at(i*tx + ti, j*ty + tj, k*tz + tk)
                = (highres.at(pos[0] * tx + tx - 1 - ti, pos[1] * ty + ty - 1 - tj, pos[2] * tz + tz - 1 - tk) != 255 ? solid_color : 255);
        } } }
      } else {
        ForIndex(tk, tz) { ForIndex(tj, ty) { ForIndex(ti, tx) {
              detailed.at(i*tx + ti, j*ty + tj, k*tz + tk) = solid_color;
        } } }
      }
    }
  } } }
  ForIndex(i, sx) { ForIndex(j, sy) { ForRangeReverse(k, sz - 1, 0) {
        fwrite(&detailed.at(i, j, k), sizeof(uchar), 1, f);
  } } }
  fwrite(palette.raw(), sizeof(v3b), 256, f);
  fclose(f);
}

/* -------------------------------------------------------- */

// Implements model synthesis for a 3D problem
// This is using the basic building blocks above.
// The approach used here is similar to Paul Merrell's model
// synthesis: it starts empty and attempts to synthesize within
// sub-domains. This works best on difficult problems. 
// WFC is also possible by synthesizing within the entire domain.
//
// Some of the constants below (number of iterations, etc.) could
// be changed for better/faster results depending on the input problem.
// Whether everything can be determined automatically is an interesting
// (and likely difficult) question.
void solve3D()
{
  Timer tm("solve3D");

  string fullpath = string(SRC_PATH "/exemplars/") + problem + ".slab.vox";

  //// setup a 3D problem
  load3DProblem(fullpath.c_str());

  // array being synthesized
  Array3D<Presence> S;
  S.allocate(sz, sz, sz);

  //// init as empty 
  if (pal2id.find(254) != pal2id.end()) {
    // ground is being used
    init_global_empty(S, pal2id[255], pal2id[254]);
  } else {
    // no ground: use an empty border along all faces
    init_global_empty(S, pal2id[255]);
  }
  
  //// synthesize subsets
  int num_failed    = 0;
  int num_success   = 0;
  int num_passes    = sz; // increases on larger domains.
  int num_sub_synth = 32; // will use twice that on ground level
  ForIndex(p, num_passes) {
    ForIndex(n, p == 0 ? 2 * num_sub_synth : num_sub_synth) {
      // random size
      int subsz = min(15, 8 + (rand() % 9));
      // random location
      // (forces the first pass to be on the ground, as many problems have ground constraints)
      AAB<3, int> sub;
      sub.minCorner() = v3i(
        rand() % (sz - subsz),
        rand() % (sz - subsz),
        p == 0 ? 0 : rand() % (sz - subsz));
      sub.maxCorner() = sub.minCorner() + v3i(subsz, subsz, subsz);
      // backup current
      Array3D<Presence> backup = S;
      // try reseting the subdomain (may fail)
      int num_solids_before = num_solids_sub(S, pal2id[255]/*empty*/, sub);
      if (reinit_sub(S, pal2id[255], sub)) {
        // try synthesizing (may fail)
        int num_solids;
        if (synthesize(S, pal2id[255]/*empty*/, num_solids, sub)) {
          if (num_solids >= num_solids_before) { // only accept if less (or eq) non empty appear
            num_success++;
          } else {
            num_failed++;
            S = backup;
          }
        } else {
          // synthesis failed: retry
          num_failed++;
          S = backup;
        }
      } else {
        // reinit failed: cannot work here 
        num_failed++;
        S = backup;
      }
    }
    // display progress
    Console::cursorGotoPreviousLineStart();
    std::cerr << sprint("attempt %3d / %3d, failures: %3d, successes: %3d\n", (p+1) * num_sub_synth, num_sub_synth*num_passes, num_failed, num_success);
  }

  // output final
  saveAsVox(SRC_PATH "/results/synthesized.slab.vox", S);
  // output detailed if a tilemap exists
  string low = (string(SRC_PATH "/exemplars/") + tilemap + ".slab.vox");
  string detailed = (string(SRC_PATH "/exemplars/") + tilemap + "_detailed.slab.vox");
  if (LibSL::System::File::exists(detailed.c_str())) {
    saveAsVoxDetailed(
      low.c_str(),
      detailed.c_str(),
      SRC_PATH "/results/synthesized_detailed.slab.vox",
      S);
  }

}

/* -------------------------------------------------------- */

// This is where it all begins.
int main(int argc, char **argv) 
{
  try {

    // random seed
    srand(time(NULL));
    
    // let's synthesize!
    std::cerr << Console::white << "Synthesizing a voxel model!" << Console::gray << std::endl << std::endl;
    solve3D();

  } catch (Fatal& e) {
    std::cerr << Console::red << e.message() << Console::gray << std::endl;
    return (-1);
  }

  return (0);
}

/* -------------------------------------------------------- */
