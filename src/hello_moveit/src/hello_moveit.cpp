#include <cstddef>
#include <geometry_msgs/msg/detail/pose__struct.hpp>
#include <memory>
#include <rclcpp/logger.hpp>
#include <rclcpp/logging.hpp>
#include <string>
#include <tf2/LinearMath/Quaternion.hpp>
#include <tf2_geometry_msgs/tf2_geometry_msgs.hpp>
#include <thread>
#include <rclcpp/rclcpp.hpp>
#include <geometry_msgs/msg/pose.hpp>
#include <moveit/move_group_interface/move_group_interface.h>
#include <vector>
#include <cmath>


using MoveGroup = moveit::planning_interface::MoveGroupInterface;
using Plan = MoveGroup::Plan;

void print_pose(const rclcpp::Logger& logger, const std::string& label, const geometry_msgs::msg::Pose& pose) 
{
    RCLCPP_INFO(logger, "%s: x=%.3f y=%.3f z=%.3f", label.c_str(), pose.position.x, pose.position.y, pose.position.z);
}

void print_joints(const rclcpp::Logger& logger, const std::vector<double>& joints, const std::string& label)
{
    RCLCPP_INFO(logger, "%s", label.c_str());
    for(size_t i = 0; i < joints.size(); ++i)
        RCLCPP_INFO(logger, "Joint %zu = %.4f rad (%.2f degrees)", i + 1, joints[i], joints[i] * 180.0 / M_PI);
}

void execute_pose(MoveGroup& group, const geometry_msgs::msg::Pose& target, const rclcpp::Logger& logger)
{
    group.clearPathConstraints();
    group.setStartStateToCurrentState();
    group.setPoseTarget(target);

    Plan plan;
    auto result = group.plan(plan);

    if(static_cast<bool>(result))
    {
        RCLCPP_INFO(logger, "Planning succeeded!");
        RCLCPP_INFO(logger, "Planning time: %.3f seconds", plan.planning_time_);
        RCLCPP_INFO(logger, "Executing...");

        auto execution = group.execute(plan);

        RCLCPP_INFO(logger, "Execution code: %d", execution.val);
    }
    else 
    {
        RCLCPP_ERROR(logger, "Planning failed!");
    }

    group.clearPoseTargets();
}

void execute_joint_targets(MoveGroup& group, const std::vector<double>& joints, const rclcpp::Logger& logger)
{
    group.setStartStateToCurrentState();

    if(!group.setJointValueTarget(joints))
    {
        RCLCPP_ERROR(logger, "Invalid joint target");
        return;
    }

    Plan plan;
    
    auto result = group.plan(plan);
    if (static_cast<bool>(result))
    {
        RCLCPP_INFO(logger, "Joint-space planning succeeded!");
        group.execute(plan);
    }
    else 
    {
        RCLCPP_ERROR(logger, "Joint-space planning failed!");
    }
}

void cartesian_movement(MoveGroup& group, const geometry_msgs::msg::Pose& start, const rclcpp::Logger& logger)
{
    RCLCPP_INFO(logger, "XYZ MOVEMENT");

    geometry_msgs::msg::Pose target = start;

    target.position.x += 0.05;
    target.position.y += 0.05;
    target.position.z += 0.05;
    
    print_pose(logger, "Start", start);
    print_pose(logger, "Target", target);

    execute_pose(group, target, logger);
}

void orientation(MoveGroup& group, const geometry_msgs::msg::Pose& start, const rclcpp::Logger& logger)
{
    RCLCPP_INFO(logger, "ORIENTATION");

    geometry_msgs::msg::Pose target = start;

    tf2::Quaternion initial;
    tf2::fromMsg(start.orientation, initial);

    tf2::Quaternion rotation;
    rotation.setRPY(0.0, 0.0, M_PI / 4.0);

    tf2::Quaternion combined = rotation * initial;
    combined.normalize();

    target.orientation = tf2::toMsg(combined);

    print_pose(logger, "Target position", target);

    execute_pose(group, target, logger);
}

void joint_space_movement(MoveGroup& group, const rclcpp::Logger& logger)
{
    RCLCPP_INFO(logger, "JOINT SPACE");
    auto state = group.getCurrentState(2.0);

    if(!state)
    {
        RCLCPP_ERROR(logger, "Could not get robot state");
        return;
    }

    const auto* joint_model_group = state->getJointModelGroup("panda_arm");

    if(!joint_model_group)
    {
        RCLCPP_ERROR(logger, "Joint group not found");
        return;
    }

    std::vector<double> joints;
    state->copyJointGroupPositions(joint_model_group, joints);

    print_joints(logger, joints, "Current configuration");

    joints[0] +=10;

    print_joints(logger, joints, "Target configuration");

    execute_joint_targets(group, joints, logger);
}

