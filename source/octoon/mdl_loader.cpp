#include <octoon/mdl_loader.h>
#include <octoon/texture_loader.h>
#include <mi/mdl_sdk.h>
#include <regex>
#include <map>
#include <thread>

#ifdef MI_PLATFORM_WINDOWS
#	ifdef UNICODE
#		define FMT_LPTSTR "%ls"
#	else
#		define FMT_LPTSTR "%s"
#	endif
#endif

#define MDL_MAX_TEXTURES 32
const std::string MDL_USE_TEXTURE_PREFIX = "VisRTX_use_";

namespace std
{
	template<class BidirIt, class Traits, class CharT, class UnaryFunction>
	std::basic_string<CharT> regex_replace(BidirIt first, BidirIt last, const std::basic_regex<CharT, Traits>& re, UnaryFunction f)
	{
		std::basic_string<CharT> s;

		typename std::match_results<BidirIt>::difference_type positionOfLastMatch = 0;
		auto endOfLastMatch = first;

		auto callback = [&](const std::match_results<BidirIt>& match)
		{
			auto positionOfThisMatch = match.position(0);
			auto diff = positionOfThisMatch - positionOfLastMatch;

			auto startOfThisMatch = endOfLastMatch;
			std::advance(startOfThisMatch, diff);

			s.append(endOfLastMatch, startOfThisMatch);
			s.append(f(match));

			auto lengthOfMatch = match.length(0);

			positionOfLastMatch = positionOfThisMatch + lengthOfMatch;

			endOfLastMatch = startOfThisMatch;
			std::advance(endOfLastMatch, lengthOfMatch);
		};

		std::regex_iterator<BidirIt> begin(first, last, re), end;
		std::for_each(begin, end, callback);

		s.append(endOfLastMatch, last);

		return s;
	}

	template<class Traits, class CharT, class UnaryFunction>
	std::string regex_replace(const std::string& s, const std::basic_regex<CharT, Traits>& re, UnaryFunction f)
	{
		return regex_replace(s.cbegin(), s.cend(), re, f);
	}
}

namespace octoon
{
	void check_success(bool expr, const char* errMsg)
	{
		if (expr) return;

		throw std::runtime_error(errMsg);
	}

	void check_success(bool expr, const std::string& errMsg)
	{
		if (expr) return;

		throw std::runtime_error(errMsg);
	}

	void check_success(mi::Sint32 errVal, const char* errMsg)
	{
		if (errVal >= 0) return;

		throw std::runtime_error(std::string(errMsg) + " (" + std::to_string(errVal) + ")");
	}

	void check_success(mi::Sint32 errVal, const std::string& errMsg)
	{
		if (errVal >= 0) return;

		throw std::runtime_error(errMsg + " (" + std::to_string(errVal) + ")");
	}

	struct Material_parameter
	{
		typedef void(Remap_func(mi::base::IInterface*));

		mi::base::Handle<mi::IData>                 value;
		mi::base::Handle<mi::neuraylib::ICanvas>    texture;

		std::string                                 value_type;
		std::string                                 bake_path;

		Remap_func* remap_func;

		Material_parameter() : remap_func(nullptr)
		{

		}

		Material_parameter(const std::string& value_type, Remap_func* func = nullptr)
			: value_type(value_type)
			, remap_func(func)
		{
		}
	};

	void remap_normal(mi::base::IInterface* icanvas)
	{
		mi::base::Handle<mi::neuraylib::ICanvas> canvas(
			icanvas->get_interface<mi::neuraylib::ICanvas>());
		if (!canvas)
			return;
		// Convert normal values from the interval [-1.0,1.0] to [0.0, 1.0]
		mi::base::Handle<mi::neuraylib::ITile> tile(canvas->get_tile(0, 0));
		mi::Float32* data = static_cast<mi::Float32*>(tile->get_data());

		const mi::Uint32 n = canvas->get_resolution_x() * canvas->get_resolution_y() * 3;
		for (mi::Uint32 i = 0; i < n; ++i)
		{
			data[i] = (data[i] + 1.f) * 0.5f;
		}
	}

	template <typename T>
	mi::IData* create_value(mi::neuraylib::ITransaction* transaction, const char* type_name, const T& default_value)
	{
		mi::base::Handle<mi::base::IInterface> value(transaction->create(type_name));
		mi::base::Handle<mi::IData> data(value->get_interface<mi::IData>());
		mi::set_value(data.get(), default_value);

		data->retain();
		return data.get();
	}

	// Converts the given expression to a direct_call, thereby resolving temporaries
	// Returns nullptr if the passed expression is not a direct call
	const mi::neuraylib::IExpression_direct_call* to_direct_call(
		const mi::neuraylib::IExpression* expr,
		const mi::neuraylib::ICompiled_material* cm)
	{
		if (!expr) return nullptr;
		switch (expr->get_kind())
		{
		case mi::neuraylib::IExpression::EK_DIRECT_CALL:
			return expr->get_interface<mi::neuraylib::IExpression_direct_call>();
		case mi::neuraylib::IExpression::EK_TEMPORARY:
		{
			mi::base::Handle<const mi::neuraylib::IExpression_temporary> expr_temp(
				expr->get_interface<const mi::neuraylib::IExpression_temporary>());
			mi::base::Handle<const mi::neuraylib::IExpression_direct_call> ref_expr(
				cm->get_temporary<const mi::neuraylib::IExpression_direct_call>(
					expr_temp->get_index()));
			ref_expr->retain();
			return ref_expr.get();
		}
		default:
			break;
		}
		return nullptr;
	}

	// Returns the semantic of the function definition which corresponds to
	// the given call or DS::UNKNOWN in case the expression
	// is nullptr
	mi::neuraylib::IFunction_definition::Semantics get_call_semantic(
		mi::neuraylib::ITransaction* transaction,
		const mi::neuraylib::IExpression_direct_call* call)
	{
		if (!(call))
			return mi::neuraylib::IFunction_definition::DS_UNKNOWN;

		mi::base::Handle<const mi::neuraylib::IFunction_definition> mdl_def(
			transaction->access<const mi::neuraylib::IFunction_definition>(
				call->get_definition()));

		return mdl_def->get_semantic();
	}

