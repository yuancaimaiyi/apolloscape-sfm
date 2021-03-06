#ifndef CV_GL_SFM_H_
#define CV_GL_SFM_H_


#include <unordered_set>
#include <mutex>
#include <atomic>
#include <condition_variable>

#include "cv_gl/sfm_common.h"


// #include <ceres/ceres.h>

#include <opencv2/opencv.hpp>
// #include "opencv2/xfeatures2d.hpp"
#include <glm/glm.hpp>

#include "cv_gl/utils.h"
#include "cv_gl/camera.h"
#include "cv_gl/ccomp.hpp"
#include "cv_gl/cache_storage.hpp"

// #include <cereal/cereal.hpp>

// #include "cv_gl/serialization_mat.hpp"

// #include <cereal/details/traits.hpp>

class SfM3D {
public:
  typedef std::pair<int, int> IntPair;
  

  enum SfMStatus {
    RECONSTRUCTION,
    FINISH
  };
  SfM3D() : SfM3D{std::vector<CameraIntrinsics>()} {}
  explicit SfM3D(std::vector<CameraIntrinsics> camera_intrs)
      : intrinsics_(camera_intrs), 
        proc_status_(RECONSTRUCTION),
        vis_version_(0),
        repr_error_thresh(1.0),
        max_merge_dist(1.0) {

    // TESTS: For serialization
    // p.x = 1.0;
    // p.y = 2.1;
    // pd.x = 1.0;
    // pd.y = 2.1;

    // piv.push_back(0);
    // piv.push_back(1);
    // piv.push_back(2);

    // cv::KeyPoint kp;
    // kp.pt = pd;
    // kp.octave = 4;

    
    // kp.pt = pd;
    // kps.push_back(kp);
    // kp.pt = p;
    // kps.push_back(kp);

    // mat.create(3, 3, CV_64F);
    // dm.queryIdx = 1;
    // dm.trainIdx = 2;

    // v3 = glm::dvec3(2.0);
    
  }
  void AddImages(const std::vector<ImageData>& camera1_images,
                 const std::vector<ImageData>& camera2_images,
                 const bool make_pairs = true, const int look_back = 5);
  void ExtractFeatures();
  void Print(std::ostream& os = std::cout) const;
  void MatchImageFeatures(const int skip_thresh = 10, 
                          const double max_line_dist = 10.0, 
                          const bool use_cache = true);

  void InitReconstruction();
  void ReconstructAll();
  void PrintFinalStats();

  bool GetMapPointsVec(std::vector<Point3DColor>& glm_points);
  bool GetMapCamerasWithPointsVec(MapCameras& map_cameras);
  void GetMapPointsAndCameras(std::vector<Point3DColor>& glm_points,
                              MapCameras& map_cameras,
                              int& last_version);

  cv::Mat GetImage(int cam_id, bool full_size = false) const;
  CameraInfo GetCameraInfo(int cam_id) const;
  cv::KeyPoint GetKeypoint(int cam_id, int point_id) const;

  int ImageCount() const;
  int MapSize() const;

  void RestoreImages();
  void ClearImages();

  void SetProcStatus(SfMStatus proc_status);
  bool IsFinished();
  void EmitMapUpdate();


  // Debug methods
  void ShowFeatures(int img_id);
  void ShowMatches(const Matches& matches);
  void ShowMatchesLineConstraints(const Matches& matches, const double line_dist );


  // void SetResizeScale(const double scale) { resize_scale_ = scale; }


  double repr_error_thresh;
  double max_merge_dist;
  double resize_scale = 0.08;
  

  // TODO: https://www.patrikhuber.ch/blog/6-serialising-opencv-matrices-using-boost-and-cereal
  // https://github.com/patrikhuber/eos/blob/master/include/eos/morphablemodel/io/mat_cerealisation.hpp
  template<class Archive>
  void serialize(Archive& archive) {
    // archive (p, pd, piv, kps);
    //archive (p, pd, piv, kps); // , pd /*, kps*/
    // archive(mat);
    // archive(mats);
    // archive(ip);
    // archive(dm);
    archive(intrinsics_);
    archive(image_data_);
    archive(cameras_);
    // archive(images_);
    archive(resize_scale);
    archive(repr_error_thresh);
    archive(max_merge_dist);
    archive(images_resized_);
    archive(image_features_);
    archive(image_pairs_);
    archive(image_matches_);
    archive(todo_views_);
    archive(used_views_);
    archive(map_);
    archive(matches_index_);
    archive(ccomp_);
  }
private:
  void GenerateAllPairs();

  void TriangulatePointsFromViews(const int first_id, 
                                  const int second_id, 
                                  Map3D& map);
  void OptimizeCurrentMap() { OptimizeMap(map_); }
  void OptimizeMap(Map3D& map);
  

  void ReconstructNextView(const int next_img_id);
  void ReconstructNextViewPair(const int first_id, const int second_id);

  int FindMaxSizeMatch(const bool within_todo_views = false) const;

  bool IsPairInOrder(const int p1, const int p2);
  

  // Data Initial
  std::vector<CameraIntrinsics> intrinsics_;
  std::vector<ImageData> image_data_;
  std::vector<CameraInfo> cameras_;
  std::vector<cv::Mat> images_;


  std::vector<cv::Mat> images_resized_;

  // Pre-processing & Feature Extraction
  std::vector<Features> image_features_;
  std::vector<ImagePair> image_pairs_;

  // Matching
  CComponents<IntPair> ccomp_;
  std::vector<Matches> image_matches_;
  std::map<IntPair, int> matches_index_;
  

  // Reconstruction
  std::unordered_set<int> used_views_;
  std::unordered_set<int> todo_views_;
  Map3D map_;

  
  bool use_cache = true;

  // Preprocessing storage
  CacheStorage cache_storage;


  std::mutex map_mutex;
  std::condition_variable map_update_;
  std::atomic<long> vis_version_;

  std::atomic<SfMStatus> proc_status_;


  // Test Part
  // cv::Point2f p;
  // std::vector<int> piv;
  // cv::Point2d pd;
  // std::vector<cv::KeyPoint> kps;
  // std::vector<cv::Mat> mats;
  // ImagePair ip;
  // cv::DMatch dm;
  // cv::Mat mat;
  // glm::dvec3 v3;
  // std::vector<IntPair> int_pairs_;
  // std::pair<int, int> int_pair_;
};


std::ostream& operator<<(std::ostream& os, const SfM3D& sfm);


// ============ Ceres Types / Functions ====

// void OptimizeBundle(Map3D& map, const std::vector<CameraInfo>& cameras, const std::vector<Features>& features);

struct SuperCostFunctor {
  template <typename T> bool operator()(const T* const x, T* residual) const {
    residual[0] = 10.0 - x[0];
    return true;
  }
};



#endif  // CV_GL_SFM_H_