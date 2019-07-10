#ifndef OCTOON_PMM_H_
#define OCTOON_PMM_H_

#include <string>
#include <vector>
#include <optional>
#include <octoon/io/istream.h>
#include <octoon/math/vector2.h>
#include <octoon/math/vector3.h>
#include <octoon/math/vector4.h>

namespace octoon
{
	typedef math::Vector3  PmmVector3;
	typedef math::Vector4  PmmVector4;

	typedef math::Vector3  PmmColor3;
	typedef math::Vector4  PmmColor4;
	typedef math::float3x3 PmmFloat3x3;
	typedef math::float4x4 PmmFloat4x4;

	typedef io::istream istream;

	class PmmInt2
	{
	public:
		std::int32_t x;
		std::int32_t y;

		std::optional<PmmInt2> load(istream& reader);

		static std::optional<std::vector<PmmInt2>> load_arrays(istream& reader);
		static std::optional<std::vector<PmmInt2>> load_fixed_arrays(istream& reader, std::size_t len);
	};

	class PmmName
	{
	public:
		static std::string sjis2utf8(const std::string& sjis);
		static std::optional<std::string> load(istream& reader);
		static std::optional<std::string> load_fixed_utf8(istream& reader, std::size_t len);
		static std::optional<std::vector<std::string>> load_arrays(istream& reader);
	};

	class PmmUint8
	{
	public:
		std::uint8_t value;

		static std::optional<std::vector<std::uint8_t>> load_array_from_u8(istream& reader);
		static std::optional<std::vector<std::uint8_t>> load_fixed_arrays(istream& reader, std::size_t len);
	};

	class PmmInt32
	{
	public:
		static std::optional<std::vector<std::int32_t>> load_arrays(istream& reader);
	};

	class PmmFloat
	{
		static std::optional<std::vector<float>> load_arrays(istream& reader);
	};

	struct PmmHeader
	{
		std::string magic;
		std::uint16_t reserve1;
		std::uint16_t reserve2;
		std::uint16_t reserve3;
		std::uint32_t view_width;
		std::uint32_t view_height;
		std::uint32_t frame_width;
		float edit_view_angle;
		std::uint8_t is_edit_camera_light_accessory;
		std::uint8_t is_open_camera_panel;
		std::uint8_t is_open_light_panel;
		std::uint8_t is_open_accessory_panel;
		std::uint8_t is_open_bone_panel;
		std::uint8_t is_open_morph_panel;
		std::uint8_t is_open_selfshadow_panel;
		std::uint8_t selected_model_index;
		std::uint8_t num_models;
		std::uint8_t summary[21];

		PmmHeader();

		static std::optional<PmmHeader> load(istream& reader);
	};

	struct PmmKeyframe
	{
		std::int32_t data_index;
		std::int32_t frame_number;
		std::int32_t pre_index;
		std::int32_t next_index;
		std::uint8_t is_selected;

		PmmKeyframe();

		static std::optional<PmmKeyframe> load(istream& reader);
		static std::optional<std::vector<PmmKeyframe>> load_arrays(istream& reader);
	};

	struct PmmKeyframeBone
	{
		std::int32_t data_index;
		std::int32_t frame_number;
		std::int32_t pre_index;
		std::int32_t next_index;
		std::uint8_t interpolation_x[4];
		std::uint8_t interpolation_y[4];
		std::uint8_t interpolation_z[4];
		std::uint8_t interpolation_rotation[4];
		PmmVector3 translation;
		PmmVector4 quaternion;
		std::uint8_t is_selected;
		std::uint8_t is_physics_disabled;

		PmmKeyframeBone();

		static std::optional<PmmKeyframeBone> load(istream& reader, bool is_init);
		static std::optional<std::vector<PmmKeyframeBone>> load_fixed_arrays(istream& reader, std::size_t len, bool is_init);
		static std::optional<std::vector<PmmKeyframeBone>> load_arrays(istream& reader, bool is_init);
	};

	struct PmmKeyframeMorph
	{
		std::int32_t data_index;
		std::int32_t frame_number;
		std::int32_t pre_index;
		std::int32_t next_index;
		float value;
		std::uint8_t is_selected;

		PmmKeyframeMorph();

		bool load(istream& reader, bool is_init);

		static std::optional<std::vector<PmmKeyframeMorph>> load_fixed_arrays(istream& reader, std::size_t len, bool is_init);
		static std::optional<std::vector<PmmKeyframeMorph>> load_arrays(istream& reader, bool is_init);
	};

	struct PmmKeyframeOp
	{
		std::int32_t data_index;
		std::int32_t frame_number;
		std::int32_t pre_index;
		std::int32_t next_index;
		std::uint8_t is_display;
		std::vector<std::uint8_t> is_ik_enabled;
		std::vector<PmmInt2> op_data;
		std::uint8_t is_selected;

		PmmKeyframeOp() noexcept;

		static std::optional<PmmKeyframeOp> load(istream& reader, std::size_t ik_count, std::size_t op_count, bool is_init);
		static std::optional<std::vector<PmmKeyframeOp>> load_arrays(istream& reader, std::size_t ik_count, std::size_t op_count, bool is_init);
	};

