# Copyright 2024 Intelligent Robotics Lab
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import os

from ament_index_python.packages import get_package_share_directory

from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node


def generate_launch_description():
    namespace = LaunchConfiguration('namespace')
    pkg_dir = get_package_share_directory('cs4home_sound_project')

    declare_namespace_cmd = DeclareLaunchArgument(
        'namespace',
        default_value='',
        description='Namespace')

    # Specify the actions
    cm1_cmd = Node(
        package='cs4home_sound_project',
        executable='sound_cognitive_module',
        name='sound_recognition',
        namespace=namespace,
        output='screen',
        parameters=[
            os.path.join(pkg_dir, 'config', 'params.yaml')
        ],)
    
    cm2_cmd = Node(
        package='cs4home_sound_project',
        executable='sound_supervision_cognitive_module',
        name='sound_supervision',
        namespace=namespace,
        output='screen',
        parameters=[
            os.path.join(pkg_dir, 'config', 'params.yaml')
        ],)
    
    cm3_cmd = Node(
        package='cs4home_sound_project',
        executable='sound_context_evaluation_cognitive_module',
        name='sound_context_evaluation',
        namespace=namespace,
        output='screen',
        parameters=[
            os.path.join(pkg_dir, 'config', 'params.yaml')
        ],)
    
    cm4_cmd = Node(
        package='cs4home_sound_project',
        executable='human_assistance_cognitive_module',
        name='human_assistance',
        namespace=namespace,
        output='screen',
        parameters=[
            os.path.join(pkg_dir, 'config', 'params.yaml')
        ],)
    
    cm5_cmd = Node(
        package='cs4home_sound_project',
        executable='emergency_cognitive_module',
        name='emergency_action',
        namespace=namespace,
        output='screen',
        parameters=[
            os.path.join(pkg_dir, 'config', 'params.yaml')
        ],)
    
    m_cmd = Node(
        package='cs4home_sound_project',
        executable='sound_master',
        name='sound_master',
        namespace=namespace,
        output='screen',
        parameters=[
            os.path.join(pkg_dir, 'config', 'params.yaml')
        ],)

    # Create the launch description and populate
    ld = LaunchDescription()

    ld.add_action(declare_namespace_cmd)

    # Declare the launch options
    ld.add_action(cm1_cmd)
    ld.add_action(cm2_cmd)
    ld.add_action(cm3_cmd)
    ld.add_action(cm4_cmd)
    ld.add_action(cm5_cmd)
    #ld.add_action(m_cmd)

    return ld
