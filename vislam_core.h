#ifndef VISLAM_CORE_H
#define VISLAM_CORE_H

#include <vector>
#include <string>
#include <stdint.h>

#ifdef _MSC_VER
// We are using a Microsoft compiler:

#ifdef VISLAM_CORE_SHARED_LIBS
#ifdef VISLAM_CORE_EXPORT
#define VISLAM_CORE_API __declspec(dllexport)
#else
#define VISLAM_CORE_API __declspec(dllimport)
#endif
#else
#define VISLAM_CORE_API
#endif

#else
// Not Microsoft compiler so set empty definition:
#define VISLAM_CORE_API
#endif

typedef enum  VISLAM_CORE_API TrackStatusType {
  CAM_INITIALIZING = 0,   ///< Motion estimation from camera image is being initialized
  IMU_INITIALIZING,       ///< Motion estimation from imu data is being initialized
  INIT_SUCCESS,
  POSE_VALID,             ///< The pose of this estimate is valid
  POSE_INVALID,           ///< The pose of this estimate is not valid
  POSE_UNKNOWN,            ///< Could not estimate pose at this time
  CONFIG_FAILED,			// reard config file failed
};

/// The PoseData struct contains pose information returned from motion tracking.
typedef struct VISLAM_CORE_API PoseData {

  /// The timestamp of the pose estimate, in seconds.
  double timestamp;
  /// Orientation, as a quaternion, of the pose of the target frame with
  /// reference to the base frame.
  /// Specified as (w,x,y,z)
  float orientation[4];
  /// Orientation, as euler angle, of the pose of the target frame with
  /// reference to the base frame.
  /// Specified as (pan, tilt, roll)
  float eulerAngle[3];
  /// Translation, ordered x, y, z, of the pose of the target frame with
  /// reference to the base frame.
  float translation[3];
  /// The unique id of the frame for this pose. 
  uint64_t frameId;
  /// Integer levels are determined by service.
  uint32_t confidence;
  /// Reserved for metric accuracy.
  float accuracy;
};

/// The Transformation struct contains an orientation and translation
/// that can be applied to transform a point.
typedef struct  VISLAM_CORE_API Transformation {
  /// Orientation, as a quaternion, of the transformation of the target frame
  /// with reference to the base frame.
  /// Specified as (w,x,y,z) where RotationAngle is in radians:
	float orientation[4];
  /// Translation, ordered x, y, z, of the transformation of the target frame
  /// with reference to the base frame.
	float translation[3];
};

/// The ImageBuffer contains information about a byte buffer holding
/// image data. This data is populated by the service when it returns an image.
typedef struct VISLAM_CORE_API StereoImageBuffer {
  /// The width of the image data.
  uint32_t width;
  /// The height of the image data.
  uint32_t height;
  /// The number of bytes per scanline of image data.
  uint32_t stride;
  /// The timestamp of this image, in seconds.
  double timestamp;
  /// The frame Id of this image.
  uint64_t frameId;
  /// Image data of both cameras.
  uint8_t *leftImage, *rightImage;
  /// Exposure duration of this image in nanoseconds.
  uint64_t exposureDuration;
};

/// The ImuBuffer contains information about a byte buffer holding
/// imu data.
typedef struct VISLAM_CORE_API ImuBuffer {
  /// The timestamp of this imu data, in seconds.
  double timestamp;
  /// The id of this imu data.
  uint64_t imuId;
  /// Gyroscope data of this imu buffer, (gx, gy, gz)
  float gyro[3];
  /// Accelerometer data of this imu buffer, (ax, ay, az)
  float acc[3];
  /// Magnetometer data of this imu buffer, (mx, my, mz)
  float mag[3];
};

typedef struct VISLAM_CORE_API Landmark {
  /// Stability or accuracy of this landmark.
  float confidence;
  /// Position of this landmark, specified as (x,y,z)
  float pos[3];
};


