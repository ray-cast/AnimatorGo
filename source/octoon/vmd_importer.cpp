#include <octoon/vmd_importer.h>
#include <octoon/asset_database.h>
#include <octoon/math/vector2.h>
#include <octoon/math/vector3.h>
#include <octoon/math/vector4.h>
#include <octoon/math/quat.h>
#include <octoon/runtime/except.h>
#include <octoon/runtime/string.h>
#include <octoon/animation/path_interpolator.h>
#include <octoon/io/fstream.h>
#include <iconv.h>
#include <map>
#include <fstream>

namespace octoon
{
	OctoonImplementSubClass(VMDImporter, AssetImporter, "VMDImporter")

	std::string sjis2utf8(const std::string& sjis)
	{
		std::size_t in_size = sjis.size();
		std::size_t out_size = sjis.size() * 2;

		auto inbuf = std::make_unique<char[]>(in_size);
		auto outbuf = std::make_unique<char[]>(out_size);
		char* in = inbuf.get();
		char* out = outbuf.get();

		std::memcpy(in, sjis.c_str(), in_size);

		iconv_t ic = nullptr;

		try
		{
			ic = iconv_open("utf-8", "SJIS");
			iconv(ic, &in, &in_size, &out, &out_size);
			iconv_close(ic);

			return std::string(outbuf.get());
		}
		catch (const std::exception&)
		{
			iconv_close(ic);
		}

		return std::string();
	}

	std::string utf82sjis(const std::string& utf8)
	{
		std::size_t in_size = utf8.size();
		std::size_t out_size = utf8.size() * 2;

		auto inbuf = std::make_unique<char[]>(in_size);
		auto outbuf = std::make_unique<char[]>(out_size);
		char* in = inbuf.get();
		char* out = outbuf.get();

		std::memcpy(in, utf8.c_str(), in_size);

		iconv_t ic = nullptr;

		try
		{
			ic = iconv_open("SJIS", "utf-8");
			iconv(ic, &in, &in_size, &out, &out_size);
			iconv_close(ic);

			return std::string(outbuf.get());
		}
		catch (const std::exception&)
		{
			iconv_close(ic);
		}

		return std::string();
	}

	void
	VMD::load(std::istream& stream) noexcept(false)
	{
		static_assert(sizeof(VMDMotion) == 111, "");
		static_assert(sizeof(VMDMorph) == 23, "");
		static_assert(sizeof(VMDCamera) == 61, "");
		static_assert(sizeof(VMDLight) == 28, "");
		static_assert(sizeof(VMDSelfShadow) == 9, "");

		if (!stream.read((char*)&this->Header, sizeof(this->Header))) {
			throw runtime_error::create(R"(Cannot read property "Header" from stream)");
		}

		if (std::strncmp(this->Header.magic, "Vocaloid Motion", 30) != 0 && std::strncmp(this->Header.magic, "Vocaloid Motion Data 0002", 30) != 0)
			throw runtime_error::create(R"(Invalid Magic Token)");

		if (!stream.read((char*)&this->NumMotion, sizeof(this->NumMotion))) {
			throw runtime_error::create(R"(Cannot read property "NumMotion" from stream)");
		}

		if (this->NumMotion > 0)
		{
			this->MotionLists.resize(this->NumMotion);

			if (!stream.read((char*)this->MotionLists.data(), sizeof(VMDMotion) * this->NumMotion)) {
				throw runtime_error::create(R"(Cannot read property "VMDMotion" from stream)");
			}
		}

		if (!stream.read((char*)&this->NumMorph, sizeof(this->NumMorph))) {
			throw runtime_error::create(R"(Cannot read property "NumMorph" from stream)");
		}

		if (this->NumMorph > 0)
		{
			this->MorphLists.resize(this->NumMorph);

			if (!stream.read((char*)this->MorphLists.data(), sizeof(VMDMorph) * this->NumMorph)) {
				throw runtime_error::create(R"(Cannot read property "VMDMorph" from stream)");
			}
		}

		if (!stream.read((char*)&this->NumCamera, sizeof(this->NumCamera))) {
			throw runtime_error::create(R"(Cannot read property "NumCamera" from stream)");
		}

		if (this->NumCamera > 0)
		{
			this->CameraLists.resize(this->NumCamera);

			if (!stream.read((char*)this->CameraLists.data(), sizeof(VMDCamera) * this->NumCamera)) {
				throw runtime_error::create(R"(Cannot read property "VMDCamera" from stream)");
			}
		}

		if (!stream.read((char*)&this->NumLight, sizeof(this->NumLight))) {
			throw runtime_error::create(R"(Cannot read property "NumLight" from stream)");
		}

		if (this->NumLight > 0)
		{
			this->LightLists.resize(this->NumLight);

			if (!stream.read((char*)this->LightLists.data(), sizeof(VMDLight) * this->NumLight)) {
				throw runtime_error::create(R"(Cannot read property "VMDLight" from stream)");
			}
		}

		if (!stream.read((char*)&this->NumSelfShadow, sizeof(this->NumSelfShadow))) {
			throw runtime_error::create(R"(Cannot read property "NumSelfShadow" from stream)");
		}

		if (this->NumSelfShadow > 0)
		{
			this->SelfShadowLists.resize(this->NumSelfShadow);

			if (!stream.read((char*)this->SelfShadowLists.data(), sizeof(VMDSelfShadow) * this->NumSelfShadow)) {
				throw runtime_error::create(R"(Cannot read property "VMDSelfShadow" from stream)");
			}
		}
	}

