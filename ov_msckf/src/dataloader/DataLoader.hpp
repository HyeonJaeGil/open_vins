#pragma once
#include "ImageLoader.hpp"
#include "ImuLoader.hpp"
#include "utils/sensor_data.h"
#include <deque>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>


class DataLoader {
public:
  DataLoader(std::string &folder, std::string &csv_file) 
  : folder_(folder), csv_file_(csv_file) {
    image_loader_ = std::make_unique<ImageLoader>(folder_);
    imu_loader_ = std::make_unique<ImuLoader>(csv_file_);
    last_camera_data_ = image_loader_->loadNextData();
    last_imu_data_ = imu_loader_->loadNextData();
  }

  std::shared_ptr<ov_core::OneHotData> loadNextData() {
    auto data = std::make_shared<ov_core::OneHotData>();
    if (last_camera_data_.timestamp < last_imu_data_.timestamp) {
      last_camera_data_ = image_loader_->loadNextData();
      data->camera_data = last_camera_data_;
      data->sensor_type = ov_core::OneHotData::SensorType::CAMERA;
    } else {
      last_imu_data_ = imu_loader_->loadNextData();
      data->imu_data = last_imu_data_;
      data->sensor_type = ov_core::OneHotData::SensorType::IMU;
    }
    return data;
  }

private:
  std::string folder_;
  std::string csv_file_;
  std::unique_ptr<ImageLoader> image_loader_;
  std::unique_ptr<ImuLoader> imu_loader_;
  ov_core::CameraData last_camera_data_;
  ov_core::ImuData last_imu_data_;
};