	struct PmmKeyframeCamera
	{
		std::int32_t data_index;
		std::int32_t frame;
		std::int32_t pre_index;
		std::int32_t next_index;
		float distance;
		PmmVector3 eye;
		PmmVector3 rotation;
		std::int32_t looking_model_index;
		std::int32_t looking_bone_index;
		std::uint8_t interpolation_x[4];
		std::uint8_t interpolation_y[4];
		std::uint8_t interpolation_z[4];
		std::uint8_t interpolation_rotation[4];
		std::uint8_t interpolation_distance[4];
		std::uint8_t interpolation_angleview[4];
		std::uint8_t is_parse;
		std::uint32_t fov;
		std::uint8_t is_selected;

		PmmKeyframeCamera();

		static std::optional<PmmKeyframeCamera> load(istream& reader, bool is_init);
		static std::optional<std::vector<PmmKeyframeCamera>> load_arrays(istream& reader, bool is_init);
	};

	struct PmmKeyframeLight
	{
		std::int32_t data_index;
		std::int32_t frame_number;
		std::int32_t pre_index;
		std::int32_t next_index;
		PmmVector3 rgb;
		PmmVector3 xyz;
		std::uint8_t is_selected;

		PmmKeyframeLight();

		static std::optional<PmmKeyframeLight> load(istream& reader, bool is_init);
		static std::optional<std::vector<PmmKeyframeLight>> load_arrays(istream& reader, bool is_init);
	};

	struct PmmKeyFrameGravity
	{
		std::int32_t data_index;
		std::int32_t frame_number;
		std::int32_t pre_index;
		std::int32_t next_index;
		std::uint8_t is_add_noize;
		std::uint32_t noize_amount;
		float acceleration;
		PmmVector3 direction;
		std::uint8_t is_selected;

		PmmKeyFrameGravity();

		static std::optional<PmmKeyFrameGravity> load(istream& reader, bool is_init);
		static std::optional<std::vector<PmmKeyFrameGravity>> load_arrays(istream& reader, bool is_init);
	};

	struct PmmKeyFrameSelfShadow
	{
		std::int32_t data_index;
		std::int32_t frame_number;
		std::int32_t pre_index;
		std::int32_t next_index;
		std::uint8_t mode;
		float distance;
		std::uint8_t is_selected;

		PmmKeyFrameSelfShadow();

		static std::optional<PmmKeyFrameSelfShadow> load(istream& reader, bool is_init);
		static std::optional<std::vector<PmmKeyFrameSelfShadow>> load_arrays(istream& reader, bool is_init);
	};

	struct PmmBone
	{
		PmmVector3 translation;
		PmmVector4 quaternion;
		std::uint8_t is_edit_un_commited;
		std::uint8_t is_physics_disabled;
		std::uint8_t is_row_selected;

		PmmBone();

		static std::optional<PmmBone> load(istream& reader);
		static std::optional<std::vector<PmmBone>> load_arrays(istream& reader);
	};

	struct PmmOp
	{
		std::int32_t keyframe_begin;
		std::int32_t keyframe_end;
		std::int32_t model_index;
		std::int32_t parent_bone_index;

		PmmOp();

		static std::optional<PmmOp> load(istream& reader);
		static std::optional<std::vector<PmmOp>> load_arrays(istream& reader);
	};

	struct PmmModel
	{
		std::uint8_t number;
		std::string name;
		std::string name_en;
		std::string path;
		std::uint8_t keyframe_editor_toplevel_rows;
		std::vector<std::string> bone_name;
		std::vector<std::string> morph_name;
		std::vector<std::int32_t> ik_index;
		std::vector<std::int32_t> op_index;
		std::uint8_t draw_order;
		std::uint8_t edit_is_display;
		std::int32_t edit_selected_bone;
		std::int32_t skin_panel[4];
		std::vector<std::uint8_t> is_frame_open;
		std::int32_t vscroll;
		std::int32_t last_frame;
		std::vector<PmmKeyframeBone> bone_init_frame;
		std::vector<PmmKeyframeBone> bone_key_frame;
		std::vector<PmmKeyframeMorph> morph_init_frame;
		std::vector<PmmKeyframeMorph> morph_key_frame;
		PmmKeyframeOp op_init_frame;
		std::vector<PmmKeyframeOp> op_key_frames;
		std::vector<PmmBone> bone_current_datas;
		std::vector<float> morph_current_datas;
		std::vector<std::uint8_t> is_current_ik_enabled_datas;
		std::vector<PmmOp> op_current_data;
		std::uint8_t is_add_blend;
		float edge_width;
		std::uint8_t is_selfshadow_enabled;
		std::uint8_t calc_order;

		PmmModel();

		static std::optional<PmmModel> load(istream& reader);
		static std::optional<std::vector<PmmModel>> load_arrays(istream& reader, std::size_t len);
	};

	struct PmmCamera
	{
		PmmVector3 eye;
		PmmVector3 target;
		PmmVector3 rotation;
		std::uint8_t isorthro;

		PmmCamera();

		static std::optional<PmmCamera> load(istream& reader);
		static std::optional<std::vector<PmmCamera>> load_arrays(istream& reader);
	};