	// Returns the argument 'argument_name' of the given call
	const mi::neuraylib::IExpression_direct_call* get_argument_as_call(
		const mi::neuraylib::ICompiled_material* cm,
		const mi::neuraylib::IExpression_direct_call* call,
		const char* argument_name)
	{
		if (!call) return nullptr;

		mi::base::Handle<const mi::neuraylib::IExpression_list> call_args(
			call->get_arguments());
		mi::base::Handle<const mi::neuraylib::IExpression> arg(
			call_args->get_expression(argument_name));
		return to_direct_call(arg.get(), cm);
	}

	// Looks up the sub expression 'path' within the compiled material
	// starting at parent_call. If parent_call is nullptr,the
	// material will be traversed from the root
	const mi::neuraylib::IExpression_direct_call* lookup_call(
		const std::string& path,
		const mi::neuraylib::ICompiled_material* cm,
		const mi::neuraylib::IExpression_direct_call* parent_call = nullptr
	)
	{
		mi::base::Handle<const mi::neuraylib::IExpression_direct_call>
			result_call;

		if (parent_call == nullptr)
		{
			mi::base::Handle<const mi::neuraylib::IExpression> expr(cm->lookup_sub_expression(path.c_str()));
			result_call = to_direct_call(expr.get(), cm);
		}
		else
		{
			result_call = mi::base::make_handle_dup(parent_call);

			std::string remaining_path = path;
			std::size_t p = 0;
			do
			{
				std::string arg = remaining_path;
				p = remaining_path.find(".");
				if (p != std::string::npos)
				{
					arg = remaining_path.substr(0, p);
					remaining_path = remaining_path.substr(p + 1, remaining_path.size() - p - 1);
				}
				result_call = get_argument_as_call(cm, result_call.get(), arg.c_str());
				if (!result_call)
					return nullptr;
			} while (p != std::string::npos);
		}
		result_call->retain();
		return result_call.get();
	}

	typedef std::map<std::string, Material_parameter> Material;

	void rough_to_gloss(mi::base::IInterface* ii)
	{
		mi::base::Handle<mi::neuraylib::ICanvas> canvas(
			ii->get_interface<mi::neuraylib::ICanvas>());
		if (canvas)
		{
			mi::base::Handle<mi::neuraylib::ITile> tile(canvas->get_tile(0, 0));
			mi::Float32* data = static_cast<mi::Float32*>(tile->get_data());

			const mi::Uint32 n = canvas->get_resolution_x() * canvas->get_resolution_y();
			for (mi::Uint32 i = 0; i < n; ++i)
			{
				data[i] = 1.0f - data[i];
			}
			return;
		}
		mi::base::Handle<mi::IFloat32> value(ii->get_interface<mi::IFloat32>());
		if (value)
		{
			mi::Float32 f;
			mi::get_value(value.get(), f);
			value->set_value(1.0f - f);
		}
	}