	void
	VMD::save(std::ostream& stream) noexcept(false)
	{
		if (!stream.write((char*)&this->Header, sizeof(this->Header))) {
			throw runtime_error::create(R"(Cannot write property "Header" from stream)");
		}

		if (!stream.write((char*)&this->NumMotion, sizeof(this->NumMotion))) {
			throw runtime_error::create(R"(Cannot write property "NumMotion" from stream)");
		}

		if (this->NumMotion > 0 && this->MotionLists.size() > 0)
		{
			if (!stream.write((char*)this->MotionLists.data(), sizeof(VMDMotion) * this->NumMotion)) {
				throw runtime_error::create(R"(Cannot write property "VMDMotion" from stream)");
			}
		}

		if (!stream.write((char*)&this->NumMorph, sizeof(this->NumMorph))) {
			throw runtime_error::create(R"(Cannot write property "NumMorph" from stream)");
		}

		if (this->NumMorph > 0 && this->MorphLists.size() > 0)
		{
			if (!stream.write((char*)this->MorphLists.data(), sizeof(VMDMorph) * this->NumMorph)) {
				throw runtime_error::create(R"(Cannot write property "VMDMorph" from stream)");
			}
		}

		if (!stream.write((char*)&this->NumCamera, sizeof(this->NumCamera))) {
			throw runtime_error::create(R"(Cannot write property "NumCamera" from stream)");
		}

		if (this->NumCamera > 0 && this->CameraLists.size() > 0)
		{
			if (!stream.write((char*)this->CameraLists.data(), sizeof(VMDCamera) * this->NumCamera)) {
				throw runtime_error::create(R"(Cannot write property "VMDCamera" from stream)");
			}
		}

		if (!stream.write((char*)&this->NumLight, sizeof(this->NumLight))) {
			throw runtime_error::create(R"(Cannot write property "NumLight" from stream)");
		}

		if (this->NumLight > 0 && this->LightLists.size() > 0)
		{
			if (!stream.write((char*)this->LightLists.data(), sizeof(VMDLight) * this->NumLight)) {
				throw runtime_error::create(R"(Cannot write property "VMDLight" from stream)");
			}
		}

		if (!stream.write((char*)&this->NumSelfShadow, sizeof(this->NumSelfShadow))) {
			throw runtime_error::create(R"(Cannot write property "NumSelfShadow" from stream)");
		}

		if (this->NumSelfShadow > 0 && this->SelfShadowLists.size() > 0)
		{
			if (!stream.write((char*)this->SelfShadowLists.data(), sizeof(VMDSelfShadow) * this->NumSelfShadow)) {
				throw runtime_error::create(R"(Cannot write property "VMDSelfShadow" from stream)");
			}
		}
	}

	bool
	VMD::load(const std::filesystem::path& filepath) noexcept(false)
	{
		std::ifstream stream(filepath, std::ios_base::binary);
		if (stream)
		{
			load(stream);
			return true;
		}

		return false;
	}

	bool
	VMD::save(const std::filesystem::path& filepath) noexcept(false)
	{
		std::ofstream stream(filepath, std::ios_base::binary);
		if (stream)
		{
			save(stream);
			return true;
		}

		return false;
	}

	VMDImporter::VMDImporter() noexcept
	{
	}

	VMDImporter::~VMDImporter() noexcept
	{
	}

