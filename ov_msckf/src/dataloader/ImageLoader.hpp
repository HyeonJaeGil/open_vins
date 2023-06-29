#pragma once
#include "utils/sensor_data.h"
#include <deque>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <experimental/filesystem>

namespace fs = std::experimental::filesystem;


class ImageLoader {
public:
  ImageLoader(std::string &folder) : folder_(folder) {
    if (folder.back() == '/') {
        folder = folder.substr(0, folder.size() - 1);
    }
    if (!fs::exists(folder)){
        std::cerr << "folder does not exist: " << folder << std::endl;
        exit(1);}
    if (!fs::is_directory(folder)){
        std::cerr << "folder is not a directory: " << folder << std::endl;
        exit(1);}
    cacheFilename(folder);
  }

void cacheFilename(std::string &folder){
    // save all filenames under the folder to filenames_

    std::set<std::string> paths_set;
    for (auto &entry : fs::directory_iterator(folder)){
        if (entry.path().extension() == ".png" || entry.path().extension() == ".jpg"){
            paths_set.insert(entry.path().stem().string());
        }
    }
    filenames_.clear();
    std::list<std::string> temp_list(std::make_move_iterator(paths_set.begin()), 
                                        std::make_move_iterator(paths_set.end()));
    filenames_ = std::vector<std::string>(std::make_move_iterator(temp_list.begin()),
                                        std::make_move_iterator(temp_list.end()));
}

ov_core::CameraData filename2data(std::string filename){
    ov_core::CameraData data;
    data.timestamp = std::stod(filename) * 1e-9;
    data.sensor_ids = {0};
    data.images.push_back(cv::imread(folder_ + "/" + filename + ".png"));
    return data;
}

ov_core::CameraData loadNextData() {
    if (index_ >= filenames_.size()) {
        std::cout << "No more data to load!" << std::endl;
        exit(0);
    }
    return filename2data(filenames_[index_++]);
}

private:
  std::string folder_;
  std::vector<std::string> filenames_;
  size_t index_ = 0;
};