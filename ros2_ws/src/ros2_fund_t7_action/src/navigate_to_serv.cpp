#include <cmath>
#include <memory>
#include <thread>

#include "rclcpp/rclcpp.hpp"
#include "rclcpp_action/rclcpp_action.hpp"

#include "geometry_msgs/msg/twist.hpp"
#include "turtlesim/msg/pose.hpp"

#include "ros2_fund_t7_action/action/navigate_to_pose.hpp"

class NavigateToPoseServer : public rclcpp::Node
{
public:
  using NavigateToPose = ros2_fund_t7_action::action::NavigateToPose;
  using GoalHandleNavigateToPose = rclcpp_action::ServerGoalHandle<NavigateToPose>;

  NavigateToPoseServer()
  : Node("navigate_to_pose_server")
  {
    // Publisher used to send velocity commands to turtlesim
    cmd_vel_pub_ = this->create_publisher<geometry_msgs::msg::Twist>(
      "/turtle1/cmd_vel",
      10);

    // Subscriber used to receive the current turtle pose
    pose_sub_ = this->create_subscription<turtlesim::msg::Pose>(
      "/turtle1/pose",
      10,
      std::bind(&NavigateToPoseServer::pose_callback, this, std::placeholders::_1));

    // Action server used to receive navigation goals
    action_server_ = rclcpp_action::create_server<NavigateToPose>(
      this,
      "navigate_to_pose",
      std::bind(&NavigateToPoseServer::handle_goal, this, std::placeholders::_1, std::placeholders::_2),
      std::bind(&NavigateToPoseServer::handle_cancel, this, std::placeholders::_1),
      std::bind(&NavigateToPoseServer::handle_accepted, this, std::placeholders::_1));

    RCLCPP_INFO(this->get_logger(), "'NavigateToPose' action server started.");
  }

private:
  rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr cmd_vel_pub_;
  rclcpp::Subscription<turtlesim::msg::Pose>::SharedPtr pose_sub_;
  rclcpp_action::Server<NavigateToPose>::SharedPtr action_server_;

  turtlesim::msg::Pose current_pose_;
  bool pose_received_ = false;

  /*
   * Store the latest pose received from turtlesim
   */
  void pose_callback(const turtlesim::msg::Pose::SharedPtr msg)
  {
    current_pose_ = *msg;
    pose_received_ = true;
  }

  /*
   * Decide whether a new goal should be accepted or rejected
   */
  rclcpp_action::GoalResponse handle_goal(
    const rclcpp_action::GoalUUID &,
    std::shared_ptr<const NavigateToPose::Goal> goal)
  {
    RCLCPP_INFO(
      this->get_logger(),
      "Received goal: x=%.2f, y=%.2f",
      goal->x,
      goal->y);

    return rclcpp_action::GoalResponse::ACCEPT_AND_EXECUTE;
  }

  /*
   * Allow the client to cancel an active goal
   */
  rclcpp_action::CancelResponse handle_cancel(
    const std::shared_ptr<GoalHandleNavigateToPose>)
  {
    RCLCPP_INFO(this->get_logger(), "Cancel request received.");
    return rclcpp_action::CancelResponse::ACCEPT;
  }

  /*
   * Start goal execution in a separate thread
   * This avoids blocking the ROS2 executor
   */
  void handle_accepted(const std::shared_ptr<GoalHandleNavigateToPose> goal_handle)
  {
    std::thread{
      std::bind(&NavigateToPoseServer::execute, this, goal_handle)
    }.detach();
  }

  /*
   * Execute the navigation loop
   * The turtle moves towards the goal while publishing feedback
   */
  void execute(const std::shared_ptr<GoalHandleNavigateToPose> goal_handle)
  {
    const auto goal = goal_handle->get_goal();

    auto feedback = std::make_shared<NavigateToPose::Feedback>();
    auto result = std::make_shared<NavigateToPose::Result>();

    // Rate of the control loop (10 Hz)
    rclcpp::Rate loop_rate(10);

    const double linear_gain = 1.0;
    const double angular_gain = 4.0;
    const double goal_tolerance = 0.1;

    while (rclcpp::ok()) {
      // Wait until the first pose from turtlesim has been received
      if (!pose_received_) {
        RCLCPP_INFO(this->get_logger(), "Waiting for turtle pose...");
        loop_rate.sleep();
        continue;
      }

      // If the client cancels the goal, stop the turtle and report cancellation
      if (goal_handle->is_canceling()) {
        publish_stop();

        result->message = "Goal canceled.";

        goal_handle->canceled(result);
        RCLCPP_INFO(this->get_logger(), "Goal canceled. Current position: (%.2f, %.2f).", current_pose_.x, current_pose_.y);
        return;
      }

      const double dx = goal->x - current_pose_.x;
      const double dy = goal->y - current_pose_.y;
      const double distance = std::sqrt(dx * dx + dy * dy);

      // Publish feedback with current pose and remaining distance
      feedback->current_x = current_pose_.x;
      feedback->current_y = current_pose_.y;
      feedback->distance_to_goal = distance;
      goal_handle->publish_feedback(feedback);

      // Finish when the turtle is close enough to the target
      if (distance < goal_tolerance) {
        publish_stop();

        result->message = "Goal reached. Current position: (" + std::to_string(current_pose_.x) + ", " + std::to_string(current_pose_.y) + ").";

        goal_handle->succeed(result);
        RCLCPP_INFO(this->get_logger(), "Goal reached. Current position: (%.2f, %.2f).", current_pose_.x, current_pose_.y);
        return;
      }

      // Goal not yet reached, compute control commands to move the turtle
      const double target_angle = std::atan2(dy, dx);
      const double angle_error = normalize_angle(target_angle - current_pose_.theta);

      geometry_msgs::msg::Twist cmd_vel;

      // Simple proportional control law
      cmd_vel.linear.x = linear_gain * distance;
      cmd_vel.angular.z = angular_gain * angle_error;

      cmd_vel_pub_->publish(cmd_vel);

      loop_rate.sleep();
    }
  }

  /*
   * Normalize angle to the range [-pi, pi].
   */
  double normalize_angle(double angle)
  {
    while (angle > M_PI) {
      angle -= 2.0 * M_PI;
    }

    while (angle < -M_PI) {
      angle += 2.0 * M_PI;
    }

    return angle;
  }

  /*
   * Publish zero velocity to stop the turtle.
   */
  void publish_stop()
  {
    geometry_msgs::msg::Twist stop_msg;
    cmd_vel_pub_->publish(stop_msg);
  }
};

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);

  auto node = std::make_shared<NavigateToPoseServer>();

  rclcpp::spin(node);

  rclcpp::shutdown();

  return 0;
}