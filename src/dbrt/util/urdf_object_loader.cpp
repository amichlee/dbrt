/*
 * This is part of the Bayesian Object Tracking (bot),
 * (https://github.com/bayesian-object-tracking)
 *
 * Copyright (c) 2015 Max Planck Society,
 * 				 Autonomous Motion Department,
 * 			     Institute for Intelligent Systems
 *
 * This Source Code Form is subject to the terms of the GNU General Public
 * License License (GNU GPL). A copy of the license can be found in the LICENSE
 * file distributed with this source code.
 */

/**
 * \file urdf_object_model_loader.cpp
 * \date January 2016
 * \author Jan Issac (jan.issac@gmail.com)
 */

#include <ros/ros.h>
#include <fl/util/profiling.hpp>
#include <dbrt/util/urdf_object_loader.hpp>

namespace dbrt
{
UrdfObjectModelLoader::UrdfObjectModelLoader(
    const std::shared_ptr<KinematicsFromURDF>& urdf_kinematics)
    : urdf_kinematics_(urdf_kinematics)
{
}

void UrdfObjectModelLoader::load(
    std::vector<std::vector<Eigen::Vector3d>>& vertices,
    std::vector<std::vector<std::vector<int>>>& triangle_indices) const
{
    std::vector<boost::shared_ptr<PartMeshModel>> part_meshes_;
    urdf_kinematics_->GetPartMeshes(part_meshes_);

    vertices.resize(part_meshes_.size());
    triangle_indices.resize(part_meshes_.size());
    for (size_t i = 0; i < part_meshes_.size(); i++)
    {
        vertices[i] = *(part_meshes_[i]->get_vertices());
        triangle_indices[i] = *(part_meshes_[i]->get_indices());
    }
}
}
