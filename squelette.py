# 1. Générer un arbre depuis la position initiale
# 2. Générer un arbre depuis la position cible  
# 3. Connecter les deux arbres de manière probabiliste
# 4. Trouver le chemin le plus court
# 5. Appliquer le lissage pour optimiser la trajectoire

#les importations necessaire
from typing import List, Dict, Any

class RobotMotionPlanner:
    """
    Generic Robot Motion Planner (multi-DOF)
    ----------------------------------------
    Features:
    - Supports any number of degrees of freedom (DOF EXAMPLE 6DOF)
    - Supports obstacles input
    - Modular tree-based planning (RRTConnect)
    - Optional trajectory smoothing
    """

    # initialization method
    def __init__(self, dof: int = 6) -> None:
        """
        Args:
            dof: Number of robot joints (degrees of freedom)
        """
        self.dof = dof
        self.start: List[float] = []
        self.goal: List[float] = [] 
        self.obstacles: List[Dict[str, Any]] = []
        self.tree_start: Any = None
        self.tree_goal: Any = None
        self.path: List[List[float]] = []

    
    # Input Setup, setters
    def set_start(self, start_position: List[float]):
        """Define start position"""
        if len(start_position) != self.dof:
            raise ValueError(f"Start position must have {self.dof} values")
        self.start = start_position

    def set_goal(self, goal_position: List[float]):
        """Define goal position"""
        if len(goal_position) != self.dof:
            raise ValueError(f"Goal position must have {self.dof} values")
        self.goal = goal_position

    def set_obstacles(self, obstacles: List[Dict[str, Any]]):
        """Set environment obstacles"""
        self.obstacles = obstacles

    # Tree-based Planning
  
    def generate_tree(self, root: List[float], tree_name: str = "start"):
        """
        Generate a search tree from the given root.
        Args:
            root: Root configuration (joint angles)
            tree_name: 'start' or 'goal' to distinguish trees
        """
        # TODO: Implement RRTConnect tree generation
        pass

    def connect_trees(self):
        """Connect start and goal trees"""
        # TODO:  Implement connection logic
        pass

    def find_shortest_path(self):
        """Extract the shortest path from start to goal"""
        # TODO: Implement path extraction
        pass
# Trajectory optimization
    def smooth_path(self):
        """Apply smoothing to the found path"""
        #TODO: Implement path smoothing
        pass

    # Main motioin planning method

    def plan_motion(
        self,
        start_position: List[float],
        goal_position: List[float],
        obstacles: List[Dict[str, Any]] = None,
        smoothing: bool = True
    ) -> Dict[str, Any]:
        """
        Plan a full trajectory from start to goal.

        Returns:
            Dictionary containing:
            - joint_angles
            - velocities
            - accelerations
            - time_from_start
            - success: bool
        """

        #input setup
        self.set_start(start_position)
        self.set_goal(goal_position)
        if obstacles:
            self.set_obstacles(obstacles)

        # Step 1: Generate trees
        self.generate_tree(self.start, tree_name="start")
        self.generate_tree(self.goal, tree_name="goal")
        # Step 2: Connect trees
        self.connect_trees()
        # Step 3: Extract shortest path
        self.find_shortest_path()
        # Step 4: Apply smoothing
        if smoothing:
            self.smooth_path()
        # Step 5: Format output
        trajectory_data = {
            "joint_angles": self.path,
            "velocities": [],  # to be computed
            "accelerations": [],  # to be computed
            "time_from_start": [],  # to be computed
            "success": bool(self.path)
        }
        return trajectory_data


#computing outputs HELPER FUNCTION
def compute_joint_derivatives(joint_trajectory: List[List[float]],
    time_stamps: List[float]
) -> Dict[str, List[List[float]]]:
    """
    Compute velocities and accelerations from joint trajectory and time stamps.

    Args:
        joint_trajectory: List of joint angle configurations
        time_stamps: Corresponding time stamps for each configuration
    Returns:
        Dictionary with 
            - 'angles': input joint trajectory
            - 'velocities': first derivative of angles [rad/s]
            - 'accelerations': second derivative of angles [rad/s^2]
            - 'time': same as input
    """

    # TODO: 1. Validate Inputs
    n_points = len(joint_trajectory)
    n_joints = len(joint_trajectory[0])
    if n_points < 2:
        raise ValueError("Trajectory must have at least two points.")

    # TODO: 2. Initialize Output Containers
    velocities: List[List[float]] = []
    accelerations: List[List[float]] = []

    # TODO: 3. Compute Velocities (First Derivative)
    # TODO: 4. Compute Accelerations (Second Derivative)
    # TODO: 5. Return Results as a dictionary
    
    return {
        'angles': joint_trajectory,
        'velocities': velocities,
        'accelerations': accelerations,
        'time': time_stamps
    }


#Steps after
#TODO: implement trajectory excexution after planning
#TODO: combine planning and executing on the robot
