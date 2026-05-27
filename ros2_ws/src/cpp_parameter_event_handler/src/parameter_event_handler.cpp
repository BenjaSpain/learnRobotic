#include <memory>

#include "rclcpp/rclcpp.hpp"

class SampleNodeWithParameters : public rclcpp::Node
{
    public:
        SampleNodeWithParameters() : Node("node_with_parameters")
        {
            this->declare_parameter("an_int_param", 0);
            // Double params will be monitored with a parameter event callback, so we can demonstrate monitoring remote node parameters as well as this node's parameters. This parameter is intended to be updated by a remote node, "parameter_blackboard".
            this->declare_parameter("another_double_param", 0.0);

            // Create a parameter subscriber that can be used to monitor parameter changes
            // (for this node's parameters as well as other nodes' parameters)
            param_subscriber_ = std::make_shared<rclcpp::ParameterEventHandler>(this);

            // Set a callback for this node's integer parameter, "an_int_param"
            auto cb = [this](const rclcpp::Parameter & p) {
                RCLCPP_INFO(
                this->get_logger(), "cb: Received an update to parameter \"%s\" of type %s: \"%ld\"",
                p.get_name().c_str(),
                p.get_type_name().c_str(),
                p.as_int());
            };
            // Callback to remote node parameter (parameter_blackboard),
            auto cb2 = [this](const rclcpp::Parameter & p) {
                RCLCPP_INFO(
                this->get_logger(), "cb2: Received an update to parameter \"%s\" of type: %s: \"%.02lf\"",
                p.get_name().c_str(),
                p.get_type_name().c_str(),
                p.as_double());
            };

            // Callback for all parameter events, including remote node parameter events. 
            // This callback will be called for any parameter event
            auto event_cb = [this](const rcl_interfaces::msg::ParameterEvent & parameter_event) {
                RCLCPP_INFO(
                    this->get_logger(), "Received parameter event from node \"%s\"",
                    parameter_event.node.c_str());

                for (const auto& p : parameter_event.changed_parameters) {
                    RCLCPP_INFO(
                    this->get_logger(), "Inside event: \"%s\" changed to %s",
                    p.name.c_str(),
                    rclcpp::Parameter::from_parameter_msg(p).value_to_string().c_str());
                };
            };

            // Set callback for remote node's parameter, "a_double_param" on node "parameter_blackboard"
            auto remote_node_name = std::string("parameter_blackboard");
            auto remote_param_name = std::string("a_double_param");
            cb_handle2_ = param_subscriber_->add_parameter_callback(remote_param_name, cb2, remote_node_name);

            // Callback handle must be captured to ensure the callback remains registered
            cb_handle_ = param_subscriber_->add_parameter_callback("an_int_param", cb);
            // All parameter events callback that are not specific callbacks will be handled here. 
            event_cb_handle_ = param_subscriber_->add_parameter_event_callback(event_cb);
        }

    private:
        std::shared_ptr<rclcpp::ParameterEventHandler> param_subscriber_;
        std::shared_ptr<rclcpp::ParameterCallbackHandle> cb_handle_;            // This node param callback
        std::shared_ptr<rclcpp::ParameterCallbackHandle> cb_handle2_;           // Remote node param callback
        std::shared_ptr<rclcpp::ParameterEventCallbackHandle> event_cb_handle_; // All parameter events callback
};

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<SampleNodeWithParameters>());
  rclcpp::shutdown();

  return 0;
}