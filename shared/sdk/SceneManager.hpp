#pragma once

class REManagedObject;
class RECamera;

namespace sdk {
void* get_scene_manager();
REManagedObject* get_main_view();
REManagedObject* get_current_scene();
RECamera* get_primary_camera();
void set_timescale(REManagedObject* scene, float timescale);
void set_timescale(float timescale);

float get_timescale(REManagedObject* scene);
float get_timescale();
} // namespace sdk
