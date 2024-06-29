#ifndef OCTOON_ANIMATION_H_
#define OCTOON_ANIMATION_H_

#include <unordered_map>
#include <octoon/animation/animation_clip.h>

namespace octoon
{
	struct AnimatorStateInfo
	{
		bool finish;
		float time;
		float timeLength;
	};

	class OCTOON_EXPORT Animation final : public Object
	{
		OctoonDeclareSubClass(Animation, Object)
	public:			
		Animation() noexcept;
		Animation(std::shared_ptr<AnimationClip>&& _clip, std::string_view _name) noexcept;
		Animation(const std::shared_ptr<AnimationClip>& _clip, std::string_view _name) noexcept;
		Animation(std::unordered_map<std::string, std::shared_ptr<AnimationClip>>&& _clips) noexcept;
		Animation(const std::unordered_map<std::string, std::shared_ptr<AnimationClip>>& _clips) noexcept;
		Animation(std::string&& _name, std::unordered_map<std::string, std::shared_ptr<AnimationClip>>&& _clips) noexcept;
		Animation(std::string_view _name, const std::unordered_map<std::string, std::shared_ptr<AnimationClip>>& _clips) noexcept;
		explicit Animation(std::string_view name) noexcept;

		void setName(std::string_view name) noexcept;
		const std::string& getName() const noexcept;

		void setTime(const float& time) noexcept;

		void addClip(std::shared_ptr<AnimationClip>&& clip_, std::string_view key) noexcept;
		void addClip(const std::shared_ptr<AnimationClip>& clip_, std::string_view key) noexcept;

		void setClip(std::shared_ptr<AnimationClip>&& clip_, std::string_view key) noexcept;
		void setClip(const std::shared_ptr<AnimationClip>& clip_, std::string_view key) noexcept;

		void setClips(const std::unordered_map<std::string, std::shared_ptr<AnimationClip>>& clips) noexcept;
		void setClips(std::unordered_map<std::string, std::shared_ptr<AnimationClip>>&& clips) noexcept;

		std::shared_ptr<AnimationClip> getClip(const std::string& key);
		const std::shared_ptr<AnimationClip>& getClip(const std::string& key) const;

		bool hasClip(std::string_view key) const;

		bool setDefaultClip(std::string_view key);

		void evaluate(const float& delta) noexcept;

		bool empty() const noexcept;

		std::size_t size() const noexcept;

	public:
		const std::shared_ptr<AnimationClip>& operator[](const char* key) const
		{
			return this->clips.at(key);
		}

		const std::shared_ptr<AnimationClip>& operator[](const std::string& key) const
		{
			return this->clips.at(key);
		}

	public:
		std::string name;
		std::shared_ptr<AnimationClip> clip;
		std::unordered_map<std::string, std::shared_ptr<AnimationClip>> clips;
	};
}

#endif