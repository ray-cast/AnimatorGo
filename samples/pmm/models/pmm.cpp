#include "pmm.h"
#include <iconv.h>
#include <codecvt>

namespace octoon
{
	std::optional<PmmInt2>
	PmmInt2::load(istream& reader)
	{
		PmmInt2 v;
		if (!reader.read((char*)& v.x, sizeof(v.x))) return std::nullopt;
		if (!reader.read((char*)& v.y, sizeof(v.y))) return std::nullopt;
		return v;
	}

	std::optional<std::vector<PmmInt2>>
	PmmInt2::load_arrays(istream& reader)
	{
		std::uint32_t len = 0;
		reader.read((char*)& len, sizeof(len));

		auto array = std::vector<PmmInt2>(len);
		for (auto& it : array)
			it.load(reader);

		return array;
	}

	std::optional<std::vector<PmmInt2>>
	PmmInt2::load_fixed_arrays(istream& reader, std::size_t len)
	{
		auto array = std::vector<PmmInt2>(len);
		for (auto& it : array)
			it.load(reader);

		return array;
	}

	/*std::string
	PmmName::sjis2utf8(const std::string& sjis)
	{
		std::size_t in_size = sjis.size();
		std::size_t out_size = sjis.size() * 2;

		auto inbuf = std::make_unique<char[]>(in_size + 1);
		auto outbuf = std::make_unique<char[]>(out_size);
		char* in = inbuf.get();
		char* out = outbuf.get();

		std::memset(in, 0, in_size + 1);
		std::memcpy(in, sjis.c_str(), in_size);

		iconv_t ic = nullptr;

		try
		{
			ic = iconv_open("GBK", "SJIS");
			iconv(ic, &in, &in_size, &out, &out_size);
			iconv_close(ic);

			char* in = inbuf.get();
			char* out = outbuf.get();

			ic = iconv_open("UTF-8", "GBK");
			iconv(ic, &out, &out_size, &in, &in_size);
			iconv_close(ic);
		}
		catch (const std::exception&)
		{
			iconv_close(ic);
		}

		return std::string(inbuf.get());
	}*/
	std::string
	PmmName::sjis2utf8(const std::string& sjis)
	{
		std::string utf8_string;

		LPCCH pSJIS = (LPCCH)sjis.c_str();
		int utf16size = ::MultiByteToWideChar(932, MB_ERR_INVALID_CHARS, pSJIS, -1, 0, 0);
		if (utf16size != 0)
		{
			auto pUTF16 = std::make_unique<WCHAR[]>(utf16size);
			if (::MultiByteToWideChar(932, 0, (LPCCH)pSJIS, -1, pUTF16.get(), utf16size) != 0)
			{
				std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> cv;
				return cv.to_bytes(pUTF16.get());
			}
		}

		return utf8_string;
	}

	std::optional<std::string>
	PmmName::load(istream& reader)
	{
		std::uint8_t len = 0;
		reader.read((char*)& len, sizeof(len));

		auto bytes = std::string(len, 0);
		reader.read(bytes.data(), bytes.size());

		return sjis2utf8(bytes);
	}

	std::optional<std::string>
	PmmName::load_fixed_utf8(istream& reader, std::size_t len)
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

