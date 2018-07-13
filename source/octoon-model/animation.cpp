#include <octoon/model/animation.h>

#include <map>

using namespace octoon::math;

namespace octoon
{
	namespace model
	{
		struct MotionSegment
		{
			int m0;
			int m1;
		};

		BoneAnimation::BoneAnimation() noexcept
			: _bone(0)
			, _frame(-1)
		{
		}

		BoneAnimation::~BoneAnimation() noexcept
		{
		}

		void
		BoneAnimation::setName(const std::string& name) noexcept
		{
			_name = name;
		}

		const std::string&
		BoneAnimation::getName() const noexcept
		{
			return _name;
		}

		void
		BoneAnimation::setPosition(const float3& position) noexcept
		{
			_position = position;
		}

		const float3&
		BoneAnimation::getPosition() const noexcept
		{
			return _position;
		}

		void
		BoneAnimation::setRotation(const Quaternion& quat) noexcept
		{
			_rotation = quat;
		}

		const Quaternion&
		BoneAnimation::getRotation() const noexcept
		{
			return _rotation;
		}

		void
		BoneAnimation::setBoneIndex(const std::int32_t bone) noexcept
		{
			_bone = bone;
		}

		std::int32_t
		BoneAnimation::getBoneIndex() const noexcept
		{
			return _bone;
		}

		void
		BoneAnimation::setFrameIndex(std::int32_t frame) noexcept
		{
			_frame = frame;
		}

		std::int32_t
		BoneAnimation::getFrameIndex() const noexcept
		{
			return _frame;
		}

		void
		BoneAnimation::setInterpX(const PathInterpolator<float>& interp) noexcept
		{
			interpX = interp;
		}

		void
		BoneAnimation::setInterpY(const PathInterpolator<float>& interp) noexcept
		{
			interpY = interp;
		}

		void
		BoneAnimation::setInterpZ(const PathInterpolator<float>& interp) noexcept
		{
			interpZ = interp;
		}

		void
		BoneAnimation::setInterpRotation(const PathInterpolator<float>& interp) noexcept
		{
			interpRotation = interp;
		}

		const PathInterpolator<float>&
		BoneAnimation::getInterpX() const noexcept
		{
			return interpX;
		}

		const PathInterpolator<float>&
		BoneAnimation::getInterpY() const noexcept
		{
			return interpY;
		}

		const PathInterpolator<float>&
		BoneAnimation::getInterpZ() const noexcept
		{
			return interpZ;
		}

		const PathInterpolator<float>&
		BoneAnimation::getInterpRotation() const noexcept
		{
			return interpRotation;
		}

		Animation::Animation() noexcept
			: _frame(0)
			, _fps(30)
			, _totalTime(0)
		{
		}

		Animation::~Animation() noexcept
		{
		}

		void
		Animation::setName(const std::string& name) noexcept
		{
			_name = name;
		}

		const std::string&
		Animation::getName() const noexcept
		{
			return _name;
		}

		void
		Animation::setCurrentFrame(std::size_t frame) noexcept
		{
			_frame = frame;
		}

		std::size_t
		Animation::getCurrentFrame() const noexcept
		{
			return _frame;
		}

		void
		Animation::addBoneAnimation(const BoneAnimation& anim) noexcept
		{
			_boneAnimation.push_back(anim);
		}

		BoneAnimation&
		Animation::getBoneAnimation(std::size_t index) noexcept
		{
			return _boneAnimation[index];
		}

		const BoneAnimation&
		Animation::getBoneAnimation(std::size_t index) const noexcept
		{
			return _boneAnimation[index];
		}

		std::size_t
		Animation::getNumBoneAnimation() const noexcept
		{
			return _boneAnimation.size();
		}

		void
		Animation::addMorphAnimation(const MorphAnimation& anim) noexcept
		{
			_morphAnimation.push_back(anim);
		}

		MorphAnimation&
		Animation::getMorphAnimation(std::size_t index) noexcept
		{
			return _morphAnimation[index];
		}

		const MorphAnimation&
		Animation::getMorphAnimation(std::size_t index) const noexcept
		{
			return _morphAnimation[index];
		}

		std::size_t
		Animation::getNumMorphAnimation() const noexcept
		{
			return _morphAnimation.size();
		}

		void
		Animation::setBoneArray(const Bones& bones) noexcept
		{
			_bones = bones;
			this->updateBones(bones);
		}

		void
		Animation::setBoneArray(Bones&& bones) noexcept
		{
			_bones = std::move(bones);
			this->updateBones(_bones);
		}