	void setup_target_material(std::string_view target_model, mi::neuraylib::ITransaction* transaction, const mi::neuraylib::ICompiled_material* cm, Material& out_material)
	{
		mi::base::Handle<const mi::neuraylib::IExpression_direct_call> parent_call(lookup_call("surface.scattering", cm));
		mi::neuraylib::IFunction_definition::Semantics semantic(get_call_semantic(transaction, parent_call.get()));

		if (target_model == "diffuse")
		{
			out_material["color"] = Material_parameter("Rgb_fp");
			out_material["roughness"] = Material_parameter("Float32");
			out_material["normal"] = Material_parameter("Float32<3>", remap_normal);

			out_material["color"].bake_path = "surface.scattering.tint";
			out_material["roughness"].bake_path = "surface.scattering.roughness";
			out_material["normal"].bake_path = "geometry.normal";

		}
		else if (target_model == "diffuse_glossy")
		{
			out_material["diffuse_color"] = Material_parameter("Rgb_fp");
			out_material["glossy_color"] = Material_parameter("Rgb_fp");
			out_material["glossy_roughness"] = Material_parameter("Float32");
			out_material["glossy_weight"] = Material_parameter("Float32");
			out_material["ior"] = Material_parameter("Float32");
			out_material["normal"] = Material_parameter("Float32<3>", remap_normal);

			switch (semantic)
			{
			case mi::neuraylib::IFunction_definition::DS_INTRINSIC_DF_DIFFUSE_REFLECTION_BSDF:
				out_material["diffuse_color"].bake_path = "surface.scattering.tint";
				break;
			case mi::neuraylib::IFunction_definition::DS_INTRINSIC_DF_SIMPLE_GLOSSY_BSDF:
				out_material["glossy_color"].bake_path = "surface.scattering.tint";
				out_material["glossy_roughness"].bake_path = "surface.scattering.roughness_u";
				break;
			case mi::neuraylib::IFunction_definition::DS_INTRINSIC_DF_FRESNEL_LAYER:
				out_material["diffuse_color"].bake_path = "surface.scattering.base.tint";
				out_material["glossy_color"].bake_path = "surface.scattering.layer.tint";
				out_material["glossy_roughness"].bake_path = "surface.scattering.layer.roughness_u";
				out_material["glossy_weight"].bake_path = "surface.scattering.weight";
				out_material["ior"].bake_path = "surface.scattering.ior";
				break;
			default:
				break;
			}
			out_material["normal"].bake_path = "geometry.normal";
		}
		else if (target_model == "ue4" || target_model == "transmissive_pbr")
		{
			out_material["base_color"] = Material_parameter("Rgb_fp");
			out_material["metallic"] = Material_parameter("Float32");
			out_material["specular"] = Material_parameter("Float32");
			out_material["roughness"] = Material_parameter("Float32");
			out_material["normal"] = Material_parameter("Float32<3>", remap_normal);
			out_material["clearcoat_weight"] = Material_parameter("Float32");
			out_material["clearcoat_roughness"] = Material_parameter("Float32");
			out_material["clearcoat_normal"] = Material_parameter("Float32<3>", remap_normal);
			out_material["opacity"] = Material_parameter("Float32");

			mi::base::Handle<const mi::neuraylib::IExpression> emission(cm->lookup_sub_expression("surface.emission.emission"));
			if (emission.is_valid_interface())
			{
				out_material["emission"] = Material_parameter("Rgb_fp");
				out_material["emission"].bake_path = "surface.emission.emission";
			}

			mi::base::Handle<const mi::neuraylib::IExpression> intensity(cm->lookup_sub_expression("surface.emission.intensity"));
			if (intensity.is_valid_interface())
			{
				out_material["emission_intensity"] = Material_parameter("Float32");
				out_material["emission_intensity"].bake_path = "surface.emission.intensity";
			}

			std::string path_prefix = "surface.scattering.";

			bool is_transmissive_pbr = false;
			if (target_model == "transmissive_pbr")
			{
				is_transmissive_pbr = true;

				out_material["anisotropy"] = Material_parameter("Float32");
				out_material["anisotropy_rotation"] = Material_parameter("Float32");
				out_material["transparency"] = Material_parameter("Float32");
				out_material["transmission_color"] = Material_parameter("Rgb_fp");
				out_material["attenuation_color"] = Material_parameter("Rgb_fp");
				out_material["attenuation_distance"] = Material_parameter("Float32");
				out_material["subsurface_color"] = Material_parameter("Rgb_fp");
				out_material["volume_ior"] = Material_parameter("Rgb_fp");

				out_material["attenuation_color"].bake_path = "volume.absorption_coefficient.s.v.attenuation";
				out_material["subsurface_color"].bake_path = "volume.absorption_coefficient.s.v.subsurface";
				out_material["attenuation_distance"].bake_path = "volume.scattering_coefficient.s.v.distance";
				out_material["volume_ior"].bake_path = "ior";
			}
			
			if (semantic == mi::neuraylib::IFunction_definition::DS_INTRINSIC_DF_CUSTOM_CURVE_LAYER)
			{
				out_material["clearcoat_weight"].bake_path = path_prefix + "weight";
				out_material["clearcoat_roughness"].bake_path = path_prefix + "layer.roughness_u";
				out_material["clearcoat_normal"].bake_path = path_prefix + "normal";

				parent_call = lookup_call("base", cm, parent_call.get());
				semantic = get_call_semantic(transaction, parent_call.get());
				path_prefix += "base.";
			}

			if (semantic == mi::neuraylib::IFunction_definition::DS_INTRINSIC_DF_WEIGHTED_LAYER)
			{
				out_material["normal"].bake_path = path_prefix + "normal";

				parent_call = lookup_call("layer", cm, parent_call.get());
				semantic = get_call_semantic(transaction, parent_call.get());
				path_prefix += "layer.";
			}

			if (semantic == mi::neuraylib::IFunction_definition::DS_INTRINSIC_DF_NORMALIZED_MIX)
			{
				out_material["metallic"].bake_path = path_prefix + "components.1.weight";

				if (is_transmissive_pbr) {
					out_material["roughness"].bake_path = path_prefix + "components.1.component.roughness_u.s.r.roughness";
					out_material["anisotropy"].bake_path = path_prefix + "components.1.component.roughness_u.s.r.anisotropy";
					out_material["anisotropy_rotation"].bake_path = path_prefix + "components.1.component.roughness_u.s.r.rotation";
				}
				else
				{
					out_material["roughness"].bake_path = path_prefix + "components.1.component.roughness_u";
				}

				out_material["base_color"].bake_path = path_prefix + "components.1.component.tint";

				parent_call = lookup_call("components.0.component", cm, parent_call.get());
				semantic = get_call_semantic(transaction, parent_call.get());
				path_prefix += "components.0.component.";
			}

			if (semantic == mi::neuraylib::IFunction_definition::DS_INTRINSIC_DF_CUSTOM_CURVE_LAYER)
			{
				out_material["specular"].bake_path = path_prefix + "weight";
				if (is_transmissive_pbr)
				{
					out_material["roughness"].bake_path = path_prefix + "layer.roughness_u.s.r.roughness";
					out_material["anisotropy"].bake_path = path_prefix + "layer.roughness_u.s.r.anisotropy";
					out_material["anisotropy_rotation"].bake_path = path_prefix + "layer.roughness_u.s.r.rotation";
				}
				else
				{
					out_material["roughness"].bake_path = path_prefix + "layer.roughness_u";
				}

				parent_call = lookup_call("base", cm, parent_call.get());
				semantic = get_call_semantic(transaction, parent_call.get());
				path_prefix += "base.";
			}

			if (semantic == mi::neuraylib::IFunction_definition::DS_INTRINSIC_DF_NORMALIZED_MIX)
			{
				out_material["transparency"].bake_path = path_prefix + "components.1.weight";
				out_material["transmission_color"].bake_path = path_prefix + "components.1.component.tint";

				parent_call = lookup_call("components.0.component", cm, parent_call.get());
				semantic = get_call_semantic(transaction, parent_call.get());
				path_prefix += "components.0.component.";
			}
			if (semantic == mi::neuraylib::IFunction_definition::DS_INTRINSIC_DF_MICROFACET_GGX_VCAVITIES_BSDF)
			{
				if (out_material["metallic"].bake_path.empty())
					out_material["metallic"].value = create_value(transaction, "Float32", 1.0f);
				if (out_material["roughness"].bake_path.empty())
					out_material["roughness"].bake_path = path_prefix + "roughness_u";
				if (out_material["base_color"].bake_path.empty())
					out_material["base_color"].bake_path = path_prefix + "tint";
			}
			else if (semantic == mi::neuraylib::IFunction_definition::DS_INTRINSIC_DF_DIFFUSE_REFLECTION_BSDF)
			{
				if (out_material["base_color"].bake_path.empty())
					out_material["base_color"].bake_path = path_prefix + "tint";
			}

			mi::base::Handle<const mi::neuraylib::IExpression> cutout(cm->lookup_sub_expression("geometry.cutout_opacity"));
			if (cutout.is_valid_interface())
				out_material["opacity"].bake_path = "geometry.cutout_opacity";
		}
		else if (target_model == "specular_glossy")
		{
			out_material["base_color"] = Material_parameter("Rgb_fp");
			out_material["f0"] = Material_parameter("Rgb_fp");
			out_material["f0_color"] = Material_parameter("Rgb_fp");
			out_material["f0_refl"] = Material_parameter("Float32");
			out_material["f0_weight"] = Material_parameter("Float32");
			out_material["glossiness"] = Material_parameter("Float32", rough_to_gloss);
			out_material["opacity"] = Material_parameter("Float32");
			out_material["normal_map"] = Material_parameter("Float32<3>", remap_normal);

			switch (semantic)
			{
			case mi::neuraylib::IFunction_definition::DS_INTRINSIC_DF_DIFFUSE_REFLECTION_BSDF:
				out_material["base_color"].bake_path = "surface.scattering.tint";
				out_material["f0_weight"].value = create_value(transaction, "Float32", 0.0f);
				out_material["f0_color"].value = create_value(transaction, "Color", mi::Color(0.0f));
				break;
			case mi::neuraylib::IFunction_definition::DS_INTRINSIC_DF_MICROFACET_GGX_VCAVITIES_BSDF:
				out_material["f0_color"].bake_path = "surface.scattering.tint";
				out_material["f0_refl"].value = create_value(transaction, "Float32", 1.0f);
				out_material["f0_weight"].value = create_value(transaction, "Float32", 1.0f);
				out_material["glossiness"].bake_path = "surface.scattering.roughness_u"; // needs inversion
				break;
			case mi::neuraylib::IFunction_definition::DS_INTRINSIC_DF_CUSTOM_CURVE_LAYER:
				out_material["base_color"].bake_path = "surface.scattering.base.tint";
				out_material["f0_color"].bake_path = "surface.scattering.layer.tint";
				out_material["f0_refl"].bake_path = "surface.scattering.normal_reflectivity";
				out_material["f0_weight"].bake_path = "surface.scattering.weight";
				out_material["glossiness"].bake_path = "surface.scattering.layer.roughness_u"; // needs inversion

				break;
			default:
				break;
			}

			out_material["normal_map"].bake_path = "geometry.normal";
			out_material["opacity"].bake_path = "geometry.cutout_opacity";
		}
	}

