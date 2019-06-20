/*#include <cstdio>
#include <string>
#include <vector>
#include <octoon/math/vector2.h>
#include <octoon/math/vector3.h>
#include <octoon/math/vector4.h>

namespace octoon
{
	typedef math::Vector2  PmmVector2;
	typedef math::Vector3  PmmVector3;
	typedef math::Vector4  PmmVector4;

	typedef math::Vector3  PmmColor3;
	typedef math::Vector4  PmmColor4;
	typedef math::float3x3 PmmFloat3x3;
	typedef math::float4x4 PmmFloat4x4;

	typedef wchar_t                PmmChar;
	typedef std::int8_t            PmmInt8;
	typedef std::int16_t           PmmInt16;
	typedef std::int32_t           PmmInt32;
	typedef std::uint8_t           PmmUInt8;
	typedef std::uint16_t          PmmUInt16;
	typedef std::uint32_t          PmmUInt32;

	typedef float                  PmmFloat;

	struct PmmHeader
	{
		std::string magic;
		PmmUInt16 reserve1;
		PmmUInt16 reserve2;
		PmmUInt16 reserve3;
		PmmUInt32 view_width;
		PmmUInt32 view_height;
		PmmUInt32 frame_width;
		PmmFloat edit_view_angle;
		PmmInt8 is_edit_camera_light_accessory;
		PmmInt8 is_open_camera_panel;
		PmmInt8 is_open_light_panel;
		PmmInt8 is_open_accessory_panel;
		PmmInt8 is_open_bone_panel;
		PmmInt8 is_open_morph_panel;
		PmmInt8 is_open_selfshadow_panel;
		PmmInt8 selected_model_index;
	};

	struct PmmBoneFrame
	{
		PmmInt32 data_index;
		PmmInt32 frame_number;
		PmmInt32 pre_index;
		PmmInt32 next_index;
		PmmUInt8 interpolation_x[4];
		PmmUInt8 interpolation_y[4];
		PmmUInt8 interpolation_z[4];
		PmmUInt8 interpolation_rotation[4];
		PmmVector3 translation;
		PmmVector4 quaternion;
		PmmUInt8 is_selected;
		PmmUInt8 is_physics_disabled;
	};

	struct PmmMorphFrame
	{
		PmmInt32 data_index;
		PmmInt32 frame_number;
		PmmInt32 pre_index;
		PmmInt32 next_index;
		PmmFloat value;
		PmmUInt8 is_selected;
	};

	struct PmmOpFrame
	{
		PmmInt32 data_index;
		PmmInt32 frame_number;
		PmmInt32 pre_index;
		PmmInt32 next_index;
		PmmUInt8 is_display;
		std::vector<PmmUInt8> is_ik_enabled;
		std::vector<math::char2> op_data;
		PmmUInt8 is_selected8;
	};

	struct PmmBoneCurrentData
	{
		PmmVector3 translation;
		PmmVector4 quaternion;
		PmmUInt8 is_edit_un_commited;
		PmmUInt8 is_physics_disabled;
		PmmUInt8 is_row_selected;
	};

	struct PmmOpCurrentData
	{
		PmmInt32 keyframe_begin;
		PmmInt32 keyframe_end;
		PmmInt32 model_index;
		PmmInt32 parent_bone_index;
	};

	struct PmmModel
	{
		PmmUInt8 number;
		std::string name;
		std::string name_en;
		std::string path;
		PmmUInt8 keyframe_editor_toplevel_rows;
		std::vector<std::string> bone_name;
		std::vector<std::string> morph_name;
		std::vector<PmmInt32> ik_index;
		std::vector<PmmInt32> op_index;
		PmmUInt8 draw_order;
		PmmUInt8 edit_is_display;
		PmmInt32 edit_selected_bone;
		PmmInt32 skin_panel[4];
		std::vector<PmmUInt8> is_frame_open;
		PmmInt32 vscroll;
		PmmInt32 last_frame;
		std::vector<PmmBoneFrame> bone_init_frame;
		std::vector<PmmBoneFrame> bone_key_frame;
		std::vector<PmmMorphFrame> morph_init_frame;
		std::vector<PmmMorphFrame> morph_key_frame;
		PmmOpFrame op_init_frame;
		std::vector<PmmOpFrame> op_key_frames;
		std::vector<PmmBoneCurrentData> bone_current_datas;
		std::vector<PmmFloat> morph_current_datas;
		std::vector<PmmUInt8> is_current_ik_enabled_datas;
		std::vector<PmmOpCurrentData> op_current_data;
		PmmUInt8 is_add_blend;
		PmmFloat edge_width;
		PmmUInt8 is_selfshadow_enabled;
		PmmUInt8 calc_order8;
	};

	struct PmmCameraFrame
	{
		PmmInt32 data_index;
		PmmInt32 frame_number;
		PmmInt32 pre_index;
		PmmInt32 next_index;
		PmmFloat distance;
		PmmVector3 eye_position;
		PmmVector3 rotation;
		PmmInt32 looking_model_index;
		PmmInt32 looking_bone_index;
		PmmUInt8 interpolation_x[4];
		PmmUInt8 interpolation_y[4];
		PmmUInt8 interpolation_z[4];
		PmmUInt8 interpolation_rotation[4];
		PmmUInt8 interpolation_distance[4];
		PmmUInt8 interpolation_angleview[4];
		PmmUInt8 is_parse;
		PmmUInt32 angle_view;
		PmmUInt8 is_selected;
	};

	struct PmmCameraCurrentData
	{
		PmmVector3 eye_position;
		PmmVector3 target_position;
		PmmVector3 rotation;
		 PmmUInt8  isorthro;
	};

	struct PmmLightFrame
	{
		PmmInt32 data_index;
		PmmInt32 frame_number;
		PmmInt32 pre_index;
		PmmInt32 next_index;
		PmmVector3 rgb;
		PmmVector3 xyz;
		PmmUInt8 is_selected;
	};

	struct PmmLightCurrentData
	{
		PmmVector3 rgb;
		PmmVector3 xyz;
		PmmUInt8 is_selected;
	};

	struct PmmKeyFrame
	{
		PmmInt32 data_index;
		PmmInt32 frame_number;
		PmmInt32 pre_index;
		PmmInt32 next_index;
		PmmUInt8 is_selected8;
	};

	struct PmmDataBody
	{
		PmmUInt8 transparency;
		PmmUInt8 is_visible;
		PmmInt32 parent_model_index;
		PmmInt32 parent_bone_index;
		PmmVector3 translation;
		PmmVector3 rotation;
		PmmFloat scale;
		PmmUInt8 is_shadow_enabled;
	};

	struct PmmAccessoryData
	{
		PmmUInt8 index;
		std::string name;
		std::string path;
		PmmUInt8 draw_order;
		PmmKeyFrame init_frame;
		std::vector<PmmKeyFrame> key_frames;
		PmmDataBody current_data;
		PmmUInt8 is_add_blend8;
	};

	struct PmmGravityCurrentData
	{
		PmmFloat acceleration;
		PmmUInt32 noize_amount;
		PmmVector3 direction;
		PmmUInt8 is_add_noize8;
	};

	struct PmmGravityKeyFrame
	{
		PmmInt32 data_index;
		PmmInt32 frame_number;
		PmmInt32 pre_index;
		PmmInt32 next_index;
		PmmUInt8 is_add_noize;
		PmmUInt32 noize_amount;
		PmmFloat acceleration;
		PmmVector3 direction;
		PmmUInt8 is_selected8;
	};

	struct PmmSelfShadowKeyFrame
	{
		PmmInt32 data_index;
		PmmInt32 frame_number;
		PmmInt32 pre_index;
		PmmInt32 next_index;
		PmmUInt8 mode;
		PmmFloat distance;
		PmmUInt8 is_selected8;
	};

	struct PmmCSelectorChoiceData
	{
		PmmUInt8 mode_index;
		PmmUInt32 selector_choice2;
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
		PmmUInt8 selected_accessory_index;
		PmmUInt32 accessory_vscroll;
		PmmUInt8 accessory_count;
		std::vector<std::string> accessory_name;
		std::vector<PmmAccessoryData> accessory_datas;
		PmmUInt32 current_frame_position;
		PmmUInt32 h_scroll_position;
		PmmUInt32 h_scroll_scale;
		PmmUInt32 bone_operation_kind;
		PmmUInt8 looking_at;
		PmmUInt8 is_repeat;
		PmmUInt8 is_play_from_frame;
		PmmUInt8 is_play_to_frame;
		PmmUInt32 play_start_frame;
		PmmUInt32 play_end_frame;
		PmmUInt8 is_wave_enabled;
		std::string wave_path;
		PmmUInt32 avi_offset_x;
		PmmUInt32 avi_offset_y;
		PmmFloat avi_scale;
		std::string avi_path;
		PmmUInt32 is_show_avi;
		PmmUInt32 background_image_offset_x;
		PmmUInt32 background_image_offset_y;
		PmmUInt32 background_image_scale;
		std::string background_image_path;
		PmmUInt8 is_show_background_image;
		PmmUInt8 is_show_infomation;
		PmmUInt8 is_show_axis;
		PmmUInt8 is_show_groundshadow;
		PmmFloat fps_limit;
		PmmUInt32 screen_capture_mode;
		PmmUInt32 accessory_number_render_after_model;
		PmmFloat ground_shadow_brightness;
		PmmUInt8 is_transparent_ground_shadow;
		PmmUInt8 physics_mode;
		PmmGravityCurrentData gravity_current_data;
		PmmGravityKeyFrame gravity_init_frame;
		std::vector<PmmGravityKeyFrame> gravity_key_frames;
		PmmUInt8 is_show_selfshadow;
		PmmFloat selfshadow_current_data;
		PmmSelfShadowKeyFrame selfshadow_init_frame;
		std::vector<PmmSelfShadowKeyFrame> selfshadow_keyframes;
		PmmUInt32 edge_color_r;
		PmmUInt32 edge_color_g;
		PmmUInt32 edge_color_b;
		PmmUInt8 is_black_background;
		PmmInt32 camera_current_looking_at_model;
		PmmInt32 camera_current_looking_at_bone;
		PmmFloat unknown_array[16];
		PmmUInt8 is_view_look_at_enabled;
		PmmUInt8 unknown;
		PmmUInt8 is_physics_ground_enabled;
		PmmUInt32 frame_text_box;
		PmmUInt8 selector_choice_selection_following;
		std::vector<PmmCSelectorChoiceData> selector_choice_datas;
	};

	impl PmmInt2
	{
		fn load(reader:&mut Cursor<&[u8]>) -> Result<(i32,i32)>
		{
			auto x = reader.read_i32::<LittleEndian>()?;
			auto y = reader.read_i32::<LittleEndian>()?;
			Ok((x, y))
		}

		fn load_arrays(reader:&mut Cursor<&[u8]>) -> Result<std::vector<(i32,i32)>>
		{
			auto array = Vec::with_capacity(reader.read_u32::<LittleEndian>()? as usize);
			for _ in 0,array.capacity()
			{
				array.push(PmmInt2::load(reader)?);
			}

			Ok(array)
		}

		fn load_fixed_arrays(reader:&mut Cursor<&[u8]>, len:usize) -> Result<std::vector<(i32,i32)>>
		{
			auto array = Vec::with_capacity(len as usize);
			for _ in 0,array.capacity()
			{
				array.push(PmmInt2::load(reader)?);
			}

			Ok(array)
		}
	}

	impl PmmVector2
	{
		fn load(reader:&mut Cursor<&[u8]>) -> Result<(f32,f32)>
		{
			auto x = reader.read_f32::<LittleEndian>()?;
			auto y = reader.read_f32::<LittleEndian>()?;
			Ok((x, y))
		}
	}

	impl PmmVector3
	{
		fn load(reader:&mut Cursor<&[u8]>) -> Result<PmmVector3>
		{
			auto x = reader.read_f32::<LittleEndian>()?;
			auto y = reader.read_f32::<LittleEndian>()?;
			auto z = reader.read_f32::<LittleEndian>()?;
			Ok((x, y, z))
		}
	}

	impl PmmVector4
	{
		fn load(reader:&mut Cursor<&[u8]>) -> Result<PmmVector4>
		{
			auto x = reader.read_f32::<LittleEndian>()?;
			auto y = reader.read_f32::<LittleEndian>()?;
			auto z = reader.read_f32::<LittleEndian>()?;
			auto w = reader.read_f32::<LittleEndian>()?;
			Ok((x, y, z, w))
		}
	}

	impl PmmName
	{
		fn load(reader:&mut Cursor<&[u8]>) -> Result<std::string>
		{
			auto bytes = Vec::with_capacity(reader.read_u8()? as usize);
			for _ in 0,bytes.capacity()
			{
				bytes.push(reader.read_u8()?);
			}

			Ok(WINDOWS_31J.decode(&bytes, DecoderTrap::Ignore).unwrap())
		}

		fn load_fixed_utf8(reader:&mut Cursor<&[u8]>, len:usize) -> Result<std::string>
		{
			auto bytes = Vec();
			for _ in 0,len
			{
				auto ch = reader.read_u8()?;
				if ch == 0 { break; }
				bytes.push(ch);
			}

			if bytes.len() < len-1
			{
				reader.seek(SeekFrom::Current((len - bytes.len() - 1) as i64))?;
			}

			Ok(WINDOWS_31J.decode(&bytes, DecoderTrap::Ignore).unwrap())
		}

		fn load_arrays(reader:&mut Cursor<&[u8]>) -> Result<std::vector<std::string>>
		{
			auto array = Vec::with_capacity(reader.read_u32::<LittleEndian>()? as usize);
			for _ in 0,array.capacity()
			{
				array.push(PmmName::load(reader)?);
			}

			Ok(array)
		}
	}

	impl PmmUint8
	{
		fn load_array_from_u8(reader:&mut Cursor<&[u8]>) -> Result<std::vector<u8>>
		{
			auto array = Vec::with_capacity(reader.read_u8()? as usize);
			for _ in 0,array.capacity()
			{
				array.push(reader.read_u8()?);
			}

			Ok(array)
		}

		fn load_fixed_arrays(reader:&mut Cursor<&[u8]>, len:usize) -> Result<std::vector<u8>>
		{
			auto array = Vec::with_capacity(len);
			for _ in 0,array.capacity()
			{
				array.push(reader.read_u8()?);
			}

			Ok(array)
		}
	}

	impl PmmInt32
	{
		fn load_arrays(reader:&mut Cursor<&[u8]>) -> Result<std::vector<i32>>
		{
			auto array = Vec::with_capacity(reader.read_u32::<LittleEndian>()? as usize);
			for _ in 0,array.capacity()
			{
				array.push(reader.read_i32::<LittleEndian>()?);
			}

			Ok(array)
		}
	}

	impl PmmFloat
	{
		fn load_arrays(reader:&mut Cursor<&[u8]>) -> Result<std::vector<f32>>
		{
			auto array = Vec::with_capacity(reader.read_u32::<LittleEndian>()? as usize);
			for _ in 0,array.capacity()
			{
				array.push(reader.read_f32::<LittleEndian>()?);
			}

			Ok(array)
		}
	}

	impl PmmHeader
	{
		fn new() -> Self
		{
			Self
			{
				magic:std::string(),
				reserve1:0,
				reserve2:0,
				reserve3:0,
				view_width:0,
				view_height:0,
				frame_width:0,
				edit_view_angle:0.0,
				is_edit_camera_light_accessory :0,
				is_open_camera_panel:0,
				is_open_light_panel:0,
				is_open_accessory_panel:0,
				is_open_bone_panel:0,
				is_open_morph_panel:0,
				is_open_selfshadow_panel:0,
				selected_model_index:0,
			}
		}

		fn valid(self) -> Result<Self>
		{
			if self.magic != "Polygon Movie maker 0002" { return Err(Error::LoaderError("Invalid magic in PMM Header".to_std::string())); }
			if self.view_width == 0 { return Err(Error::LoaderError("Invalid width in PMM Header".to_std::string())); }
			if self.view_height == 0 { return Err(Error::LoaderError("Invalid height in PMM Header".to_std::string())); }
			if self.frame_width == 0 { return Err(Error::LoaderError("Invalid frame width in PMM Header".to_std::string())); }
			if self.edit_view_angle <= 0.0 { return Err(Error::LoaderError("Invalid FOV in PMM Header".to_std::string())); }

			Ok(self)
		}

		fn load(reader:&mut Cursor<&[u8]>) -> Result<Self>
		{
			auto this = PmmHeader();
			this.magic = PmmName::load_fixed_utf8(reader, 24)?;
			this.reserve1 = reader.read_u16::<LittleEndian>()?;
			this.reserve2 = reader.read_u16::<LittleEndian>()?;
			this.reserve3 = reader.read_u16::<LittleEndian>()?;
			this.view_width = reader.read_u32::<LittleEndian>()?;
			this.view_height = reader.read_u32::<LittleEndian>()?;
			this.frame_width = reader.read_u32::<LittleEndian>()?;
			this.edit_view_angle = reader.read_f32::<LittleEndian>()?;
			this.is_edit_camera_light_accessory = reader.read_u8()?;
			this.is_open_camera_panel = reader.read_u8()?;
			this.is_open_light_panel = reader.read_u8()?;
			this.is_open_accessory_panel = reader.read_u8()?;
			this.is_open_bone_panel = reader.read_u8()?;
			this.is_open_morph_panel = reader.read_u8()?;
			this.is_open_selfshadow_panel = reader.read_u8()?;
			this.selected_model_index = reader.read_u8()?;

			this.valid()
		}
	}

	impl PmmBoneFrame
	{
		fn new() -> Self
		{
			Self
			{
				data_index:-1,
				frame_number:0,
				pre_index:-1,
				next_index:-1,
				interpolation_x:[0;4],
				interpolation_y:[0;4],
				interpolation_z:[0;4],
				interpolation_rotation:[0;4],
				translation:(0.0,0.0,0.0),
				quaternion:(0.0,0.0,0.0,1.0),
				is_selected:0,
				is_physics_disabled:0,
			}
		}

		fn load(reader:&mut Cursor<&[u8]>, is_init:bool) -> Result<Self>
		{
			auto this = PmmBoneFrame();
			this.data_index = if is_init { -1 } else { reader.read_i32::<LittleEndian>()? };
			this.frame_number = reader.read_i32::<LittleEndian>()?;
			this.pre_index = reader.read_i32::<LittleEndian>()?;
			this.next_index = reader.read_i32::<LittleEndian>()?;
			for i in 0,this.interpolation_x.len() { this.interpolation_x[i] = reader.read_u8()?; }
			for i in 0,this.interpolation_y.len() { this.interpolation_y[i] = reader.read_u8()?; }
			for i in 0,this.interpolation_z.len() { this.interpolation_z[i] = reader.read_u8()?; }
			for i in 0,this.interpolation_rotation.len() { this.interpolation_rotation[i] = reader.read_u8()?; }
			this.translation = PmmVector3::load(reader)?;
			this.quaternion = PmmVector4::load(reader)?;
			this.is_selected = reader.read_u8()?;
			this.is_physics_disabled = reader.read_u8()?;

			Ok(this)
		}

		fn load_fixed_arrays(reader:&mut Cursor<&[u8]>, len:usize, is_init:bool) -> Result<std::vector<Self>>
		{
			auto array = Vec::with_capacity(len as usize);
			for _ in 0,array.capacity()
			{
				array.push(PmmBoneFrame::load(reader, is_init)?);
			}

			Ok(array)
		}

		fn load_arrays(reader:&mut Cursor<&[u8]>, is_init:bool) -> Result<std::vector<Self>>
		{
			auto len = reader.read_u32::<LittleEndian>()? as usize;
			PmmBoneFrame::load_fixed_arrays(reader, len, is_init)
		}
	}

	impl PmmMorphFrame
	{
		fn new() -> Self
		{
			Self
			{
				data_index:-1,
				frame_number:0,
				pre_index:-1,
				next_index:-1,
				value:0.0,
				is_selected:0,
			}
		}

		fn load(reader:&mut Cursor<&[u8]>, is_init:bool) -> Result<Self>
		{
			auto this = PmmMorphFrame();
			this.data_index = if is_init { -1 } else { reader.read_i32::<LittleEndian>()? };
			this.frame_number = reader.read_i32::<LittleEndian>()?;
			this.pre_index = reader.read_i32::<LittleEndian>()?;
			this.next_index = reader.read_i32::<LittleEndian>()?;
			this.value = reader.read_f32::<LittleEndian>()?;
			this.is_selected = reader.read_u8()?;

			Ok(this)
		}

		fn load_fixed_arrays(reader:&mut Cursor<&[u8]>, len:usize, is_init:bool) -> Result<std::vector<Self>>
		{
			auto array = Vec::with_capacity(len as usize);
			for _ in 0,array.capacity()
			{
				array.push(PmmMorphFrame::load(reader, is_init)?);
			}

			Ok(array)
		}

		fn load_arrays(reader:&mut Cursor<&[u8]>, is_init:bool) -> Result<std::vector<Self>>
		{
			auto len = reader.read_u32::<LittleEndian>()? as usize;
			PmmMorphFrame::load_fixed_arrays(reader, len, is_init)
		}
	}

	impl PmmOpFrame
	{
		fn new() -> Self
		{
			Self
			{
				data_index:-1,
				frame_number:-1,
				pre_index:-1,
				next_index:-1,
				is_display:0,
				is_ik_enabled:Vec(),
				op_data:Vec(),
				is_selected:0
			}
		}

		fn load(reader:&mut Cursor<&[u8]>, ik_count:usize, op_count:usize, is_init:bool) -> Result<Self>
		{
			auto this = PmmOpFrame();
			this.data_index = if is_init { -1 } else { reader.read_i32::<LittleEndian>()? };
			this.frame_number = reader.read_i32::<LittleEndian>()?;
			this.pre_index = reader.read_i32::<LittleEndian>()?;
			this.next_index = reader.read_i32::<LittleEndian>()?;
			this.is_display = reader.read_u8()?;
			this.is_ik_enabled = PmmUint8::load_fixed_arrays(reader, ik_count)?;
			this.op_data = PmmInt2::load_fixed_arrays(reader, op_count)?;
			this.is_selected = reader.read_u8()?;

			Ok(this)
		}

		fn load_arrays(reader:&mut Cursor<&[u8]>, ik_count:usize, op_count:usize, is_init:bool) -> Result<std::vector<Self>>
		{
			auto array = Vec::with_capacity(reader.read_u32::<LittleEndian>()? as usize);
			for _ in 0,array.capacity()
			{
				array.push(PmmOpFrame::load(reader, ik_count, op_count, is_init)?);
			}

			Ok(array)
		}
	}

	impl PmmGravityCurrentData
	{
		fn new() -> Self
		{
			Self
			{
				acceleration:0.0,
				noize_amount:0,
				direction:(0.0,0.0,0.0),
				is_add_noize:0
			}
		}

		fn load(reader:&mut Cursor<&[u8]>) -> Result<Self>
		{
			auto this = PmmGravityCurrentData();
			this.acceleration = reader.read_f32::<LittleEndian>()?;
			this.noize_amount = reader.read_u32::<LittleEndian>()?;
			this.direction = PmmVector3::load(reader)?;
			this.is_add_noize = reader.read_u8()?;

			Ok(this)
		}

		fn load_arrays(reader:&mut Cursor<&[u8]>) -> Result<std::vector<Self>>
		{
			auto array = Vec::with_capacity(reader.read_u32::<LittleEndian>()? as usize);
			for _ in 0,array.capacity()
			{
				array.push(PmmGravityCurrentData::load(reader)?);
			}

			Ok(array)
		}
	}

	impl PmmGravityKeyFrame
	{
		fn new() -> Self
		{
			Self
			{
				data_index:-1,
				frame_number:-1,
				pre_index:-1,
				next_index:-1,
				is_add_noize:0,
				noize_amount:0,
				acceleration:0.0,
				direction:(0.0,0.0,0.0),
				is_selected:0
			}
		}

		fn load(reader:&mut Cursor<&[u8]>, is_init:bool) -> Result<Self>
		{
			auto this = PmmGravityKeyFrame();
			this.data_index = if is_init { -1} else { reader.read_i32::<LittleEndian>()? };
			this.frame_number = reader.read_i32::<LittleEndian>()?;
			this.pre_index = reader.read_i32::<LittleEndian>()?;
			this.next_index = reader.read_i32::<LittleEndian>()?;
			this.is_add_noize = reader.read_u8()?;
			this.noize_amount = reader.read_u32::<LittleEndian>()?;
			this.acceleration = reader.read_f32::<LittleEndian>()?;
			this.direction = PmmVector3::load(reader)?;
			this.is_selected = reader.read_u8()?;

			Ok(this)
		}

		fn load_arrays(reader:&mut Cursor<&[u8]>, is_init:bool) -> Result<std::vector<Self>>
		{
			auto array = Vec::with_capacity(reader.read_u32::<LittleEndian>()? as usize);
			for _ in 0,array.capacity()
			{
				array.push(PmmGravityKeyFrame::load(reader, is_init)?);
			}

			Ok(array)
		}
	}

	impl PmmSelfShadowKeyFrame
	{
		fn new() -> Self
		{
			Self
			{
				data_index:-1,
				frame_number:-1,
				pre_index:-1,
				next_index:-1,
				mode:0,
				distance:0.0,
				is_selected:0
			}
		}

		fn load(reader:&mut Cursor<&[u8]>, is_init:bool) -> Result<Self>
		{
			auto this = PmmSelfShadowKeyFrame();
			this.data_index = if is_init { -1} else { reader.read_i32::<LittleEndian>()? };
			this.frame_number = reader.read_i32::<LittleEndian>()?;
			this.pre_index = reader.read_i32::<LittleEndian>()?;
			this.next_index = reader.read_i32::<LittleEndian>()?;
			this.mode = reader.read_u8()?;
			this.distance = reader.read_f32::<LittleEndian>()?;
			this.is_selected = reader.read_u8()?;

			Ok(this)
		}

		fn load_arrays(reader:&mut Cursor<&[u8]>, is_init:bool) -> Result<std::vector<Self>>
		{
			auto array = Vec::with_capacity(reader.read_u32::<LittleEndian>()? as usize);
			for _ in 0,array.capacity()
			{
				array.push(PmmSelfShadowKeyFrame::load(reader, is_init)?);
			}

			Ok(array)
		}
	}

	impl PmmBoneCurrentData
	{
		fn new() -> Self
		{
			Self
			{
				translation:(0.0,0.0,0.0),
				quaternion:(0.0,0.0,0.0,0.0),
				is_edit_un_commited:0,
				is_physics_disabled:0,
				is_row_selected:0,
			}
		}

		fn load(reader:&mut Cursor<&[u8]>) -> Result<Self>
		{
			auto this = PmmBoneCurrentData();
			this.translation = PmmVector3::load(reader)?;
			this.quaternion = PmmVector4::load(reader)?;
			this.is_edit_un_commited = reader.read_u8()?;
			this.is_physics_disabled = reader.read_u8()?;
			this.is_row_selected = reader.read_u8()?;

			Ok(this)
		}

		fn load_arrays(reader:&mut Cursor<&[u8]>) -> Result<std::vector<Self>>
		{
			auto model = Vec::with_capacity(reader.read_u32::<LittleEndian>()? as usize);
			for _ in 0,model.capacity()
			{
				model.push(PmmBoneCurrentData::load(reader)?);
			}

			Ok(model)
		}
	}

	impl PmmOpCurrentData
	{
		fn new() -> Self
		{
			Self
			{
				keyframe_begin:-1,
				keyframe_end:-1,
				model_index:-1,
				parent_bone_index:-1,
			}
		}

		fn load(reader:&mut Cursor<&[u8]>) -> Result<Self>
		{
			auto this = PmmOpCurrentData();
			this.keyframe_begin = reader.read_i32::<LittleEndian>()?;
			this.keyframe_end = reader.read_i32::<LittleEndian>()?;
			this.model_index = reader.read_i32::<LittleEndian>()?;
			this.parent_bone_index = reader.read_i32::<LittleEndian>()?;

			Ok(this)
		}

		fn load_arrays(reader:&mut Cursor<&[u8]>) -> Result<std::vector<Self>>
		{
			auto model = Vec::with_capacity(reader.read_u32::<LittleEndian>()? as usize);
			for _ in 0,model.capacity()
			{
				model.push(PmmOpCurrentData::load(reader)?);
			}

			Ok(model)
		}
	}

	impl PmmModel
	{
		fn new() -> Self
		{
			Self
			{
				number:0,
				name:std::string(),
				name_en:std::string(),
				path:std::string(),
				keyframe_editor_toplevel_rows:0,
				bone_name:Vec(),
				morph_name:Vec(),
				ik_index:Vec(),
				op_index:Vec(),
				draw_order:0,
				edit_is_display:0,
				edit_selected_bone:0,
				skin_panel:[0;4],
				is_frame_open:Vec(),
				vscroll:0,
				last_frame:0,
				bone_init_frame:Vec(),
				bone_key_frame:Vec(),
				morph_init_frame:Vec(),
				morph_key_frame:Vec(),
				op_init_frame:PmmOpFrame(),
				op_key_frames:Vec(),
				bone_current_datas:Vec(),
				morph_current_datas:Vec(),
				is_current_ik_enabled_datas:Vec(),
				op_current_data:Vec(),
				is_add_blend:0,
				edge_width:1.0,
				is_selfshadow_enabled:1,
				calc_order:1
			}
		}

		fn load(reader:&mut Cursor<&[u8]>) -> Result<Self>
		{
			auto this = PmmModel();
			this.number = reader.read_u8()?;
			this.name = PmmName::load(reader)?;
			this.name_en = PmmName::load(reader)?;
			this.path = PmmName::load_fixed_utf8(reader, 256)?;
			this.keyframe_editor_toplevel_rows = reader.read_u8()?;
			this.bone_name = PmmName::load_arrays(reader)?;
			this.morph_name = PmmName::load_arrays(reader)?;
			this.ik_index = PmmInt32::load_arrays(reader)?;
			this.op_index = PmmInt32::load_arrays(reader)?;
			this.draw_order = reader.read_u8()?;
			this.edit_is_display = reader.read_u8()?;
			this.edit_selected_bone = reader.read_i32::<LittleEndian>()?;
			this.skin_panel[0] = reader.read_i32::<LittleEndian>()?;
			this.skin_panel[1] = reader.read_i32::<LittleEndian>()?;
			this.skin_panel[2] = reader.read_i32::<LittleEndian>()?;
			this.skin_panel[3] = reader.read_i32::<LittleEndian>()?;
			this.is_frame_open = PmmUint8::load_array_from_u8(reader)?;
			this.vscroll = reader.read_i32::<LittleEndian>()?;
			this.last_frame = reader.read_i32::<LittleEndian>()?;
			this.bone_init_frame = PmmBoneFrame::load_fixed_arrays(reader, this.bone_name.len(), true)?;
			this.bone_key_frame = PmmBoneFrame::load_arrays(reader, false)?;
			this.morph_init_frame = PmmMorphFrame::load_fixed_arrays(reader, this.morph_name.len(), true)?;
			this.morph_key_frame = PmmMorphFrame::load_arrays(reader, false)?;
			this.op_init_frame = PmmOpFrame::load(reader, this.ik_index.len(), this.op_index.len(), true)?;
			this.op_key_frames = PmmOpFrame::load_arrays(reader, this.ik_index.len(), this.op_index.len(), false)?;

			for _ in 0,this.bone_name.len()
			{
				this.bone_current_datas.push(PmmBoneCurrentData::load(reader)?);
			}

			for _ in 0,this.morph_name.len()
			{
				this.morph_current_datas.push(reader.read_f32::<LittleEndian>()?);
			}

			for _ in 0,this.ik_index.len()
			{
				this.is_current_ik_enabled_datas.push(reader.read_u8()?);
			}

			for _ in 0,this.op_index.len()
			{
				this.op_current_data.push(PmmOpCurrentData::load(reader)?);
			}

			this.is_add_blend = reader.read_u8()?;
			this.edge_width = reader.read_f32::<LittleEndian>()?;
			this.is_selfshadow_enabled = reader.read_u8()?;
			this.calc_order = reader.read_u8()?;

			Ok(this)
		}

		fn load_arrays(reader:&mut Cursor<&[u8]>) -> Result<std::vector<Self>>
		{
			auto model = Vec::with_capacity(reader.read_u8()? as usize);
			for _ in 0,model.capacity()
			{
				model.push(PmmModel::load(reader)?);
			}

			Ok(model)
		}
	}

	impl PmmCameraFrame
	{
		fn new() -> Self
		{
			Self
			{
				data_index:0,
				frame_number:0,
				pre_index:0,
				next_index:0,
				distance:0.0,
				eye_position:(0.0,0.0,0.0),
				rotation:(0.0,0.0,0.0),
				looking_model_index:0,
				looking_bone_index:0,
				interpolation_x:[0; 4],
				interpolation_y:[0; 4],
				interpolation_z:[0; 4],
				interpolation_rotation:[0; 4],
				interpolation_distance:[0; 4],
				interpolation_angleview:[0; 4],
				is_parse:0,
				angle_view:0,
				is_selected:0
			}
		}

		fn load(reader:&mut Cursor<&[u8]>, is_init:bool) -> Result<Self>
		{
			auto this = PmmCameraFrame();
			this.data_index = if is_init { -1 } else { reader.read_i32::<LittleEndian>()? };
			this.frame_number = reader.read_i32::<LittleEndian>()?;
			this.pre_index = reader.read_i32::<LittleEndian>()?;
			this.next_index = reader.read_i32::<LittleEndian>()?;
			this.distance = reader.read_f32::<LittleEndian>()?;
			this.eye_position = PmmVector3::load(reader)?;
			this.rotation = PmmVector3::load(reader)?;
			this.looking_model_index = reader.read_i32::<LittleEndian>()?;
			this.looking_bone_index = reader.read_i32::<LittleEndian>()?;
			for i in 0,this.interpolation_x.len() { this.interpolation_x[i] = reader.read_u8()?; }
			for i in 0,this.interpolation_y.len() { this.interpolation_y[i] = reader.read_u8()?; }
			for i in 0,this.interpolation_z.len() { this.interpolation_z[i] = reader.read_u8()?; }
			for i in 0,this.interpolation_rotation.len() { this.interpolation_rotation[i] = reader.read_u8()?; }
			for i in 0,this.interpolation_distance.len() { this.interpolation_distance[i] = reader.read_u8()?; }
			for i in 0,this.interpolation_angleview.len() { this.interpolation_angleview[i] = reader.read_u8()?; }
			this.is_parse = reader.read_u8()?;
			this.angle_view = reader.read_u32::<LittleEndian>()?;
			this.is_selected = reader.read_u8()?;

			Ok(this)
		}

		fn load_arrays(reader:&mut Cursor<&[u8]>, is_init:bool) -> Result<std::vector<Self>>
		{
			auto model = Vec::with_capacity(reader.read_u32::<LittleEndian>()? as usize);
			for _ in 0,model.capacity()
			{
				model.push(PmmCameraFrame::load(reader, is_init)?);
			}

			Ok(model)
		}
	}

	impl PmmCameraCurrentData
	{
		fn new() -> Self
		{
			Self
			{
				eye_position:(0.0,0.0,0.0),
				target_position:(0.0,0.0,0.0),
				rotation:(0.0,0.0,0.0),
				isorthro:0,
			}
		}

		fn load(reader:&mut Cursor<&[u8]>) -> Result<Self>
		{
			auto this = PmmCameraCurrentData();
			this.eye_position = PmmVector3::load(reader)?;
			this.target_position = PmmVector3::load(reader)?;
			this.rotation = PmmVector3::load(reader)?;
			this.isorthro = reader.read_u8()?;

			Ok(this)
		}

		fn load_arrays(reader:&mut Cursor<&[u8]>) -> Result<std::vector<Self>>
		{
			auto model = Vec::with_capacity(reader.read_u8()? as usize);
			for _ in 0,model.capacity()
			{
				model.push(PmmCameraCurrentData::load(reader)?);
			}

			Ok(model)
		}
	}

	impl PmmLightFrame
	{
		fn new() -> Self
		{
			Self
			{
				data_index:-1,
				frame_number:-1,
				pre_index:-1,
				next_index:-1,
				rgb:(0.0,0.0,0.0),
				xyz:(0.0,0.0,0.0),
				is_selected:0,
			}
		}

		fn load(reader:&mut Cursor<&[u8]>, is_init:bool) -> Result<Self>
		{
			auto this = PmmLightFrame();
			this.data_index = if is_init { -1 } else { reader.read_i32::<LittleEndian>()? };
			this.frame_number = reader.read_i32::<LittleEndian>()?;
			this.pre_index = reader.read_i32::<LittleEndian>()?;
			this.next_index = reader.read_i32::<LittleEndian>()?;
			this.rgb = PmmVector3::load(reader)?;
			this.xyz = PmmVector3::load(reader)?;
			this.is_selected = reader.read_u8()?;

			Ok(this)
		}

		fn load_arrays(reader:&mut Cursor<&[u8]>, is_init:bool) -> Result<std::vector<Self>>
		{
			auto model = Vec::with_capacity(reader.read_u32::<LittleEndian>()? as usize);
			for _ in 0,model.capacity()
			{
				model.push(PmmLightFrame::load(reader, is_init)?);
			}

			Ok(model)
		}
	}

	impl PmmLightCurrentData
	{
		fn new() -> Self
		{
			Self
			{
				rgb:(0.0,0.0,0.0),
				xyz:(0.0,0.0,0.0),
				is_selected:0,
			}
		}

		fn load(reader:&mut Cursor<&[u8]>) -> Result<Self>
		{
			auto this = PmmLightCurrentData();
			this.rgb = PmmVector3::load(reader)?;
			this.xyz = PmmVector3::load(reader)?;
			this.is_selected = reader.read_u8()?;

			Ok(this)
		}

		fn load_arrays(reader:&mut Cursor<&[u8]>) -> Result<std::vector<Self>>
		{
			auto model = Vec::with_capacity(reader.read_u8()? as usize);
			for _ in 0,model.capacity()
			{
				model.push(PmmLightCurrentData::load(reader)?);
			}

			Ok(model)
		}
	}

	impl PmmKeyFrame
	{
		fn new() -> Self
		{
			Self
			{
				data_index:-1,
				frame_number:-1,
				pre_index:-1,
				next_index:-1,
				is_selected:0
			}
		}

		fn load(reader:&mut Cursor<&[u8]>) -> Result<Self>
		{
			auto this = PmmKeyFrame();
			this.data_index = reader.read_i32::<LittleEndian>()?;
			this.frame_number = reader.read_i32::<LittleEndian>()?;
			this.pre_index = reader.read_i32::<LittleEndian>()?;
			this.next_index = reader.read_i32::<LittleEndian>()?;
			this.is_selected = reader.read_u8()?;

			Ok(this)
		}

		fn load_arrays(reader:&mut Cursor<&[u8]>) -> Result<std::vector<Self>>
		{
			auto model = Vec::with_capacity(reader.read_u8()? as usize);
			for _ in 0,model.capacity()
			{
				model.push(PmmKeyFrame::load(reader)?);
			}

			Ok(model)
		}
	}

	impl PmmDataBody
	{
		fn new() -> Self
		{
			Self
			{
				transparency:0,
				is_visible:0,
				parent_model_index:0,
				parent_bone_index:0,
				translation:(0.0,0.0,0.0),
				rotation:(0.0,0.0,0.0),
				scale:0.0,
				is_shadow_enabled:0,
			}
		}

		fn load(reader:&mut Cursor<&[u8]>) -> Result<Self>
		{
			auto this = PmmDataBody();
			this.transparency = reader.read_u8()?;
			this.is_visible = 0;
			this.parent_model_index = reader.read_i32::<LittleEndian>()?;
			this.parent_bone_index = reader.read_i32::<LittleEndian>()?;
			this.translation = PmmVector3::load(reader)?;
			this.rotation = PmmVector3::load(reader)?;
			this.scale = reader.read_f32::<LittleEndian>()?;
			this.is_shadow_enabled = reader.read_u8()?;

			Ok(this)
		}

		fn load_arrays(reader:&mut Cursor<&[u8]>) -> Result<std::vector<Self>>
		{
			auto model = Vec::with_capacity(reader.read_u8()? as usize);
			for _ in 0,model.capacity()
			{
				model.push(PmmDataBody::load(reader)?);
			}

			Ok(model)
		}
	}

	impl PmmAccessoryData
	{
		fn new() -> Self
		{
			Self
			{
				index:0,
				name:std::string(),
				path:std::string(),
				draw_order:0,
				init_frame:PmmKeyFrame(),
				key_frames:Vec(),
				current_data:PmmDataBody(),
				is_add_blend:0
			}
		}

		fn load(reader:&mut Cursor<&[u8]>) -> Result<Self>
		{
			auto this = PmmAccessoryData();
			this.index = reader.read_u8()?;
			this.name = PmmName::load_fixed_utf8(reader, 100)?;
			this.path = PmmName::load_fixed_utf8(reader, 256)?;
			this.draw_order = reader.read_u8()?;
			this.init_frame = PmmKeyFrame::load(reader)?;
			this.key_frames = PmmKeyFrame::load_arrays(reader)?;
			this.current_data = PmmDataBody::load(reader)?;
			this.is_add_blend = reader.read_u8()?;

			Ok(this)
		}

		fn load_arrays(reader:&mut Cursor<&[u8]>) -> Result<std::vector<Self>>
		{
			auto model = Vec::with_capacity(reader.read_u8()? as usize);
			for _ in 0,model.capacity()
			{
				model.push(PmmAccessoryData::load(reader)?);
			}

			Ok(model)
		}
	}

	impl PmmCSelectorChoiceData
	{
		fn new() -> Self
		{
			Self
			{
				mode_index:0,
				selector_choice:0,
			}
		}

		fn load(reader:&mut Cursor<&[u8]>) -> Result<Self>
		{
			auto this = PmmCSelectorChoiceData();
			this.mode_index = reader.read_u8()?;
			this.selector_choice = reader.read_u32::<LittleEndian>()?;

			Ok(this)
		}

		fn load_arrays(reader:&mut Cursor<&[u8]>) -> Result<std::vector<Self>>
		{
			auto model = Vec::with_capacity(reader.read_u8()? as usize);
			for _ in 0,model.capacity()
			{
				model.push(PmmCSelectorChoiceData::load(reader)?);
			}

			Ok(model)
		}
	}

	impl PMMFile
	{
		fn new() -> Self
		{
			Self
			{
				header:PmmHeader(),
				model:Vec(),
				camera_init_frame:PmmCameraFrame(),
				camera_key_frames:Vec(),
				camera_current_data:PmmCameraCurrentData(),
				light_init_frame:PmmLightFrame(),
				light_key_frames:Vec(),
				light_current_data:PmmLightCurrentData(),
				selected_accessory_index:0,
				accessory_vscroll:0,
				accessory_count:0,
				accessory_name:Vec(),
				accessory_datas:Vec(),
				current_frame_position:0,
				h_scroll_position:0,
				h_scroll_scale:0,
				bone_operation_kind:0,
				looking_at:0,
				is_repeat:0,
				is_play_from_frame:0,
				is_play_to_frame:0,
				play_start_frame:0,
				play_end_frame:0,
				is_wave_enabled:0,
				wave_path:std::string(),
				avi_offset_x: 0,
				avi_offset_y: 0,
				avi_scale: 0.0,
				avi_path: std::string(),
				is_show_avi: 0,
				background_image_offset_x: 0,
				background_image_offset_y: 0,
				background_image_scale: 0,
				background_image_path: std::string(),
				is_show_background_image: 0,
				is_show_infomation: 0,
				is_show_axis: 0,
				is_show_groundshadow: 0,
				fps_limit: 0.0,
				screen_capture_mode: 0,
				accessory_number_render_after_model: 0,
				ground_shadow_brightness: 0.0,
				is_transparent_ground_shadow: 0,
				physics_mode: 0,
				gravity_current_data:PmmGravityCurrentData(),
				gravity_init_frame:PmmGravityKeyFrame(),
				gravity_key_frames:Vec(),
				is_show_selfshadow:0,
				selfshadow_current_data:0.0,
				selfshadow_init_frame:PmmSelfShadowKeyFrame(),
				selfshadow_keyframes:Vec(),
				edge_color_r:0,
				edge_color_g:0,
				edge_color_b:0,
				is_black_background:0,
				camera_current_looking_at_model:0,
				camera_current_looking_at_bone:0,
				unknown_array:[0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0],
				is_view_look_at_enabled:0,
				unknown:0,
				is_physics_ground_enabled:0,
				frame_text_box:0,
				selector_choice_selection_following:0,
				selector_choice_datas:Vec(),
			}
		}

		fn load(buf:&[u8]) -> Result<Self>
		{
			auto reader = Cursor(buf);
			auto pmm = PMMFile();
			pmm.header = PmmHeader::load(&mut reader)?;
			pmm.model = PmmModel::load_arrays(&mut reader)?;
			pmm.camera_init_frame = PmmCameraFrame::load(&mut reader, true)?;
			pmm.camera_key_frames = PmmCameraFrame::load_arrays(&mut reader, false)?;
			pmm.camera_current_data = PmmCameraCurrentData::load(&mut reader)?;
			pmm.light_init_frame = PmmLightFrame::load(&mut reader, true)?;
			pmm.light_key_frames = PmmLightFrame::load_arrays(&mut reader, false)?;
			pmm.light_current_data = PmmLightCurrentData::load(&mut reader)?;
			pmm.selected_accessory_index = reader.read_u8()?;
			pmm.accessory_vscroll = reader.read_u32::<LittleEndian>()?;
			pmm.accessory_count = reader.read_u8()?;

			for _ in 0,pmm.accessory_count
			{
				pmm.accessory_name.push(PmmName::load_fixed_utf8(&mut reader, 100)?);
			}

			for _ in 0,pmm.accessory_count
			{
				pmm.accessory_datas.push(PmmAccessoryData::load(&mut reader)?);
			}

			pmm.current_frame_position = reader.read_u32::<LittleEndian>()?;
			pmm.h_scroll_position = reader.read_u32::<LittleEndian>()?;
			pmm.h_scroll_scale = reader.read_u32::<LittleEndian>()?;
			pmm.bone_operation_kind = reader.read_u32::<LittleEndian>()?;
			pmm.looking_at = reader.read_u8()?;
			pmm.is_repeat = reader.read_u8()?;
			pmm.is_play_from_frame = reader.read_u8()?;
			pmm.is_play_to_frame = reader.read_u8()?;
			pmm.play_start_frame = reader.read_u32::<LittleEndian>()?;
			pmm.play_end_frame = reader.read_u32::<LittleEndian>()?;
			pmm.is_wave_enabled = reader.read_u8()?;
			pmm.wave_path = PmmName::load_fixed_utf8(&mut reader, 256)?;
			pmm.avi_offset_x = reader.read_u32::<LittleEndian>()?;
			pmm.avi_offset_y = reader.read_u32::<LittleEndian>()?;
			pmm.avi_scale = reader.read_f32::<LittleEndian>()?;
			pmm.avi_path = PmmName::load_fixed_utf8(&mut reader, 256)?;
			pmm.is_show_avi = reader.read_u32::<LittleEndian>()?;
			pmm.background_image_offset_x = reader.read_u32::<LittleEndian>()?;
			pmm.background_image_offset_y = reader.read_u32::<LittleEndian>()?;
			pmm.background_image_scale = reader.read_u32::<LittleEndian>()?;
			pmm.background_image_path = PmmName::load_fixed_utf8(&mut reader, 255)?;
			pmm.is_show_background_image = reader.read_u8()?;
			pmm.is_show_infomation = reader.read_u8()?;
			pmm.is_show_axis = reader.read_u8()?;
			pmm.is_show_groundshadow = reader.read_u8()?;
			pmm.fps_limit = reader.read_f32::<LittleEndian>()?;
			pmm.screen_capture_mode = reader.read_u32::<LittleEndian>()?;
			pmm.accessory_number_render_after_model = reader.read_u32::<LittleEndian>()?;
			pmm.ground_shadow_brightness = reader.read_f32::<LittleEndian>()?;
			pmm.is_transparent_ground_shadow = reader.read_u8()?;
			pmm.physics_mode =  reader.read_u8()?;
			pmm.gravity_current_data = PmmGravityCurrentData::load(&mut reader)?;
			pmm.gravity_init_frame = PmmGravityKeyFrame::load(&mut reader, true)?;
			pmm.gravity_key_frames = PmmGravityKeyFrame::load_arrays(&mut reader, false)?;
			pmm.is_show_selfshadow = reader.read_u8()?;
			pmm.selfshadow_current_data = reader.read_f32::<LittleEndian>()?;
			pmm.selfshadow_init_frame = PmmSelfShadowKeyFrame::load(&mut reader, true)?;
			pmm.selfshadow_keyframes = PmmSelfShadowKeyFrame::load_arrays(&mut reader, false)?;
			pmm.edge_color_r = reader.read_u32::<LittleEndian>()?;
			pmm.edge_color_g = reader.read_u32::<LittleEndian>()?;
			pmm.edge_color_b = reader.read_u32::<LittleEndian>()?;
			pmm.is_black_background = reader.read_u8()?;
			pmm.camera_current_looking_at_model = reader.read_i32::<LittleEndian>()?;
			pmm.camera_current_looking_at_bone = reader.read_i32::<LittleEndian>()?;
			for i in 0,pmm.unknown_array.len() { pmm.unknown_array[i] = reader.read_f32::<LittleEndian>()?; }
			pmm.is_view_look_at_enabled = reader.read_u8()?;
			pmm.unknown = reader.read_u8()?;
			pmm.is_physics_ground_enabled = reader.read_u8()?;
			pmm.frame_text_box = reader.read_u32::<LittleEndian>()?;
			pmm.selector_choice_selection_following = reader.read_u8()?;
			pmm.selector_choice_datas = PmmCSelectorChoiceData::load_arrays(&mut reader)?;

			Ok(pmm)
		}
	}

	class PMMLoader 
	{
		PMMLoader()
		{
		}

		bool can_read(&self, buf:&[u8])
		{
			PmmHeader::load(&mut Cursor(buf)).is_ok()
		}

		Result<Scene> do_load(&self, buf:&[u8])
		{
			auto pmm = PMMFile::load(buf)?;
			auto scene = Scene();

			auto camera = PerspectiveCamera::builder()
				.main(true)
				.set_fov(30.0)
				.set_translate(float!(0.0,0.1,10.0))
				.build();

			scene.add(camera);

			for model in pmm.model
			{
				auto model = ModelLoader::open(model.path)?;
				model.set_scale(float!(0.1,0.1,0.1));
				model.set_translate(float!(0.0,-0.8,20.0));
				scene.add(model);
			}

			Ok(scene)
		}

		Result<std::vector<u8>> do_save(&self, _:&Scene)
		{
  			Err(Error::LoaderError("Not Implmention yet".to_std::string()))
		}
	}
}*/