#ifndef OCTOON_H_
#define OCTOON_H_

#include <octoon/game_app.h>
#include <octoon/game_scene.h>
#include <octoon/game_object.h>
#include <octoon/game_prefabs.h>

#include <octoon/video/line_material.h>
#include <octoon/video/text_material.h>
#include <octoon/video/blinn_material.h>
#include <octoon/video/phong_material.h>
#include <octoon/video/ggx_material.h>

#include <octoon/input_feature.h>

#include <octoon/mesh_filter_component.h>
#include <octoon/transform_component.h>
#include <octoon/guizmo_component.h>
#include <octoon/first_person_camera_component.h>
#include <octoon/editor_camera_component.h>
#include <octoon/ortho_camera_component.h>
#include <octoon/perspective_camera_component.h>
#include <octoon/film_camera_component.h>
#include <octoon/mesh_renderer_component.h>
#include <octoon/path_component.h>
#include <octoon/text_component.h>
#include <octoon/animation_component.h>

#include <octoon/model/model.h>
#include <octoon/model/text_meshing.h>
#include <octoon/model/property.h>

#include <octoon/ui/imgui.h>

namespace octoon
{
	typedef MeshFilterComponent MeshFilter;
	typedef TransformComponent Transform;
	typedef GuizmoComponent Guizmo;
	typedef FirstPersonCameraComponent FirstPersonCamera;
	typedef CameraComponent Camera;
	typedef MeshRendererComponent MeshRenderer;
}

#endif