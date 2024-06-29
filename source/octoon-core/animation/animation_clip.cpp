#include <octoon/animation/animation_clip.h>

namespace octoon
{
	OctoonImplementSubClass(AnimationClip, Object, "AnimationClip")

	AnimationClip::AnimationClip() noexcept
		: finish(false)
		, timeLength(0)
	{
	}

	AnimationClip::AnimationClip(std::string_view _name) noexcept
		: name(_name)
		, finish(false)
		, timeLength(0)
	{
	}

	void
	AnimationClip::setName(std::string_view _name) noexcept
	{
		this->name = _name;
	}

	const std::string&
	AnimationClip::getName() const noexcept
	{
		return this->name;
	}

	void
	AnimationClip::setCurve(std::string_view relativePath, std::string_view propertyName, AnimationCurve<float>&& curve) noexcept
	{
		this->bindings[std::string(relativePath)][std::string(propertyName)] = std::move(curve);

		this->timeLength = 0;

		for (auto& binding : this->bindings)
		{
			for (auto& it : binding.second)
				timeLength = std::max(it.second.timeLength, timeLength);
		}
	}

	void
	AnimationClip::setCurve(std::string_view relativePath, std::string_view propertyName, const AnimationCurve<float>& curve) noexcept
	{
		this->bindings[std::string(relativePath)][std::string(propertyName)] = curve;

		this->timeLength = 0;

		for (auto& binding : this->bindings)
		{
			for (auto& it : binding.second)
				timeLength = std::max(it.second.timeLength, timeLength);
		}
	}

	bool
	AnimationClip::hasCurve(std::string_view relativePath) const noexcept
	{
		return this->bindings.find(std::string(relativePath)) != this->bindings.end();
	}

	AnimationCurve<float>&
	AnimationClip::getCurve(std::string_view relativePath, std::string_view propertyName) noexcept
	{
		return this->bindings.at(std::string(relativePath)).at(std::string(propertyName));
	}

	const AnimationCurve<float>&
	AnimationClip::getCurve(std::string_view relativePath, std::string_view propertyName) const noexcept
	{
		return this->bindings.at(std::string(relativePath)).at(std::string(propertyName));
	}

	void
	AnimationClip::clearCurve() noexcept
	{
		this->timeLength = 0;
		this->bindings.clear();
	}

	bool
	AnimationClip::empty() const noexcept
	{
		return this->bindings.empty();
	}

	std::size_t
	AnimationClip::size() const noexcept
	{
		return this->bindings.size();
	}

	void
	AnimationClip::evaluate(float delta) noexcept
	{
		this->finish = true;

		for (auto& binding : this->bindings)
		{
			for (auto& curve : binding.second)
			{
				curve.second.evaluate(delta);
				this->finish &= curve.second.finish;
			}
		}
	}

	void
	AnimationClip::setTime(float time) noexcept
	{
		for (auto& binding : this->bindings)
		{
			for (auto& curve : binding.second)
				curve.second.setTime(time);
		}

		this->finish = true;

		for (auto& binding : this->bindings)
		{
			for (auto& curve : binding.second)
				this->finish &= curve.second.finish;
		}
	}
}