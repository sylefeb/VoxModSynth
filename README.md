# VoxModSynth
A simple, efficient, easily hackable C++ implementation of 3d model synthesis / wave function collapse

Sylvain Lefebvre 
@sylefeb

 - The expected voxel format is '.slab.vox' as exported by MagicaVoxel
 - The output can be directly imported into MagicaVoxel (use MagicaVoxel viewer for larger outputs, as MagicaVoxel clamps to 128^3)
 - Pallete indices are used as tile ids (labels).
 - Pallete index 255 is empty, 254 is ground.
 - Input files are in subdir exemplars/
 - Output is produced in subdir results/

For more details on model synthesis:
http://graphics.stanford.edu/~pmerrell/ (Example-Based Model Synthesis)
https://github.com/mxgmn/WaveFunctionCollapse
 
Limitations:
 - all labels are currently equiprobable (will be updated soon)
