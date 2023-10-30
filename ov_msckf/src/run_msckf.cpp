#include "dataloader/DataLoader.hpp"
#include <iomanip>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <memory>
#include "core/VioManager.h"
#include "core/VioManagerOptions.h"
#include "utils/dataset_reader.h"

using namespace ov_core;
using namespace ov_type;
using namespace ov_msckf;


void feed_data(std::shared_ptr<VioManager> app, std::shared_ptr<ov_core::OneHotData> data, cv::Mat& mask) {
    if (data->sensor_type == ov_core::OneHotData::SensorType::IMU) {
      // convert into correct format
      ov_core::ImuData message;
      message.timestamp = data->imu_data.timestamp;
      message.wm << data->imu_data.wm[0], data->imu_data.wm[1], data->imu_data.wm[2];
      message.am << data->imu_data.am[0], data->imu_data.am[1], data->imu_data.am[2];

      // send it to our VIO system
      app->feed_measurement_imu(message);

    } else if (data->sensor_type == ov_core::OneHotData::SensorType::CAMERA) {
      // convert into correct format
      ov_core::CameraData message;
      message.timestamp = data->camera_data.timestamp;
      message.sensor_ids.push_back(data->camera_data.sensor_ids[0]);
      cv::Mat img_grayscale;
      cv::cvtColor(data->camera_data.images[0], img_grayscale, cv::COLOR_BGR2GRAY);
      message.images.push_back(img_grayscale);
      message.masks.push_back(mask.clone());

      cv::imshow("image", data->camera_data.images[0]);
      cv::waitKey(10);

      // send it to our VIO system
      app->feed_measurement_camera(message);


      // // Load the mask if we are using it, else it is empty
      // // TODO: in the future we should get this from external pixel segmentation
      // if (app->get_params().use_mask) {
      //   message.masks.push_back(app->get_params().masks.at(0));
      // } else {
      //   message.masks.push_back(cv::Mat::zeros(cv_ptr->image.rows, cv_ptr->image.cols, CV_8UC1));
      // }

    }
}


int main() {

  // set config path
  std::string config_path = "../../config/euroc_mav/estimator_config.yaml";
  auto parser = std::make_shared<ov_core::YamlParser>(config_path);

  // Verbosity
  std::string verbosity = "DEBUG";
  parser->parse_config("verbosity", verbosity); // parse "verbosity" from config file
  ov_core::Printer::setPrintLevel(verbosity);

  // Create our VIO system
  VioManagerOptions params;
  params.print_and_load(parser);
  params.use_multi_threading_subs = true;
  std::shared_ptr<VioManager> sys = std::make_shared<VioManager>(params);

  // set data folder
  std::string folder = "/home/hj/d435_2023-06-27-20-57-29/rgb";
  std::string csv_file = "/home/hj/d435_2023-06-27-20-57-29/imu.csv";
  DataLoader dataloader(folder, csv_file);
  std::shared_ptr<ov_core::OneHotData> data;

  // create mask and set row from 384 to 512 to 0
  cv::Mat mask = cv::Mat::ones(480, 640, CV_8UC1) * 255;
  for (int i = 384; i < 480; i++) {
    for (int j = 0; j < 640; j++) {
      mask.at<uchar>(i, j) = 0;
    }
  }

  // load data
  std::cout << std::fixed << std::setprecision(6);
  while (true) {
    data = dataloader.loadNextData();
    feed_data(sys, data, mask);

    std::cout << "initialized: " << sys->initialized() << std::endl;
  }
  return 0;
}