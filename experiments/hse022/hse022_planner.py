import json
import os

def plan_and_execute():
    print("=== HSE-022 Multi-Constraint Planner ===")
    
    room_path = 'C:/Users/Rico/HarmonicStringEngine/experiments/hse019/room.hsc'
    
    # 1. Load current world
    with open(room_path, 'r') as f:
        world = json.load(f)
    
    objects = world.get("objects", [])
    
    # Goal: Move dining to [1.5, -0.7, -2.0]
    target_table_pos = [1.5, -0.7, -2.0]
    
    print(f"Goal: Move dining to {target_table_pos}")

    # 2. Conflict Detection (Predict Side-by-Side Arrangement)
    print("Conflict Detection: Evaluating side-by-side arrangement...")
    predicted_back_pos = 1.5 + 1.5
    if predicted_back_pos + 0.05 > 3.0:
        print(f"  CONFLICT DETECTED: Seating 2 back ({predicted_back_pos + 0.05}) intersects Wall Right (3.0)")
        print("  Switching to Z-axis arrangement strategy.")
        
        # New Plan (Z-axis)
        target_table_pos = [1.5, -0.7, -1.8]
        moves = {
            "table_top": {"position": target_table_pos},
            "chair_1": {"position": [1.5, -1.0, -1.1], "rotation": [0, 180, 0]},
            "chair_1_back": {"position": [1.5, -0.6, -0.8], "rotation": [0, 180, 0]},
            "chair_2": {"position": [1.5, -1.0, -2.5], "rotation": [0, 0, 0]},
            "chair_2_back": {"position": [1.5, -0.6, -2.8], "rotation": [0, 0, 0]}
        }
    else:
        print("  No conflict detected for side-by-side.")
        moves = {
            "table_top": {"position": target_table_pos},
            "chair_1": {"position": [0.3, -1.0, -2.0], "rotation": [0, 90, 0]},
            "chair_1_back": {"position": [0.0, -0.6, -2.0], "rotation": [0, 90, 0]},
            "chair_2": {"position": [2.7, -1.0, -2.0], "rotation": [0, -90, 0]},
            "chair_2_back": {"position": [3.0, -0.6, -2.0], "rotation": [0, -90, 0]}
        }

    # 3. Apply transformations
    for obj in objects:
        if obj["id"] in moves:
            m = moves[obj["id"]]
            if "position" in m: obj["position"] = m["position"]
            if "rotation" in m: obj["rotation"] = m["rotation"]
            print(f"  Transformed {obj['id']}")

    # 4. Persistence
    with open(room_path, 'w') as f:
        json.dump(world, f, indent=2)
    
    print("\nTransformation saved to room.hsc")
    print("HSE-022 Planning Successful.")

if __name__ == "__main__":
    plan_and_execute()