	void bake_target_material_inputs(
		mi::neuraylib::Baker_resource baker_resource,
		mi::Uint32 baking_samples,
		mi::Uint32 baking_resolution,
		mi::neuraylib::ITransaction* transaction,
		const mi::neuraylib::ICompiled_material* cm,
		mi::neuraylib::IMdl_distiller_api* distiller_api,
		mi::neuraylib::IImage_api* image_api,
		Material& out_material)
	{
		for (Material::iterator it = out_material.begin(); it != out_material.end(); ++it)
		{
			Material_parameter& param = it->second;

			// Do not attempt to bake empty paths
			if (param.bake_path.empty())
				continue;

			// Create baker for current path
			mi::base::Handle<const mi::neuraylib::IBaker> baker(distiller_api->create_baker(cm, param.bake_path.c_str(), baker_resource));
			if (!baker.is_valid_interface())
				continue;

			if (baker->is_uniform())
			{
				mi::base::Handle<mi::IData> value;
				if (param.value_type == "Rgb_fp")
				{
					mi::base::Handle<mi::IColor> v(
						transaction->create<mi::IColor>());
					value = v->get_interface<mi::IData>();
				}
				else if (param.value_type == "Float32<3>")
				{
					mi::base::Handle<mi::IFloat32_3> v(
						transaction->create<mi::IFloat32_3>());
					value = v->get_interface<mi::IData>();
				}
				else if (param.value_type == "Float32")
				{
					mi::base::Handle<mi::IFloat32> v(
						transaction->create<mi::IFloat32>());
					value = v->get_interface<mi::IData>();
				}
				else
				{
					std::cout << "Ignoring unsupported value type '" << param.value_type
						<< "'" << std::endl;
					continue;
				}

				// Bake constant value
				mi::Sint32 result = baker->bake_constant(value.get());

				if (param.remap_func)
					param.remap_func(value.get());

				param.value = value;
			}
			else
			{
				// Create a canvas
				mi::base::Handle<mi::neuraylib::ICanvas> canvas(image_api->create_canvas(param.value_type.c_str(), baking_resolution, baking_resolution));
				// Bake texture
				mi::Sint32 result = baker->bake_texture(canvas.get(), baking_samples);

				if (param.remap_func)
					param.remap_func(canvas.get());

				param.texture = canvas;
			}
		}
	}
	class Canvas_exporter
	{
		bool m_in_parallel = true;
		std::map<std::string/*filename*/, mi::base::Handle<const mi::neuraylib::ICanvas>> m_canvases;

	public:
		Canvas_exporter(bool parallel)
			: m_in_parallel(parallel)
		{
		}
		void add_canvas(const std::string& filename, const mi::neuraylib::ICanvas* canvas)
		{
			m_canvases[filename] = mi::base::make_handle_dup(canvas);
		}

