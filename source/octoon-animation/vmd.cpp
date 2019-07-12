#include <octoon/animation/vmd.h>
#include <iconv.h>
#include <map>

namespace octoon
{
	namespace animation
	{
		static bool VMDMotionSorter(const VMDMotion &m1, const VMDMotion &m2)
		{
			return m1.frame < m2.frame;
		}

		static bool VMDMorphSorter(const VMDMorph &m1, const VMDMorph &m2)
		{
			return m1.frame < m2.frame;
		}

		static bool VMDCameraFrameSorter(const VMDCamera &m1, const VMDCamera &m2)
		{
			return m1.frame < m2.frame;
		}

		static bool VMDLightFrameSorter(const VMDLight &m1, const VMDLight &m2)
		{
			return m1.frame < m2.frame;
		}

		static bool VMDSelfShadowFrameSorter(const VMDSelfShadow& m1, const VMDSelfShadow& m2)
		{
			return m1.frame < m2.frame;
		}

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
			}
			catch (const std::exception&)
			{
				iconv_close(ic);
			}

			return std::string(outbuf.get());
		}

		VMDHandler::VMDHandler() noexcept
		{
		}

		VMDHandler::~VMDHandler() noexcept
		{
		}

		bool
		VMDHandler::doCanRead(io::istream& stream) const noexcept
		{
			static_assert(sizeof(VMDMotion) == 111);
			static_assert(sizeof(VMDMorph) == 23);
			static_assert(sizeof(VMDCamera) == 61);
			static_assert(sizeof(VMDLight) == 28);
			static_assert(sizeof(VMDSelfShadow) == 9);

			VMD_Header hdr;

			if (stream.read((char*)&hdr, sizeof(hdr)))
			{
				if (std::strncmp(hdr.magic, "Vocaloid Motion Data 0002", 30) == 0)
					return true;
			}

			return false;
		}

		bool
		VMDHandler::doCanRead(const char* type) const noexcept
		{
			return std::strncmp(type, "vmd", 3) == 0;
		}

		bool
		VMDHandler::doLoad(io::istream& stream, Animation<float>& animation) noexcept
		{
			VMD vmd;
			if (!stream.read((char*)&vmd.Header, sizeof(vmd.Header))) return false;
			if (!stream.read((char*)&vmd.NumMotion, sizeof(vmd.NumMotion))) return false;

			if (vmd.NumMotion > 0)
			{
				vmd.MotionLists.resize(vmd.NumMotion);

				if (!stream.read((char*)vmd.MotionLists.data(), sizeof(VMDMotion) * vmd.NumMotion)) return false;

				std::sort(vmd.MotionLists.begin(), vmd.MotionLists.end(), VMDMotionSorter);
			}

			if (!stream.read((char*)&vmd.NumMorph, sizeof(vmd.NumMorph))) return false;
			if (vmd.NumMorph > 0)
			{
				vmd.MorphLists.resize(vmd.NumMorph);

				if (!stream.read((char*)vmd.MorphLists.data(), sizeof(VMDMorph) * vmd.NumMorph)) return false;

				std::sort(vmd.MorphLists.begin(), vmd.MorphLists.end(), VMDMorphSorter);
			}

			if (!stream.read((char*)&vmd.NumCamera, sizeof(vmd.NumCamera))) return false;
			if (vmd.NumCamera > 0)
			{
				vmd.CameraLists.resize(vmd.NumCamera);

				if (!stream.read((char*)vmd.CameraLists.data(), sizeof(VMDCamera) * vmd.NumCamera)) return false;

				std::sort(vmd.CameraLists.begin(), vmd.CameraLists.end(), VMDCameraFrameSorter);
			}

			if (!stream.read((char*)&vmd.NumLight, sizeof(vmd.NumLight))) return false;
			if (vmd.NumLight > 0)
			{
				vmd.LightLists.resize(vmd.NumLight);

				if (!stream.read((char*)vmd.LightLists.data(), sizeof(VMDLight) * vmd.NumLight)) return false;

				std::sort(vmd.LightLists.begin(), vmd.LightLists.end(), VMDLightFrameSorter);
			}

			if (!stream.read((char*)&vmd.NumSelfShadow, sizeof(vmd.NumSelfShadow))) return false;
			if (vmd.NumSelfShadow > 0)
			{
				vmd.SelfShadowLists.resize(vmd.NumSelfShadow);

				if (!stream.read((char*)vmd.SelfShadowLists.data(), sizeof(VMDSelfShadow) * vmd.NumSelfShadow)) return false;

				std::sort(vmd.SelfShadowLists.begin(), vmd.SelfShadowLists.end(), VMDSelfShadowFrameSorter);
			}

			std::map<std::string, AnimationClip<float>> motions;

			for (auto& it : vmd.MotionLists)
			{
				if (motions.find(it.name) == motions.end())
					motions[it.name].setName(sjis2utf8(vmd.Header.name));

				auto& clip = motions[it.name];
				clip.getCurve("Position.X").insert(Keyframe((float)it.frame, it.location.x));
				clip.getCurve("Position.Y").insert(Keyframe((float)it.frame, it.location.y));
				clip.getCurve("Position.Z").insert(Keyframe((float)it.frame, it.location.z));
				clip.getCurve("Rotation.X").insert(Keyframe((float)it.frame, it.rotate.x));
				clip.getCurve("Rotation.Y").insert(Keyframe((float)it.frame, it.rotate.y));
				clip.getCurve("Rotation.Z").insert(Keyframe((float)it.frame, it.rotate.z));
				clip.getCurve("Rotation.W").insert(Keyframe((float)it.frame, it.rotate.w));
			}

			animation.setName(sjis2utf8(vmd.Header.name));
			for (auto& it : motions)
				animation.addClip(it.second);
			return true;
		}

		bool
		VMDHandler::doSave(io::ostream& stream, const Animation<float>& animation) noexcept
		{
			VMD vmd;
			std::memset(&vmd.Header, 0, sizeof(vmd.Header));
			std::memcpy(vmd.Header.magic, "Vocaloid Motion Data 0002", 15);

			stream.write((char*)&vmd.Header, sizeof(vmd.Header));

			return true;
		}
	}
}