void IK_solutions(MoveGroup& group, const geometry_msgs::msg::Pose& target, const rclcpp::Logger& logger)
{
    RCLCPP_INFO(logger, "MULTIPLE IK SOLUTIONS");

    const std::string tip = group.getEndEffectorLink();

    for (int attempt = 0; attempt < 3; ++attempt)
    {
        auto state = group.getCurrentState(2.0);
        if(!state) continue;

        const auto* joint_model_group = state->getJointModelGroup("panda_arm");
        if(!joint_model_group) continue;

        auto solver = joint_model_group->getSolverInstance();

        if (!solver)
        {
            RCLCPP_ERROR(logger, "NO IK SOLVER LOADED FOR panda_arm");
            continue;
        }

        RCLCPP_INFO(logger, "IK solver loaded!");
        RCLCPP_INFO(logger, "IK solver tip: %s", solver->getTipFrame().c_str());

        std::vector<double> seed;
        state->copyJointGroupPositions(joint_model_group, seed);

        if (attempt == 1) seed[0] += 0.20;
        if (attempt == 2) seed[2] -= 0.20;

        state->setJointGroupPositions(joint_model_group, seed);
        bool found = state->setFromIK(joint_model_group, target, tip, 2.0);

        if (found) 
        {
            std::vector<double> solution;

            state->copyJointGroupPositions(joint_model_group, solution);
            print_joints(logger, solution, "IK solution attempt " + std::to_string(attempt + 1));
        }
        else 
        {
            RCLCPP_WARN(logger, "No IK solution for attempt %d", attempt + 1);
        }
    }
}

void direct_IK(MoveGroup& group, const geometry_msgs::msg::Pose& target, const rclcpp::Logger& logger)
{
    RCLCPP_INFO(logger, "DIRECT IK");
    auto state = group.getCurrentState(2.0);

    if (!state)
    {
        RCLCPP_ERROR(logger, "Could not get robot state");
        return;
    }

    const auto* joint_model_group = state->getJointModelGroup("panda_arm");
    if(!joint_model_group) return;

    const std::string tip = group.getEndEffectorLink();
    bool found = state->setFromIK(joint_model_group, target, tip, 2.0);

    if (found)
    {
        std::vector<double> solution;

        state->copyJointGroupPositions(joint_model_group, solution);

        print_joints(logger, solution, "calculated IK solution");
        RCLCPP_INFO(logger, "IK succeeded.");
    }
    else 
    {
        RCLCPP_WARN(logger, "IK failed!");
    }
}


int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::NodeOptions options;
    options.automatically_declare_parameters_from_overrides(true);

    options.parameter_overrides({
        rclcpp::Parameter("robot_description_kinematics.panda_arm.kinematics_solver", "kdl_kinematics_plugin/KDLKinematicsPlugin"),

        rclcpp::Parameter("robot_description_kinematics.panda_arm.kinematics_solver_search_resolution", 0.005),
        rclcpp::Parameter("robot_description_kinematics.panda_arm.kinematics_solver_timeout", 0.05)
    });
    auto node = std::make_shared<rclcpp::Node>("hello_moveit", options);
    
    rclcpp::Logger logger = node->get_logger();
    rclcpp::executors::SingleThreadedExecutor executor;
    executor.add_node(node);

    std::thread spinner([&executor]()
    {
        executor.spin();
    });

    MoveGroup group(node, "panda_arm");

    group.setMaxVelocityScalingFactor(0.10);
    group.setMaxAccelerationScalingFactor(0.10);
    group.setPlanningTime(5.0);

    std::string tip = group.getEndEffectorLink();

    RCLCPP_INFO(logger, "Planning frame: %s", group.getPlanningFrame().c_str());
    RCLCPP_INFO(logger, "End effector link: %s", tip.c_str());

    geometry_msgs::msg::Pose start = group.getCurrentPose(tip).pose;

    print_pose(logger, "Current end effector pose", start);
    
    cartesian_movement(group, start, logger);
    


    rclcpp::shutdown();
    spinner.join();
    return 0;
}