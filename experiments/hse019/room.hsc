{
  "name": "HSE-019 Room (Hierarchical)",
  "materials": [
    {
      "id": "mat_ceiling",
      "albedo": [0.7,0.7,0.65],
      "roughness": 0.9,
      "metallic": 0
    },
    {
      "id": "mat_wood",
      "albedo": [0.3,0.2,0.1],
      "roughness": 0.5,
      "metallic": 0
    },
    {
      "id": "mat_wall",
      "albedo": [0.6,0.55,0.5],
      "roughness": 0.5,
      "metallic": 0
    },
    {
      "id": "mat_floor",
      "albedo": [0.4,0.3,0.2],
      "roughness": 0.8,
      "metallic": 0
    }
  ],
  "lights": [
    {
      "id": "main_light",
      "position": [0,1.2,0],
      "color": [1,0.9,0.8],
      "intensity": 1.5
    }
  ],
  "camera": {
    "position": [4.5,2,4.5],
    "target": [0,0,0],
    "fov": 60
  },
  "objects": [
    {
      "id": "floor",
      "type": "Quad",
      "position": [0,-1.5,0],
      "rotation": [-90,0,0],
      "scale": [6,6,1],
      "rotation_speed": [0,0,0],
      "material": "mat_floor"
    },
    {
      "id": "ceiling",
      "type": "Quad",
      "position": [0,1.5,0],
      "rotation": [90,0,0],
      "scale": [6,6,1],
      "rotation_speed": [0,0,0],
      "material": "mat_ceiling"
    },
    {
      "id": "wall_back",
      "type": "Quad",
      "position": [0,0,-3],
      "rotation": [0,0,0],
      "scale": [6,3,1],
      "rotation_speed": [0,0,0],
      "material": "mat_wall"
    },
    {
      "id": "wall_front_lower",
      "type": "Quad",
      "position": [0,-0.75,3],
      "rotation": [0,180,0],
      "scale": [6,1.5,1],
      "rotation_speed": [0,0,0],
      "material": "mat_wall"
    },
    {
      "id": "wall_front_upper",
      "type": "Quad",
      "position": [0,0.75,3],
      "rotation": [0,180,0],
      "scale": [6,1.5,1],
      "rotation_speed": [0,0,0],
      "material": "mat_wall"
    },
    {
      "id": "wall_front_left",
      "type": "Quad",
      "position": [-2,0,3],
      "rotation": [0,180,0],
      "scale": [2,3,1],
      "rotation_speed": [0,0,0],
      "material": "mat_wall"
    },
    {
      "id": "wall_front_right",
      "type": "Quad",
      "position": [2,0,3],
      "rotation": [0,180,0],
      "scale": [2,3,1],
      "rotation_speed": [0,0,0],
      "material": "mat_wall"
    },
    {
      "id": "wall_left",
      "type": "Quad",
      "position": [-3,0,0],
      "rotation": [0,90,0],
      "scale": [6,3,1],
      "rotation_speed": [0,0,0],
      "material": "mat_wall"
    },
    {
      "id": "wall_right_upper",
      "type": "Quad",
      "position": [3,0.75,0],
      "rotation": [0,-90,0],
      "scale": [6,1.5,1],
      "rotation_speed": [0,0,0],
      "material": "mat_wall"
    },
    {
      "id": "wall_right_lower_left",
      "type": "Quad",
      "position": [3,-0.75,-1.5],
      "rotation": [0,-90,0],
      "scale": [3,1.5,1],
      "rotation_speed": [0,0,0],
      "material": "mat_wall"
    },
    {
      "id": "wall_right_lower_right",
      "type": "Quad",
      "position": [3,-0.75,1.5],
      "rotation": [0,-90,0],
      "scale": [3,1.5,1],
      "rotation_speed": [0,0,0],
      "material": "mat_wall"
    },
    {
      "id": "table",
      "type": "Group",
      "position": [1.5,-0.7,-1.8],
      "rotation": [0,0,0],
      "scale": [1,1,1],
      "rotation_speed": [0,0,0],
      "material": "Default"
    },
    {
      "id": "table_top",
      "type": "Cube",
      "position": [0,0,0],
      "rotation": [0,0,0],
      "scale": [2,0.1,1.2],
      "rotation_speed": [0,0,0],
      "material": "mat_wood",
      "parent": "table"
    },
    {
      "id": "table_leg_1",
      "type": "Cube",
      "position": [-0.9,-0.4,-0.5],
      "rotation": [0,0,0],
      "scale": [0.1,0.8,0.1],
      "rotation_speed": [0,0,0],
      "material": "mat_wood",
      "parent": "table"
    },
    {
      "id": "table_leg_2",
      "type": "Cube",
      "position": [-0.9,-0.4,0.5],
      "rotation": [0,0,0],
      "scale": [0.1,0.8,0.1],
      "rotation_speed": [0,0,0],
      "material": "mat_wood",
      "parent": "table"
    },
    {
      "id": "table_leg_3",
      "type": "Cube",
      "position": [0.9,-0.4,-0.5],
      "rotation": [0,0,0],
      "scale": [0.1,0.8,0.1],
      "rotation_speed": [0,0,0],
      "material": "mat_wood",
      "parent": "table"
    },
    {
      "id": "table_leg_4",
      "type": "Cube",
      "position": [0.9,-0.4,0.5],
      "rotation": [0,0,0],
      "scale": [0.1,0.8,0.1],
      "rotation_speed": [0,0,0],
      "material": "mat_wood",
      "parent": "table"
    },
    {
      "id": "chair_1_assembly",
      "type": "Group",
      "position": [1.5,-1,-1.1],
      "rotation": [0,180,0],
      "scale": [1,1,1],
      "rotation_speed": [0,0,0],
      "material": "Default"
    },
    {
      "id": "chair_1",
      "type": "Cube",
      "position": [0,0,0],
      "rotation": [0,0,0],
      "scale": [0.6,0.1,0.6],
      "rotation_speed": [0,0,0],
      "material": "Default",
      "parent": "chair_1_assembly"
    },
    {
      "id": "chair_1_back",
      "type": "Cube",
      "position": [0,0.4,0.3],
      "rotation": [0,0,0],
      "scale": [0.6,0.8,0.1],
      "rotation_speed": [0,0,0],
      "material": "Default",
      "parent": "chair_1_assembly"
    },
    {
      "id": "chair_2_assembly",
      "type": "Group",
      "position": [1.5,-1,-2.5],
      "rotation": [0,0,0],
      "scale": [1,1,1],
      "rotation_speed": [0,0,0],
      "material": "Default"
    },
    {
      "id": "chair_2",
      "type": "Cube",
      "position": [0,0,0],
      "rotation": [0,0,0],
      "scale": [0.6,0.1,0.6],
      "rotation_speed": [0,0,0],
      "material": "Default",
      "parent": "chair_2_assembly"
    },
    {
      "id": "chair_2_back",
      "type": "Cube",
      "position": [0,0.4,0.3],
      "rotation": [0,0,0],
      "scale": [0.6,0.8,0.1],
      "rotation_speed": [0,0,0],
      "material": "Default",
      "parent": "chair_2_assembly"
    }
  ]
}
