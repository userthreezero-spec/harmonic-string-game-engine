# HSE-015 Acceptance Criteria

## Functional Requirements

### AC-1: Orbit position computation
Given a camera at orbit radius R, yaw θ, target T
When updateOrbit(dt) is called
Then position = (T.x + R*cos(θ), T.y, T.z + R*sin(θ))
And yaw increments by speed * dt

### AC-2: Orbit enable/disable
Given a camera with orbit disabled
When enableOrbit(true) is called
Then isOrbitEnabled() returns true
And subsequent updateOrbit(dt) advances the camera

### AC-3: Orbit respects deltaTime
Given a camera with orbit enabled at speed ω
When updateOrbit(dt1) is called, then updateOrbit(dt2)
Then total yaw change = ω * (dt1 + dt2)

### AC-4: Scene::update triggers orbit
Given a scene with an active camera with orbit enabled
When scene->update(dt) is called
Then the active camera's position changes

### AC-5: Orbit preserves target focus
Given a camera orbiting target T
When updateOrbit(dt) is called
Then the camera's view matrix is recomputed to face T

### AC-6: Bridge orbit commands
Given a bridge connected to a scene
When CMD_ORBIT_START is received with radius R and speed S
Then the active camera orbit is enabled with radius R and speed S
And an acknowledgment is sent

### AC-7: Bridge orbit stop
Given a bridge with orbit active
When CMD_ORBIT_STOP is received
Then the active camera orbit is disabled
And an acknowledgment is sent

### AC-8: Bridge orbit set
Given a bridge with orbit active
When CMD_ORBIT_SET is received with new radius R and speed S
Then the active camera orbit radius = R and speed = S

## Build Requirements
- AC-9: `cmake .. -G "MSYS Makefiles" && make clean && make -j$(nproc)` succeeds
- AC-10: All tests pass (existing + new orbit tests)

## Runtime Requirements
- AC-11: orbit_demo runs 60 frames without crash
- AC-12: Camera position changes each frame when orbiting

## Non-Functional
- AC-13: Orbit is runtime-only (not persisted in .hsc)
- AC-14: Orbit disabled by default (backward compatible)
