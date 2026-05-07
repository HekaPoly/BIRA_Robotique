#include <rclcpp/rclcpp.hpp>
#include <moveit/planning_pipeline/planning_pipeline.h>
#include <moveit/robot_model_loader/robot_model_loader.h>
#include <moveit/kinematic_constraints/utils.h>
#include <moveit/robot_state/conversions.h>
#include <bira_motion_planning/srv/plan_trajectory.hpp>

#include <mutex>

/*
 * Serveur de planification BIRA
 */
class MotionPlanningServer : public rclcpp::Node {
public:
  MotionPlanningServer() : Node("motion_planning_server",
      rclcpp::NodeOptions().allow_undeclared_parameters(true)
                          .automatically_declare_parameters_from_overrides(true)) {
      RCLCPP_INFO(this->get_logger(), "Nœud créé. En attente d'initialisation...");
  }

  void initialize() {
    RCLCPP_INFO(this->get_logger(), "Chargement du RobotModel depuis 'robot_description'...");

    // 1. Chargement du modèle
    robot_model_loader_ = std::make_shared<robot_model_loader::RobotModelLoader>(
        shared_from_this(), "robot_description");
    
    auto robot_model = robot_model_loader_->getModel();
    if (!robot_model) {
      throw std::runtime_error("Le RobotModel n'a pas pu être chargé. Vérifiez vos paramètres Launch.");
    }

    std::string plugin = this->get_parameter("planning_plugin").as_string();
    RCLCPP_INFO(this->get_logger(), "Planning plugin lu : %s", plugin.c_str());

    // 2. Configuration de la Pipeline (Plugin OMPL + Adapters)
    planning_pipeline_ = std::make_shared<planning_pipeline::PlanningPipeline>(
        robot_model,
        shared_from_this(),
        "",  // parameter_namespace
        "ompl_interface/OMPLPlanner",
        std::vector<std::string>{
            "default_planner_request_adapters/AddTimeOptimalParameterization",
            "default_planner_request_adapters/FixWorkspaceBounds",
            "default_planner_request_adapters/FixStartStateBounds",
            "default_planner_request_adapters/FixStartStateCollision",
            "default_planner_request_adapters/FixStartStatePathConstraints"
        }
    );

    // 3. Initialisation de la scène de planification (vide)
    planning_scene_ = std::make_shared<planning_scene::PlanningScene>(robot_model);

    // 4. Création du Service
    service_ = this->create_service<bira_motion_planning::srv::PlanTrajectory>(
        "plan_kinematic_path",
        std::bind(&MotionPlanningServer::handle_planning_request, this, std::placeholders::_1, std::placeholders::_2));

    RCLCPP_INFO(this->get_logger(), "Service de planification BIRA opérationnel.");
  }

private:
  void handle_planning_request(
      const std::shared_ptr<bira_motion_planning::srv::PlanTrajectory::Request> request,
      std::shared_ptr<bira_motion_planning::srv::PlanTrajectory::Response> response) {

    RCLCPP_INFO(this->get_logger(), "Requête reçue. Calcul en cours...");

    planning_interface::MotionPlanRequest req;
    planning_interface::MotionPlanResponse res;

    // Configuration OMPL
    req.group_name = "meca500_arm"; 
    req.allowed_planning_time = 5.0;
    req.num_planning_attempts = 5;

    // --- CONSTRUCTION DU START STATE ---
    moveit::core::RobotState start_state(planning_scene_->getRobotModel());
    
    if (request->start_joints.name.size() != request->start_joints.position.size()) {
      RCLCPP_ERROR(this->get_logger(), "Erreur : Taille des noms et positions des joints incohérente.");
      return;
    }

    for (size_t i = 0; i < request->start_joints.name.size(); ++i) {
      start_state.setJointPositions(request->start_joints.name[i], &request->start_joints.position[i]);
    }
    start_state.update(); // Met à jour la cinématique directe

    // Conversion pour la requête
    moveit::core::robotStateToRobotStateMsg(start_state, req.start_state);

    // --- CONSTRUCTION DU GOAL ---
    const double tol_pos = 0.01; 
    const double tol_ang = 0.02;
    
    auto goal_constraints = kinematic_constraints::constructGoalConstraints(
        "meca_axis_6_link", request->final_pos, tol_pos, tol_ang);
    req.goal_constraints.push_back(goal_constraints);

    // --- CALCUL (THREAD-SAFE) ---
    {
      std::lock_guard<std::mutex> lock(planning_mutex_);
      // On synchronise la scène avec l'état de départ pour le détecteur de collision
      planning_scene_->setCurrentState(start_state);
      planning_pipeline_->generatePlan(planning_scene_, req, res);
    }

    // --- RÉPONSE ---
    if (res.error_code_.val == moveit_msgs::msg::MoveItErrorCodes::SUCCESS) {
      res.getMessage(response->traj);
      RCLCPP_INFO(this->get_logger(), "Succès ! Temps de calcul : %f s", res.planning_time_);
    } else {
      RCLCPP_ERROR(this->get_logger(), "Échec OMPL. Code erreur : %d", res.error_code_.val);
    }
  }

  // Membres
  std::shared_ptr<robot_model_loader::RobotModelLoader> robot_model_loader_;
  std::shared_ptr<planning_pipeline::PlanningPipeline> planning_pipeline_;
  std::shared_ptr<planning_scene::PlanningScene> planning_scene_;
  rclcpp::Service<bira_motion_planning::srv::PlanTrajectory>::SharedPtr service_;
  std::mutex planning_mutex_;
};

int main(int argc, char** argv) {
  rclcpp::init(argc, argv);
  
  // Instanciation via shared_ptr obligatoire pour enable_shared_from_this
  auto node = std::make_shared<MotionPlanningServer>();
  
  try {
    node->initialize();
  } catch (const std::exception& e) {
    RCLCPP_FATAL(node->get_logger(), "CRASH INITIALISATION : %s", e.what());
    return 1;
  }

  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}