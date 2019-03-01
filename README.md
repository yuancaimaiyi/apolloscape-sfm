# 3D Reconstruction of Apolloscape ZPark Dataset using Structure from Motion pipeline with OpenGL visualization

3D reconstruction and Structure from Motion are an essential parts of many algorithms of today's research in Visual Odometry, SLAM and localization tasks. In this project I've build the sparse 3D reconstruction from known poses implementing Structure from Motion pipeline with bundle andjustment on C++ and OpenGL visualization.

Previously, I've explored the Apolloscape dataset in the [project for localization task](https://github.com/bexcite/apolloscape-loc) via building Pytorch reader and training PoseNet to directly regress the car pose.

# Dependencies & Build

Build dependencies:
- Boost 1.59 Filesystem
- OpenCV 3.4
- Ceres Solver 1.14_04
- gflags 2.2.2

Build commands:
```
mkdir build
cd build
cmake ..
make
```

# Data Folder

Download Apolloscape ZPark dataset from its official [page](http://apolloscape.auto/scene.html) and unpack it into a folder. Then symbolically link it to `./data/apolloscape` from the project root directory:

```
ln -s <DATA FOLDER>/apolloscape ./data
```

ZPark sample dataset should appear in the path `./data/apolloscape/zpark-sample` as a result.

# Run 3D Reconstruction

The main file to run is `./bin/3d_recon` from `./build` folder. Here is an example to reconstruct only first sequence alone with the simultaneous 3D visualization:
```
./bin/3d_recon --records="1"
```
Or the full ZPark sample dataset for all 14 records without visualization (will take some time):
```
make && ./bin/3d_recon --records="1,2,3,4,6,7,8,9,10,11,12,13,14" \
--pairs_look_back=2 --matches_num_thresh=60 --matches_line_dist_thresh=10.0 \
--sfm_repr_error_thresh=10.0 --sfm_max_merge_dist=5.0 --noviz \
--output=sfm_out_all_r1_14.bin
```

## Cache

Extracted features and matched pairs of keypoints with descriptors are stored in a cache folder `./build/_features_cache` so subsequent runs that do not introduce new image pairs are using pre-calculated values stored earlier. Its speed up my tests iterations dramatically.

## Serialization

Cached featured and the final map serialization is implemented with Cereal. So the quickest way to see something is to restore the previous map. In the results folder there is one such map that can be viewed without reconstruction step as:
```
# TBD the example map
./bin/3d_recon --restore=../results/sfm_example.bin
```