	void
	VMDImporter::save(std::ostream& stream, const Animation& animation) noexcept(false)
	{
		auto sjis = utf82sjis(animation.name);

		VMD vmd;
		std::memset(&vmd.Header, 0, sizeof(vmd.Header));
		std::memcpy(&vmd.Header.magic, "Vocaloid Motion Data 0002", 15);
		std::memcpy(&vmd.Header.name, sjis.data(), sjis.size());

		vmd.NumLight = 0;
		vmd.NumCamera = 0;
		vmd.NumMorph = 0;
		vmd.NumMotion = 0;
		vmd.NumSelfShadow = 0;

		if (animation.hasClip("Motion"))
		{
			auto& clip = animation.getClip("Motion");

			for (auto& binding : clip->bindings)
			{
				auto name = utf82sjis(binding.first);
				auto& eyeX = binding.second.at("LocalPosition.x");
				auto& eyeY = binding.second.at("LocalPosition.y");
				auto& eyeZ = binding.second.at("LocalPosition.z");
				auto& rotation = binding.second.at("LocalRotation");

				for (std::size_t i = 0; i < eyeX.frames.size(); i++)
				{
					auto interpolatorEyeX = std::dynamic_pointer_cast<PathInterpolator<float>>(eyeX.frames[i].interpolator);
					auto interpolatorEyeY = std::dynamic_pointer_cast<PathInterpolator<float>>(eyeY.frames[i].interpolator);
					auto interpolatorEyeZ = std::dynamic_pointer_cast<PathInterpolator<float>>(eyeZ.frames[i].interpolator);
					auto interpolatorRotation = std::dynamic_pointer_cast<PathInterpolator<float>>(rotation.frames[i].interpolator);

					VMDMotion motion;
					std::memset(&motion, 0, sizeof(VMDMotion));
					std::memcpy(&motion.name, name.data(), name.size());
					motion.frame = (VMD_uint32_t)(eyeX.frames[i].time * 30.0f);
					motion.translate.x = eyeX.frames[i].value.getFloat();
					motion.translate.y = eyeY.frames[i].value.getFloat();
					motion.translate.z = eyeZ.frames[i].value.getFloat();
					motion.rotate = rotation.frames[i].value.getQuaternion();

					motion.interpolation_x[0] = static_cast<std::int8_t>(interpolatorEyeX->xa * 127);
					motion.interpolation_x[2] = static_cast<std::int8_t>(interpolatorEyeX->xb * 127);
					motion.interpolation_x[1] = static_cast<std::int8_t>(interpolatorEyeX->ya * 127);
					motion.interpolation_x[3] = static_cast<std::int8_t>(interpolatorEyeX->yb * 127);

					motion.interpolation_y[0] = static_cast<std::int8_t>(interpolatorEyeY->xa * 127);
					motion.interpolation_y[2] = static_cast<std::int8_t>(interpolatorEyeY->xb * 127);
					motion.interpolation_y[1] = static_cast<std::int8_t>(interpolatorEyeY->ya * 127);
					motion.interpolation_y[3] = static_cast<std::int8_t>(interpolatorEyeY->yb * 127);

					motion.interpolation_z[0] = static_cast<std::int8_t>(interpolatorEyeZ->xa * 127);
					motion.interpolation_z[2] = static_cast<std::int8_t>(interpolatorEyeZ->xb * 127);
					motion.interpolation_z[1] = static_cast<std::int8_t>(interpolatorEyeZ->ya * 127);
					motion.interpolation_z[3] = static_cast<std::int8_t>(interpolatorEyeZ->yb * 127);

					motion.interpolation_rotation[0] = static_cast<std::int8_t>(interpolatorRotation->xa * 127);
					motion.interpolation_rotation[2] = static_cast<std::int8_t>(interpolatorRotation->xb * 127);
					motion.interpolation_rotation[1] = static_cast<std::int8_t>(interpolatorRotation->ya * 127);
					motion.interpolation_rotation[3] = static_cast<std::int8_t>(interpolatorRotation->yb * 127);

					vmd.MotionLists.push_back(std::move(motion));
				}
			}

			vmd.NumMotion = (VMD_uint32_t)vmd.MotionLists.size();
		}

		if (animation.hasClip("Morph"))
		{
			auto& clip = animation.getClip("Morph");

			for (auto& binding : clip->bindings)
			{
				for (auto& curve : binding.second)
				{
					std::vector<VMDMorph> morphData;

					auto name = utf82sjis(curve.first);

					for (auto& keyframe : curve.second.frames)
					{
						VMDMorph morph;
						std::memset(&morph, 0, sizeof(VMDMorph));
						std::memcpy(&morph.name, name.data(), name.size());
						morph.frame = (VMD_uint32_t)(keyframe.time * 30.0f);
						morph.weight = keyframe.value.getFloat();

						vmd.MorphLists.push_back(std::move(morph));
					}
				}
			}

			vmd.NumMorph = (VMD_uint32_t)vmd.MorphLists.size();
		}

		if (animation.hasClip("Camera"))
		{
			auto& clip = animation.getClip("Camera");

			for (auto& binding : clip->bindings)
			{
				auto& eyeX = binding.second.at("LocalPosition.x");
				auto& eyeY = binding.second.at("LocalPosition.y");
				auto& eyeZ = binding.second.at("LocalPosition.z");
				auto& rotation = binding.second.at("LocalEulerAnglesRaw");
				auto& distance = binding.second.at("LocalForward");
				auto& fov = binding.second.at("Camera:fov");

				for (std::size_t i = 0; i < eyeX.frames.size(); i++)
				{
					auto interpolatorEyeX = std::dynamic_pointer_cast<PathInterpolator<float>>(eyeX.frames[i].interpolator);
					auto interpolatorEyeY = std::dynamic_pointer_cast<PathInterpolator<float>>(eyeY.frames[i].interpolator);
					auto interpolatorEyeZ = std::dynamic_pointer_cast<PathInterpolator<float>>(eyeZ.frames[i].interpolator);
					auto interpolatorRotation = std::dynamic_pointer_cast<PathInterpolator<float>>(rotation.frames[i].interpolator);
					auto interpolatorDistance = std::dynamic_pointer_cast<PathInterpolator<float>>(distance.frames[i].interpolator);
					auto interpolatorViewingAngle = std::dynamic_pointer_cast<PathInterpolator<float>>(fov.frames[i].interpolator);

					VMDCamera motion;
					motion.frame = (VMD_uint32_t)(eyeX.frames[i].time * 30.0f);
					motion.location.x = eyeX.frames[i].value.getFloat();
					motion.location.y = eyeY.frames[i].value.getFloat();
					motion.location.z = eyeZ.frames[i].value.getFloat();
					motion.distance = distance.frames[i].value.getFloat();
					motion.rotation = -rotation.frames[i].value.getFloat3();
					motion.viewingAngle = (VMD_uint32_t)(fov.frames[i].value.getFloat());
					motion.perspective = 1;

					motion.interpolation_x[0] = static_cast<std::int8_t>(interpolatorEyeX->xa * 127);
					motion.interpolation_x[2] = static_cast<std::int8_t>(interpolatorEyeX->xb * 127);
					motion.interpolation_x[1] = static_cast<std::int8_t>(interpolatorEyeX->ya * 127);
					motion.interpolation_x[3] = static_cast<std::int8_t>(interpolatorEyeX->yb * 127);

					motion.interpolation_y[0] = static_cast<std::int8_t>(interpolatorEyeY->xa * 127);
					motion.interpolation_y[2] = static_cast<std::int8_t>(interpolatorEyeY->xb * 127);
					motion.interpolation_y[1] = static_cast<std::int8_t>(interpolatorEyeY->ya * 127);
					motion.interpolation_y[3] = static_cast<std::int8_t>(interpolatorEyeY->yb * 127);

					motion.interpolation_z[0] = static_cast<std::int8_t>(interpolatorEyeZ->xa * 127);
					motion.interpolation_z[2] = static_cast<std::int8_t>(interpolatorEyeZ->xb * 127);
					motion.interpolation_z[1] = static_cast<std::int8_t>(interpolatorEyeZ->ya * 127);
					motion.interpolation_z[3] = static_cast<std::int8_t>(interpolatorEyeZ->yb * 127);

					motion.interpolation_rotation[0] = static_cast<std::int8_t>(interpolatorRotation->xa * 127);
					motion.interpolation_rotation[2] = static_cast<std::int8_t>(interpolatorRotation->xb * 127);
					motion.interpolation_rotation[1] = static_cast<std::int8_t>(interpolatorRotation->ya * 127);
					motion.interpolation_rotation[3] = static_cast<std::int8_t>(interpolatorRotation->yb * 127);

					motion.interpolation_distance[0] = static_cast<std::int8_t>(interpolatorDistance->xa * 127);
					motion.interpolation_distance[2] = static_cast<std::int8_t>(interpolatorDistance->xb * 127);
					motion.interpolation_distance[1] = static_cast<std::int8_t>(interpolatorDistance->ya * 127);
					motion.interpolation_distance[3] = static_cast<std::int8_t>(interpolatorDistance->yb * 127);

					motion.interpolation_angleview[0] = static_cast<std::int8_t>(interpolatorViewingAngle->xa * 127);
					motion.interpolation_angleview[2] = static_cast<std::int8_t>(interpolatorViewingAngle->xb * 127);
					motion.interpolation_angleview[1] = static_cast<std::int8_t>(interpolatorViewingAngle->ya * 127);
					motion.interpolation_angleview[3] = static_cast<std::int8_t>(interpolatorViewingAngle->yb * 127);

					vmd.CameraLists.push_back(motion);
				}
			}

			vmd.NumCamera = (VMD_uint32_t)vmd.CameraLists.size();
		}

		vmd.save(stream);
	}

