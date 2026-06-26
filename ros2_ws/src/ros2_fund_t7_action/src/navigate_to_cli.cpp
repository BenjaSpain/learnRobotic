#include <cstdlib>
#include <memory>
#include <thread>
#include <chrono>

#include "rclcpp/rclcpp.hpp"
#include "rclcpp_action/rclcpp_action.hpp"

#include "ros2_fund_t7_action/action/navigate_to_pose.hpp"

class NavigateToPoseClient : public rclcpp::Node
{
public:
  using NavigateToPose = ros2_fund_t7_action::action::NavigateToPose;
  using GoalHandleNavigateToPose = rclcpp_action::ClientGoalHandle<NavigateToPose>;

  NavigateToPoseClient()
  : Node("navigate_to_pose_client")
  {
    // Action client connected to the action server name
    action_client_ = rclcpp_action::create_client<NavigateToPose>(
      this,
      "navigate_to_pose");
  }

  /*
   * Send a target position to the action server
   */
  void send_goal(float x, float y)
  {
    if (!action_client_->wait_for_action_server()) {
      RCLCPP_ERROR(this->get_logger(), "Action server not available.");
      rclcpp::shutdown();
      return;
    }

    NavigateToPose::Goal goal;
    goal.x = x;
    goal.y = y;

    RCLCPP_INFO(
      this->get_logger(),
      "Sending goal: x=%.2f, y=%.2f",
      goal.x,
      goal.y);

    rclcpp_action::Client<NavigateToPose>::SendGoalOptions options;

    // Callback called when the server accepts or rejects the goal
    options.goal_response_callback =
      std::bind(&NavigateToPoseClient::goal_response_callback, this, std::placeholders::_1);

    // Callback called repeatedly while the server publishes feedback
    options.feedback_callback =
      std::bind(
        &NavigateToPoseClient::feedback_callback,
        this,
        std::placeholders::_1,
        std::placeholders::_2);

    // Callback called when the action finishes
    options.result_callback =
      std::bind(&NavigateToPoseClient::result_callback, this, std::placeholders::_1);

    action_client_->async_send_goal(goal, options);
  }

  /*
   * Cancel the current goal
   */
  void cancel_goal()
  {
    if (!goal_handle_) {
      RCLCPP_WARN(this->get_logger(), "No active goal to cancel.");
      return;
    }

    RCLCPP_WARN(this->get_logger(), "Sending cancel request...");
    action_client_->async_cancel_goal(goal_handle_);
  }

private:
  rclcpp_action::Client<NavigateToPose>::SharedPtr action_client_;
  // A handle to the goal, used to cancel the goal if needed
  GoalHandleNavigateToPose::SharedPtr goal_handle_;

  /*
   * Called when action server responds about the goal request
   */
  void goal_response_callback(const GoalHandleNavigateToPose::SharedPtr & goal_handle)
  {
    if (!goal_handle) {
      RCLCPP_ERROR(this->get_logger(), "Goal was rejected.");
    } else {
      goal_handle_ = goal_handle;
      RCLCPP_INFO(this->get_logger(), "Goal accepted.");
    }
  }

  /*
   * Called whenever the action server publishes feedback
   */
  void feedback_callback(
    GoalHandleNavigateToPose::SharedPtr,
    const std::shared_ptr<const NavigateToPose::Feedback> feedback)
  {
    RCLCPP_INFO(
      this->get_logger(),
      "Feedback: current_x=%.2f, current_y=%.2f, distance=%.2f",
      feedback->current_x,
      feedback->current_y,
      feedback->distance_to_goal);
  }

  /*
   * Called when the action server finishes processing the goal
   */
  void result_callback(const GoalHandleNavigateToPose::WrappedResult & result)
  {
    switch (result.code) {
      case rclcpp_action::ResultCode::SUCCEEDED:
        RCLCPP_INFO(this->get_logger(), "Result: %s", result.result->message.c_str());
        break;

      case rclcpp_action::ResultCode::ABORTED:
        RCLCPP_ERROR(this->get_logger(), "Goal aborted.");
        break;

      case rclcpp_action::ResultCode::CANCELED:
        RCLCPP_WARN(this->get_logger(), "Goal canceled.");
        break;

      default:
        RCLCPP_ERROR(this->get_logger(), "Unknown result code.");
        break;
    }

    rclcpp::shutdown();
  }
};

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);

  // Check the correct number of arguments
  if (argc != 3 && argc != 4) {
    RCLCPP_ERROR(
      rclcpp::get_logger("navigate_to_pose_client"),
      "Example of Use: ros2 run ros2_fund_t7_action navigate_to_cli <x> <y> [<cancel_after_seconds>]");
    return 1;
  }

  auto client = std::make_shared<NavigateToPoseClient>();

  client->send_goal(
    std::atof(argv[1]),
    std::atof(argv[2]));

  // If 4th argument is provided, set up a thread to cancel the goal after that many seconds
  if (argc == 4) {
    const double countdown_to_cancel = std::atof(argv[3]);

    // Start a thread that will sleep for the specified duration and then cancel the goal
    std::thread([client, countdown_to_cancel]() {
      std::this_thread::sleep_for(
        std::chrono::duration<double>(countdown_to_cancel));

      client->cancel_goal();
    }).detach();
  }

  rclcpp::spin(client);
}