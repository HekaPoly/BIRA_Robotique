
#include <memory>
#include <thread>
#include <vector>
#include <string>
#include <iostream>
#include <cmath>

#include <rclcpp/rclcpp.hpp>
#include <geometry_msgs/msg/pose.hpp>
#include <geometry_msgs/msg/pose_stamped.hpp>

#include <moveit/move_group_interface/move_group_interface.h>
#include <moveit/robot_state/robot_state.h>
#include <moveit/robot_model/joint_model_group.h>
#include <moveit/planning_scene_interface/planning_scene_interface.h>

#include <moveit_msgs/msg/robot_trajectory.hpp>
#include <moveit_msgs/msg/collision_object.hpp>
#include <moveit_msgs/msg/orientation_constraint.hpp>
#include <moveit_msgs/msg/constraints.hpp>

#include <shape_msgs/msg/solid_primitive.hpp>

#include <tf2/LinearMath/Quaternion.h>
#include <tf2_geometry_msgs/tf2_geometry_msgs.hpp>

using MoveGroup =
    moveit::planning_interface::MoveGroupInterface;

using Plan = MoveGroup::Plan;



// --------------------------------------------------
// COMMON FUNCTIONS
// --------------------------------------------------

void printPose(
    const rclcpp::Logger& logger,
    const std::string& label,
    const geometry_msgs::msg::Pose& pose)
{
    RCLCPP_INFO(logger,
        "%s: x=%.3f y=%.3f z=%.3f",
        label.c_str(),
        pose.position.x,
        pose.position.y,
        pose.position.z);
}


void printJoints(
    const rclcpp::Logger& logger,
    const std::vector<double>& joints,
    const std::string& label)
{
    RCLCPP_INFO(logger, "%s", label.c_str());

    for (size_t i = 0; i < joints.size(); ++i)
    {
        RCLCPP_INFO(logger,
            "Joint %zu = %.4f rad (%.2f degrees)",
            i + 1,
            joints[i],
            joints[i] * 180.0 / M_PI);
    }
}


void executePose(
    MoveGroup& group,
    const geometry_msgs::msg::Pose& target,
    const rclcpp::Logger& logger)
{
    group.clearPathConstraints();
    group.setStartStateToCurrentState();
    group.setPoseTarget(target);

    Plan plan;

    auto result = group.plan(plan);

    if (static_cast<bool>(result))
    {
        RCLCPP_INFO(logger, "Planning succeeded!");
        RCLCPP_INFO(logger,
            "Planning time: %.3f seconds",
            plan.planning_time_);

        RCLCPP_INFO(logger, "Executing...");

        auto execution = group.execute(plan);

        RCLCPP_INFO(logger,
            "Execution code: %d",
            execution.val);
    }
    else
    {
        RCLCPP_ERROR(logger, "Planning failed!");
    }

    group.clearPoseTargets();
}


