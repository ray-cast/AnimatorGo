#ifndef OCTOON_ANIMATION_H_
#define OCTOON_ANIMATION_H_

#include <octoon/model/bone.h>

#include <octoon/math/mathfwd.h>
#include <octoon/math/vector3.h>
#include <octoon/math/quat.h>

#include <string>
#include <cstdint>

namespace octoon
{
	namespace model
	{
		struct Interpolation
		{
			std::uint8_t interpX[4];
			std::uint8_t interpY[4];
			std::uint8_t interpZ[4];
			std::uint8_t interpW[4];
		};

		struct MotionSegment
		{
			int m0;
			int m1;
		};

		class BoneAnimation
		{
		public:
			BoneAnimation() noexcept;
			~BoneAnimation() noexcept;

			void setName(const std::string& name) noexcept;
			const std::string& getName() const noexcept;

			void setPosition(const math::Vector3& position) noexcept;
			const math::Vector3& getPosition() const noexcept;

			void setRotation(const math::Quaternion& position) noexcept;
			const math::Quaternion& getRotation() const noexcept;

			void setBoneIndex(const std::int32_t) noexcept;
			std::int32_t getBoneIndex() const noexcept;

			void setFrameNo(std::int32_t frame) noexcept;
			std::int32_t getFrameNo() const noexcept;

			void setInterpolation(const Interpolation& interp) noexcept;
			const Interpolation& getInterpolation() const noexcept;

		private:
			std::string _name;

			std::int32_t _bone;
			std::int32_t _frame;

			math::Vector3 _position;
			math::Quaternion _rotation;
			Interpolation _interpolation;
		};

		class MorphAnimation
		{
		};

		class AnimationProperty final
		{
		public:
			AnimationProperty() noexcept;
			~AnimationProperty() noexcept;

			void setName(const std::string& name) noexcept;
			const std::string& getName() const noexcept;

			void setCurrentFrame(std::size_t frame) noexcept;
			std::size_t getCurrentFrame() const noexcept;

			void setBoneArray(const Bones& bones) noexcept;
			void setBoneArray(Bones&& bones) noexcept;
			const Bones& getBoneArray() const noexcept;

			void setIKArray(const InverseKinematics& ik) noexcept;
			void setIKArray(InverseKinematics&& ik) noexcept;
			const InverseKinematics& getIKArray() const noexcept;

			void addBoneAnimation(const BoneAnimation& anim) noexcept;
			BoneAnimation& getBoneAnimation(std::size_t index) noexcept;
			const BoneAnimation& getBoneAnimation(std::size_t index) const noexcept;
			std::size_t getNumBoneAnimation() const noexcept;

			void addMorphAnimation(const MorphAnimation& anim) noexcept;
			MorphAnimation& getMorphAnimation(std::size_t index) noexcept;
			const MorphAnimation& getMorphAnimation(std::size_t index) const noexcept;
			std::size_t getNumMorphAnimation() const noexcept;

			AnimationPropertyPtr clone() noexcept;

			void updateFrame(float delta) noexcept;
			void updateMotion() noexcept;
			void updateBoneMotion() noexcept;
			bool updateBoneMotion(std::size_t index) noexcept;
			void updateBoneMatrix() noexcept;
			void updateBoneMatrix(Bone& bone) noexcept;
			void updateIK() noexcept;

			MotionSegment findMotionSegment(int frame, const std::vector<std::size_t>& motions) noexcept;
			void interpolateMotion(math::Quaternion& rotation, math::Vector3& position, const std::vector<std::size_t>& motions, std::size_t frame) noexcept;

		private:
			AnimationProperty(const AnimationProperty&) = delete;
			AnimationProperty& operator=(const AnimationProperty&) = delete;

		private:
			void updateIK(Bones& _bones, const IKAttr& ik) noexcept;
			void updateBones(const Bones& _bones) noexcept;
			void updateTransform(Bone& bone, const math::float3& translate, const math::Quaternion& rotate) noexcept;

		private:

			std::string _name;

			std::size_t _fps;
			std::size_t _frame;

			float _delta;

			Bones _bones;
			InverseKinematics _iks;

			std::vector<BoneAnimation> _boneAnimation;
			std::vector<MorphAnimation> _morphAnimation;
			std::vector<std::vector<std::size_t>> _bindAnimation;
			std::vector<std::vector<std::size_t>> _bindAnimations;
		};
	}
}
#endif // !OCTOON_ANIMATION_H_
