#pragma once
#include "utils/sensor_data.h"
#include <deque>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

class ImuLoader {
public:
  ImuLoader(std::string &csv_file) : csv_file_(csv_file) {
    parseCSV(csv_file_);
  }

  /**
   * @brief Convert a line of csv to a vector of doubles
   *
   * @param std::string
   * @return std::vector<double>
   */
  std::vector<double> line2vec(std::string line) {
    // line consists of 7 values, separated by commas
    std::vector<double> vec;
    std::string delimiter = ",";
    size_t pos = 0;
    std::string token;
    while ((pos = line.find(delimiter)) != std::string::npos) {
      token = line.substr(0, pos);
      vec.push_back(std::stod(token));
      line.erase(0, pos + delimiter.length());
    }
    return vec;
  }

  /**
   * @brief Parse a csv file and store the data in a vector of ImuData
   *
   * @param csv_file
   */
  void parseCSV(std::string &csv_file) {
    std::ifstream file(csv_file);
    std::string line;
    std::getline(file, line); // delete first line
    while (std::getline(file, line)) {
      std::vector<double> vec = line2vec(line);
      ov_core::ImuData imu_data;
      imu_data.timestamp = vec[0];
      imu_data.am << vec[1], vec[2], vec[3];
      imu_data.wm << vec[4], vec[5], vec[6];
      imu_data_.push_back(imu_data);
    }
  }

  /**
   * @brief Load the next ImuData from the vector
   *
   * @return ov_core::ImuData
   */
  ov_core::ImuData loadNextData() {
    if (index_ >= imu_data_.size()) {
      std::cout << "No more data to load!" << std::endl;
      exit(0);
    }
    return imu_data_[index_++];
  }

private:
  std::string csv_file_;
  std::vector<ov_core::ImuData> imu_data_;
  size_t index_ = 0;
};
