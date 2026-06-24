#include <chrono>
#include <cmath>
#include <memory>
#include <vector>

#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/point.hpp"
#include "visualization_msgs/msg/marker.hpp"

using namespace std::chrono_literals;

/*
 * Calculate a fixed number of points from the parametric curve:
 *   x(t) = a * cos(2πt)
 *   y(t) = b * sin(4πt)
 * The generated points are returned in temporal order, from t = 0 to t < 1
 */
std::vector<geometry_msgs::msg::Point> calculate_curve_points(
    std::size_t number_of_points,
    double amplitude_x,
    double amplitude_y)
{
    std::vector<geometry_msgs::msg::Point> curve_points;

    // Reserve memory for all points to avoid repeated memory reallocations
    curve_points.reserve(number_of_points);

    // Sample the curve using evenly spaced values of t
    for (std::size_t i = 0; i < number_of_points; ++i) {
        const double t = static_cast<double>(i) / static_cast<double>(number_of_points);

        // Calculate one point of the parametric curve
        geometry_msgs::msg::Point point;
        point.x = amplitude_x * std::cos(2.0 * M_PI * t);
        point.y = amplitude_y * std::sin(4.0 * M_PI * t);
        point.z = 0.0;  // 2D curve displayed in the XY plane

        curve_points.push_back(point);
    }

    return curve_points;
}

int main(int argc, char ** argv)
{
    rclcpp::init(argc, argv);

    auto node = rclcpp::Node::make_shared("curve_marker_publisher");

    // Publisher for RViz Marker messages
    auto marker_pub =
        node->create_publisher<visualization_msgs::msg::Marker>(
        "curve_with_marker",
        10);

    // 5 Hz (one publication every 200 ms)
    const double freqHz = 5.0;
    rclcpp::Rate loop_rate(freqHz);

    // Curve configuration.
    const std::size_t number_of_points = 1000;  // Total number of points to sample from the parametric curve
    const double a = 4.0;                       // Amplitude in the x direction
    const double b = 5.0;                       // Amplitude in the y direction

    // Calculate curve points
    const auto curve_points = calculate_curve_points(number_of_points, a, b);

    std::size_t point_index = 0;

    while (rclcpp::ok()) {
        visualization_msgs::msg::Marker marker;

        // Header information.
        // frame_id must match the Fixed Frame configured in RViz
        marker.header.frame_id = "map";
        marker.header.stamp = node->now();

        // Namespace and ID identify the marker in RViz
        // Reusing the same ID makes the new marker replace the previous one
        marker.ns = "curve_marker";
        marker.id = 0;

        // Publish a single sphere (point) marker at the current curve point
        marker.type = visualization_msgs::msg::Marker::SPHERE;
        marker.action = visualization_msgs::msg::Marker::ADD;

        // Set marker position to the current sampled point
        marker.pose.position = curve_points[point_index];

        // Valid orientation quaternion
        marker.pose.orientation.w = 1.0;

        // Sphere size in RViz.
        marker.scale.x = 0.25;
        marker.scale.y = 0.25;
        marker.scale.z = 0.25;

        // Marker color: green and fully visible
        marker.color.g = 1.0f;
        marker.color.a = 1.0f;

        // Publish the current marker
        marker_pub->publish(marker);

        // Advance to the next point. Wraps around to the first point after the last one
        point_index = (point_index + 1) % curve_points.size();

        // Allow ROS2 to process pending callbacks 
        rclcpp::spin_some(node);

        // Sleep to maintain publishing frequency
        loop_rate.sleep();
    }

    // Cleanly shut down ROS2
    rclcpp::shutdown();

    return 0;
}