	void
	VMDImporter::onImportAsset(AssetImporterContext& context) noexcept(false)
	{
		auto filepath = AssetDatabase::instance()->getAbsolutePath(context.getAssetPath());
		std::ifstream stream(filepath, std::ios_base::binary);
		if (stream)
		{
			VMD vmd;
			vmd.load(stream);

			auto animation = std::make_shared<Animation>();
			animation->setName(sjis2utf8(vmd.Header.name));

			if (animation->getName().empty())
				animation->setName((char*)filepath.filename().u8string().c_str());

			if (vmd.NumMotion > 0)
			{
				std::unordered_map<std::string, std::vector<VMDMotion>> motionList;
				for (auto& motion : vmd.MotionLists)
					motionList[sjis2utf8(motion.name)].push_back(motion);

				auto clip = std::make_shared<AnimationClip>();
				clip->setName(sjis2utf8(vmd.Header.name));

				for (auto it = motionList.begin(); it != motionList.end(); ++it)
				{
					Keyframes<float> translateX;
					Keyframes<float> translateY;
					Keyframes<float> translateZ;
					Keyframes<float> rotation;

					auto& motionData = (*it).second;

					translateX.reserve(motionData.size());
					translateY.reserve(motionData.size());
					translateZ.reserve(motionData.size());
					rotation.reserve(motionData.size());
					rotation.reserve(motionData.size());
					rotation.reserve(motionData.size());

					for (auto& data : motionData)
					{
						auto interpolationX = std::make_shared<PathInterpolator<float>>(data.interpolation_x[0] / 127.0f, data.interpolation_x[2] / 127.0f, data.interpolation_x[1] / 127.0f, data.interpolation_x[3] / 127.0f);
						auto interpolationY = std::make_shared<PathInterpolator<float>>(data.interpolation_y[0] / 127.0f, data.interpolation_y[2] / 127.0f, data.interpolation_y[1] / 127.0f, data.interpolation_y[3] / 127.0f);
						auto interpolationZ = std::make_shared<PathInterpolator<float>>(data.interpolation_z[0] / 127.0f, data.interpolation_z[2] / 127.0f, data.interpolation_z[1] / 127.0f, data.interpolation_z[3] / 127.0f);
						auto interpolationRotation = std::make_shared<PathInterpolator<float>>(data.interpolation_rotation[0] / 127.0f, data.interpolation_rotation[2] / 127.0f, data.interpolation_rotation[1] / 127.0f, data.interpolation_rotation[3] / 127.0f);

						translateX.emplace_back((float)data.frame / 30.0f, data.translate.x, interpolationX);
						translateY.emplace_back((float)data.frame / 30.0f, data.translate.y, interpolationY);
						translateZ.emplace_back((float)data.frame / 30.0f, data.translate.z, interpolationZ);
						rotation.emplace_back((float)data.frame / 30.0f, math::Quaternion(data.rotate.x, data.rotate.y, data.rotate.z, data.rotate.w), interpolationRotation);
					}

					clip->setCurve((*it).first, "LocalPosition.x", AnimationCurve(std::move(translateX)));
					clip->setCurve((*it).first, "LocalPosition.y", AnimationCurve(std::move(translateY)));
					clip->setCurve((*it).first, "LocalPosition.z", AnimationCurve(std::move(translateZ)));
					clip->setCurve((*it).first, "LocalRotation", AnimationCurve(std::move(rotation)));
				}

				animation->addClip(std::move(clip), "Motion");
			}

			if (vmd.NumMorph > 0)
			{
				std::unordered_map<std::string, std::vector<VMDMorph>> morphList;
				for (auto& motion : vmd.MorphLists)
					morphList[sjis2utf8(motion.name)].push_back(motion);

				auto clip = std::make_shared<AnimationClip>();

				for (auto it = morphList.begin(); it != morphList.end(); ++it)
				{
					auto& morphData = (*it).second;

					Keyframes<float> keyframe;
					keyframe.reserve(morphData.size());

					for (auto& morph : morphData)
						keyframe.emplace_back((float)morph.frame / 30.0f, morph.weight);

					clip->setCurve("", (*it).first, AnimationCurve(std::move(keyframe)));
				}

				animation->addClip(std::move(clip), "Morph");
			}

			if (vmd.NumCamera > 0)
			{
				Keyframes<float> distance;
				Keyframes<float> eyeX;
				Keyframes<float> eyeY;
				Keyframes<float> eyeZ;
				Keyframes<float> rotation;
				Keyframes<float> fov;

				distance.reserve(vmd.CameraLists.size());
				eyeX.reserve(vmd.CameraLists.size());
				eyeY.reserve(vmd.CameraLists.size());
				eyeZ.reserve(vmd.CameraLists.size());
				rotation.reserve(vmd.CameraLists.size());
				fov.reserve(vmd.CameraLists.size());

				for (auto& it : vmd.CameraLists)
				{
					auto interpolationDistance = std::make_shared<PathInterpolator<float>>(it.interpolation_distance[0] / 127.0f, it.interpolation_distance[2] / 127.0f, it.interpolation_distance[1] / 127.0f, it.interpolation_distance[3] / 127.0f);
					auto interpolationX = std::make_shared<PathInterpolator<float>>(it.interpolation_x[0] / 127.0f, it.interpolation_x[2] / 127.0f, it.interpolation_x[1] / 127.0f, it.interpolation_x[3] / 127.0f);
					auto interpolationY = std::make_shared<PathInterpolator<float>>(it.interpolation_y[0] / 127.0f, it.interpolation_y[2] / 127.0f, it.interpolation_y[1] / 127.0f, it.interpolation_y[3] / 127.0f);
					auto interpolationZ = std::make_shared<PathInterpolator<float>>(it.interpolation_z[0] / 127.0f, it.interpolation_z[2] / 127.0f, it.interpolation_z[1] / 127.0f, it.interpolation_z[3] / 127.0f);
					auto interpolationRotation = std::make_shared<PathInterpolator<float>>(it.interpolation_rotation[0] / 127.0f, it.interpolation_rotation[2] / 127.0f, it.interpolation_rotation[1] / 127.0f, it.interpolation_rotation[3] / 127.0f);
					auto interpolationAngleView = std::make_shared<PathInterpolator<float>>(it.interpolation_angleview[0] / 127.0f, it.interpolation_angleview[2] / 127.0f, it.interpolation_angleview[1] / 127.0f, it.interpolation_angleview[3] / 127.0f);

					distance.emplace_back((float)it.frame / 30.0f, it.distance, interpolationDistance);
					eyeX.emplace_back((float)it.frame / 30.0f, it.location.x, interpolationX);
					eyeY.emplace_back((float)it.frame / 30.0f, it.location.y, interpolationY);
					eyeZ.emplace_back((float)it.frame / 30.0f, it.location.z, interpolationZ);
					rotation.emplace_back((float)it.frame / 30.0f, -math::float3(it.rotation.x, it.rotation.y, it.rotation.z), interpolationRotation);
					fov.emplace_back((float)it.frame / 30.0f, (float)it.viewingAngle, interpolationAngleView);
				}

				auto clip = std::make_shared<AnimationClip>();
				clip->setCurve("", "LocalPosition.x", AnimationCurve(std::move(eyeX)));
				clip->setCurve("", "LocalPosition.y", AnimationCurve(std::move(eyeY)));
				clip->setCurve("", "LocalPosition.z", AnimationCurve(std::move(eyeZ)));
				clip->setCurve("", "LocalEulerAnglesRaw", AnimationCurve(std::move(rotation)));
				clip->setCurve("", "LocalForward", AnimationCurve(std::move(distance)));
				clip->setCurve("", "Camera:fov", AnimationCurve(std::move(fov)));

				animation->addClip(std::move(clip), "Camera");
			}

			context.setMainObject(animation);
		}
	}

	void
	VMDImporter::save(const std::filesystem::path& filepath, const Animation& animation) noexcept(false)
	{
		std::ofstream stream(filepath, io::ios_base::in | io::ios_base::out);
		if (stream)
			save(stream, animation);
	}
}