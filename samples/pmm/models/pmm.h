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
			for (auto& it : array)
				it.load(reader);

			return array;
		}

		static std::optional<std::vector<PmmInt2>> load_fixed_arrays(istream& reader, std::size_t len)
		{
			auto array = std::vector<PmmInt2>(len);
			for (auto& it : array)
				it.load(reader);

			return array;
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
			auto bytes = std::string();
			for (std::size_t i = 0; i < len; i++)
			{
				char ch = 0;
				reader.read(& ch, 1);
				if (ch == 0)
					break;
				bytes += ch;
			}

			if (bytes.size() < len - 1)
				reader.seekg(len - bytes.size() - 1, std::ios_base::cur);

			return bytes;
			// return WINDOWS_31J.decode(&bytes, DecoderTrap::Ignore).unwrap();
		}

		static std::optional<std::vector<std::string>> load_arrays(istream& reader)
		{
			std::uint32_t len = 0;
			reader.read((char*)& len, sizeof(len));

			auto array = std::vector<std::string>(len);
			for (std::size_t i = 0; i < len; i++)
				array[i] = PmmName::load(reader).value();

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
			for (std::size_t i = 0; i < len; i++)
			{
				std::uint8_t ch = 0;
				reader.read((char*)& ch, sizeof(ch));
				array[i] = ch;
			}

			return array;
		}

		static std::optional<std::vector<std::uint8_t>> load_fixed_arrays(istream& reader, std::size_t len)
		{
			auto array = std::vector<std::uint8_t>(len);
			for (std::size_t i = 0; i < len; i++)
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
			reader.read((char*)array.data(), len * sizeof(std::int32_t));

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
			reader.read((char*)array.data(), len * sizeof(float));

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
		std::uint8_t is_edit_camera_light_accessory;
		std::uint8_t is_open_camera_panel;
		std::uint8_t is_open_light_panel;
		std::uint8_t is_open_accessory_panel;
		std::uint8_t is_open_bone_panel;
		std::uint8_t is_open_morph_panel;
		std::uint8_t is_open_selfshadow_panel;
		std::uint8_t selected_model_index;

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

	struct PmmKeyframe
	{
		std::int32_t data_index;
		std::int32_t frame_number;
		std::int32_t pre_index;
		std::int32_t next_index;
		std::uint8_t is_selected;

		PmmKeyframe()
			: data_index(-1)
			, frame_number(-1)
			, pre_index(-1)
			, next_index(-1)
			, is_selected(0)
		{
		}

		static std::optional<PmmKeyframe> load(istream& reader)
		{
			auto data = PmmKeyframe();
			reader.read((char*)& data.data_index, sizeof(data.data_index));
			reader.read((char*)& data.frame_number, sizeof(data.frame_number));
			reader.read((char*)& data.pre_index, sizeof(data.pre_index));
			reader.read((char*)& data.next_index, sizeof(data.next_index));
			reader.read((char*)& data.is_selected, sizeof(data.is_selected));

			return data;
		}

		static std::optional<std::vector<PmmKeyframe>> load_arrays(istream& reader)
		{
			std::uint8_t len = 0;
			reader.read((char*)& len, sizeof(len));

			auto data = std::vector<PmmKeyframe>(len);
			for (std::size_t i = 0; i < data.size(); i++)
				data[i] = PmmKeyframe::load(reader).value();

			return data;
		}
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

		PmmKeyframeBone()
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
			if (!is_init) reader.read((char*)& this->data_index, sizeof(this->data_index));
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

		static std::optional<std::vector<PmmKeyframeBone>> load_fixed_arrays(istream& reader, std::size_t len, bool is_init)
		{
			auto array = std::vector<PmmKeyframeBone>(len);
			for (std::size_t i = 0; i < len; i++)
				array[i].load(reader, is_init);

			return array;
		}

		static std::optional<std::vector<PmmKeyframeBone>> load_arrays(istream& reader, bool is_init)
		{
			std::uint32_t len = 0;
			reader.read((char*)& len, sizeof(len));
			return PmmKeyframeBone::load_fixed_arrays(reader, len, is_init);
		}
	};

	struct PmmKeyframeMorph
	{
		std::int32_t data_index;
		std::int32_t frame_number;
		std::int32_t pre_index;
		std::int32_t next_index;
		float value;
		std::uint8_t is_selected;

		PmmKeyframeMorph()
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
			if (!is_init) reader.read((char*)& this->data_index, sizeof(this->data_index));
			reader.read((char*)& this->frame_number, sizeof(this->frame_number));
			reader.read((char*)& this->pre_index, sizeof(this->pre_index));
			reader.read((char*)& this->next_index, sizeof(this->next_index));
			reader.read((char*)& this->value, sizeof(this->value));
			reader.read((char*)& this->is_selected, sizeof(this->is_selected));

			return true;
		}

		static std::optional<std::vector<PmmKeyframeMorph>> load_fixed_arrays(istream& reader, std::size_t len, bool is_init)
		{
			auto array = std::vector<PmmKeyframeMorph>(len);
			for (std::size_t i = 0; i < len; i++)
				array[i].load(reader, is_init);

			return array;
		}

		static std::optional<std::vector<PmmKeyframeMorph>> load_arrays(istream& reader, bool is_init)
		{
			std::uint32_t len = 0;
			reader.read((char*)& len, sizeof(len));
			return PmmKeyframeMorph::load_fixed_arrays(reader, len, is_init);
		}
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

		PmmKeyframeOp() noexcept
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
			if (!is_init) reader.read((char*)& this->data_index, sizeof(this->data_index));
			reader.read((char*)& this->frame_number, sizeof(this->frame_number));
			reader.read((char*)& this->pre_index, sizeof(this->pre_index));
			reader.read((char*)& this->next_index, sizeof(this->next_index));
			reader.read((char*)& this->is_display, sizeof(this->is_display));
			this->is_ik_enabled = PmmUint8::load_fixed_arrays(reader, ik_count).value();
			this->op_data = PmmInt2::load_fixed_arrays(reader, op_count).value();
			reader.read((char*)& this->is_selected, sizeof(this->is_selected));

			return true;
		}

		static std::optional<std::vector<PmmKeyframeOp>> load_arrays(istream& reader, std::size_t ik_count, std::size_t op_count, bool is_init)
		{
			std::uint32_t len = 0;
			reader.read((char*)& len, sizeof(len));

			auto array = std::vector<PmmKeyframeOp>(len);
			for (std::size_t i = 0; i < len; i++)
				array[i].load(reader, ik_count, op_count, is_init);

			return array;
		}
	};

	struct PmmKeyframeCamera
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

		PmmKeyframeCamera()
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

		static std::optional<PmmKeyframeCamera> load(istream& reader, bool is_init)
		{
			PmmKeyframeCamera data;
			if (!is_init) reader.read((char*)& data.data_index, sizeof(data.data_index));
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

		static std::optional<std::vector<PmmKeyframeCamera>> load_arrays(istream& reader, bool is_init)
		{
			std::uint32_t len = 0;
			reader.read((char*)& len, sizeof(len));

			auto model = std::vector<PmmKeyframeCamera>(len);
			for (std::size_t i = 0; i < model.size(); i++)
			{
				PmmKeyframeCamera frame;
				frame.load(reader, is_init);
				model.push_back(frame);
			}

			return model;
		}
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

		PmmKeyframeLight()
			: data_index(-1)
			, frame_number(-1)
			, pre_index(-1)
			, next_index(-1)
			, rgb(PmmVector3(0.0, 0.0, 0.0))
			, xyz(PmmVector3(0.0, 0.0, 0.0))
			, is_selected(0)
		{
		}

		static std::optional<PmmKeyframeLight> load(istream& reader, bool is_init)
		{
			PmmKeyframeLight data;
			if (!is_init) reader.read((char*)& data.data_index, sizeof(data.data_index));
			reader.read((char*)& data.frame_number, sizeof(data.frame_number));
			reader.read((char*)& data.pre_index, sizeof(data.pre_index));
			reader.read((char*)& data.next_index, sizeof(data.next_index));
			reader.read((char*)& data.rgb, sizeof(data.rgb));
			reader.read((char*)& data.xyz, sizeof(data.xyz));
			reader.read((char*)& data.is_selected, sizeof(data.is_selected));

			return data;
		}

		static std::optional<std::vector<PmmKeyframeLight>> load_arrays(istream& reader, bool is_init)
		{
			std::uint32_t len = 0;
			reader.read((char*)& len, sizeof(len));

			auto model = std::vector<PmmKeyframeLight>(len);
			for (std::size_t i = 0; i < model.size(); i++)
				model[i].load(reader, is_init);

			return model;
		}
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

		PmmKeyFrameGravity()
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

		static std::optional<PmmKeyFrameGravity> load(istream& reader, bool is_init)
		{
			PmmKeyFrameGravity data;
			if (!is_init) reader.read((char*)& data.data_index, sizeof(data.data_index));
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

		static std::optional<std::vector<PmmKeyFrameGravity>> load_arrays(istream& reader, bool is_init)
		{
			std::uint32_t len = 0;
			reader.read((char*)& len, sizeof(len));

			auto array = std::vector<PmmKeyFrameGravity>(len);
			for (std::size_t i = 0; i < len; i++)
				array[i] = PmmKeyFrameGravity::load(reader, is_init).value();

			return array;
		}
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

		PmmKeyFrameSelfShadow()
			: data_index(-1)
			, frame_number(-1)
			, pre_index(-1)
			, next_index(-1)
			, mode(0)
			, distance(0.0)
			, is_selected(0)
		{
		}

		static std::optional<PmmKeyFrameSelfShadow> load(istream& reader, bool is_init)
		{
			PmmKeyFrameSelfShadow data;
			if (!is_init) reader.read((char*)& data.data_index, sizeof(data.data_index));
			reader.read((char*)& data.frame_number, sizeof(data.frame_number));
			reader.read((char*)& data.pre_index, sizeof(data.pre_index));
			reader.read((char*)& data.next_index, sizeof(data.next_index));
			reader.read((char*)& data.mode, sizeof(data.mode));
			reader.read((char*)& data.distance, sizeof(data.distance));
			reader.read((char*)& data.is_selected, sizeof(data.is_selected));

			return data;
		}

		static std::optional<std::vector<PmmKeyFrameSelfShadow>> load_arrays(istream& reader, bool is_init)
		{
			std::uint32_t len = 0;
			reader.read((char*)& len, sizeof(len));

			auto array = std::vector<PmmKeyFrameSelfShadow>(len);
			for (std::size_t i = 0; i < len; i++)
			{
				PmmKeyFrameSelfShadow frame;
				frame.load(reader, is_init);
				array.push_back(frame);
			}

			return array;
		}
	};

	struct PmmBone
	{
		PmmVector3 translation;
		PmmVector4 quaternion;
		std::uint8_t is_edit_un_commited;
		std::uint8_t is_physics_disabled;
		std::uint8_t is_row_selected;

		PmmBone()
			: translation(PmmVector3(0.0, 0.0, 0.0))
			, quaternion(PmmVector4(0.0, 0.0, 0.0, 0.0))
			, is_edit_un_commited(0)
			, is_physics_disabled(0)
			, is_row_selected(0)
		{
		}

		static std::optional<PmmBone> load(istream& reader)
		{
			PmmBone data;
			reader.read((char*)& data.translation, sizeof(data.translation));
			reader.read((char*)& data.quaternion, sizeof(data.quaternion));
			reader.read((char*)& data.is_edit_un_commited, sizeof(data.is_edit_un_commited));
			reader.read((char*)& data.is_physics_disabled, sizeof(data.is_physics_disabled));
			reader.read((char*)& data.is_row_selected, sizeof(data.is_row_selected));

			return data;
		}

		static std::optional<std::vector<PmmBone>> load_arrays(istream& reader)
		{
			std::uint32_t len = 0;
			reader.read((char*)& len, sizeof(len));

			auto model = std::vector<PmmBone>(len);
			for (std::size_t i = 0; i < model.size(); i++)
			{
				model.push_back(PmmBone::load(reader).value());
			}

			return model;
		}
	};

	struct PmmOp
	{
		std::int32_t keyframe_begin;
		std::int32_t keyframe_end;
		std::int32_t model_index;
		std::int32_t parent_bone_index;

		PmmOp()
			: keyframe_begin(1)
			, keyframe_end(-1)
			, model_index(-1)
			, parent_bone_index(-1)
		{

		}

		static std::optional<PmmOp> load(istream& reader)
		{
			PmmOp data;
			reader.read((char*)& data.keyframe_begin, sizeof(data.keyframe_begin));
			reader.read((char*)& data.keyframe_end, sizeof(data.keyframe_end));
			reader.read((char*)& data.model_index, sizeof(data.model_index));
			reader.read((char*)& data.parent_bone_index, sizeof(data.parent_bone_index));
			return data;
		}

		static std::optional<std::vector<PmmOp>> load_arrays(istream& reader)
		{
			std::uint32_t len = 0;
			reader.read((char*)& len, sizeof(len));

			auto model = std::vector<PmmOp>(len);
			for (std::size_t i = 0; i < model.size(); i++)
			{
				model.push_back(PmmOp::load(reader).value());
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
			, calc_order(0)
		{
		}

		static std::optional<PmmModel> load(istream& reader)
		{
			PmmModel data;
			reader.read((char*)& data.number, sizeof(data.number));
			data.name = PmmName::load(reader).value();
			data.name_en = PmmName::load(reader).value();
			data.path = PmmName::load_fixed_utf8(reader, 256).value();
			reader.read((char*)& data.keyframe_editor_toplevel_rows, sizeof(data.keyframe_editor_toplevel_rows));
			data.bone_name = PmmName::load_arrays(reader).value();
			data.morph_name = PmmName::load_arrays(reader).value();
			data.ik_index = PmmInt32::load_arrays(reader).value();
			data.op_index = PmmInt32::load_arrays(reader).value();
			reader.read((char*)& data.draw_order, sizeof(data.draw_order));
			reader.read((char*)& data.edit_is_display, sizeof(data.edit_is_display));
			reader.read((char*)& data.edit_selected_bone, sizeof(data.edit_selected_bone));
			reader.read((char*)& data.skin_panel, sizeof(data.skin_panel));
			data.is_frame_open = PmmUint8::load_array_from_u8(reader).value();
			reader.read((char*)& data.vscroll, sizeof(data.vscroll));
			reader.read((char*)& data.last_frame, sizeof(data.last_frame));
			data.bone_init_frame = PmmKeyframeBone::load_fixed_arrays(reader, data.bone_name.size(), true).value();
			data.bone_key_frame = PmmKeyframeBone::load_arrays(reader, false).value();
			data.morph_init_frame = PmmKeyframeMorph::load_fixed_arrays(reader, data.morph_name.size(), true).value();
			data.morph_key_frame = PmmKeyframeMorph::load_arrays(reader, false).value();
			data.op_init_frame.load(reader, data.ik_index.size(), data.op_index.size(), true);
			data.op_key_frames = PmmKeyframeOp::load_arrays(reader, data.ik_index.size(), data.op_index.size(), false).value();

			for (std::size_t i = 0; i < data.bone_name.size(); i++)
			{
				data.bone_current_datas.push_back(PmmBone::load(reader).value());
			}

			for (std::size_t i = 0; i < data.morph_name.size(); i++)
			{
				float ch = 0.0;
				reader.read((char*)& ch, sizeof(ch));
				data.morph_current_datas.push_back(ch);
			}

			for (std::size_t i = 0; i < data.ik_index.size(); i++)
			{
				char ch = 0.0;
				reader.read(&ch, sizeof(ch));
				data.is_current_ik_enabled_datas.push_back(ch);
			}

			for (std::size_t i = 0; i < data.op_index.size(); i++)
			{
				data.op_current_data.push_back(PmmOp::load(reader).value());
			}

			reader.read((char*)& data.is_add_blend, sizeof(data.is_add_blend));
			reader.read((char*)& data.edge_width, sizeof(data.edge_width));
			reader.read((char*)& data.is_selfshadow_enabled, sizeof(data.is_selfshadow_enabled));
			reader.read((char*)& data.calc_order, sizeof(data.calc_order));

			return data;
		}

		static std::optional<std::vector<PmmModel>> load_arrays(istream& reader)
		{
			std::uint8_t len = 0;
			reader.read((char*)& len, sizeof(len));

			auto models = std::vector<PmmModel>(len);
			for (auto& it : models)
				it = PmmModel::load(reader).value();

			return models;
		}
	};

	struct PmmCamera
	{
		PmmVector3 eye;
		PmmVector3 target;
		PmmVector3 rotation;
		std::uint8_t isorthro;

		PmmCamera()
			: eye(PmmVector3(0.0, 0.0, 0.0))
			, target(PmmVector3(0.0, 0.0, 0.0))
			, rotation(PmmVector3(0.0, 0.0, 0.0))
			, isorthro(0)
		{
		}

		static std::optional<PmmCamera> load(istream& reader)
		{
			PmmCamera data;
			reader.read((char*)& data.eye, sizeof(data.eye));
			reader.read((char*)& data.target, sizeof(data.target));
			reader.read((char*)& data.rotation, sizeof(data.rotation));
			reader.read((char*)& data.isorthro, sizeof(data.isorthro));

			return data;
		}

		static std::optional<std::vector<PmmCamera>> load_arrays(istream& reader)
		{
			std::uint8_t len = 0;
			reader.read((char*)& len, sizeof(len));

			auto model = std::vector<PmmCamera>(len);
			for (std::size_t i = 0; i < model.size(); i++)
				model[i] = PmmCamera::load(reader).value();

			return model;
		}
	};

	struct PmmLight
	{
		PmmVector3 rgb;
		PmmVector3 xyz;
		std::uint8_t is_selected;

		PmmLight()
			: rgb(PmmVector3(0.0, 0.0, 0.0))
			, xyz(PmmVector3(0.0, 0.0, 0.0))
			, is_selected(0)
		{
		}

		static std::optional<PmmLight> load(istream& reader)
		{
			auto data = PmmLight();
			reader.read((char*)& data.rgb, sizeof(data.rgb));
			reader.read((char*)& data.xyz, sizeof(data.xyz));
			reader.read((char*)& data.is_selected, sizeof(data.is_selected));

			return data;
		}

		static std::optional<std::vector<PmmLight>> load_arrays(istream& reader)
		{
			std::uint8_t len = 0;
			reader.read((char*)& len, sizeof(len));

			auto model = std::vector<PmmLight>(len);
			for (std::size_t i = 0; i < model.size(); i++)
				model[i] = PmmLight::load(reader).value();

			return model;
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
				model[i] = PmmDataBody::load(reader).value();

			return model;
		}
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
			data.key_frames = PmmKeyframe::load_arrays(reader).value();
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
				model[i] = PmmAccessoryData::load(reader).value();

			return model;
		}
	};

	struct PmmGravity
	{
		float acceleration;
		std::uint32_t noize_amount;
		PmmVector3 direction;
		std::uint8_t is_add_noize;

		PmmGravity()
			:acceleration(0.0)
			, noize_amount(0)
			, direction(PmmVector3(0.0, 0.0, 0.0))
			, is_add_noize(0)
		{
		}

		static std::optional<PmmGravity> load(istream& reader)
		{
			PmmGravity data;
			reader.read((char*)& data.acceleration, sizeof(data.acceleration));
			reader.read((char*)& data.noize_amount, sizeof(data.noize_amount));
			reader.read((char*)& data.direction, sizeof(data.direction));
			reader.read((char*)& data.is_add_noize, sizeof(data.is_add_noize));

			return data;
		}

		static std::optional<std::vector<PmmGravity>> load_arrays(istream& reader)
		{
			std::uint32_t len = 0;
			reader.read((char*)& len, sizeof(len));

			auto array = std::vector<PmmGravity>(len);
			for (std::size_t i = 0; i < len; i++)
				array[i] = PmmGravity::load(reader).value();

			return array;
		}
	};

	struct PmmCSelectorChoice
	{
		std::uint8_t mode_index;
		std::uint32_t selector_choice;

		PmmCSelectorChoice()
			: mode_index(0)
			, selector_choice(0)
		{
		}

		static std::optional<PmmCSelectorChoice> load(istream& reader)
		{
			PmmCSelectorChoice data;
			reader.read((char*)& data.mode_index, sizeof(data.mode_index));
			reader.read((char*)& data.selector_choice, sizeof(data.selector_choice));

			return data;
		}

		static std::optional<std::vector<PmmCSelectorChoice>> load_arrays(istream& reader)
		{
			std::uint8_t len = 0;
			reader.read((char*)& len, sizeof(len));

			auto model = std::vector<PmmCSelectorChoice>(len);
			for (std::size_t i = 0; i < model.size(); i++)
				model[i] = PmmCSelectorChoice::load(reader).value();

			return model;
		}
	};

	struct PMMFile
	{
		PmmHeader header;
		std::vector<PmmModel> model;
		PmmKeyframeCamera camera_init_frame;
		std::vector<PmmKeyframeCamera> camera_key_frames;
		PmmCamera camera_current_data;
		PmmKeyframeLight light_init_frame;
		std::vector<PmmKeyframeLight> light_key_frames;
		PmmLight light_current_data;
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
			pmm.camera_key_frames = PmmKeyframeCamera::load_arrays(reader, false).value();
			pmm.camera_current_data.load(reader);
			pmm.light_init_frame.load(reader, true);
			pmm.light_key_frames = PmmKeyframeLight::load_arrays(reader, false).value();
			pmm.light_current_data.load(reader);
			reader.read((char*)& pmm.selected_accessory_index, sizeof(pmm.selected_accessory_index));
			reader.read((char*)& pmm.accessory_vscroll, sizeof(pmm.accessory_vscroll));
			reader.read((char*)& pmm.accessory_count, sizeof(pmm.accessory_count));

			for (std::size_t i = 0; i < pmm.accessory_count; i++)
				pmm.accessory_name.push_back(PmmName::load_fixed_utf8(reader, 100).value());

			for (std::size_t i = 0; i < pmm.accessory_count; i++)
				pmm.accessory_datas.push_back(PmmAccessoryData::load(reader).value());

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
			pmm.gravity_key_frames = PmmKeyFrameGravity::load_arrays(reader, false).value();
			reader.read((char*)& pmm.is_show_selfshadow, sizeof(pmm.is_show_selfshadow));
			reader.read((char*)& pmm.selfshadow_current_data, sizeof(pmm.selfshadow_current_data));
			pmm.selfshadow_init_frame.load(reader, true);
			pmm.selfshadow_keyframes = PmmKeyFrameSelfShadow::load_arrays(reader, false).value();
			reader.read((char*)& pmm.edge_color_r, sizeof(pmm.edge_color_r));
			reader.read((char*)& pmm.edge_color_g, sizeof(pmm.edge_color_g));
			reader.read((char*)& pmm.edge_color_b, sizeof(pmm.edge_color_b));
			reader.read((char*)& pmm.is_black_background, sizeof(pmm.is_black_background));
			reader.read((char*)& pmm.camera_current_looking_at_model, sizeof(pmm.camera_current_looking_at_model));
			reader.read((char*)& pmm.camera_current_looking_at_bone, sizeof(pmm.camera_current_looking_at_bone));

			for (std::size_t i = 0; i < 16; i++)
				reader.read((char*)& pmm.unknown_array[i], sizeof(pmm.unknown_array[i]));

			reader.read((char*)& pmm.is_view_look_at_enabled, sizeof(pmm.is_view_look_at_enabled));
			reader.read((char*)& pmm.unknown, sizeof(pmm.unknown));
			reader.read((char*)& pmm.is_physics_ground_enabled, sizeof(pmm.is_physics_ground_enabled));
			reader.read((char*)& pmm.frame_text_box, sizeof(pmm.frame_text_box));
			reader.read((char*)& pmm.selector_choice_selection_following, sizeof(pmm.selector_choice_selection_following));
			pmm.selector_choice_datas = PmmCSelectorChoice::load_arrays(reader).value();

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