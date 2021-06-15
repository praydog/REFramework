#pragma once

class REManagedObject;

namespace sdk {
void* get_scene_manager();
REManagedObject* get_current_scene();
void set_timescale(REManagedObject* scene, float timescale);
void set_timescale(float timescale);

float get_timescale(REManagedObject* scene);
float get_timescale();
} // namespace sdk