/// Initialize motion tracker with config files
VISLAM_CORE_API TrackStatusType InitMotionTracker(const std::string &deviceConfig, const std::string &trackerConfig);

/// Start motion track service
VISLAM_CORE_API TrackStatusType StartMotionTrack(void);

/// Update stereo images for motion track
/// @param[in] imageBuffer
/// @param[out] pose
VISLAM_CORE_API TrackStatusType UpdateImage(const StereoImageBuffer *imageBuffer, PoseData *pose);

/// Update stereo images for motion track, using factor for smoothing
/// @param[in] imageBuffer
/// @param[in] factor[2] = {rotation, translation}
/// @param[out] pose
VISLAM_CORE_API TrackStatusType UpdateImage(const StereoImageBuffer *imageBuffer, float factor[2], PoseData *pose);

/// Update stereo images for motion track, using factor for smoothing with SLERP
/// @param[in] imageBuffer
/// @param[in] factor[2] = {rotation, translation}
/// @param[out] pose
VISLAM_CORE_API TrackStatusType UpdateImageWithSLERP(const StereoImageBuffer *imageBuffer, float factor[2], PoseData *pose);
VISLAM_CORE_API void showUpdateImageTime(char *msg);

/// Undistort and rectify camera image for AR render
/// @param[in] imageBuffer
/// @param[out] rectifiedImage
VISLAM_CORE_API void UndistortRectifyImage(const StereoImageBuffer *imageBuffer, uint8_t *rectifiedImage);

/// Update imu data for motion track
/// @param[in] imuBuffer
/// @param[out] pose
VISLAM_CORE_API TrackStatusType UpdateIMU(const ImuBuffer *imuBuffer, PoseData *pose);

/// Reset motion track service
VISLAM_CORE_API bool ResetMotionTrack(void);

/// Stop motion track service
VISLAM_CORE_API bool StopMotionTrack(void);

/// Load offline generated map for motion track
/// @param[in] mapfile
VISLAM_CORE_API bool LoadMap(const std::string &mapfile);

/// Save map for motion track to disk
/// @param[in] mapfile
VISLAM_CORE_API bool SaveMap(const std::string &mapfile);

/// Get a vector of landmarks of the map online
/// @param[out] landmarks
VISLAM_CORE_API bool GetLandmarks(std::vector<Landmark> &landmarks);

/// Get the pose of the lastest frame
/// @param[out] pose
VISLAM_CORE_API void GetPose(PoseData *pose);

/// Get the view matrix of the lastest frame for OpenGL render
/// @param[out] rowMajor4x4
VISLAM_CORE_API void GetViewMatrix(float *rowMajor4x4);

/// Get the projection matrix of the camera for OpenGL render
/// @param[out] rowMajor4x4
VISLAM_CORE_API void GetProjectionMatrix(float *rowMajor4x4);

/// Get the projection model view matrix for OpenGL render
/// @param[in] model4x4
/// @param[out] rowMajor4x4
VISLAM_CORE_API void GetProjectionModelViewMatrix(float *model4x4, float *rowMajor4x4);

/// Transfer pose to the projection model view matrix for OpenGL render
/// @param[in] pose
/// @param[out] rowMajor4x4
VISLAM_CORE_API void Pose2ProjectionModelViewMatrix(const PoseData *pose, float *rowMajor4x4);

/// Transfer pose to translation matrix, (R, t)
/// @param[in] pose
/// @param[out] rowMajor4x4
VISLAM_CORE_API void Pose2TranslationMatrix(const PoseData *pose, float* rowMajor4x4);

VISLAM_CORE_API void SaveImage(const StereoImageBuffer *imageBuffer, std::string savePathLeft, std::string savePahtRight);
VISLAM_CORE_API void ShowImage(const StereoImageBuffer *imageBuffer);

VISLAM_CORE_API void SetMapName(const std::string mapfile);

VISLAM_CORE_API TrackStatusType UpdateImageWithStatic(const StereoImageBuffer *imageBuffer, PoseData *pose);


#endif