	struct PmmLight
	{
		PmmVector3 rgb;
		PmmVector3 xyz;
		std::uint8_t is_selected;

		PmmLight();

		static std::optional<PmmLight> load(istream& reader);
		static std::optional<std::vector<PmmLight>> load_arrays(istream& reader);
	};

	struct PmmDataBody
	{
		std::uint8_t transparency;
		std::uint8_t is_visible;
		std::int32_t parent_model_index;
		std::int32_t parent_bone_index;
		PmmVector3 translation;
		PmmVector3 rotation;
		float scale;
		std::uint8_t is_shadow_enabled;

		PmmDataBody();

		static std::optional<PmmDataBody> load(istream& reader);
		static std::optional<std::vector<PmmDataBody>> load_arrays(istream& reader);
	};

	struct PmmAccessoryData
	{
		std::uint8_t index;
		std::string name;
		std::string path;
		std::uint8_t draw_order;
		PmmKeyframe init_frame;
		std::vector<PmmKeyframe> key_frames;
		PmmDataBody current_data;
		std::uint8_t is_add_blend;

		PmmAccessoryData();

		static std::optional<PmmAccessoryData> load(istream& reader);
		static std::optional<std::vector<PmmAccessoryData>> load_arrays(istream& reader);
	};

	struct PmmGravity
	{
		float acceleration;
		std::uint32_t noize_amount;
		PmmVector3 direction;
		std::uint8_t is_add_noize;

		PmmGravity();

		static std::optional<PmmGravity> load(istream& reader);
		static std::optional<std::vector<PmmGravity>> load_arrays(istream& reader);
	};

	struct PmmCSelectorChoice
	{
		std::uint8_t mode_index;
		std::uint32_t selector_choice;

		PmmCSelectorChoice();

		static std::optional<PmmCSelectorChoice> load(istream& reader);
		static std::optional<std::vector<PmmCSelectorChoice>> load_arrays(istream& reader);
	};

	struct PMMFile
	{
		PmmHeader header;
		std::vector<PmmModel> model;
		PmmKeyframeCamera camera_init_frame;
		std::vector<PmmKeyframeCamera> camera_key_frames;
		PmmCamera camera;
		PmmKeyframeLight main_light_frame;
		std::vector<PmmKeyframeLight> main_light_frames;
		PmmLight main_light;
		std::uint8_t selected_accessory_index;
		std::uint32_t accessory_vscroll;
		std::uint8_t accessory_count;
		std::vector<std::string> accessory_name;
		std::vector<PmmAccessoryData> accessory_datas;
		std::uint32_t current_frame_position;
		std::uint32_t h_scroll_position;
		std::uint32_t h_scroll_scale;
		std::uint32_t bone_operation_kind;
		std::uint8_t looking_at;
		std::uint8_t is_repeat;
		std::uint8_t is_play_from_frame;
		std::uint8_t is_play_to_frame;
		std::uint32_t play_start_frame;
		std::uint32_t play_end_frame;
		std::uint8_t is_wave_enabled;
		std::string wave_path;
		std::uint32_t avi_offset_x;
		std::uint32_t avi_offset_y;
		float avi_scale;
		std::string avi_path;
		std::uint32_t is_show_avi;
		std::uint32_t background_image_offset_x;
		std::uint32_t background_image_offset_y;
		std::uint32_t background_image_scale;
		std::string background_image_path;
		std::uint8_t is_show_background_image;
		std::uint8_t is_show_infomation;
		std::uint8_t is_show_axis;
		std::uint8_t is_show_groundshadow;
		float fps_limit;
		std::uint32_t screen_capture_mode;
		std::uint32_t accessory_number_render_after_model;
		float ground_shadow_brightness;
		std::uint8_t is_transparent_ground_shadow;
		std::uint8_t physics_mode;
		PmmGravity gravity_current_data;
		PmmKeyFrameGravity gravity_init_frame;
		std::vector<PmmKeyFrameGravity> gravity_key_frames;
		std::uint8_t is_show_selfshadow;
		float selfshadow_current_data;
		PmmKeyFrameSelfShadow selfshadow_init_frame;
		std::vector<PmmKeyFrameSelfShadow> selfshadow_keyframes;
		std::uint32_t edge_color_r;
		std::uint32_t edge_color_g;
		std::uint32_t edge_color_b;
		std::uint8_t is_black_background;
		std::int32_t camera_current_looking_at_model;
		std::int32_t camera_current_looking_at_bone;
		float unknown_array[16];
		std::uint8_t is_view_look_at_enabled;
		std::uint8_t unknown;
		std::uint8_t is_physics_ground_enabled;
		std::uint32_t frame_text_box;
		std::uint8_t selector_choice_selection_following;
		std::vector<PmmCSelectorChoice> selector_choice_datas;

		PMMFile() noexcept;
		static std::optional<PMMFile> load(istream& reader);
	};

	class PMMLoader
	{
	public:
		PMMLoader();

		bool can_read(io::istream& reader);
		bool do_load(io::istream& reader, PMMFile& pmm);
	};
}

#endif