		void do_export(mi::neuraylib::IMdl_impexp_api* mdl_impexp_api)
		{
			auto export_canvas = [mdl_impexp_api](const char* filename, const mi::neuraylib::ICanvas* canvas)
			{
				mdl_impexp_api->export_canvas(filename, canvas) ;
			};
			std::vector<std::thread> threads;

			for (auto& canvas_file : m_canvases)
			{
				const char* filename(canvas_file.first.c_str());
				const mi::neuraylib::ICanvas* canvas(canvas_file.second.get());
				if (m_in_parallel)
				{
					threads.emplace_back(
						std::thread(export_canvas, filename, canvas)
					);
				}
				else
				{
					export_canvas(filename, canvas);
				}
			}
			for (auto& t : threads)
			{
				t.join();
			}
		}
	};
	void process_target_material(
		const std::string& material_name,
		const Material& material,
		bool save_baked_textures,
		bool parallel,
		mi::neuraylib::IMdl_impexp_api* mdl_impexp_api)
	{
		Canvas_exporter canvas_exporter(parallel);

		for (Material::const_iterator it = material.begin(); it != material.end(); ++it)
		{
			const std::string& param_name = it->first;
			const Material_parameter& param = it->second;

			std::cout << "Parameter: '" << param_name << "': ";
			if (param.bake_path.empty())
			{
				std::cout << " no matching bake path found in target material." << std::endl;

				if (param.value)   std::cout << "--> value set to ";
				if (param.texture) std::cout << "--> calculated ";
			}
			else
			{
				std::cout << "path '" << param.bake_path << "' baked to ";
			}

			if (param.texture)
			{
				std::cout << "texture." << std::endl << std::endl;

				// write texture to disc
				std::stringstream file_name;
				file_name << material_name << "-" << param_name << ".png";

				if (save_baked_textures)
					canvas_exporter.add_canvas(file_name.str(), param.texture.get());
			}
			else if (param.value)
			{
				std::cout << "constant ";

				if (param.value_type == "Rgb_fp")
				{
					mi::base::Handle<mi::IColor> color(param.value->get_interface<mi::IColor>());
					mi::Color c;
					color->get_value(c);
					std::cout << "color (" << c.r << ", " << c.g << ", " << c.b << ")." << std::endl;
				}
				else if (param.value_type == "Float32")
				{
					mi::base::Handle<mi::IFloat32> value(param.value->get_interface<mi::IFloat32>());
					mi::Float32 v;
					value->get_value(v);
					std::cout << "float " << v << "." << std::endl;
				}
				else if (param.value_type == "Float32<3>")
				{
					mi::base::Handle<mi::IFloat32_3> value(param.value->get_interface<mi::IFloat32_3>());
					mi::Float32_3 v;
					value->get_value(v);
					std::cout << "vector (" << v.x << ", " << v.y << ", " << v.z << ")." << std::endl;
				}
			}
		}

		canvas_exporter.do_export(mdl_impexp_api);
	}

	class MDLContext
	{
	public:
		mi::base::Handle<mi::neuraylib::INeuray> neuray;
		mi::base::Handle<mi::neuraylib::IDatabase> database;
		mi::base::Handle<mi::neuraylib::IScope> globalScope;
		mi::base::Handle<mi::neuraylib::IImage_api> imageAPI;
		mi::base::Handle<mi::neuraylib::ITransaction> transaction;
		mi::base::Handle<mi::neuraylib::IMdl_impexp_api> impexp_api;
		mi::base::Handle<mi::neuraylib::IMdl_factory> factory;
		mi::base::Handle<mi::neuraylib::IMdl_execution_context> executionContext;
		mi::base::Handle<mi::neuraylib::IMdl_distiller_api> distilling;
		mi::base::Handle<mi::neuraylib::IMdl_configuration> config;

		static inline mi::neuraylib::INeuray* load_and_get_ineuray(const char* filename = 0)
		{
			static void* g_dso_handle = 0;

			if (!filename)
				filename = "libmdl_sdk" MI_BASE_DLL_FILE_EXT;

#ifdef MI_PLATFORM_WINDOWS
			void* handle = LoadLibraryA((LPSTR)filename);
			if (!handle) {
				LPTSTR buffer = 0;
				LPCTSTR message = TEXT("unknown failure");
				DWORD error_code = GetLastError();
				if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
					FORMAT_MESSAGE_IGNORE_INSERTS, 0, error_code,
					MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&buffer, 0, 0))
					message = buffer;
				fprintf(stderr, "Failed to load library (%u): " FMT_LPTSTR, error_code, message);
				if (buffer)
					LocalFree(buffer);
				return 0;
			}
			void* symbol = GetProcAddress((HMODULE)handle, "mi_factory");
			if (!symbol) {
				LPTSTR buffer = 0;
				LPCTSTR message = TEXT("unknown failure");
				DWORD error_code = GetLastError();
				if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
					FORMAT_MESSAGE_IGNORE_INSERTS, 0, error_code,
					MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&buffer, 0, 0))
					message = buffer;
				fprintf(stderr, "GetProcAddress error (%u): " FMT_LPTSTR, error_code, message);
				if (buffer)
					LocalFree(buffer);
				return 0;
			}
#else // MI_PLATFORM_WINDOWS
			void* handle = dlopen(filename, RTLD_LAZY);
			if (!handle) {
				fprintf(stderr, "%s\n", dlerror());
				return 0;
			}
			void* symbol = dlsym(handle, "mi_factory");
			if (!symbol) {
				fprintf(stderr, "%s\n", dlerror());
				return 0;
			}