void executeJointTarget(
    MoveGroup& group,
    const std::vector<double>& joints,
    const rclcpp::Logger& logger)
{
    group.setStartStateToCurrentState();

    if (!group.setJointValueTarget(joints))
    {
        RCLCPP_ERROR(logger, "Invalid joint target!");
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


// --------------------------------------------------
// EXPERIMENT 1: CARTESIAN XYZ MOVEMENT
// --------------------------------------------------

void scenario1(
    MoveGroup& group,
    const geometry_msgs::msg::Pose& start,
    const rclcpp::Logger& logger)
{
    RCLCPP_INFO(logger, "EXPERIMENT 1: XYZ MOVEMENT");

    geometry_msgs::msg::Pose target = start;

    target.position.x += 0.05;
    target.position.y += 0.05;
    target.position.z += 0.05;

    printPose(logger, "Start", start);
    printPose(logger, "Target", target);

    executePose(group, target, logger);
}


// --------------------------------------------------
// EXPERIMENT 2: ORIENTATION
// --------------------------------------------------

void scenario2(
    MoveGroup& group,
    const geometry_msgs::msg::Pose& start,
    const rclcpp::Logger& logger)
{
    RCLCPP_INFO(logger, "EXPERIMENT 2: ORIENTATION");

    geometry_msgs::msg::Pose target = start;

    tf2::Quaternion initial;
    tf2::fromMsg(start.orientation, initial);

    tf2::Quaternion rotation;
    rotation.setRPY(0.0, 0.0, M_PI / 4.0);

    tf2::Quaternion combined = rotation * initial;
    combined.normalize();

    target.orientation = tf2::toMsg(combined);

    printPose(logger, "Target position", target);

    executePose(group, target, logger);
}


// --------------------------------------------------
// EXPERIMENT 3: JOINT-SPACE MOVEMENT
// --------------------------------------------------

void scenario3(
    MoveGroup& group,
    const rclcpp::Logger& logger)
{
    RCLCPP_INFO(logger, "EXPERIMENT 3: JOINT SPACE");

    auto state = group.getCurrentState(2.0);

    if (!state)
    {
        RCLCPP_ERROR(logger, "Could not get robot state");
        return;
    }

    const auto* jmg =
        state->getJointModelGroup("panda_arm");

    if (!jmg)
    {
        RCLCPP_ERROR(logger, "Joint group not found");
        return;
    }

    std::vector<double> joints;

    state->copyJointGroupPositions(jmg, joints);

    printJoints(logger, joints, "Current configuration");

    // Change only the first arm joint by a small amount.
    joints[0] += 0.10;

    printJoints(logger, joints, "Target configuration");

    executeJointTarget(group, joints, logger);
}


// --------------------------------------------------
// EXPERIMENT 4: MULTIPLE IK SOLUTIONS
// --------------------------------------------------

void scenario4(
    MoveGroup& group,
    const geometry_msgs::msg::Pose& target,
    const rclcpp::Logger& logger)
{
    RCLCPP_INFO(logger, "EXPERIMENT 4: MULTIPLE IK SOLUTIONS");

    const std::string tip = group.getEndEffectorLink();

    for (int attempt = 0; attempt < 3; ++attempt)
    {
        auto state = group.getCurrentState(2.0);

        if (!state)
            continue;

        const auto* jmg =
            state->getJointModelGroup("panda_arm");

        if (!jmg)
            continue;

        std::vector<double> seed;
        state->copyJointGroupPositions(jmg, seed);

        // Try different initial configurations.
        if (attempt == 1)
            seed[0] += 0.20;

        if (attempt == 2)
            seed[2] -= 0.20;

        state->setJointGroupPositions(jmg, seed);

        bool found = state->setFromIK(
            jmg,
            target,
            tip,
            2.0
        );

        if (found)
        {
            std::vector<double> solution;

            state->copyJointGroupPositions(
                jmg,
                solution
            );

            printJoints(
                logger,
                solution,
                "IK solution attempt " + std::to_string(attempt + 1)
            );
        }
        else
        {
            RCLCPP_WARN(logger,
                "No IK solution for attempt %d",
                attempt + 1);
        }
    }
}


// --------------------------------------------------
// EXPERIMENT 5: DIRECT IK
// --------------------------------------------------

void scenario5(
    MoveGroup& group,
    const geometry_msgs::msg::Pose& target,
    const rclcpp::Logger& logger)
{
    RCLCPP_INFO(logger, "EXPERIMENT 5: DIRECT IK");

    auto state = group.getCurrentState(2.0);

    if (!state)
    {
        RCLCPP_ERROR(logger, "Could not get robot state");
        return;
    }

    const auto* jmg =
        state->getJointModelGroup("panda_arm");

    if (!jmg)
        return;

    const std::string tip = group.getEndEffectorLink();

    bool found = state->setFromIK(
        jmg,
        target,
        tip,
        2.0
    );

    if (found)
    {
        std::vector<double> solution;

        state->copyJointGroupPositions(
            jmg,
            solution
        );

        printJoints(logger, solution, "Calculated IK solution");

        RCLCPP_INFO(logger,
            "IK succeeded. Robot has NOT moved.");
    }
    else
    {
        RCLCPP_WARN(logger, "IK failed!");
    }
}


// --------------------------------------------------
// EXPERIMENT 6: CARTESIAN PATH
// --------------------------------------------------

void scenario6(
    MoveGroup& group,
    const geometry_msgs::msg::Pose& start,
    const rclcpp::Logger& logger)
{
    RCLCPP_INFO(logger, "EXPERIMENT 6: CARTESIAN PATH");

    std::vector<geometry_msgs::msg::Pose> waypoints;

    geometry_msgs::msg::Pose p1 = start;
    p1.position.z += 0.05;

    geometry_msgs::msg::Pose p2 = p1;
    p2.position.y += 0.05;

    geometry_msgs::msg::Pose p3 = p2;
    p3.position.z -= 0.05;

    waypoints.push_back(p1);
    waypoints.push_back(p2);
    waypoints.push_back(p3);

    moveit_msgs::msg::RobotTrajectory trajectory;

    double fraction = group.computeCartesianPath(
        waypoints,
        0.01,
        0.0,
        trajectory
    );

    RCLCPP_INFO(logger,
        "Cartesian completion: %.1f%%",
        fraction * 100.0);

    if (fraction >= 0.999)
    {
        RCLCPP_INFO(logger, "Executing Cartesian trajectory");

        group.execute(trajectory);
    }
    else
    {
        RCLCPP_WARN(logger,
            "Incomplete Cartesian path. Not executing.");
    }
}


// --------------------------------------------------
// EXPERIMENT 7: OBSTACLE PLANNING
// --------------------------------------------------

void scenario7(
    MoveGroup& group,
    const geometry_msgs::msg::Pose& start,
    const rclcpp::Logger& logger)
{
    RCLCPP_INFO(logger, "EXPERIMENT 7: OBSTACLE AVOIDANCE");

    moveit::planning_interface::PlanningSceneInterface scene;

    moveit_msgs::msg::CollisionObject obstacle;

    obstacle.header.frame_id = group.getPlanningFrame();
    obstacle.id = "experiment_obstacle";

    shape_msgs::msg::SolidPrimitive box;

    box.type = box.BOX;
    box.dimensions = {0.10, 0.10, 0.10};

    geometry_msgs::msg::Pose obstacle_pose;

    obstacle_pose.position.x = start.position.x;
    obstacle_pose.position.y = start.position.y + 0.075;
    obstacle_pose.position.z = start.position.z;

    obstacle_pose.orientation.w = 1.0;

    obstacle.primitives.push_back(box);
    obstacle.primitive_poses.push_back(obstacle_pose);

    obstacle.operation = obstacle.ADD;

    scene.applyCollisionObject(obstacle);

    RCLCPP_INFO(logger, "Obstacle added to Planning Scene.");

    geometry_msgs::msg::Pose target = start;

    target.position.y += 0.15;

    executePose(group, target, logger);

    RCLCPP_INFO(logger,
        "Obstacle remains in the Planning Scene.");
}


// --------------------------------------------------
// EXPERIMENT 8: ORIENTATION CONSTRAINT
// --------------------------------------------------

void scenario8(
    MoveGroup& group,
    const geometry_msgs::msg::Pose& start,
    const rclcpp::Logger& logger)
{
    RCLCPP_INFO(logger, "EXPERIMENT 8: ORIENTATION CONSTRAINT");

    moveit_msgs::msg::OrientationConstraint orientation;

    orientation.header.frame_id = group.getPlanningFrame();

    orientation.link_name = group.getEndEffectorLink();

    orientation.orientation = start.orientation;

    orientation.absolute_x_axis_tolerance = 0.10;
    orientation.absolute_y_axis_tolerance = 0.10;
    orientation.absolute_z_axis_tolerance = 0.10;

    orientation.weight = 1.0;

    moveit_msgs::msg::Constraints constraints;

    constraints.orientation_constraints.push_back(
        orientation
    );

    group.setPathConstraints(constraints);

    geometry_msgs::msg::Pose target = start;

    target.position.z += 0.05;
    target.position.y += 0.05;

    executePose(group, target, logger);

    group.clearPathConstraints();
}


// --------------------------------------------------
// EXPERIMENT 9: PLANNER COMPARISON
// --------------------------------------------------

void scenario9(
    MoveGroup& group,
    const geometry_msgs::msg::Pose& target,
    const rclcpp::Logger& logger,
    const std::string& planner_id)
{
    RCLCPP_INFO(logger, "EXPERIMENT 9: PLANNER COMPARISON");

    group.setPlannerId(planner_id);

    group.setPlanningTime(5.0);
    group.setNumPlanningAttempts(5);

    group.setStartStateToCurrentState();
    group.setPoseTarget(target);

    Plan plan;

    auto result = group.plan(plan);

    if (static_cast<bool>(result))
    {
        const auto& points =
            plan.trajectory_.joint_trajectory.points;

        RCLCPP_INFO(logger, "Planner: %s", planner_id.c_str());

        RCLCPP_INFO(logger,
            "Planning time: %.4f",
            plan.planning_time_);

        RCLCPP_INFO(logger,
            "Number of trajectory points: %zu",
            points.size());

        RCLCPP_INFO(logger, "Planning succeeded.");
    }
    else
    {
        RCLCPP_WARN(logger, "Planning failed.");
    }

    group.clearPoseTargets();
}


// --------------------------------------------------
// EXPERIMENT 10: IK SEED EXPLORATION
// --------------------------------------------------

void scenario10(
    MoveGroup& group,
    const geometry_msgs::msg::Pose& target,
    const rclcpp::Logger& logger)
{
    RCLCPP_INFO(logger, "EXPERIMENT 10: IK SEEDS");

    const std::string tip = group.getEndEffectorLink();

    for (int attempt = 0; attempt < 3; ++attempt)
    {
        auto state = group.getCurrentState(2.0);

        if (!state)
            continue;

        const auto* jmg =
            state->getJointModelGroup("panda_arm");

        if (!jmg)
            continue;

        std::vector<double> seed;

        state->copyJointGroupPositions(jmg, seed);

        if (attempt == 1)
            seed[0] += 0.15;

        if (attempt == 2)
            seed[4] -= 0.15;

        state->setJointGroupPositions(jmg, seed);

        bool found = state->setFromIK(
            jmg,
            target,
            tip,
            2.0
        );

        if (found)
        {
            std::vector<double> solution;

            state->copyJointGroupPositions(jmg, solution);

            printJoints(
                logger,
                solution,
                "Seed experiment " + std::to_string(attempt + 1)
            );
        }
        else
        {
            RCLCPP_WARN(logger,
                "Seed %d failed",
                attempt + 1);
        }
    }
}


// --------------------------------------------------
// MAIN
// --------------------------------------------------

int main(int argc, char* argv[])
{
    rclcpp::init(argc, argv);

    auto node = std::make_shared<rclcpp::Node>(
        "moveit_experiments",
        rclcpp::NodeOptions()
            .automatically_declare_parameters_from_overrides(true)
    );

    auto logger = node->get_logger();

    rclcpp::executors::SingleThreadedExecutor executor;

    executor.add_node(node);

    std::thread spinner([&executor]()
    {
        executor.spin();
    });

    try
    {
        MoveGroup group(node, "panda_arm");

        group.setMaxVelocityScalingFactor(0.10);
        group.setMaxAccelerationScalingFactor(0.10);

        group.setPlanningTime(5.0);

        std::string tip = group.getEndEffectorLink();

        RCLCPP_INFO(logger, "Planning frame: %s",
                    group.getPlanningFrame().c_str());

        RCLCPP_INFO(logger, "End effector link: %s",
                    tip.c_str());

        geometry_msgs::msg::Pose start =
            group.getCurrentPose(tip).pose;

        printPose(logger, "Current end effector pose", start);

        int scenario = 1;

        if (argc >= 2)
            scenario = std::stoi(argv[1]);

        switch (scenario)
        {
            case 1:
                scenario1(group, start, logger);
                break;

            case 2:
                scenario2(group, start, logger);
                break;

            case 3:
                scenario3(group, logger);
                break;

            case 4:
                scenario4(group, start, logger);
                break;

            case 5:
                scenario5(group, start, logger);
                break;

            case 6:
                scenario6(group, start, logger);
                break;

            case 7:
                scenario7(group, start, logger);
                break;

            case 8:
                scenario8(group, start, logger);
                break;

            case 9:
            {
                std::string planner = "RRTConnectkConfigDefault";

                if (argc >= 3)
                    planner = argv[2];

                geometry_msgs::msg::Pose target = start;
                target.position.y += 0.05;

                scenario9(group, target, logger, planner);
                break;
            }

            case 10:
                scenario10(group, start, logger);
                break;

            default:
                RCLCPP_ERROR(logger, "Unknown scenario: %d", scenario);
                break;
        }
    }
    catch (const std::exception& e)
    {
        RCLCPP_ERROR(logger, "Exception: %s", e.what());
    }

    rclcpp::shutdown();

    if (spinner.joinable())
        spinner.join();

    return 0;
}
