#ifndef OCTOON_ANIMATION_CLIP_H_
#define OCTOON_ANIMATION_CLIP_H_

#include <unordered_map>
#include <octoon/runtime/object.h>
#include <octoon/animation/animation_curve.h>

namespace octoon
{
	class OCTOON_EXPORT AnimationClip final : public Object
	{
		OctoonDeclareSubClass(AnimationClip, Object)
	public:
		AnimationClip() noexcept;
		explicit AnimationClip(std::string_view _name) noexcept;

		void setName(std::string_view _name) noexcept;
		const std::string& getName() const noexcept;

		void setCurve(std::string_view relativePath, std::string_view propertyName, AnimationCurve<float>&& curve) noexcept;
		void setCurve(std::string_view relativePath, std::string_view propertyName, const AnimationCurve<float>& curve) noexcept;

		bool hasCurve(std::string_view relativePath) const noexcept;

		AnimationCurve<float>& getCurve(std::string_view relativePath, std::string_view propertyName) noexcept;
		const AnimationCurve<float>& getCurve(std::string_view relativePath, std::string_view propertyName) const noexcept;

		void clearCurve() noexcept;

		bool empty() const noexcept;

		std::size_t size() const noexcept;

		void evaluate(float delta) noexcept;

		void setTime(float time) noexcept;

	public:
		std::string name;
		std::unordered_map<std::string, std::unordered_map<std::string, AnimationCurve<float>>> bindings;
		bool finish;
		float timeLength;
	};
}

#endif