		return sjis2utf8(bytes);
	}

	std::optional<std::vector<std::string>>
	PmmName::load_arrays(istream& reader)
	{
		std::uint32_t len = 0;
		reader.read((char*)& len, sizeof(len));

		auto array = std::vector<std::string>(len);
		for (std::size_t i = 0; i < len; i++)
			array[i] = PmmName::load(reader).value();

		return array;
	}

	std::optional<std::vector<std::uint8_t>>
	PmmUint8::load_array_from_u8(istream& reader)
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

	std::optional<std::vector<std::uint8_t>>
	PmmUint8::load_fixed_arrays(istream& reader, std::size_t len)
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

	std::optional<std::vector<std::int32_t>>
	PmmInt32::load_arrays(istream& reader)
	{
		std::uint32_t len = 0;
		reader.read((char*)& len, sizeof(len));

		auto array = std::vector<std::int32_t>(len);
		reader.read((char*)array.data(), len * sizeof(std::int32_t));

		return array;
	}

	std::optional<std::vector<float>>
	PmmFloat::load_arrays(istream& reader)
	{
		std::uint32_t len = 0;
		reader.read((char*)& len, sizeof(len));

		auto array = std::vector<float>(len);
		reader.read((char*)array.data(), len * sizeof(float));

		return array;
	}

	PmmHeader::PmmHeader()
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
		, num_models(0)
	{
	}

	std::optional<PmmHeader>
	PmmHeader::load(istream& reader)
	{
		PmmHeader data;
		data.magic = PmmName::load_fixed_utf8(reader, 24).value();
		reader.read((char*)& data.reserve1, sizeof(data.reserve1));
		reader.read((char*)& data.reserve2, sizeof(data.reserve2));
		reader.read((char*)& data.reserve3, sizeof(data.reserve3));
		reader.read((char*)& data.view_width, sizeof(data.view_width));
		reader.read((char*)& data.view_height, sizeof(data.view_height));
		reader.read((char*)& data.frame_width, sizeof(data.frame_width));
		reader.read((char*)& data.edit_view_angle, sizeof(data.edit_view_angle));
		reader.read((char*)& data.is_edit_camera_light_accessory, sizeof(data.is_edit_camera_light_accessory));
		reader.read((char*)& data.is_open_camera_panel, sizeof(data.is_open_camera_panel));
		reader.read((char*)& data.is_open_light_panel, sizeof(data.is_open_light_panel));
		reader.read((char*)& data.is_open_accessory_panel, sizeof(data.is_open_accessory_panel));
		reader.read((char*)& data.is_open_bone_panel, sizeof(data.is_open_bone_panel));
		reader.read((char*)& data.is_open_morph_panel, sizeof(data.is_open_morph_panel));
		reader.read((char*)& data.is_open_selfshadow_panel, sizeof(data.is_open_selfshadow_panel));
		if (data.magic == "Polygon Movie maker 0002")
		{
			reader.read((char*)& data.selected_model_index, sizeof(data.selected_model_index));
			reader.read((char*)& data.num_models, sizeof(data.num_models));
		}
		else
		{
			reader.read((char*)& data.num_models, sizeof(data.num_models));
			reader.read((char*)& data.summary, sizeof(data.summary));
		}
		
		if (data.magic != "Polygon Movie maker 0002" && data.magic != "Polygon Movie maker 0001") { return std::nullopt; }
		if (data.view_width == 0) { return std::nullopt; }
		if (data.view_height == 0) { return std::nullopt; }
		if (data.frame_width == 0) { return std::nullopt; }
		if (data.edit_view_angle <= 0.0) { return std::nullopt; }
		return data;
	}

	PmmKeyframe::PmmKeyframe()
		: data_index(-1)
		, frame_number(-1)
		, pre_index(-1)
		, next_index(-1)
		, is_selected(0)
	{
	}

	std::optional<PmmKeyframe>
	PmmKeyframe::load(istream& reader)
	{
		auto data = PmmKeyframe();
		reader.read((char*)& data.data_index, sizeof(data.data_index));
		reader.read((char*)& data.frame_number, sizeof(data.frame_number));
		reader.read((char*)& data.pre_index, sizeof(data.pre_index));
		reader.read((char*)& data.next_index, sizeof(data.next_index));
		reader.read((char*)& data.is_selected, sizeof(data.is_selected));

		return data;
	}

	std::optional<std::vector<PmmKeyframe>>
	PmmKeyframe::load_arrays(istream& reader)
	{
		std::uint8_t len = 0;
		reader.read((char*)& len, sizeof(len));

		auto data = std::vector<PmmKeyframe>(len);
		for (std::size_t i = 0; i < data.size(); i++)
			data[i] = PmmKeyframe::load(reader).value();

		return data;
	}

	PmmKeyframeBone::PmmKeyframeBone()
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

	std::optional<PmmKeyframeBone>
	PmmKeyframeBone::load(istream& reader, bool is_init)
	{
		PmmKeyframeBone data;
		if (!is_init) reader.read((char*)& data.data_index, sizeof(data.data_index));
		reader.read((char*)& data.frame_number, sizeof(data.frame_number));
		reader.read((char*)& data.pre_index, sizeof(data.pre_index));
		reader.read((char*)& data.next_index, sizeof(data.next_index));
		reader.read((char*)& data.interpolation_x, sizeof(data.interpolation_x));
		reader.read((char*)& data.interpolation_y, sizeof(data.interpolation_y));
		reader.read((char*)& data.interpolation_z, sizeof(data.interpolation_z));
		reader.read((char*)& data.interpolation_rotation, sizeof(data.interpolation_rotation));
		reader.read((char*)& data.translation, sizeof(data.translation));
		reader.read((char*)& data.quaternion, sizeof(data.quaternion));
		reader.read((char*)& data.is_selected, sizeof(data.is_selected));
		reader.read((char*)& data.is_physics_disabled, sizeof(data.is_physics_disabled));
		return data;
	}

	std::optional<std::vector<PmmKeyframeBone>>
	PmmKeyframeBone::load_fixed_arrays(istream& reader, std::size_t len, bool is_init)
	{
		auto array = std::vector<PmmKeyframeBone>(len);
		for (std::size_t i = 0; i < len; i++)
			array[i].load(reader, is_init);

		return array;
	}

	std::optional<std::vector<PmmKeyframeBone>>
	PmmKeyframeBone::load_arrays(istream& reader, bool is_init)
	{
		std::uint32_t len = 0;
		reader.read((char*)& len, sizeof(len));
		return PmmKeyframeBone::load_fixed_arrays(reader, len, is_init);
	}

	PmmKeyframeMorph::PmmKeyframeMorph()
		: data_index(1)
		, frame_number(0)
		, pre_index(-1)
		, next_index(-1)
		, value(0.0)
		, is_selected(0)
	{
	}

	bool 
	PmmKeyframeMorph::load(istream& reader, bool is_init)
	{
		if (!is_init) reader.read((char*)& this->data_index, sizeof(this->data_index));
		reader.read((char*)& this->frame_number, sizeof(this->frame_number));
		reader.read((char*)& this->pre_index, sizeof(this->pre_index));
		reader.read((char*)& this->next_index, sizeof(this->next_index));
		reader.read((char*)& this->value, sizeof(this->value));
		reader.read((char*)& this->is_selected, sizeof(this->is_selected));

		return true;
	}

	std::optional<std::vector<PmmKeyframeMorph>>
	PmmKeyframeMorph::load_fixed_arrays(istream& reader, std::size_t len, bool is_init)
	{
		auto array = std::vector<PmmKeyframeMorph>(len);
		for (std::size_t i = 0; i < len; i++)
			array[i].load(reader, is_init);

		return array;
	}

	std::optional<std::vector<PmmKeyframeMorph>>
	PmmKeyframeMorph::load_arrays(istream& reader, bool is_init)
	{
		std::uint32_t len = 0;
		reader.read((char*)& len, sizeof(len));
		return PmmKeyframeMorph::load_fixed_arrays(reader, len, is_init);
	}

	PmmKeyframeOp::PmmKeyframeOp() noexcept
		: data_index(-1)
		, frame_number(-1)
		, pre_index(-1)
		, next_index(-1)
		, is_display(0)
		, is_selected(0)
	{
	}

	std::optional<PmmKeyframeOp>
	PmmKeyframeOp::load(istream& reader, std::size_t ik_count, std::size_t op_count, bool is_init)
	{
		PmmKeyframeOp data;
		if (!is_init) reader.read((char*)& data.data_index, sizeof(data.data_index));
		reader.read((char*)& data.frame_number, sizeof(data.frame_number));
		reader.read((char*)& data.pre_index, sizeof(data.pre_index));
		reader.read((char*)& data.next_index, sizeof(data.next_index));
		reader.read((char*)& data.is_display, sizeof(data.is_display));
		data.is_ik_enabled = PmmUint8::load_fixed_arrays(reader, ik_count).value();
		data.op_data = PmmInt2::load_fixed_arrays(reader, op_count).value();
		reader.read((char*)& data.is_selected, sizeof(data.is_selected));

		return data;
	}

	std::optional<std::vector<PmmKeyframeOp>>
	PmmKeyframeOp::load_arrays(istream& reader, std::size_t ik_count, std::size_t op_count, bool is_init)
	{
		std::uint32_t len = 0;
		reader.read((char*)& len, sizeof(len));

		auto array = std::vector<PmmKeyframeOp>(len);
		for (std::size_t i = 0; i < len; i++)
			array[i] = PmmKeyframeOp::load(reader, ik_count, op_count, is_init).value();

		return array;
	}

	PmmKeyframeCamera::PmmKeyframeCamera()
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

	std::optional<PmmKeyframeCamera>
	PmmKeyframeCamera::load(istream& reader, bool is_init)
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

	std::optional<std::vector<PmmKeyframeCamera>>
	PmmKeyframeCamera::load_arrays(istream& reader, bool is_init)
	{
		std::uint32_t len = 0;
		reader.read((char*)& len, sizeof(len));

		auto model = std::vector<PmmKeyframeCamera>(len);
		for (std::size_t i = 0; i < model.size(); i++)
			model[i] = PmmKeyframeCamera::load(reader, is_init).value();

		return model;
	}

	PmmKeyframeLight::PmmKeyframeLight()
		: data_index(-1)
		, frame_number(-1)
		, pre_index(-1)
		, next_index(-1)
		, rgb(PmmVector3(0.0, 0.0, 0.0))
		, xyz(PmmVector3(0.0, 0.0, 0.0))
		, is_selected(0)
	{
	}

	std::optional<PmmKeyframeLight>
	PmmKeyframeLight::load(istream& reader, bool is_init)
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

	std::optional<std::vector<PmmKeyframeLight>>
	PmmKeyframeLight::load_arrays(istream& reader, bool is_init)
	{
		std::uint32_t len = 0;
		reader.read((char*)& len, sizeof(len));

		auto model = std::vector<PmmKeyframeLight>(len);
		for (std::size_t i = 0; i < model.size(); i++)
			model[i] = PmmKeyframeLight::load(reader, is_init).value();

		return model;
	}

	PmmKeyFrameGravity::PmmKeyFrameGravity()
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

	std::optional<PmmKeyFrameGravity>
	PmmKeyFrameGravity::load(istream& reader, bool is_init)
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

	std::optional<std::vector<PmmKeyFrameGravity>>
	PmmKeyFrameGravity::load_arrays(istream& reader, bool is_init)
	{
		std::uint32_t len = 0;
		reader.read((char*)& len, sizeof(len));

		auto array = std::vector<PmmKeyFrameGravity>(len);
		for (std::size_t i = 0; i < len; i++)
			array[i] = PmmKeyFrameGravity::load(reader, is_init).value();

		return array;
	}

	PmmKeyFrameSelfShadow::PmmKeyFrameSelfShadow()
		: data_index(-1)
		, frame_number(-1)
		, pre_index(-1)
		, next_index(-1)
		, mode(0)
		, distance(0.0)
		, is_selected(0)
	{
	}

	std::optional<PmmKeyFrameSelfShadow>
	PmmKeyFrameSelfShadow::load(istream& reader, bool is_init)
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

	std::optional<std::vector<PmmKeyFrameSelfShadow>>
	PmmKeyFrameSelfShadow::load_arrays(istream& reader, bool is_init)
	{
		std::uint32_t len = 0;
		reader.read((char*)& len, sizeof(len));

		auto array = std::vector<PmmKeyFrameSelfShadow>(len);
		for (std::size_t i = 0; i < len; i++)
			array.push_back(PmmKeyFrameSelfShadow::load(reader, is_init).value());

		return array;
	}

	PmmBone::PmmBone()
		: translation(PmmVector3(0.0, 0.0, 0.0))
		, quaternion(PmmVector4(0.0, 0.0, 0.0, 0.0))
		, is_edit_un_commited(0)
		, is_physics_disabled(0)
		, is_row_selected(0)
	{
	}

	std::optional<PmmBone>
	PmmBone::load(istream& reader)
	{
		PmmBone data;
		reader.read((char*)& data.translation, sizeof(data.translation));
		reader.read((char*)& data.quaternion, sizeof(data.quaternion));
		reader.read((char*)& data.is_edit_un_commited, sizeof(data.is_edit_un_commited));
		reader.read((char*)& data.is_physics_disabled, sizeof(data.is_physics_disabled));
		reader.read((char*)& data.is_row_selected, sizeof(data.is_row_selected));

		return data;
	}

	std::optional<std::vector<PmmBone>>
	PmmBone::load_arrays(istream& reader)
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

	PmmOp::PmmOp()
		: keyframe_begin(1)
		, keyframe_end(-1)
		, model_index(-1)
		, parent_bone_index(-1)
	{

	}

	std::optional<PmmOp>
	PmmOp::load(istream& reader)
	{
		PmmOp data;
		reader.read((char*)& data.keyframe_begin, sizeof(data.keyframe_begin));
		reader.read((char*)& data.keyframe_end, sizeof(data.keyframe_end));
		reader.read((char*)& data.model_index, sizeof(data.model_index));
		reader.read((char*)& data.parent_bone_index, sizeof(data.parent_bone_index));
		return data;
	}

	std::optional<std::vector<PmmOp>>
	PmmOp::load_arrays(istream& reader)
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

	PmmModel::PmmModel()
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

	std::optional<PmmModel>
	PmmModel::load(istream& reader)
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
		data.op_init_frame = PmmKeyframeOp::load(reader, data.ik_index.size(), data.op_index.size(), true).value();
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
			char ch = 0;
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

	std::optional<std::vector<PmmModel>>
	PmmModel::load_arrays(istream& reader, std::size_t len)
	{
		auto models = std::vector<PmmModel>(len);
		for (auto& it : models)
			it = PmmModel::load(reader).value();

		return models;
	}

	PmmCamera::PmmCamera()
		: eye(PmmVector3(0.0, 0.0, 0.0))
		, target(PmmVector3(0.0, 0.0, 0.0))
		, rotation(PmmVector3(0.0, 0.0, 0.0))
		, isorthro(0)
	{
	}

	std::optional<PmmCamera>
	PmmCamera::load(istream& reader)
	{
		PmmCamera data;
		reader.read((char*)& data.eye, sizeof(data.eye));
		reader.read((char*)& data.target, sizeof(data.target));
		reader.read((char*)& data.rotation, sizeof(data.rotation));
		reader.read((char*)& data.isorthro, sizeof(data.isorthro));

		return data;
	}

	std::optional<std::vector<PmmCamera>>
	PmmCamera::load_arrays(istream& reader)
	{
		std::uint8_t len = 0;
		reader.read((char*)& len, sizeof(len));

		auto model = std::vector<PmmCamera>(len);
		for (std::size_t i = 0; i < model.size(); i++)
			model[i] = PmmCamera::load(reader).value();

		return model;
	}

	PmmLight::PmmLight()
		: rgb(PmmVector3(0.0, 0.0, 0.0))
		, xyz(PmmVector3(0.0, 0.0, 0.0))
		, is_selected(0)
	{
	}

	std::optional<PmmLight>
	PmmLight::load(istream& reader)
	{
		auto data = PmmLight();
		reader.read((char*)& data.rgb, sizeof(data.rgb));
		reader.read((char*)& data.xyz, sizeof(data.xyz));
		reader.read((char*)& data.is_selected, sizeof(data.is_selected));

		return data;
	}

	std::optional<std::vector<PmmLight>>
	PmmLight::load_arrays(istream& reader)
	{
		std::uint8_t len = 0;
		reader.read((char*)& len, sizeof(len));

		auto model = std::vector<PmmLight>(len);
		for (std::size_t i = 0; i < model.size(); i++)
			model[i] = PmmLight::load(reader).value();

		return model;
	}

	PmmDataBody::PmmDataBody()
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

	std::optional<PmmDataBody> 
	PmmDataBody::load(istream& reader)
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

	std::optional<std::vector<PmmDataBody>>
	PmmDataBody::load_arrays(istream& reader)
	{
		std::uint8_t len = 0;
		reader.read((char*)& len, sizeof(len));

		auto model = std::vector<PmmDataBody>(len);
		for (std::size_t i = 0; i < model.size(); i++)
			model[i] = PmmDataBody::load(reader).value();

		return model;
	}

	PmmAccessoryData::PmmAccessoryData()
		: index(0)
		, draw_order(0)
		, is_add_blend(0)
	{
	}

	std::optional<PmmAccessoryData>
	PmmAccessoryData::load(istream& reader)
	{
		PmmAccessoryData data;
		reader.read((char*)& data.index, sizeof(data.index));
		data.name = PmmName::load_fixed_utf8(reader, 100).value();
		data.path = PmmName::load_fixed_utf8(reader, 256).value();
		reader.read((char*)& data.draw_order, sizeof(data.draw_order));
		data.init_frame = PmmKeyframe::load(reader).value();
		data.key_frames = PmmKeyframe::load_arrays(reader).value();
		data.current_data = PmmDataBody::load(reader).value();
		reader.read((char*)& data.is_add_blend, sizeof(data.is_add_blend));

		return data;
	}

	std::optional<std::vector<PmmAccessoryData>>
	PmmAccessoryData::load_arrays(istream& reader)
	{
		std::uint8_t len = 0;
		reader.read((char*)& len, sizeof(len));

		auto model = std::vector<PmmAccessoryData>(len);
		for (std::size_t i = 0; i < model.size(); i++)
			model[i] = PmmAccessoryData::load(reader).value();

		return model;
	}

	PmmGravity::PmmGravity()
		: acceleration(0.0)
		, noize_amount(0)
		, direction(PmmVector3(0.0, 0.0, 0.0))
		, is_add_noize(0)
	{
	}

	std::optional<PmmGravity> 
	PmmGravity::load(istream& reader)
	{
		PmmGravity data;
		reader.read((char*)& data.acceleration, sizeof(data.acceleration));
		reader.read((char*)& data.noize_amount, sizeof(data.noize_amount));
		reader.read((char*)& data.direction, sizeof(data.direction));
		reader.read((char*)& data.is_add_noize, sizeof(data.is_add_noize));

		return data;
	}

	std::optional<std::vector<PmmGravity>> 
	PmmGravity::load_arrays(istream& reader)
	{
		std::uint32_t len = 0;
		reader.read((char*)& len, sizeof(len));

		auto array = std::vector<PmmGravity>(len);
		for (std::size_t i = 0; i < len; i++)
			array[i] = PmmGravity::load(reader).value();

		return array;
	}

	PmmCSelectorChoice::PmmCSelectorChoice()
		: mode_index(0)
		, selector_choice(0)
	{
	}

	std::optional<PmmCSelectorChoice>
	PmmCSelectorChoice::load(istream& reader)
	{
		PmmCSelectorChoice data;
		reader.read((char*)& data.mode_index, sizeof(data.mode_index));
		reader.read((char*)& data.selector_choice, sizeof(data.selector_choice));

		return data;
	}

	std::optional<std::vector<PmmCSelectorChoice>>
	PmmCSelectorChoice::load_arrays(istream& reader)
	{
		std::uint8_t len = 0;
		reader.read((char*)& len, sizeof(len));

		auto model = std::vector<PmmCSelectorChoice>(len);
		for (std::size_t i = 0; i < model.size(); i++)
			model[i] = PmmCSelectorChoice::load(reader).value();

		return model;
	}

	PMMFile::PMMFile() noexcept
		: selected_accessory_index(0)
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

	std::optional<PMMFile>
	PMMFile::load(istream& reader)
	{
		try
		{
			auto pmm = PMMFile();
			pmm.header = PmmHeader::load(reader).value();
			pmm.model = PmmModel::load_arrays(reader, pmm.header.num_models).value();
			pmm.camera_init_frame = PmmKeyframeCamera::load(reader, true).value();
			pmm.camera_key_frames = PmmKeyframeCamera::load_arrays(reader, false).value();
			pmm.camera_current_data = PmmCamera::load(reader).value();
			pmm.light_init_frame = PmmKeyframeLight::load(reader, true).value();
			pmm.light_key_frames = PmmKeyframeLight::load_arrays(reader, false).value();
			pmm.light_current_data = PmmLight::load(reader).value();
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
			pmm.gravity_current_data = PmmGravity::load(reader).value();
			pmm.gravity_init_frame = PmmKeyFrameGravity::load(reader, true).value();
			pmm.gravity_key_frames = PmmKeyFrameGravity::load_arrays(reader, false).value();
			reader.read((char*)& pmm.is_show_selfshadow, sizeof(pmm.is_show_selfshadow));
			reader.read((char*)& pmm.selfshadow_current_data, sizeof(pmm.selfshadow_current_data));
			pmm.selfshadow_init_frame = PmmKeyFrameSelfShadow::load(reader, true).value();
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
		catch (std::bad_optional_access& e)
		{
			return std::nullopt;
		}
	}

	PMMLoader::PMMLoader()
	{
	}

	bool 
	PMMLoader::can_read(istream& reader)
	{
		return PmmHeader::load(reader).has_value();
	}

	bool
	PMMLoader::do_load(istream& reader, PMMFile& pmm)
	{
		pmm.load(reader);
		return true;
		/*auto scene = Scene();

		auto camera = PerspectiveCamera::builder()
			.main(true)
			.set_fov(30.0)
			.set_translate(float!(0.0, 0.1, 10.0))
			.build();

		scene.add(camera);

		for model in pmm.model
		{
			auto model = ModelLoader::open(model.path);
			model.set_scale(float!(0.1,0.1,0.1));
			model.set_translate(float!(0.0,-0.8,20.0));
			scene.add(model);
		}

		return scene;*/
	}

	/*std::optional<std::vector<u8>> do_save(&self, _:&Scene)
	{
		Err(Error::LoaderError("Not Implmention yet".to_std::string()))
	}*/
}