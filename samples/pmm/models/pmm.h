#ifndef OCTOON_PMM_H_
#define OCTOON_PMM_H_

#include <cstdio>
#include <string>
#include <vector>
#include <optional>
#include <octoon/io/istream.h>
#include <octoon/math/vector2.h>
#include <octoon/math/vector3.h>
#include <octoon/math/vector4.h>

namespace octoon
{
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

		std::optional<PmmInt2> load(istream& reader)
		{
			PmmInt2 v;
			if (!reader.read((char*)& v.x, sizeof(v.x))) return std::nullopt;
			if (!reader.read((char*)& v.y, sizeof(v.y))) return std::nullopt;
			return v;
		}

		static std::optional<std::vector<PmmInt2>> load_arrays(istream& reader)
		{
			std::uint32_t len = 0;
			reader.read((char*)& len, sizeof(len));

			auto array = std::vector<PmmInt2>(len);
			for (std::size_t i = 0; i < array.size(); i++)
			{
				PmmInt2 v;
				v.load(reader);
				array.push_back(v);
			}

			return array;
		}

		static std::optional<std::vector<PmmInt2>> load_fixed_arrays(istream& reader, std::size_t len)
		{
			auto array = std::vector<PmmInt2>(len);
			for (std::size_t i = 0; i < array.size(); i++)
			{
				PmmInt2 v;
				v.load(reader);
				array.push_back(v);
			}

			return array;
		}
	};

	class PmmVector2
	{
	public:
		float x, y;

		std::optional<PmmVector2> load(istream& reader)
		{
			PmmVector2 v;
			if (!reader.read((char*)& v.x, sizeof(v.x))) return std::nullopt;
			if (!reader.read((char*)& v.y, sizeof(v.y))) return std::nullopt;
			return v;
		}
	};

	class PmmVector3
	{
	public:
		float x, y, z;

		PmmVector3()
		{
		}

		PmmVector3(float xx, float yy, float zz)
			: x(xx), y(yy), z(zz)
		{
		}

		std::optional<PmmVector3> load(istream& reader)
		{
			PmmVector3 v;
			if (!reader.read((char*)& v.x, sizeof(v.x))) return std::nullopt;
			if (!reader.read((char*)& v.y, sizeof(v.y))) return std::nullopt;
			if (!reader.read((char*)& v.z, sizeof(v.z))) return std::nullopt;
			return v;
		}
	};

	class PmmVector4
	{
	public:
		float x, y, z, w;

		PmmVector4()
		{
		}

		PmmVector4(float xx, float yy, float zz, float ww)
			: x(xx), y(yy), z(zz), w(ww)
		{
		}

		std::optional<PmmVector4> load(istream& reader)
		{
			PmmVector4 v;
			if (!reader.read((char*)& v.x, sizeof(v.x))) return std::nullopt;
			if (!reader.read((char*)& v.y, sizeof(v.y))) return std::nullopt;
			if (!reader.read((char*)& v.z, sizeof(v.z))) return std::nullopt;
			if (!reader.read((char*)& v.w, sizeof(v.w))) return std::nullopt;
			return v;
		}
	};

	class PmmName
	{
	public:
		static std::optional<std::string> load(istream& reader)
		{
			std::uint8_t len = 0;
			reader.read((char*)& len, sizeof(len));

			auto bytes = std::string(len, 0);
			reader.read(bytes.data(), bytes.size());

			return bytes;
			// return WINDOWS_31J.decode(&bytes, DecoderTrap::Ignore).unwrap();
		}

		static std::optional<std::string> load_fixed_utf8(istream& reader, std::size_t len)
		{
			auto bytes = std::string(len, 0);
			for (std::size_t i = 0; i < len; i++)
			{
				auto ch = 0;
				reader.read((char*)& ch, 1);
				if (ch == 0) { break; }
				bytes.push_back(ch);
			}

			if (bytes.size() < len - 1)
			{
				reader.seekg(std::ios_base::cur, len - bytes.size() - 1);
			}

			return bytes;
			// return WINDOWS_31J.decode(&bytes, DecoderTrap::Ignore).unwrap();
		}

		static std::optional<std::vector<std::string>> load_arrays(istream& reader)
		{
			std::uint32_t len = 0;
			reader.read((char*)& len, sizeof(len));

			auto array = std::vector<std::string>(len);
			for (std::size_t i = 0; i < array.size(); i++)
			{
				array.push_back(PmmName::load(reader).value());
			}

			return array;
		}
	};

	class PmmUint8
	{
	public:
		std::uint8_t value;

		static std::optional<std::vector<std::uint8_t>> load_array_from_u8(istream& reader)
		{
			std::uint8_t len = 0;
			reader.read((char*)& len, sizeof(len));

			auto array = std::vector<std::uint8_t>(len);
			for (std::size_t i = 0; i < array.size(); i++)
			{
				std::uint8_t ch = 0;
				reader.read((char*)& ch, sizeof(ch));
				array.push_back(ch);
			}

			return array;
		}

		static std::optional<std::vector<std::uint8_t>> load_fixed_arrays(istream& reader, std::size_t len)
		{
			auto array = std::vector<std::uint8_t>(len);
			for (std::size_t i = 0; i < array.size(); i++)
			{
				std::uint8_t ch = 0;
				reader.read((char*)& ch, sizeof(ch));
				array.push_back(ch);
			}

			return array;
		}
	};

	class PmmInt32
	{
	public:
		static std::optional<std::vector<std::int32_t>> load_arrays(istream& reader)
		{
			std::uint32_t len = 0;
			reader.read((char*)& len, sizeof(len));

			auto array = std::vector<std::int32_t>(len);
			reader.read((char*)array.data(), array.size() * sizeof(std::int32_t));

			return array;
		}
	};

	class PmmFloat
	{
		static std::optional<std::vector<float>> load_arrays(istream& reader)
		{
			std::uint32_t len = 0;
			reader.read((char*)& len, sizeof(len));

			auto array = std::vector<float>(len);
			reader.read((char*)array.data(), array.size() * sizeof(float));

			return array;
		}
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
		std::int8_t is_edit_camera_light_accessory;
		std::int8_t is_open_camera_panel;
		std::int8_t is_open_light_panel;
		std::int8_t is_open_accessory_panel;
		std::int8_t is_open_bone_panel;
		std::int8_t is_open_morph_panel;
		std::int8_t is_open_selfshadow_panel;
		std::int8_t selected_model_index;

		PmmHeader()
			: reserve1(0)
			, reserve2(0)
			, reserve3(0)
			, view_width(0)
			, view_height(0)
			, frame_width(0)
			, edit_view_angle(0.0)
			, is_edit_camera_light_accessory(0)
			, is_open_camera_panel(0)
			, is_open_light_panel(0)
			, is_open_accessory_panel(0)
			, is_open_bone_panel(0)
			, is_open_morph_panel(0)
			, is_open_selfshadow_panel(0)
			, selected_model_index(0)
		{
		}

		bool valid() noexcept
		{
			if (this->magic != "Polygon Movie maker 0002") { return false; }
			if (this->view_width == 0) { return false; }
			if (this->view_height == 0) { return false; }
			if (this->frame_width == 0) { return false; }
			if (this->edit_view_angle <= 0.0) { return false; }

			return true;
		}

		bool load(istream& reader)
		{
			this->magic = PmmName::load_fixed_utf8(reader, 24).value();
			reader.read((char*)& this->reserve1, sizeof(this->reserve1));
			reader.read((char*)& this->reserve2, sizeof(this->reserve2));
			reader.read((char*)& this->reserve3, sizeof(this->reserve3));
			reader.read((char*)& this->view_width, sizeof(this->view_width));
			reader.read((char*)& this->view_height, sizeof(this->view_height));
			reader.read((char*)& this->frame_width, sizeof(this->frame_width));
			reader.read((char*)& this->edit_view_angle, sizeof(this->edit_view_angle));
			reader.read((char*)& this->is_edit_camera_light_accessory, sizeof(this->is_edit_camera_light_accessory));
			reader.read((char*)& this->is_open_camera_panel, sizeof(this->is_open_camera_panel));
			reader.read((char*)& this->is_open_light_panel, sizeof(this->is_open_light_panel));
			reader.read((char*)& this->is_open_accessory_panel, sizeof(this->is_open_accessory_panel));
			reader.read((char*)& this->is_open_bone_panel, sizeof(this->is_open_bone_panel));
			reader.read((char*)& this->is_open_morph_panel, sizeof(this->is_open_morph_panel));
			reader.read((char*)& this->is_open_selfshadow_panel, sizeof(this->is_open_selfshadow_panel));
			reader.read((char*)& this->selected_model_index, sizeof(this->selected_model_index));
			return this->valid();
		}
	};

	struct PmmBoneFrame
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

		PmmBoneFrame()
			: data_index(-1)
			, frame_number(0)
			, pre_index(-1)
			, next_index(-1)
			, translation(PmmVector3(0.0, 0.0, 0.0))
			, quaternion(PmmVector4(0.0, 0.0, 0.0, 1.0))
			, is_selected(0)
			, is_physics_disabled(0)
		{
		}

		bool load(istream& reader, bool is_init)
		{
			if (is_init) reader.read((char*)& this->data_index, sizeof(this->data_index));
			reader.read((char*)& this->frame_number, sizeof(this->frame_number));
			reader.read((char*)& this->pre_index, sizeof(this->pre_index));
			reader.read((char*)& this->next_index, sizeof(this->next_index));
			reader.read((char*)& this->interpolation_x, sizeof(this->interpolation_x));
			reader.read((char*)& this->interpolation_y, sizeof(this->interpolation_y));
			reader.read((char*)& this->interpolation_z, sizeof(this->interpolation_z));
			reader.read((char*)& this->interpolation_rotation, sizeof(this->interpolation_rotation));
			reader.read((char*)& this->translation, sizeof(this->translation));
			reader.read((char*)& this->quaternion, sizeof(this->quaternion));
			reader.read((char*)& this->is_selected, sizeof(this->is_selected));
			reader.read((char*)& this->is_physics_disabled, sizeof(this->is_physics_disabled));
			return true;
		}

		static std::optional<std::vector<PmmBoneFrame>> load_fixed_arrays(istream& reader, std::size_t len, bool is_init)
		{
			auto array = std::vector<PmmBoneFrame>(len);
			for (std::size_t i = 0; i < array.size(); i++)
			{
				PmmBoneFrame frame;
				frame.load(reader, is_init);
				array.push_back(frame);
			}

			return array;
		}

		static std::optional<std::vector<PmmBoneFrame>> load_arrays(istream& reader, bool is_init)
		{
			std::uint32_t len = 0;
			reader.read((char*)& len, sizeof(len));
			return PmmBoneFrame::load_fixed_arrays(reader, len, is_init);
		}
	};

	struct PmmMorphFrame
	{
		std::int32_t data_index;
		std::int32_t frame_number;
		std::int32_t pre_index;
		std::int32_t next_index;
		float value;
		std::uint8_t is_selected;

		PmmMorphFrame()
			: data_index(1)
			, frame_number(0)
			, pre_index(-1)
			, next_index(-1)
			, value(0.0)
			, is_selected(0)
		{
		}

		bool load(istream& reader, bool is_init)
		{
			if (is_init) reader.read((char*)& this->data_index, sizeof(this->data_index));
			reader.read((char*)& this->frame_number, sizeof(this->frame_number));
			reader.read((char*)& this->pre_index, sizeof(this->pre_index));
			reader.read((char*)& this->next_index, sizeof(this->next_index));
			reader.read((char*)& this->value, sizeof(this->value));
			reader.read((char*)& this->is_selected, sizeof(this->is_selected));

			return true;
		}

		static std::optional<std::vector<PmmMorphFrame>> load_fixed_arrays(istream& reader, std::size_t len, bool is_init)
		{
			auto array = std::vector<PmmMorphFrame>(len);
			for (std::size_t i = 0; i < array.size(); i++)
			{
				PmmMorphFrame frame;
				frame.load(reader, is_init);
				array.push_back(frame);
			}

			return array;
		}

		static std::optional<std::vector<PmmMorphFrame>> load_arrays(istream& reader, bool is_init)
		{
			std::uint32_t len = 0;
			reader.read((char*)& len, sizeof(len));
			return PmmMorphFrame::load_fixed_arrays(reader, len, is_init);
		}
	};

	struct PmmOpFrame
	{
		std::int32_t data_index;
		std::int32_t frame_number;
		std::int32_t pre_index;
		std::int32_t next_index;
		std::uint8_t is_display;
		std::vector<std::uint8_t> is_ik_enabled;
		std::vector<PmmInt2> op_data;
		std::uint8_t is_selected;

		PmmOpFrame() noexcept
			: data_index(-1)
			, frame_number(-1)
			, pre_index(-1)
			, next_index(-1)
			, is_display(0)
			, is_selected(0)
		{
		}

		bool load(istream& reader, std::size_t ik_count, std::size_t op_count, bool is_init)
		{
			if (is_init) reader.read((char*)& this->data_index, sizeof(this->data_index));
			reader.read((char*)& this->frame_number, sizeof(this->frame_number));
			reader.read((char*)& this->pre_index, sizeof(this->pre_index));
			reader.read((char*)& this->next_index, sizeof(this->next_index));
			reader.read((char*)& this->is_display, sizeof(this->is_display));
			this->is_ik_enabled = PmmUint8::load_fixed_arrays(reader, ik_count).value();
			this->op_data = PmmInt2::load_fixed_arrays(reader, op_count).value();
			reader.read((char*)& this->is_selected, sizeof(this->is_selected));

			return true;
		}

		static std::optional<std::vector<PmmOpFrame>> load_arrays(istream& reader, std::size_t ik_count, std::size_t op_count, bool is_init)
		{
			std::uint32_t len = 0;
			reader.read((char*)& len, sizeof(len));

			auto array = std::vector<PmmOpFrame>(len);
			for (std::size_t i = 0; i < array.size(); i++)
			{
				PmmOpFrame frame;
				frame.load(reader, ik_count, op_count, is_init);
				array.push_back(frame);
			}

			return array;
		}
	};

	struct PmmBoneCurrentData
	{
		PmmVector3 translation;
		PmmVector4 quaternion;
		std::uint8_t is_edit_un_commited;
		std::uint8_t is_physics_disabled;
		std::uint8_t is_row_selected;

		PmmBoneCurrentData()
			: translation(PmmVector3(0.0, 0.0, 0.0))
			, quaternion(PmmVector4(0.0, 0.0, 0.0, 0.0))
			, is_edit_un_commited(0)
			, is_physics_disabled(0)
			, is_row_selected(0)
		{
		}

		static std::optional<PmmBoneCurrentData> load(istream& reader)
		{
			PmmBoneCurrentData data;
			reader.read((char*)& data.translation, sizeof(data.translation));
			reader.read((char*)& data.quaternion, sizeof(data.quaternion));
			reader.read((char*)& data.is_edit_un_commited, sizeof(data.is_edit_un_commited));
			reader.read((char*)& data.is_physics_disabled, sizeof(data.is_physics_disabled));
			reader.read((char*)& data.is_row_selected, sizeof(data.is_row_selected));

			return data;
		}

		static std::optional<std::vector<PmmBoneCurrentData>> load_arrays(istream& reader)
		{
			std::uint32_t len = 0;
			reader.read((char*)& len, sizeof(len));

			auto model = std::vector<PmmBoneCurrentData>(len);
			for (std::size_t i = 0; i < model.size(); i++)
			{
				model.push_back(PmmBoneCurrentData::load(reader).value());
			}

			return model;
		}
	};

	struct PmmOpCurrentData
	{
		std::int32_t keyframe_begin;
		std::int32_t keyframe_end;
		std::int32_t model_index;
		std::int32_t parent_bone_index;

		PmmOpCurrentData()
			: keyframe_begin(1)
			, keyframe_end(-1)
			, model_index(-1)
			, parent_bone_index(-1)
		{

		}

		static std::optional<PmmOpCurrentData> load(istream& reader)
		{
			PmmOpCurrentData data;
			reader.read((char*)& data.keyframe_begin, sizeof(data.keyframe_begin));
			reader.read((char*)& data.keyframe_end, sizeof(data.keyframe_end));
			reader.read((char*)& data.model_index, sizeof(data.model_index));
			reader.read((char*)& data.parent_bone_index, sizeof(data.parent_bone_index));
			return data;
		}

		static std::optional<std::vector<PmmOpCurrentData>> load_arrays(istream& reader)
		{
			std::uint32_t len = 0;
			reader.read((char*)& len, sizeof(len));

			auto model = std::vector<PmmOpCurrentData>(len);
			for (std::size_t i = 0; i < model.size(); i++)
			{
				model.push_back(PmmOpCurrentData::load(reader).value());
			}

			return model;
		}
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
		std::vector<PmmBoneFrame> bone_init_frame;
		std::vector<PmmBoneFrame> bone_key_frame;
		std::vector<PmmMorphFrame> morph_init_frame;
		std::vector<PmmMorphFrame> morph_key_frame;
		PmmOpFrame op_init_frame;
		std::vector<PmmOpFrame> op_key_frames;
		std::vector<PmmBoneCurrentData> bone_current_datas;
		std::vector<float> morph_current_datas;
		std::vector<std::uint8_t> is_current_ik_enabled_datas;
		std::vector<PmmOpCurrentData> op_current_data;
		std::uint8_t is_add_blend;
		float edge_width;
		std::uint8_t is_selfshadow_enabled;
		std::uint8_t calc_order;

		PmmModel()
			: number(0)
			, keyframe_editor_toplevel_rows(0)
			, draw_order(0)
			, edit_is_display(0)
			, edit_selected_bone(0)
			, skin_panel()
			, vscroll(0)
			, last_frame(0)
			, is_add_blend(0)
			, edge_width(1.0)
			, is_selfshadow_enabled(1)
			, calc_order()
		{
		}

		bool load(istream& reader)
		{
			reader.read((char*)& this->number, sizeof(this->number));
			this->name = PmmName::load(reader).value();
			this->name_en = PmmName::load(reader).value();
			this->path = PmmName::load_fixed_utf8(reader, 256).value();
			reader.read((char*)& this->keyframe_editor_toplevel_rows, sizeof(this->keyframe_editor_toplevel_rows));
			this->bone_name = PmmName::load_arrays(reader).value();
			this->morph_name = PmmName::load_arrays(reader).value();
			this->ik_index = PmmInt32::load_arrays(reader).value();
			this->op_index = PmmInt32::load_arrays(reader).value();
			reader.read((char*)& this->draw_order, sizeof(this->draw_order));
			reader.read((char*)& this->edit_is_display, sizeof(this->edit_is_display));
			reader.read((char*)& this->edit_selected_bone, sizeof(this->edit_selected_bone));
			reader.read((char*)& this->skin_panel, sizeof(this->skin_panel));
			this->is_frame_open = PmmUint8::load_array_from_u8(reader).value();
			reader.read((char*)& this->vscroll, sizeof(this->vscroll));
			reader.read((char*)& this->last_frame, sizeof(this->last_frame));
			this->bone_init_frame = PmmBoneFrame::load_fixed_arrays(reader, this->bone_name.size(), true).value();
			this->bone_key_frame = PmmBoneFrame::load_arrays(reader, false).value();
			this->morph_init_frame = PmmMorphFrame::load_fixed_arrays(reader, this->morph_name.size(), true).value();
			this->morph_key_frame = PmmMorphFrame::load_arrays(reader, false).value();
			this->op_init_frame.load(reader, this->ik_index.size(), this->op_index.size(), true);
			this->op_key_frames = PmmOpFrame::load_arrays(reader, this->ik_index.size(), this->op_index.size(), false).value();

			for (std::size_t i = 0; i < this->bone_name.size(); i++)
			{
				this->bone_current_datas.push_back(PmmBoneCurrentData::load(reader).value());
			}

			for (std::size_t i = 0; i < this->morph_name.size(); i++)
			{
				float ch = 0.0;
				reader.read((char*)& ch, sizeof(ch));
				this->morph_current_datas.push_back(ch);
			}

			for (std::size_t i = 0; i < this->ik_index.size(); i++)
			{
				std::uint8_t ch = 0.0;
				reader.read((char*)& ch, sizeof(ch));
				this->is_current_ik_enabled_datas.push_back(ch);
			}

			for (std::size_t i = 0; i < this->op_index.size(); i++)
			{
				this->op_current_data.push_back(PmmOpCurrentData::load(reader).value());
			}

			reader.read((char*)& this->is_add_blend, sizeof(this->is_add_blend));
			reader.read((char*)& this->edge_width, sizeof(this->edge_width));
			reader.read((char*)& this->is_selfshadow_enabled, sizeof(this->is_selfshadow_enabled));
			reader.read((char*)& this->calc_order, sizeof(this->calc_order));

			return true;
		}

		static std::optional<std::vector<PmmModel>> load_arrays(istream& reader)
		{
			std::uint8_t len = 0;
			reader.read((char*)& len, sizeof(len));

			auto models = std::vector<PmmModel>(len);
			for (std::size_t i = 0; i < models.size(); i++)
			{
				PmmModel model;
				model.load(reader);
				models.push_back(model);
			}

			return models;
		}
	};

	struct PmmCameraFrame
	{
		std::int32_t data_index;
		std::int32_t frame_number;
		std::int32_t pre_index;
		std::int32_t next_index;
		float distance;
		PmmVector3 eye_position;
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
		std::uint32_t angle_view;
		std::uint8_t is_selected;

		PmmCameraFrame()
			: data_index(-1)
			, frame_number(0)
			, pre_index(0)
			, next_index(0)
			, distance(0.0)
			, eye_position(PmmVector3(0.0, 0.0, 0.0))
			, rotation(PmmVector3(0.0, 0.0, 0.0))
			, looking_model_index(0)
			, looking_bone_index(0)
			, interpolation_x()
			, interpolation_y()
			, interpolation_z()
			, interpolation_rotation()
			, interpolation_distance()
			, interpolation_angleview()
			, is_parse(0)
			, angle_view(0)
			, is_selected(0)
		{
		}

		static std::optional<PmmCameraFrame> load(istream& reader, bool is_init)
		{
			PmmCameraFrame data;
			if (is_init) reader.read((char*)& data.data_index, sizeof(data.data_index));
			reader.read((char*)& data.frame_number, sizeof(data.frame_number));
			reader.read((char*)& data.pre_index, sizeof(data.pre_index));
			reader.read((char*)& data.next_index, sizeof(data.next_index));
			reader.read((char*)& data.distance, sizeof(data.distance));
			reader.read((char*)& data.eye_position, sizeof(data.eye_position));
			reader.read((char*)& data.rotation, sizeof(data.rotation));
			reader.read((char*)& data.looking_model_index, sizeof(data.looking_model_index));
			reader.read((char*)& data.looking_bone_index, sizeof(data.looking_bone_index));
			reader.read((char*)& data.interpolation_x, sizeof(data.interpolation_x));
			reader.read((char*)& data.interpolation_y, sizeof(data.interpolation_y));
			reader.read((char*)& data.interpolation_z, sizeof(data.interpolation_z));
			reader.read((char*)& data.interpolation_rotation, sizeof(data.interpolation_rotation));
			reader.read((char*)& data.interpolation_distance, sizeof(data.interpolation_distance));
			reader.read((char*)& data.interpolation_angleview, sizeof(data.interpolation_angleview));
			reader.read((char*)& data.is_parse, sizeof(data.is_parse));
			reader.read((char*)& data.angle_view, sizeof(data.angle_view));
			reader.read((char*)& data.is_selected, sizeof(data.is_selected));

			return data;
		}

		static std::optional<std::vector<PmmCameraFrame>> load_arrays(istream& reader, bool is_init)
		{
			std::uint32_t len = 0;
			reader.read((char*)& len, sizeof(len));

			auto model = std::vector<PmmCameraFrame>(len);
			for (std::size_t i = 0; i < model.size(); i++)
			{
				PmmCameraFrame frame;
				frame.load(reader, is_init);
				model.push_back(frame);
			}

			return model;
		}
	};

	struct PmmCameraCurrentData
	{
		PmmVector3 eye_position;
		PmmVector3 target_position;
		PmmVector3 rotation;
		std::uint8_t  isorthro;

		PmmCameraCurrentData()
			: eye_position(PmmVector3(0.0, 0.0, 0.0))
			, target_position(PmmVector3(0.0, 0.0, 0.0))
			, rotation(PmmVector3(0.0, 0.0, 0.0))
			, isorthro(0)
		{
		}

		static std::optional<PmmCameraCurrentData> load(istream& reader)
		{
			PmmCameraCurrentData data;
			reader.read((char*)& data.eye_position, sizeof(data.eye_position));
			reader.read((char*)& data.target_position, sizeof(data.target_position));
			reader.read((char*)& data.rotation, sizeof(data.rotation));
			reader.read((char*)& data.isorthro, sizeof(data.isorthro));

			return data;
		}

		static std::optional<std::vector<PmmCameraCurrentData>> load_arrays(istream& reader)
		{
			std::uint8_t len = 0;
			reader.read((char*)& len, sizeof(len));

			auto model = std::vector<PmmCameraCurrentData>(len);
			for (std::size_t i = 0; i < model.size(); i++)
			{
				model.push_back(PmmCameraCurrentData::load(reader).value());
			}

			return model;
		}
	};

	struct PmmLightFrame
	{
		std::int32_t data_index;
		std::int32_t frame_number;
		std::int32_t pre_index;
		std::int32_t next_index;
		PmmVector3 rgb;
		PmmVector3 xyz;
		std::uint8_t is_selected;

		PmmLightFrame()
			: data_index(-1)
			, frame_number(-1)
			, pre_index(-1)
			, next_index(-1)
			, rgb(PmmVector3(0.0, 0.0, 0.0))
			, xyz(PmmVector3(0.0, 0.0, 0.0))
			, is_selected(0)
		{
		}

		static std::optional<PmmLightFrame> load(istream& reader, bool is_init)
		{
			PmmLightFrame data;
			if (is_init) reader.read((char*)& data.data_index, sizeof(data.data_index));
			reader.read((char*)& data.frame_number, sizeof(data.frame_number));
			reader.read((char*)& data.pre_index, sizeof(data.pre_index));
			reader.read((char*)& data.next_index, sizeof(data.next_index));
			reader.read((char*)& data.rgb, sizeof(data.rgb));
			reader.read((char*)& data.xyz, sizeof(data.xyz));
			reader.read((char*)& data.is_selected, sizeof(data.is_selected));

			return data;
		}

		static std::optional<std::vector<PmmLightFrame>> load_arrays(istream& reader, bool is_init)
		{
			std::uint32_t len = 0;
			reader.read((char*)& len, sizeof(len));

			auto model = std::vector<PmmLightFrame>(len);
			for (std::size_t i = 0; i < model.size(); i++)
			{
				PmmLightFrame frame;
				frame.load(reader, is_init);
				model.push_back(frame);
			}

			return model;
		}
	};

	struct PmmLightCurrentData
	{
		PmmVector3 rgb;
		PmmVector3 xyz;
		std::uint8_t is_selected;

		PmmLightCurrentData()
			: rgb(PmmVector3(0.0, 0.0, 0.0))
			, xyz(PmmVector3(0.0, 0.0, 0.0))
			, is_selected(0)
		{
		}

		static std::optional<PmmLightCurrentData> load(istream& reader)
		{
			auto data = PmmLightCurrentData();
			reader.read((char*)& data.rgb, sizeof(data.rgb));
			reader.read((char*)& data.xyz, sizeof(data.xyz));
			reader.read((char*)& data.is_selected, sizeof(data.is_selected));

			return data;
		}

		static std::optional<std::vector<PmmLightCurrentData>> load_arrays(istream& reader)
		{
			std::uint8_t len = 0;
			reader.read((char*)& len, sizeof(len));

			auto model = std::vector<PmmLightCurrentData>(len);
			for (std::size_t i = 0; i < model.size(); i++)
			{
				PmmLightCurrentData data;
				data.load(reader);
				model.push_back(data);
			}

			return model;
		}
	};

	struct PmmKeyFrame
	{
		std::int32_t data_index;
		std::int32_t frame_number;
		std::int32_t pre_index;
		std::int32_t next_index;
		std::uint8_t is_selected;

		PmmKeyFrame()
			: data_index(-1)
			, frame_number(-1)
			, pre_index(-1)
			, next_index(-1)
			, is_selected(0)
		{
		}

		static std::optional<PmmKeyFrame> load(istream& reader)
		{
			auto data = PmmKeyFrame();
			reader.read((char*)& data.data_index, sizeof(data.data_index));
			reader.read((char*)& data.frame_number, sizeof(data.frame_number));
			reader.read((char*)& data.pre_index, sizeof(data.pre_index));
			reader.read((char*)& data.next_index, sizeof(data.next_index));
			reader.read((char*)& data.is_selected, sizeof(data.is_selected));

			return data;
		}

		static std::optional<std::vector<PmmKeyFrame>> load_arrays(istream& reader)
		{
			std::uint8_t len = 0;
			reader.read((char*)& len, sizeof(len));

			auto data = std::vector<PmmKeyFrame>(len);
			for (std::size_t i = 0; i < data.size(); i++)
				data.push_back(PmmKeyFrame::load(reader).value());

			return data;
		}
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

		PmmDataBody()
			: transparency(0)
			, is_visible(0)
			, parent_model_index(0)
			, parent_bone_index(0)
			, translation(PmmVector3(0.0, 0.0, 0.0))
			, rotation(PmmVector3(0.0, 0.0, 0.0))
			, scale(0.0)
			, is_shadow_enabled(0)
		{
		}

		static std::optional<PmmDataBody> load(istream& reader)
		{
			auto data = PmmDataBody();
			reader.read((char*)& data.transparency, sizeof(data.transparency));
			reader.read((char*)& data.parent_model_index, sizeof(data.parent_model_index));
			reader.read((char*)& data.parent_bone_index, sizeof(data.parent_bone_index));
			reader.read((char*)& data.translation, sizeof(data.translation));
			reader.read((char*)& data.rotation, sizeof(data.rotation));
			reader.read((char*)& data.scale, sizeof(data.scale));
			reader.read((char*)& data.is_shadow_enabled, sizeof(data.is_shadow_enabled));

			return data;
		}

		static std::optional<std::vector<PmmDataBody>> load_arrays(istream& reader)
		{
			std::uint8_t len = 0;
			reader.read((char*)& len, sizeof(len));

			auto model = std::vector<PmmDataBody>(len);
			for (std::size_t i = 0; i < model.size(); i++)
				model.push_back(PmmDataBody::load(reader).value());

			return model;
		}
	};

	struct PmmAccessoryData
	{
		std::uint8_t index;
		std::string name;
		std::string path;
		std::uint8_t draw_order;
		PmmKeyFrame init_frame;
		std::vector<PmmKeyFrame> key_frames;
		PmmDataBody current_data;
		std::uint8_t is_add_blend;

		PmmAccessoryData()
			: index(0)
			, draw_order(0)
			, is_add_blend(0)
		{
		}

		static std::optional<PmmAccessoryData> load(istream& reader)
		{
			PmmAccessoryData data;
			reader.read((char*)& data.index, sizeof(data.index));
			data.name = PmmName::load_fixed_utf8(reader, 100).value();
			data.path = PmmName::load_fixed_utf8(reader, 256).value();
			reader.read((char*)& data.draw_order, sizeof(data.draw_order));
			data.init_frame.load(reader);
			data.key_frames = PmmKeyFrame::load_arrays(reader).value();
			data.current_data.load(reader);
			reader.read((char*)& data.is_add_blend, sizeof(data.is_add_blend));

			return data;
		}

		static std::optional<std::vector<PmmAccessoryData>> load_arrays(istream& reader)
		{
			std::uint8_t len = 0;
			reader.read((char*)& len, sizeof(len));

			auto model = std::vector<PmmAccessoryData>(len);
			for (std::size_t i = 0; i < model.size(); i++)
			{
				PmmAccessoryData data;
				data.load(reader);
				model.push_back(data);
			}

			return model;
		}
	};

	struct PmmGravityCurrentData
	{
		float acceleration;
		std::uint32_t noize_amount;
		PmmVector3 direction;
		std::uint8_t is_add_noize;

		PmmGravityCurrentData()
			:acceleration(0.0)
			, noize_amount(0)
			, direction(PmmVector3(0.0, 0.0, 0.0))
			, is_add_noize(0)
		{
		}

		static std::optional<PmmGravityCurrentData> load(istream& reader)
		{
			PmmGravityCurrentData data;
			reader.read((char*)& data.acceleration, sizeof(data.acceleration));
			reader.read((char*)& data.noize_amount, sizeof(data.noize_amount));
			reader.read((char*)& data.direction, sizeof(data.direction));
			reader.read((char*)& data.is_add_noize, sizeof(data.is_add_noize));

			return data;
		}

		static std::optional<std::vector<PmmGravityCurrentData>> load_arrays(istream& reader)
		{
			std::uint32_t len = 0;
			reader.read((char*)& len, sizeof(len));

			auto array = std::vector<PmmGravityCurrentData>(len);
			for (std::size_t i = 0; i < array.size(); i++)
			{
				PmmGravityCurrentData data;
				data.load(reader);
				array.push_back(data);
			}

			return array;
		}
	};

	struct PmmGravityKeyFrame
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

		PmmGravityKeyFrame()
			: data_index(-1)
			, frame_number(-1)
			, pre_index(-1)
			, next_index(-1)
			, is_add_noize(0)
			, noize_amount(0)
			, acceleration(0.0)
			, direction(PmmVector3(0, 1, 0))
			, is_selected(0)
		{
		}

		static std::optional<PmmGravityKeyFrame> load(istream& reader, bool is_init)
		{
			PmmGravityKeyFrame data;
			reader.read((char*)& data.data_index, sizeof(data.data_index));
			reader.read((char*)& data.frame_number, sizeof(data.frame_number));
			reader.read((char*)& data.pre_index, sizeof(data.pre_index));
			reader.read((char*)& data.next_index, sizeof(data.next_index));
			reader.read((char*)& data.is_add_noize, sizeof(data.is_add_noize));
			reader.read((char*)& data.noize_amount, sizeof(data.noize_amount));
			reader.read((char*)& data.acceleration, sizeof(data.acceleration));
			reader.read((char*)& data.direction, sizeof(data.direction));
			reader.read((char*)& data.is_selected, sizeof(data.is_selected));

			return data;
		}

		static std::optional<std::vector<PmmGravityKeyFrame>> load_arrays(istream& reader, bool is_init)
		{
			std::uint32_t len = 0;
			reader.read((char*)& len, sizeof(len));

			auto array = std::vector<PmmGravityKeyFrame>(len);
			for (std::size_t i = 0; i < array.size(); i++)
			{
				PmmGravityKeyFrame frame;
				frame.load(reader, is_init);
				array.push_back(frame);
			}

			return array;
		}
	};

	struct PmmSelfShadowKeyFrame
	{
		std::int32_t data_index;
		std::int32_t frame_number;
		std::int32_t pre_index;
		std::int32_t next_index;
		std::uint8_t mode;
		float distance;
		std::uint8_t is_selected;

		PmmSelfShadowKeyFrame()
			: data_index(-1)
			, frame_number(-1)
			, pre_index(-1)
			, next_index(-1)
			, mode(0)
			, distance(0.0)
			, is_selected(0)
		{
		}

		static std::optional<PmmSelfShadowKeyFrame> load(istream& reader, bool is_init)
		{
			PmmSelfShadowKeyFrame data;
			reader.read((char*)& data.data_index, sizeof(data.data_index));
			reader.read((char*)& data.frame_number, sizeof(data.frame_number));
			reader.read((char*)& data.pre_index, sizeof(data.pre_index));
			reader.read((char*)& data.next_index, sizeof(data.next_index));
			reader.read((char*)& data.mode, sizeof(data.mode));
			reader.read((char*)& data.distance, sizeof(data.distance));
			reader.read((char*)& data.is_selected, sizeof(data.is_selected));

			return data;
		}

		static std::optional<std::vector<PmmSelfShadowKeyFrame>> load_arrays(istream& reader, bool is_init)
		{
			std::uint32_t len = 0;
			reader.read((char*)& len, sizeof(len));

			auto array = std::vector<PmmSelfShadowKeyFrame>(len);
			for (std::size_t i = 0; i < array.size(); i++)
			{
				PmmSelfShadowKeyFrame frame;
				frame.load(reader, is_init);
				array.push_back(frame);
			}

			return array;
		}
	};

	struct PmmCSelectorChoiceData
	{
		std::uint8_t mode_index;
		std::uint32_t selector_choice;

		PmmCSelectorChoiceData()
			: mode_index(0)
			, selector_choice(0)
		{
		}

		static std::optional<PmmCSelectorChoiceData> load(istream& reader)
		{
			PmmCSelectorChoiceData data;
			reader.read((char*)& data.mode_index, sizeof(data.mode_index));
			reader.read((char*)& data.selector_choice, sizeof(data.selector_choice));

			return data;
		}

		static std::optional<std::vector<PmmCSelectorChoiceData>> load_arrays(istream& reader)
		{
			std::uint8_t len = 0;
			reader.read((char*)& len, sizeof(len));

			auto model = std::vector<PmmCSelectorChoiceData>(len);
			for (std::size_t i = 0; i < model.size(); i++)
			{
				PmmCSelectorChoiceData data;
				data.load(reader);
				model.push_back(data);
			}

			return model;
		}
	};

	struct PMMFile
	{
		PmmHeader header;
		std::vector<PmmModel> model;
		PmmCameraFrame camera_init_frame;
		std::vector<PmmCameraFrame> camera_key_frames;
		PmmCameraCurrentData camera_current_data;
		PmmLightFrame light_init_frame;
		std::vector<PmmLightFrame> light_key_frames;
		PmmLightCurrentData light_current_data;
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
		PmmGravityCurrentData gravity_current_data;
		PmmGravityKeyFrame gravity_init_frame;
		std::vector<PmmGravityKeyFrame> gravity_key_frames;
		std::uint8_t is_show_selfshadow;
		float selfshadow_current_data;
		PmmSelfShadowKeyFrame selfshadow_init_frame;
		std::vector<PmmSelfShadowKeyFrame> selfshadow_keyframes;
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
		std::vector<PmmCSelectorChoiceData> selector_choice_datas;

		PMMFile() noexcept
			:selected_accessory_index(0)
			, accessory_vscroll(0)
			, accessory_count(0)
			, current_frame_position(0)
			, h_scroll_position(0)
			, h_scroll_scale(0)
			, bone_operation_kind(0)
			, looking_at(0)
			, is_repeat(0)
			, is_play_from_frame(0)
			, is_play_to_frame(0)
			, play_start_frame(0)
			, play_end_frame(0)
			, is_wave_enabled(0)
			, avi_offset_x(0)
			, avi_offset_y(0)
			, avi_scale(0.0)
			, is_show_avi(0)
			, background_image_offset_x(0)
			, background_image_offset_y(0)
			, background_image_scale(0)
			, is_show_background_image(0)
			, is_show_infomation(0)
			, is_show_axis(0)
			, is_show_groundshadow(0)
			, fps_limit(0.0)
			, screen_capture_mode(0)
			, accessory_number_render_after_model(0)
			, ground_shadow_brightness(0.0)
			, is_transparent_ground_shadow(0)
			, physics_mode(0)
			, is_show_selfshadow(0)
			, selfshadow_current_data(0.0)
			, edge_color_r(0)
			, edge_color_g(0)
			, edge_color_b(0)
			, is_black_background(0)
			, camera_current_looking_at_model(0)
			, camera_current_looking_at_bone(0)
			, is_view_look_at_enabled(0)
			, unknown(0)
			, is_physics_ground_enabled(0)
			, frame_text_box(0)
			, selector_choice_selection_following(0)
		{
			std::memset(&unknown_array, 0, sizeof(unknown_array));
		}

		static std::optional<PMMFile> load(istream& reader)
		{
			auto pmm = PMMFile();
			pmm.header.load(reader);
			pmm.model = PmmModel::load_arrays(reader).value();
			pmm.camera_init_frame.load(reader, true);
			pmm.camera_key_frames = PmmCameraFrame::load_arrays(reader, false).value();
			pmm.camera_current_data.load(reader);
			pmm.light_init_frame.load(reader, true);
			pmm.light_key_frames = PmmLightFrame::load_arrays(reader, false).value();
			pmm.light_current_data.load(reader);
			reader.read((char*)& pmm.selected_accessory_index, sizeof(pmm.selected_accessory_index));
			reader.read((char*)& pmm.accessory_vscroll, sizeof(pmm.accessory_vscroll));
			reader.read((char*)& pmm.accessory_count, sizeof(pmm.accessory_count));

			for (std::size_t i = 0; i < pmm.accessory_count; i++)
			{
				pmm.accessory_name.push_back(PmmName::load_fixed_utf8(reader, 100).value());
			}

			for (std::size_t i = 0; i < pmm.accessory_count; i++)
			{
				PmmAccessoryData data;
				data.load(reader);
				pmm.accessory_datas.push_back(data);
			}

			reader.read((char*)& pmm.current_frame_position, sizeof(pmm.current_frame_position));
			reader.read((char*)& pmm.h_scroll_position, sizeof(pmm.h_scroll_position));
			reader.read((char*)& pmm.h_scroll_scale, sizeof(pmm.h_scroll_scale));
			reader.read((char*)& pmm.bone_operation_kind, sizeof(pmm.bone_operation_kind));
			reader.read((char*)& pmm.looking_at, sizeof(pmm.looking_at));
			reader.read((char*)& pmm.is_repeat, sizeof(pmm.is_repeat));
			reader.read((char*)& pmm.is_play_from_frame, sizeof(pmm.is_play_from_frame));
			reader.read((char*)& pmm.is_play_to_frame, sizeof(pmm.is_play_to_frame));
			reader.read((char*)& pmm.play_start_frame, sizeof(pmm.play_start_frame));
			reader.read((char*)& pmm.play_end_frame, sizeof(pmm.play_end_frame));
			reader.read((char*)& pmm.is_wave_enabled, sizeof(pmm.is_wave_enabled));
			pmm.wave_path = PmmName::load_fixed_utf8(reader, 256).value();
			reader.read((char*)& pmm.avi_offset_x, sizeof(pmm.avi_offset_x));
			reader.read((char*)& pmm.avi_offset_y, sizeof(pmm.avi_offset_y));
			reader.read((char*)& pmm.avi_scale, sizeof(pmm.avi_scale));
			pmm.avi_path = PmmName::load_fixed_utf8(reader, 256).value();
			reader.read((char*)& pmm.is_show_avi, sizeof(pmm.is_show_avi));
			reader.read((char*)& pmm.background_image_offset_x, sizeof(pmm.background_image_offset_x));
			reader.read((char*)& pmm.background_image_offset_y, sizeof(pmm.background_image_offset_y));
			reader.read((char*)& pmm.background_image_scale, sizeof(pmm.background_image_scale));
			pmm.background_image_path = PmmName::load_fixed_utf8(reader, 255).value();
			reader.read((char*)& pmm.is_show_background_image, sizeof(pmm.is_show_background_image));
			reader.read((char*)& pmm.is_show_infomation, sizeof(pmm.is_show_infomation));
			reader.read((char*)& pmm.is_show_axis, sizeof(pmm.is_show_axis));
			reader.read((char*)& pmm.is_show_groundshadow, sizeof(pmm.is_show_groundshadow));
			reader.read((char*)& pmm.fps_limit, sizeof(pmm.fps_limit));
			reader.read((char*)& pmm.screen_capture_mode, sizeof(pmm.screen_capture_mode));
			reader.read((char*)& pmm.accessory_number_render_after_model, sizeof(pmm.accessory_number_render_after_model));
			reader.read((char*)& pmm.ground_shadow_brightness, sizeof(pmm.ground_shadow_brightness));
			reader.read((char*)& pmm.is_transparent_ground_shadow, sizeof(pmm.is_transparent_ground_shadow));
			reader.read((char*)& pmm.physics_mode, sizeof(pmm.physics_mode));
			pmm.gravity_current_data.load(reader);
			pmm.gravity_init_frame.load(reader, true);
			pmm.gravity_key_frames = PmmGravityKeyFrame::load_arrays(reader, false).value();
			reader.read((char*)& pmm.is_show_selfshadow, sizeof(pmm.is_show_selfshadow));
			reader.read((char*)& pmm.selfshadow_current_data, sizeof(pmm.selfshadow_current_data));
			pmm.selfshadow_init_frame.load(reader, true);
			pmm.selfshadow_keyframes = PmmSelfShadowKeyFrame::load_arrays(reader, false).value();
			reader.read((char*)& pmm.edge_color_r, sizeof(pmm.edge_color_r));
			reader.read((char*)& pmm.edge_color_g, sizeof(pmm.edge_color_g));
			reader.read((char*)& pmm.edge_color_b, sizeof(pmm.edge_color_b));
			reader.read((char*)& pmm.is_black_background, sizeof(pmm.is_black_background));
			reader.read((char*)& pmm.camera_current_looking_at_model, sizeof(pmm.camera_current_looking_at_model));
			reader.read((char*)& pmm.camera_current_looking_at_bone, sizeof(pmm.camera_current_looking_at_bone));

			for (std::size_t i = 0; i < 16; i++)
			{
				reader.read((char*)& pmm.unknown_array[i], sizeof(pmm.unknown_array[i]));
			}
			reader.read((char*)& pmm.is_view_look_at_enabled, sizeof(pmm.is_view_look_at_enabled));
			reader.read((char*)& pmm.unknown, sizeof(pmm.unknown));
			reader.read((char*)& pmm.is_physics_ground_enabled, sizeof(pmm.is_physics_ground_enabled));
			reader.read((char*)& pmm.frame_text_box, sizeof(pmm.frame_text_box));
			reader.read((char*)& pmm.selector_choice_selection_following, sizeof(pmm.selector_choice_selection_following));
			pmm.selector_choice_datas = PmmCSelectorChoiceData::load_arrays(reader).value();

			return pmm;
		}
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