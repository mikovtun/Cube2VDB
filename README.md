# Cube2VDB
This utility converts Gaussian Cube data into OpenVDB files. The resulting files can be directly imported into Blender or any other software that supports the `.vdb` format.
Right now, only orthogonal and equal length cube vectors are supported. No fancy periodic unit cells yet!
Multithreaded batch conversion is supported:
```
cube2vdb <input file 1> [input file 2] [input file 3] ...
```
You must have the `openvdb`, `blosc`, and `tbb` libraries installed.
Alternatively, you may use the provided apptainer definitions file for a no-frills, containerized program.
