/*************************************************************************
This software allows for filtering in high-dimensional measurement and
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


//#define PROFILING_ON

#include <sensor_msgs/Image.h>

#include <state_filtering/multi_object_tracker.hpp>
#include <state_filtering/tools/cloud_visualizer.hpp>


#include <cv.h>
#include <cv_bridge/cv_bridge.h>

#include <fstream>
#include <boost/filesystem.hpp>

#include <ctime>



typedef sensor_msgs::CameraInfo::ConstPtr CameraInfoPtr;
typedef Eigen::Matrix<double, -1, -1> Image;

class TrackerInterface
{
public:
    TrackerInterface(boost::shared_ptr<MultiObjectTracker> tracker): tracker_(tracker), node_handle_("~")
    {
        string config_file;
        ri::ReadParameter("config_file", config_file, node_handle_);


        path_ = config_file;
        path_ = path_.parent_path();
        cout << path_ << endl;

         time_t rawtime;
         struct tm * timeinfo;
         char buffer[80];

         time (&rawtime);
         timeinfo = localtime(&rawtime);

         strftime(buffer,80,"%d.%m.%Y_%I.%M.%S",timeinfo);
         std::string current_time(buffer);

         path_ /= "data_" + current_time + ".txt";
    }
    ~TrackerInterface() {}

    void Filter(const sensor_msgs::Image& ros_image)
    {
        double start_time; GET_TIME(start_time)
        VectorXd mean_state = tracker_->Filter(ros_image);
        double end_time; GET_TIME(end_time);
        double delta_time = end_time - start_time;
        cout << "delta time: " << delta_time << endl;


        ofstream file;
        file.open(path_.c_str(), ios::out | ios::app);
        if(file.is_open())
        {
            file << ros_image.header.stamp << " ";
            file << delta_time << " ";
            file << mean_state.transpose() << endl;
            file.close();
        }
        else
        {
            cout << "could not open file " << path_ << endl;
            exit(-1);
        }

//        cout << config_file_ << endl;





    }


private:
    boost::shared_ptr<MultiObjectTracker> tracker_;
    ros::NodeHandle node_handle_;
    boost::filesystem::path path_;
};



int main (int argc, char **argv)
{
    ros::init(argc, argv, "test_filter");
    ros::NodeHandle node_handle("~");

    // read parameters
    string depth_image_topic; ri::ReadParameter("depth_image_topic", depth_image_topic, node_handle);
    string camera_info_topic; ri::ReadParameter("camera_info_topic", camera_info_topic, node_handle);
    int initial_sample_count; ri::ReadParameter("initial_sample_count", initial_sample_count, node_handle);

    Matrix3d camera_matrix = ri::GetCameraMatrix<double>(camera_info_topic, node_handle, 2.0);

    // get observations from camera
    sensor_msgs::Image::ConstPtr ros_image =
            ros::topic::waitForMessage<sensor_msgs::Image>(depth_image_topic, node_handle, ros::Duration(10.0));
    Image image = ri::Ros2Eigen<double>(*ros_image) / 1000.; // convert to m

    vector<VectorXd> initial_states = pi::SampleTableClusters(hf::Image2Points(image, camera_matrix),
                                                              initial_sample_count);

    // intialize the filter
    boost::shared_ptr<MultiObjectTracker> tracker(new MultiObjectTracker);
    tracker->Initialize(initial_states, *ros_image, camera_matrix);
    cout << "done initializing" << endl;

    TrackerInterface interface(tracker);
    ros::Subscriber subscriber = node_handle.subscribe(depth_image_topic, 1, &TrackerInterface::Filter, &interface);


    ros::spin();
    return 0;
}