		const Bones&
		Animation::getBoneArray() const noexcept
		{
			return _bones;
		}

		void
		Animation::setIKArray(const InverseKinematics& ik) noexcept
		{
			_iks = ik;
		}

		void
		Animation::setIKArray(InverseKinematics&& ik) noexcept
		{
			_iks = std::move(ik);
		}

		const InverseKinematics&
		Animation::getIKArray() const noexcept
		{
			return _iks;
		}

		AnimationPtr
		Animation::clone() noexcept
		{
			auto anim = std::make_shared<Animation>();
			anim->_name = this->_name;
			anim->_boneAnimation = this->_boneAnimation;
			anim->_morphAnimation = this->_morphAnimation;
			anim->_frame = this->_frame;
			return anim;
		}

		void
		Animation::updateFrame(float delta) noexcept
		{
			_totalTime += delta;

			while (_totalTime > (1.0f / _fps))
			{
				_frame++;
				_totalTime -= 1.0f / _fps;
			}
		}

		void
		Animation::updateMotion() noexcept
		{
			this->updateBoneMotion();
			this->updateBoneMatrix();
			this->updateIK();
		}

		void
		Animation::updateBones(const Bones& bones) noexcept
		{
			if (bones.empty())
			{
				_bindAnimation.clear();
				return;
			}

			std::map<std::string, std::size_t> bindBoneMaps;
			for (std::size_t i = 0; i < bones.size(); i++)
			{
				bindBoneMaps[bones[i].getName()] = i + 1;
			}

			std::size_t numFrame = 0;
			std::size_t numAnimation = this->getNumBoneAnimation();

			for (std::size_t i = 0; i < numAnimation; i++)
			{
				numFrame = std::max((std::int32_t)numFrame, _boneAnimation[i].getFrameIndex());
			}

			_bindAnimation.resize(bones.size());

			for (std::size_t i = 0; i < numAnimation; i++)
			{
				auto& name = _boneAnimation[i].getName();
				auto& bone = bindBoneMaps[name];
				if (bone > 0)
				{
					_bindAnimation[bone - 1].push_back(i);
				}
			}
		}

		bool
		Animation::updateBoneMotion(std::size_t index) noexcept
		{
			auto& bone = _bones[index];
			const auto& motion = _bindAnimation[index];
			if (motion.empty())
			{
				bone.setRotation(Quaternion::Zero);

				if (bone.getParent() != (-1))
				{
					auto& parent = _bones[bone.getParent()];
					auto position = (bone.getPosition() - parent.getPosition());
					float4x4 m;
					m.makeTranslate(position);
					bone.setLocalTransform(m);
				}
				else
				{
					float4x4 m;
					m.makeTranslate(bone.getPosition());
					bone.setLocalTransform(m);
				}

				return false;
			}
			else
			{
				Vector3 position;
				Quaternion rotate;
				this->interpolateMotion(rotate, position, motion, _frame);

				if (bone.getParent() == (-1))
					updateTransform(bone, bone.getPosition() + position, rotate);
				else
					updateTransform(bone, bone.getPosition() + position - _bones[bone.getParent()].getPosition(), rotate);

				return true;
			}
		}

		void
		Animation::updateBoneMotion() noexcept
		{
			for (std::size_t i = 0; i < _bones.size(); i++)
				this->updateBoneMotion(i);
		}

		void
		Animation::updateBoneMatrix() noexcept
		{
			std::size_t size = _bones.size();
			for (std::size_t i = 0; i < size; i++)
			{
				std::intptr_t  parent = _bones[i].getParent();
				if ((std::size_t)parent > size)
					_bones[i].setTransform(_bones[i].getLocalTransform());
				else
					_bones[i].setTransform(transformMultiply(_bones[parent].getTransform(), _bones[i].getLocalTransform()));
			}
		}

		void
		Animation::updateBoneMatrix(Bone& bone) noexcept
		{
			if (bone.getParent() != (-1))
			{
				auto& parent = _bones.at(bone.getParent());
				updateBoneMatrix(parent);
				bone.setTransform(transformMultiply(parent.getTransform(), bone.getLocalTransform()));
			}
			else
			{
				bone.setTransform(bone.getLocalTransform());
			}
		}

		void
		Animation::updateIK() noexcept
		{
			for (auto& ik : _iks)
				this->updateIK(_bones, ik);
		}

