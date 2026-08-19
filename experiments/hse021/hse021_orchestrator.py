import json
import os

def execute_transformation():
    print("=== HSE-021 Semantic Transformation Orchestrator (JSON Mode) ===")
    
    room_path = 'C:/Users/Rico/HarmonicStringEngine/experiments/hse019/room.hsc'
    plan_path = 'C:/Users/Rico/HarmonicStringEngine/experiments/hse021/transformation_plan.json'
    
    # 1. Load transformation plan
    with open(plan_path, 'r') as f:
        plan = json.load(f)
    print(f"Goal: {plan['goal']}")

    # 2. Load current world state
    with open(room_path, 'r') as f:
        world = json.load(f)
    
    objects = world.get("objects", [])
    
    # 3. Apply steps
    for step in plan["steps"]:
        print(f"Executing: {step['description']}")
        
        if "targets" in step:
            for target in step["targets"]:
                found = False
                for obj in objects:
                    if obj["id"] == target["id"]:
                        obj["position"] = [target["px"], target["py"], target["pz"]]
                        print(f"  Moved {obj['id']} to {obj['position']}")
                        found = True
                        break
                if not found:
                    print(f"  Error: Object {target['id']} not found")
        else:
            found = False
            for obj in objects:
                if obj["id"] == step["target"]:
                    if step["action"] == "set_rotation_speed":
                        obj["rotation_speed"] = step["value"]
                        print(f"  Set rotation_speed of {obj['id']} to {obj['rotation_speed']}")
                    found = True
                    break
            if not found:
                print(f"  Error: Object {step['target']} not found")

    # 4. Save transformed world
    with open(room_path, 'w') as f:
        json.dump(world, f, indent=2)
    print("\nTransformation saved to room.hsc")
    
    print("\nTransformation Complete.")

if __name__ == "__main__":
    execute_transformation()
