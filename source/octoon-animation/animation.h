#ifndef OCTOON_MODEL_ANIMATION_H_
#define OCTOON_MODEL_ANIMATION_H_

#include <octoon/math/math.h>
#include <octoon/model/ik.h>
#include <octoon/model/bone.h>
#include <octoon/animation/path_interpolator.h>

#include <cstdint>

namespace octoon
{
	namespace model
	{
		class BoneAnimation
		{
		public:
			BoneAnimation() noexcept;
			~BoneAnimation() noexcept;

			void setName(const std::string& name) noexcept;
			void setPosition(const math::float3& position) noexcept;
			void setRotation(const math::Quaternion& rotation) noexcept;
			void setBoneIndex(const std::int32_t) noexcept;
			void setFrameIndex(std::int32_t frame) noexcept;
			void setInterpX(const PathInterpolator<float>& interp) noexcept;
			void setInterpY(const PathInterpolator<float>& interp) noexcept;
			void setInterpZ(const PathInterpolator<float>& interp) noexcept;
			void setInterpRotation(const PathInterpolator<float>& interp) noexcept;

			const std::string& getName() const noexcept;
			const math::float3& getPosition() const noexcept;
			const math::Quaternion& getRotation() const noexcept;
			std::int32_t getBoneIndex() const noexcept;
			std::int32_t getFrameIndex() const noexcept;
			const PathInterpolator<float>& getInterpX() const noexcept;
			const PathInterpolator<float>& getInterpY() const noexcept;
			const PathInterpolator<float>& getInterpZ() const noexcept;
			const PathInterpolator<float>& getInterpRotation() const noexcept;

		private:
			std::string _name;

			std::int32_t _bone;
			std::int32_t _frame;

			math::float3 _position;
			math::Quaternion _rotation;

			PathInterpolator<float> interpX;
			PathInterpolator<float> interpY;
			PathInterpolator<float> interpZ;
			PathInterpolator<float> interpRotation;
		};

		class MorphAnimation
		{
		};

		class Animation final
		{
		public:
			Animation() noexcept;
			~Animation() noexcept;

			void setName(const std::string& name) noexcept;
			void setCurrentFrame(std::size_t frame) noexcept;
			void setBoneArray(Bones&& bones) noexcept;
			void setBoneArray(const Bones& bones) noexcept;
			void setIKArray(InverseKinematics&& ik) noexcept;
			void setIKArray(const InverseKinematics& ik) noexcept;

			const std::string& getName() const noexcept;
			std::size_t getCurrentFrame() const noexcept;
			const Bones& getBoneArray() const noexcept;
			const InverseKinematics& getIKArray() const noexcept;

			void addBoneAnimation(const BoneAnimation& anim) noexcept;
			void addMorphAnimation(const MorphAnimation& anim) noexcept;

			BoneAnimation& getBoneAnimation(std::size_t index) noexcept;
			MorphAnimation& getMorphAnimation(std::size_t index) noexcept;

			const BoneAnimation& getBoneAnimation(std::size_t index) const noexcept;
			const MorphAnimation& getMorphAnimation(std::size_t index) const noexcept;

			std::size_t getNumBoneAnimation() const noexcept;
			std::size_t getNumMorphAnimation() const noexcept;

			AnimationPtr clone() noexcept;

			void updateFrame(float delta) noexcept;
			void updateMotion() noexcept;
			void updateBoneMotion() noexcept;
			bool updateBoneMotion(std::size_t index) noexcept;
			void updateBoneMatrix() noexcept;
			void updateBoneMatrix(Bone& bone) noexcept;
			void updateIK() noexcept;

			void interpolateMotion(math::Quaternion& rotation, math::float3& position, const std::vector<std::size_t>& motions, std::size_t frame) noexcept;

		private:
			Animation(const Animation&) = delete;
			Animation& operator=(const Animation&) = delete;

		private:
			void updateIK(Bones& _bones, const IKAttr& ik) noexcept;
			void updateBones(const Bones& _bones) noexcept;
			void updateTransform(Bone& bone, const math::float3& translate, const math::Quaternion& rotate) noexcept;

		private:
			std::string _name;

			std::size_t _fps;
			std::size_t _frame;

			float _totalTime;

			Bones _bones;
			InverseKinematics _iks;

			std::vector<BoneAnimation> _boneAnimation;
			std::vector<MorphAnimation> _morphAnimation;
			std::vector<std::vector<std::size_t>> _bindAnimation;
			std::vector<std::vector<std::size_t>> _bindAnimations;
		};
	}
}
#endif
