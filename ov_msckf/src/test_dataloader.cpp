#include "dataloader/DataLoader.hpp"
#include <iomanip>
#include <iostream>
#include <opencv2/opencv.hpp>

int main() {
  std::string folder = "/home/hj/d435_2023-06-27-20-57-29/rgb";
  std::string csv_file = "/home/hj/d435_2023-06-27-20-57-29/imu.csv";
  DataLoader dataloader(folder, csv_file);
  std::shared_ptr<ov_core::OneHotData> data;

  std::cout << std::fixed << std::setprecision(6);
  while (true) {
    data = dataloader.loadNextData();
    if (data->sensor_type == ov_core::OneHotData::SensorType::IMU) {
      std::cout << "IMU data: " << data->imu_data.timestamp << ", ";
      std::cout << "acc: " << data->imu_data.am[0] << "," << data->imu_data.am[1] << "," << data->imu_data.am[2] <<  ",";
      std::cout << "gyro: " << data->imu_data.wm[0] << "," << data->imu_data.wm[1] << "," << data->imu_data.wm[2] << std::endl;
    } else if (data->sensor_type == ov_core::OneHotData::SensorType::CAMERA) {
      std::cout << "Camera data: " << data->camera_data.timestamp << ", ";
      std::cout << data->camera_data.sensor_ids[0] << std::endl;
      cv::imshow("image", data->camera_data.images[0]);
      cv::waitKey(10);
    }
  }
  return 0;
}