#endif // MI_PLATFORM_WINDOWS
			g_dso_handle = handle;

			mi::neuraylib::INeuray* neuray = mi::neuraylib::mi_factory<mi::neuraylib::INeuray>(symbol);
			if (!neuray)
			{
				mi::base::Handle<mi::neuraylib::IVersion> version(
					mi::neuraylib::mi_factory<mi::neuraylib::IVersion>(symbol));
				if (!version)
					fprintf(stderr, "Error: Incompatible library.\n");
				else
					fprintf(stderr, "Error: Library version %s does not match header version %s.\n",
						version->get_product_version(), MI_NEURAYLIB_PRODUCT_VERSION_STRING);
				return 0;
			}
			return neuray;
		}
	};

	class DefaultLogger : public mi::base::Interface_implement<mi::base::ILogger>
	{
	public:
		void message(mi::base::Message_severity level, const char* /*module_category*/, const mi::base::Message_details& /*details*/, const char* message)
		{
			const char* severity = 0;
			switch (level) {
			case mi::base::MESSAGE_SEVERITY_FATAL:        severity = "fatal: "; break;
			case mi::base::MESSAGE_SEVERITY_ERROR:        severity = "error: "; break;
			case mi::base::MESSAGE_SEVERITY_WARNING:      severity = "warn:  "; break;
			case mi::base::MESSAGE_SEVERITY_INFO:         severity = "info:  "; break;
			case mi::base::MESSAGE_SEVERITY_VERBOSE:      return;
			case mi::base::MESSAGE_SEVERITY_DEBUG:        return;
			case mi::base::MESSAGE_SEVERITY_FORCE_32_BIT: return;
			}

			fprintf(stderr, "%s", severity);
			fprintf(stderr, "%s", message);
			putc('\n', stderr);

#ifdef MI_PLATFORM_WINDOWS
			fflush(stderr);
#endif
		}
	};

	mi::neuraylib::ICompiled_material* compile_material_instance(const mi::neuraylib::IMaterial_instance* material_instance, mi::neuraylib::IMdl_execution_context* context, bool class_compilation)
	{
		mi::Uint32 flags = class_compilation ? mi::neuraylib::IMaterial_instance::CLASS_COMPILATION : mi::neuraylib::IMaterial_instance::DEFAULT_OPTIONS;
		mi::neuraylib::ICompiled_material* compiled_material = material_instance->create_compiled_material(flags, context);

		return compiled_material;
	}

	const mi::neuraylib::ICompiled_material* create_distilled_material(mi::neuraylib::IMdl_distiller_api* distiller_api, const mi::neuraylib::ICompiled_material* compiled_material, std::string_view target_model)
	{
		mi::Sint32 result = 0;
		auto distilled_material = mi::base::make_handle(distiller_api->distill_material(compiled_material, std::string(target_model).c_str(), nullptr, &result));

		distilled_material->retain();
		return distilled_material.get();
	}

	MDLLoader::MDLLoader() noexcept(false)
		: context_(std::make_unique<MDLContext>())
		, verboseLogging_(false)
	{
		this->context_->neuray = MDLContext::load_and_get_ineuray();
		if (!this->context_->neuray.is_valid_interface())
			throw std::runtime_error("Starting MDL neuray failed");

		mi::base::Handle<mi::base::ILogger> logger(new DefaultLogger());

		this->context_->config = this->context_->neuray->get_api_component<mi::neuraylib::IMdl_configuration>();
		this->context_->config->set_logger(logger.get());
		this->context_->config->add_mdl_system_paths();
		this->context_->config->add_mdl_user_paths();

		mi::base::Handle<mi::neuraylib::IPlugin_configuration> plug_config(this->context_->neuray->get_api_component<mi::neuraylib::IPlugin_configuration>());
		if (plug_config->load_plugin_library("nv_freeimage" MI_BASE_DLL_FILE_EXT) != 0)
			throw std::runtime_error("Fatal: Failed to load the nv_freeimage plugin.\n");

		if (this->context_->neuray->start() < 0)
			throw std::runtime_error("Starting MDL SDK failed");

		this->context_->impexp_api = this->context_->neuray->get_api_component<mi::neuraylib::IMdl_impexp_api>();
		this->context_->database = this->context_->neuray->get_api_component<mi::neuraylib::IDatabase>();
		this->context_->globalScope = this->context_->database->get_global_scope();
		this->context_->factory = this->context_->neuray->get_api_component<mi::neuraylib::IMdl_factory>();
		this->context_->executionContext = this->context_->factory->create_execution_context();
		this->context_->imageAPI = this->context_->neuray->get_api_component<mi::neuraylib::IImage_api>();
		this->context_->distilling = this->context_->neuray->get_api_component<mi::neuraylib::IMdl_distiller_api>();
		this->context_->transaction = this->context_->globalScope->create_transaction();
	}

	MDLLoader::~MDLLoader() noexcept
	{
		this->context_->imageAPI.reset();
		this->context_->executionContext.reset();
		this->context_->factory.reset();
		this->context_->globalScope.reset();
		this->context_->database.reset();
		this->context_->impexp_api.reset();
		this->context_->neuray->shutdown();
	}

	bool
	MDLLoader::doCanRead(io::istream& stream) noexcept
	{
		return false;
	}

	bool
	MDLLoader::doCanRead(const char* type) noexcept
	{
		return false;
	}

	void
	MDLLoader::load(std::string_view moduleName, io::istream& stream) noexcept(false)
	{
		auto streamsize = stream.size();
		if (streamsize > 0)
		{
			const std::string mdlSource(streamsize, '0');
			stream.read((char*)mdlSource.data(), mdlSource.size());

			this->load(moduleName, mdlSource);
		}
	}

	void
	MDLLoader::load(std::string_view moduleName, std::string_view source, std::string_view target_model) noexcept(false)
	{
		mi::neuraylib::Baker_resource   baker_resource = mi::neuraylib::BAKE_ON_CPU;
		mi::Uint32                      baking_samples = 4;
		mi::Uint32                      baking_resolution = 1024;
		bool save_baked_textures = true;
		bool                            parallel = true;

		this->materials_.clear();

		auto mdlModuleName = moduleName.find("::") == 0 ? std::string(moduleName) : "::" + std::string(moduleName);

		if (source.empty())
		{
			if (this->context_->impexp_api->load_module(this->context_->transaction.get(), mdlModuleName.c_str()) < 0)
				throw std::runtime_error("Loading \"" + std::string(moduleName) + ".mdl\" failed");
		}
		else
		{
			if (this->context_->impexp_api->load_module_from_string(this->context_->transaction.get(), mdlModuleName.c_str(), std::string(source).c_str()) < 0)
				throw std::runtime_error("Loading \"" + std::string(moduleName) + "\" from memory failed");
		}

		auto module = mi::base::Handle<const mi::neuraylib::IModule>(this->context_->transaction->access<mi::neuraylib::IModule>(("mdl" + std::string(mdlModuleName)).c_str()));
		mi::Size material_count = module->get_material_count();
		if (this->verboseLogging_)
			std::cout << "The module contains the following material definitions:" << std::endl;

		for (mi::Size i = 0; i < material_count; i++)
		{
			auto material_db_name = std::string(module->get_material(i));
			auto name = material_db_name.substr(material_db_name.rfind(':') + 1);
			auto material = std::make_shared<material::MeshStandardMaterial>(name);

			auto materialDefinition = mi::base::make_handle(this->context_->transaction->access<mi::neuraylib::IMaterial_definition>(material_db_name.c_str()));
			if (materialDefinition)
			{
				mi::Sint32 errors;
				auto materialInstance = mi::base::make_handle(materialDefinition->create_material_instance(0, &errors));
				check_success(errors, "Creating material instance failed");

				if (!target_model.empty())
				{
					auto compiled_material = mi::base::make_handle(compile_material_instance(materialInstance.get(), this->context_->executionContext.get(), false));
					auto distilled_material = mi::base::make_handle(create_distilled_material(this->context_->distilling.get(), compiled_material.get(), target_model));

					Material out_material;
					setup_target_material(
						target_model,
						this->context_->transaction.get(),
						distilled_material.get(),
						out_material);

					bake_target_material_inputs(
						baker_resource,
						baking_samples,
						baking_resolution,
						this->context_->transaction.get(),
						distilled_material.get(),
						this->context_->distilling.get(),
						this->context_->imageAPI.get(),
						out_material
					);

					process_target_material(
						name,
						out_material,
						save_baked_textures,
						parallel,
						this->context_->impexp_api.get());

					for (auto& it : out_material)
					{
						auto& param_name = it.first;
						auto& param = it.second;

						if (param_name == "base_color")
						{
							if (param.texture)
							{
								std::stringstream file_name;
								file_name << name << "-" << param_name << ".png";
								material->setColorMap(octoon::TextureLoader::load(file_name.str()));
							}
							else
							{
								mi::base::Handle<mi::IColor> color(param.value->get_interface<mi::IColor>());
								mi::Color c;
								color->get_value(c);
								material->setColor(math::float3(c.r, c.g, c.b));
							}
						}
						else if (param_name == "opacity")
						{
							if (param.value)
							{
								mi::base::Handle<mi::IFloat32> value(param.value->get_interface<mi::IFloat32>());
								mi::Float32 v;
								value->get_value(v);
								material->setOpacity(v);
							}
						}
						else if (param_name == "normal")
						{
							if (param.texture)
							{
								std::stringstream file_name;
								file_name << name << "-" << param_name << ".png";
								material->setNormalMap(octoon::TextureLoader::load(file_name.str()));
							}
						}
						else if (param_name == "roughness")
						{
							if (param.texture)
							{
								std::stringstream file_name;
								file_name << name << "-" << param_name << ".png";
								material->setRoughnessMap(octoon::TextureLoader::load(file_name.str()));
							}
							else if (param.value)
							{
								mi::base::Handle<mi::IFloat32> value(param.value->get_interface<mi::IFloat32>());
								mi::Float32 v;
								value->get_value(v);
								material->setSmoothness(v);
							}
						}
						else if (param_name == "metallic")
						{
							if (param.texture)
							{
								std::stringstream file_name;
								file_name << name << "-" << param_name << ".png";
								material->setMetalnessMap(octoon::TextureLoader::load(file_name.str()));
							}
							else if (param.value)
							{
								mi::base::Handle<mi::IFloat32> value(param.value->get_interface<mi::IFloat32>());
								mi::Float32 v;
								value->get_value(v);
								material->setMetalness(v);
							}
						}
						else if (param_name == "anisotropy")
						{
							if (param.texture)
							{
								std::stringstream file_name;
								file_name << name << "-" << param_name << ".png";
								material->setAnisotropyMap(octoon::TextureLoader::load(file_name.str()));
							}
							else if (param.value)
							{
								mi::base::Handle<mi::IFloat32> value(param.value->get_interface<mi::IFloat32>());
								mi::Float32 v;
								value->get_value(v);
								material->setAnisotropy(v);
							}
						}
						else if (param_name == "specular")
						{
							if (param.texture)
							{
								std::stringstream file_name;
								file_name << name << "-" << param_name << ".png";
								material->setSpecularMap(octoon::TextureLoader::load(file_name.str()));
							}
							else if (param.value)
							{
								mi::base::Handle<mi::IFloat32> value(param.value->get_interface<mi::IFloat32>());
								mi::Float32 v;
								value->get_value(v);
								material->setSpecular(v);
							}
						}
						else if (param_name == "clearcoat_weight")
						{
							if (param.texture)
							{
								std::stringstream file_name;
								file_name << name << "-" << param_name << ".png";
								material->setClearCoatMap(octoon::TextureLoader::load(file_name.str()));
							}
							else if (param.value)
							{
								mi::base::Handle<mi::IFloat32> value(param.value->get_interface<mi::IFloat32>());
								mi::Float32 v;
								value->get_value(v);
								material->setClearCoat(v);
							}
						}
						else if (param_name == "clearcoat_roughness")
						{
							if (param.texture)
							{
								std::stringstream file_name;
								file_name << name << "-" << param_name << ".png";
								material->setClearCoatRoughnessMap(octoon::TextureLoader::load(file_name.str()));
							}
							else if (param.value)
							{
								mi::base::Handle<mi::IFloat32> value(param.value->get_interface<mi::IFloat32>());
								mi::Float32 v;
								value->get_value(v);
								material->setClearCoatRoughness(v);
							}
						}
						else if (param_name == "emission")
						{
							if (param.texture)
							{
								std::stringstream file_name;
								file_name << name << "-" << param_name << ".png";
								material->setEmissiveMap(octoon::TextureLoader::load(file_name.str()));
							}
							else if (param.value)
							{
								mi::base::Handle<mi::IColor> color(param.value->get_interface<mi::IColor>());
								mi::Color c;
								color->get_value(c);
								material->setEmissive(math::float3(c.r, c.g, c.b));
							}
						}
						else if (param_name == "emission_intensity")
						{
							if (param.value)
							{
								mi::base::Handle<mi::IFloat32> value(param.value->get_interface<mi::IFloat32>());
								mi::Float32 v;
								value->get_value(v);
								material->setEmissiveIntensity(v);
							}
						}
					}

					this->materials_.emplace_back(std::move(material));
				}
				else
				{ 
					mi::neuraylib::Argument_editor editor(this->context_->transaction.get(), std::string(name).c_str(), this->context_->factory.get());

					if (this->verboseLogging_)
						std::cout << "[MDL]  - Parameters: " << editor.get_parameter_count() << std::endl;

					for (mi::Size i = 0; i < editor.get_parameter_count(); ++i)
					{
						const char* name = editor.get_parameter_name(i);
						auto types = mi::base::Handle<const mi::neuraylib::IType_list>(editor.get_parameter_types());
						auto type = mi::base::Handle<const mi::neuraylib::IType>(types->get_type(name));
						auto resolvedType = mi::base::Handle<const mi::neuraylib::IType>(type->skip_all_type_aliases());
						auto kind = resolvedType->get_kind();

						switch (kind)
						{
						case mi::neuraylib::IType::Kind::TK_BOOL:
						{
							bool value;
							editor.get_value(name, value);
							material->set(name, value);
						}
						break;
						case mi::neuraylib::IType::Kind::TK_FLOAT:
						{
							float value;
							editor.get_value(name, value);
							material->set(name, value);
						}
						break;
						case mi::neuraylib::IType::Kind::TK_DOUBLE:
						{
							double value;
							editor.get_value(name, value);
							material->set(name, (float)value);
						}
						break;
						case mi::neuraylib::IType::Kind::TK_INT:
						{
							int value;
							editor.get_value(name, value);
							material->set(name, value);
						}
						break;
						case mi::neuraylib::IType::Kind::TK_TEXTURE:
						{
							int value;
							editor.get_value(name, value);
							material->set(name, value);
						}
						break;
						case mi::neuraylib::IType::Kind::TK_COLOR:
						{
							mi::math::Color value;
							editor.get_value(name, value);
							if (std::string_view(name) == "base_color")
								material->setColor(math::float3(value.r, value.g, value.b));
							else
								material->set(name, math::float4(value.r, value.g, value.b, value.a));
						}
						break;
						default:
							if (this->verboseLogging_)
								std::cout << "[MDL] Warning: Unhandled parameter type: " << (int)kind << std::endl;
							break;
						}

						if (this->verboseLogging_)
						{
							auto TypeToString = [](mi::neuraylib::IType::Kind kind) -> std::string
							{
								switch (kind)
								{
								case mi::neuraylib::IType::Kind::TK_BOOL:
									return "bool";
								case mi::neuraylib::IType::Kind::TK_COLOR:
									return "color";
								case mi::neuraylib::IType::Kind::TK_DOUBLE:
									return "double";
								case mi::neuraylib::IType::Kind::TK_FLOAT:
									return "float";
								case mi::neuraylib::IType::Kind::TK_INT:
									return "int";
								case mi::neuraylib::IType::Kind::TK_TEXTURE:
									return "texture";
								default:
									return "other";
								}
							};

							std::cout << "[MDL]      " << name << " (type: " << TypeToString(kind) << ")" << std::endl;
						}
					}

					this->materials_.emplace_back(std::move(material));
				}
			}
			else
			{
				if (this->verboseLogging_)
					std::cout << "[MDL] Loading material definition for \"" << material_db_name << "\" from database failed" << std::endl;
			}
		}

		module.reset();
		this->context_->transaction->commit();
	}

	void
	MDLLoader::save(io::ostream& stream, const material::MeshStandardMaterial& animation) noexcept(false)
	{
	}

	const std::vector<std::shared_ptr<material::MeshStandardMaterial>>&
	MDLLoader::getMaterials() const noexcept
	{
		return this->materials_;
	}

	void
	MDLLoader::addModulePath(std::string_view modulePath)
	{
		if (modulePath.empty())
			return;

		std::string path(modulePath);

		if (this->modulePaths_.find(path) == this->modulePaths_.end())
		{
			this->context_->config->add_mdl_path(path.c_str());
			this->context_->config->add_resource_path(path.c_str());
			this->modulePaths_.insert(path);
		}
	}

	std::string
	MDLLoader::preprocessSource(std::string_view source)
	{
		std::set<std::string> textureVariables;
		std::regex textureDeclareRegex("uniform.*?texture_2d.*?((?:[a-zA-Z][a-zA-Z0-9_]*)).*?=.*?texture_2d.*?\\(\\).*?(\\[\\[anno::unused\\(\\)\\]\\])?", std::regex::icase);

		std::string phase1 = std::regex_replace(std::string(source), textureDeclareRegex, [&](const std::smatch& match)
		{
			if (match.size() >= 2)
			{
				const std::string var = match[1];
				textureVariables.insert(var);

				bool unused = false;
				if (match.size() == 3)
				{
					const std::string anno = match[2];
					if (anno.find("anno::unused") != std::string::npos)
						unused = true;
				}

				if (unused)
					return "uniform bool " + MDL_USE_TEXTURE_PREFIX + var + " = false [[anno::unused()]], " + match.str();
				else
					return "uniform bool " + MDL_USE_TEXTURE_PREFIX + var + " = false, " + match.str();
			}

			return match.str();
		});

		std::regex textureIsValidRegex("tex::texture_isvalid.*?\\(.*?((?:[a-zA-Z][a-zA-Z0-9_]*)).*?\\)", std::regex::icase);

		std::string phase2 = std::regex_replace(phase1, textureIsValidRegex, [&](const std::smatch& match)
		{
			if (match.size() == 2)
			{
				const std::string var = match[1];

				if (textureVariables.find(var) != textureVariables.end())
					return MDL_USE_TEXTURE_PREFIX + var;
			}

			return match.str();
		});

		return phase2;
	}
}