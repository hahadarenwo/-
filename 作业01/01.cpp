﻿#include "Triangle.h"
#include "rasterizer.h"
#include <eigen3/Eigen/Eigen>
#include <iostream>
#include <opencv2/opencv.hpp>

constexpr double MY_PI = 3.1415926;

Eigen::Matrix4f get_view_matrix(Eigen::Vector3f eye_pos)
{
    Eigen::Matrix4f view = Eigen::Matrix4f::Identity();

    Eigen::Matrix4f translate;
    translate << 1, 0, 0, -eye_pos[0], 0, 1, 0, -eye_pos[1], 0, 0, 1, -eye_pos[2], 0, 0, 0, 1;

    view = translate * view;

    return view;
}

Eigen::Matrix4f get_model_matrix(float rotation_angle, char axis)
{
    Eigen::Matrix4f model = Eigen::Matrix4f::Identity();
    // TODO: Implement this function
    // Create the model matrix for rotating the triangle around the Z axis.
    // Then return it.
    Eigen::Matrix4f rotate;
    double rotateAngle = rotation_angle / 180.0 * MY_PI;
    if (axis == 'z') {
        rotate << cos(rotateAngle), -sin(rotateAngle), 0, 0,
            sin(rotateAngle), cos(rotateAngle), 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1;
        model *= rotate;
    }
    else if (axis == 'x') {
        rotate << 1, 0, 0, 0,
            0, cos(rotateAngle), -sin(rotateAngle), 0,
            0, sin(rotateAngle), cos(rotateAngle), 0,
            0, 0, 0, 1;
        model *= rotate;
    }
    else if (axis == 'y') {
        rotate << cos(rotateAngle), 0, sin(rotateAngle), 0,
            0, 1, 0, 0,
            -sin(rotateAngle), 0, cos(rotateAngle), 0,
            0, 0, 0, 1;
        model *= rotate;
    }
    return model;
}

Eigen::Matrix4f get_projection_matrix(float eye_fov, float aspect_ratio,
    float zNear, float zFar)
{
    // Students will implement this function

    Eigen::Matrix4f projection = Eigen::Matrix4f::Identity();

    // TODO: Implement this function
    // Create the projection matrix for the given parameters.
    // Then return it.

    Eigen:Matrix4f m;//透视投影矩阵
    m << zNear, 0, 0, 0,
        0, zNear, 0, 0,
        0, 0, zNear + zFar, -zNear * zFar,
        0, 0, 1, 0;
    float halfEyeAngel = eye_fov / 2 / 180 * MY_PI;
    float top = tan(halfEyeAngel) * zNear;//top y轴的最高点
    float bottom = -top;//关于原点对称，top取负号不就是最低点
    float right = top * aspect_ratio;//高度 * 宽高比就是宽度
    float left = -right;
    Eigen::Matrix4f n, p;//n移动到原点，p缩放
    n << 2 / (right - left), 0, 0, 0,
        0, 2 / (top - bottom), 0, 0,
        0, 0, 2 / (zNear - zFar), 0,
        0, 0, 0, 1;
    p << 1, 0, 0, -(right + left) / 2,
        0, 1, 0, -(top + bottom) / 2,
        0, 0, 1, -(zFar + zNear) / 2,
        0, 0, 0, 1;
    projection = n * p * m;//正交
    return projection;
}

int main(int argc, const char** argv)
{
    char rotateAxis;
    float angle = 0;
    bool command_line = false;
    std::string filename = "output.png";
    std::cin >> rotateAxis;
    if (argc >= 3) {
        command_line = true;
        angle = std::stof(argv[2]); // -r by default
        if (argc == 4) {
            filename = std::string(argv[3]);
        }
        else
            return 0;
    }

    rst::rasterizer r(700, 700);

    Eigen::Vector3f eye_pos = { 0, 0, 5 };

    std::vector<Eigen::Vector3f> pos{ {2, 0, -2}, {0, 2, -2}, {-2, 0, -2} };

    std::vector<Eigen::Vector3i> ind{ {0, 1, 2} };

    auto pos_id = r.load_positions(pos);
    auto ind_id = r.load_indices(ind);

    int key = 0;
    int frame_count = 0;

    if (command_line) {
        r.clear(rst::Buffers::Color | rst::Buffers::Depth);

        r.set_model(get_model_matrix(angle, rotateAxis));
        r.set_view(get_view_matrix(eye_pos));
        r.set_projection(get_projection_matrix(45, 1, 0.1, 50));

        r.draw(pos_id, ind_id, rst::Primitive::Triangle);
        cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
        image.convertTo(image, CV_8UC3, 1.0f);

        cv::imwrite(filename, image);

        return 0;
    }

    while (key != 27) {

        r.clear(rst::Buffers::Color | rst::Buffers::Depth);

        r.set_model(get_model_matrix(angle, rotateAxis));
        r.set_view(get_view_matrix(eye_pos));
        r.set_projection(get_projection_matrix(45, 1, 0.1, 50));

        r.draw(pos_id, ind_id, rst::Primitive::Triangle);

        cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
        image.convertTo(image, CV_8UC3, 1.0f);
        cv::imshow("image", image);
        key = cv::waitKey(10);

        std::cout << "frame count: " << frame_count++ << '\n';

        if (key == 'a') {
            angle += 10;
        }
        else if (key == 'd') {
            angle -= 10;
        }
    }

    return 0;
}
