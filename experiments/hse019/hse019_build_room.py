import json
import os

def create_room_project():
    project = {
        "name": "HSE-019 Room",
        "scene_name": "FurnishedRoom",
        "materials": [
            {"id": "mat_floor", "albedo": [0.4, 0.3, 0.2], "roughness": 0.8, "metallic": 0.0},
            {"id": "mat_wall", "albedo": [0.8, 0.75, 0.7], "roughness": 0.6, "metallic": 0.0},
            {"id": "mat_ceiling", "albedo": [0.9, 0.9, 0.9], "roughness": 0.9, "metallic": 0.0},
            {"id": "mat_wood", "albedo": [0.3, 0.2, 0.1], "roughness": 0.7, "metallic": 0.0},
            {"id": "mat_fabric", "albedo": [0.2, 0.4, 0.6], "roughness": 0.9, "metallic": 0.0}
        ],
        "objects": [
            # Structure
            {"id": "floor", "type": "Quad", "position": [0, -1.5, 0], "rotation": [-90, 0, 0], "scale": [6, 6, 1], "material": "mat_floor"},
            {"id": "ceiling", "type": "Quad", "position": [0, 1.5, 0], "rotation": [90, 0, 0], "scale": [6, 6, 1], "material": "mat_ceiling"},
            {"id": "wall_back", "type": "Quad", "position": [0, 0, -3], "rotation": [0, 0, 0], "scale": [6, 3, 1], "material": "mat_wall"},
            {"id": "wall_left", "type": "Quad", "position": [-3, 0, 0], "rotation": [0, 90, 0], "scale": [6, 3, 1], "material": "mat_wall"},
            {"id": "wall_right", "type": "Quad", "position": [3, 0, 0], "rotation": [0, -90, 0], "scale": [6, 3, 1], "material": "mat_wall"},
            
            # Front wall with Doorway
            {"id": "wall_front_left", "type": "Quad", "position": [-2, 0, 3], "rotation": [0, 180, 0], "scale": [2, 3, 1], "material": "mat_wall"},
            {"id": "wall_front_right", "type": "Quad", "position": [2, 0, 3], "rotation": [0, 180, 0], "scale": [2, 3, 1], "material": "mat_wall"},
            {"id": "wall_front_top", "type": "Quad", "position": [0, 1, 3], "rotation": [0, 180, 0], "scale": [2, 1, 1], "material": "mat_wall"},
            
            # Window on back wall (simulated by a child object or just a darker quad)
            {"id": "window_frame", "type": "Quad", "position": [0, 0.5, -2.99], "rotation": [0, 0, 0], "scale": [2, 1.5, 1], "material": "mat_fabric"},
            
            # Furniture: Table
            {"id": "table_top", "type": "Cube", "position": [0, -0.7, 0], "rotation": [0, 0, 0], "scale": [2, 0.1, 1.2], "material": "mat_wood"},
            {"id": "table_leg_1", "type": "Cube", "position": [-0.9, -1.1, -0.5], "rotation": [0, 0, 0], "scale": [0.1, 0.8, 0.1], "material": "mat_wood"},
            {"id": "table_leg_2", "type": "Cube", "position": [0.9, -1.1, -0.5], "rotation": [0, 0, 0], "scale": [0.1, 0.8, 0.1], "material": "mat_wood"},
            {"id": "table_leg_3", "type": "Cube", "position": [-0.9, -1.1, 0.5], "rotation": [0, 0, 0], "scale": [0.1, 0.8, 0.1], "material": "mat_wood"},
            {"id": "table_leg_4", "type": "Cube", "position": [0.9, -1.1, 0.5], "rotation": [0, 0, 0], "scale": [0.1, 0.8, 0.1], "material": "mat_wood"},
            
            # Furniture: Chairs
            {"id": "chair_1", "type": "Cube", "position": [-1.5, -1.0, 0], "rotation": [0, 90, 0], "scale": [0.6, 0.1, 0.6], "material": "mat_fabric", "rotation_speed": [0, 45, 0]},
            {"id": "chair_1_back", "type": "Cube", "position": [-1.8, -0.6, 0], "rotation": [0, 90, 0], "scale": [0.1, 0.8, 0.6], "material": "mat_fabric"},
            
            {"id": "chair_2", "type": "Cube", "position": [1.5, -1.0, 0], "rotation": [0, -90, 0], "scale": [0.6, 0.1, 0.6], "material": "mat_fabric"},
            {"id": "chair_2_back", "type": "Cube", "position": [1.8, -0.6, 0], "rotation": [0, -90, 0], "scale": [0.1, 0.8, 0.6], "material": "mat_fabric"}
        ],
        "lights": [
            {"id": "main_light", "position": [0, 1.2, 0], "color": [1.0, 0.9, 0.8], "intensity": 1.5}
        ],
        "camera": {
            "position": [4.5, 2.0, 4.5],
            "target": [0, 0, 0],
            "fov": 60
        }
    }
    
    with open('C:/Users/Rico/HarmonicStringEngine/experiments/hse019/room.hsc', 'w') as f:
        json.dump(project, f, indent=2)
    print("Room project generated: room.hsc")

if __name__ == "__main__":
    create_room_project()