		void
		Animation::updateIK(Bones& bones, const IKAttr& ik) noexcept
		{
			auto& effector = bones.at(ik.boneIndex);
			auto& target = bones.at(ik.targetBoneIndex);

			const float3& effectPos = effector.getTransform().getTranslate();

			for (std::uint32_t i = 0; i < ik.iterations; i++)
			{
				for (std::uint32_t j = 0; j < ik.chainLength; j++)
				{
					auto& bone = bones[ik.child[j].boneIndex];

					float3 targetPos = target.getTransform().getTranslate();
					if (math::distance(effectPos, targetPos) < EPSILON)
						return;

					float3 dstLocal = math::invTranslateVector3(bone.getTransform(), targetPos);
					float3 srcLocal = math::invTranslateVector3(bone.getTransform(), effectPos);

					srcLocal = math::normalize(srcLocal);
					dstLocal = math::normalize(dstLocal);

					float rotationDotProduct = math::dot(dstLocal, srcLocal);
					float rotationAngle = math::safe_acos(rotationDotProduct) * ik.child[j].angleWeight;

					if (rotationAngle < EPSILON_E5)
						continue;

					float3 rotationAxis = math::cross(dstLocal, srcLocal);
					rotationAxis = math::normalize(rotationAxis);

					Quaternion q0(rotationAxis, degress(rotationAngle));

					if (ik.child[j].rotateLimited)
					{
						float3 euler(math::eulerAngles(q0));
						euler.x = std::min(ik.child[j].minimumDegrees.x, euler.x);
						euler.y = std::min(ik.child[j].minimumDegrees.y, euler.y);
						euler.z = std::min(ik.child[j].minimumDegrees.z, euler.z);
						euler.x = std::max(ik.child[j].maximumDegrees.x, euler.x);
						euler.y = std::max(ik.child[j].maximumDegrees.y, euler.y);
						euler.z = std::max(ik.child[j].maximumDegrees.z, euler.z);

						q0.makeRotation(euler);
					}

					Quaternion qq = math::cross(bone.getRotation(), q0);
					updateTransform(bone, bone.getLocalTransform().getTranslate(), qq);

					this->updateBoneMatrix(target);
				}
			}
		}

		void
		Animation::updateTransform(Bone& bone, const float3& translate, const Quaternion& rotate) noexcept
		{
			float4x4 transform;
			transform.makeRotation(rotate);
			transform.setTranslate(translate);

			bone.setRotation(rotate);
			bone.setLocalTransform(transform);
		}

		MotionSegment findMotionSegment(int frame, std::vector<BoneAnimation>& bones, const std::vector<std::size_t>& motions) noexcept
		{
			MotionSegment ms;
			ms.m0 = 0;
			ms.m1 = motions.size() - 1;

			auto& anim1 = bones[motions[ms.m1]];

			if (frame >= anim1.getFrameIndex())
			{
				ms.m0 = ms.m1;
				ms.m1 = -1;
				return ms;
			}

			for (;;)
			{
				int middle = (ms.m0 + ms.m1) / 2;
				auto& anim2 = bones[motions[middle]];

				if (middle == ms.m0)
					return ms;

				if (anim2.getFrameIndex() == frame)
				{
					ms.m0 = middle;
					ms.m1 = -1;
					return ms;
				}

				if (anim2.getFrameIndex() > frame)
					ms.m1 = middle;
				else
					ms.m0 = middle;
			}
		}

		void
		Animation::interpolateMotion(Quaternion& rotation, float3& position, const std::vector<std::size_t>& motions, std::size_t frame) noexcept
		{
			auto ms = findMotionSegment(frame, _boneAnimation, motions);

			if (ms.m1 == -1)
			{
				position = this->getBoneAnimation(motions[ms.m0]).getPosition();
				rotation = this->getBoneAnimation(motions[ms.m0]).getRotation();
			}
			else
			{
				auto& anim0 = this->getBoneAnimation(motions[ms.m0]);
				auto& anim1 = this->getBoneAnimation(motions[ms.m1]);

				int diff = anim1.getFrameIndex() - anim0.getFrameIndex();
				float a0 = static_cast<float>(frame - anim0.getFrameIndex());
				float ratio = a0 / diff;

				float tx = anim0.getInterpX().getInterpolation(ratio);
				float ty = anim0.getInterpY().getInterpolation(ratio);
				float tz = anim0.getInterpZ().getInterpolation(ratio);
				float tr = anim0.getInterpRotation().getInterpolation(ratio);

				position = math::lerp(anim0.getPosition(), anim1.getPosition(), float3(tx, ty, tz));
				rotation = math::slerp(anim0.getRotation(), anim1.getRotation(), tr);
			}
		}
	}
}