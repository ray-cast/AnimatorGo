#ifndef OCTOON_H_
#define OCTOON_H_

#include <octoon/game_app.h>
#include <octoon/game_scene.h>
#include <octoon/game_object.h>
#include <octoon/game_object_manager.h>

#include <octoon/input_feature.h>
#include <octoon/timer_feature.h>
#include <octoon/input_feature.h>
#include <octoon/hal_feature.h>
#include <octoon/audio_feature.h>
#include <octoon/io_feature.h>
#include <octoon/gui_feature.h>
#include <octoon/physics_feature.h>
#include <octoon/video_feature.h>
#include <octoon/timer_feature.h>
#include <octoon/audio_feature.h>

#include <octoon/io/fstream.h>

#include <octoon/model/text_meshing.h>

#include <octoon/mesh/mesh.h>
#include <octoon/mesh/circle_mesh.h>
#include <octoon/mesh/sphere_mesh.h>
#include <octoon/mesh/cube_mesh.h>
#include <octoon/mesh/cube_wireframe_mesh.h>
#include <octoon/mesh/plane_mesh.h>
#include <octoon/mesh/ring_mesh.h>
#include <octoon/mesh/volume_mesh.h>
#include <octoon/mesh/torusknot_mesh.h>

#include <octoon/material/material.h>
#include <octoon/material/line_basic_material.h>
#include <octoon/material/mesh_basic_material.h>
#include <octoon/material/mesh_standard_material.h>

#include <octoon/animation/animation.h>
#include <octoon/animation/animation_curve.h>
#include <octoon/animation/animation_clip.h>
#include <octoon/animation/path_interpolator.h>

#include <octoon/game_base_features.h>
#include <octoon/transform_component.h>
#include <octoon/mesh_filter_component.h>

#include <octoon/cloth_component.h>
#include <octoon/sphere_collider_component.h>
#include <octoon/box_collider_component.h>
#include <octoon/capsule_collider_component.h>
#include <octoon/rigidbody_component.h>
#include <octoon/configurable_joint_component.h>

#include <octoon/ortho_camera_component.h>
#include <octoon/perspective_camera_component.h>
#include <octoon/film_camera_component.h>
#include <octoon/mesh_renderer_component.h>
#include <octoon/skinned_mesh_renderer_component.h>
#include <octoon/ambient_light_component.h>
#include <octoon/directional_light_component.h>
#include <octoon/point_light_component.h>
#include <octoon/spot_light_component.h>
#include <octoon/rectangle_light_component.h>
#include <octoon/environment_light_component.h>

#include <octoon/animator_component.h>
#include <octoon/mesh_animation_component.h>
#include <octoon/solver_component.h>
#include <octoon/rotation_limit_component.h>
#include <octoon/rotation_link_component.h>
#include <octoon/rotation_link_limit_component.h>
#include <octoon/skinned_morph_component.h>
#include <octoon/skinned_texture_component.h>
#include <octoon/skinned_bone_component.h>
#include <octoon/mesh_animation_component.h>
#include <octoon/path_component.h>
#include <octoon/text_component.h>
#include <octoon/guizmo_component.h>

#include <octoon/audio_listener_component.h>
#include <octoon/audio_source_component.h>

#include <octoon/editor_camera_component.h>
#include <octoon/first_person_camera_component.h>

#include <octoon/PMREM_loader.h>

#include <octoon/raycaster.h>
#include <octoon/ortho_camera_helper.h>
#include <octoon/perspective_camera_helper.h>
#include <octoon/film_camera_helper.h>
#include <octoon/circle_helper.h>
#include <octoon/plane_helper.h>
#include <octoon/ring_helper.h>
#include <octoon/sphere_helper.h>
#include <octoon/cube_helper.h>
#include <octoon/cone_helper.h>
#include <octoon/text_helper.h>
#include <octoon/volume_helper.h>

#include <octoon/ui/imgui.h>

#endif