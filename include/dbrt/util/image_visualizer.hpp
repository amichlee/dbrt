/*************************************************************************
This software allows for filtering in high-dimensional observation and
state spaces, as described in

M. Wuthrich, P. Pastor, M. Kalakrishnan, J. Bohg, and S. Schaal.
Probabilistic Object Tracking using a Range Camera
IEEE/RSJ Intl Conf on Intelligent Robots and Systems, 2013

In a publication based on this software pleace cite the above reference.


Copyright (C) 2014  Manuel Wuthrich

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*************************************************************************/

#ifndef POSE_TRACKING_INTERFACE_UTILS_IMAGE_VISUALIZER_HPP
#define POSE_TRACKING_INTERFACE_UTILS_IMAGE_VISUALIZER_HPP

#include <vector>
#include <string>
#include "Eigen/Core"
// cv.h cannot be included in image_visualizer.hpp and therefore cv::Mat has to
// be
// avoided as input to any function in image_visualizer
//#include <cv.h>

#include <sensor_msgs/Image.h>

namespace vis
{
class ImageVisualizer
{
public:
    ImageVisualizer(const int& n_rows, const int& n_cols);

    ~ImageVisualizer();

    void set_image(const Eigen::MatrixXd& image,
                   const float& min_value = 0,
                   const float& max_value = 0,
                   const bool& invert_image = false);

    void set_image(const std::vector<float>& image,
                   const float& min_value = 0,
                   const float& max_value = 0,
                   const bool& invert_image = false);

    void add_points(const std::vector<Eigen::Vector3f>& points,
                    const Eigen::Matrix3f& camera_matrix,
                    const Eigen::Matrix3f& R = Eigen::Matrix3f::Identity(),
                    const Eigen::Vector3f& t = Eigen::Vector3f::Zero(),
                    const std::vector<float>& colors = std::vector<float>(0));

    void add_points(const Eigen::Matrix<Eigen::Vector3d, -1, -1>& points,
                    const Eigen::Matrix3d& camera_matrix,
                    const Eigen::Matrix3d& R = Eigen::Matrix3d::Identity(),
                    const Eigen::Vector3d& t = Eigen::Vector3d::Zero(),
                    const std::vector<float>& colors = std::vector<float>(0));

    void add_points(const Eigen::VectorXd &depth_image);

    void add_points(const std::vector<int>& point_indices,
                    const std::vector<float>& colors = std::vector<float>(0));

    char show_image(const std::string& window_name = "dini mer",
                    const int& window_width = 500,
                    const int& window_height = 500,
                    const int& delay = 0) const;

    // void get_image(Eigen::MatrixXd &image) const;
    void get_image(sensor_msgs::Image& image) const;

private:
    void* image_;
    const int n_rows_, n_cols_;

    void Cart2Index(const Eigen::Vector3f& cart,
                    const Eigen::Matrix3f& camera_matrix,
                    int& row,
                    int& col) const;
};
}

#endif
