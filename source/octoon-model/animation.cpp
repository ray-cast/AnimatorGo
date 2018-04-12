#include <octoon/model/animation.h>
#include <octoon/math/mathutil.h>

#include <map>

using namespace octoon::math;

namespace octoon
{
	namespace model
	{
		BoneAnimation::BoneAnimation() noexcept
			: _bone(0)
			, _frame(-1)
		{
		}

		BoneAnimation::~BoneAnimation() noexcept
		{
		}

		void BoneAnimation::setName(const std::string& name) noexcept
		{
			_name = name;
		}

		const std::string& BoneAnimation::getName() const noexcept
		{
			return _name;
		}

		void BoneAnimation::setPosition(const Vector3& position) noexcept
		{
			_position = position;
		}

		const Vector3& BoneAnimation::getPosition() const noexcept
		{
			return _position;
		}

		void BoneAnimation::setRotation(const Quaternion& quat) noexcept
		{
			_rotation = quat;
		}

		const Quaternion& BoneAnimation::getRotation() const noexcept
		{
			return _rotation;
		}

		void BoneAnimation::setBoneIndex(const std::int32_t bone) noexcept
		{
			_bone = bone;
		}

		std::int32_t BoneAnimation::getBoneIndex() const noexcept
		{
			return _bone;
		}

		void BoneAnimation::setFrameNo(std::int32_t frame) noexcept
		{
			_frame = frame;
		}

		std::int32_t BoneAnimation::getFrameNo() const noexcept
		{
			return _frame;
		}

		void BoneAnimation::setInterpolation(const Interpolation& interp) noexcept
		{
			_interpolation = interp;
		}

		const Interpolation& BoneAnimation::getInterpolation() const noexcept
		{
			return _interpolation;
		}

		AnimationProperty::AnimationProperty() noexcept
			: _frame(0)
			, _fps(30)
			, _delta(0)
		{
		}

		AnimationProperty::~AnimationProperty() noexcept
		{
		}

		void AnimationProperty::setName(const std::string& name) noexcept
		{
			_name = name;
		}

		const std::string& AnimationProperty::getName() const noexcept
		{
			return _name;
		}

		void AnimationProperty::setCurrentFrame(std::size_t frame) noexcept
		{
			_frame = frame;
		}

		std::size_t AnimationProperty::getCurrentFrame() const noexcept
		{
			return _frame;
		}

		void AnimationProperty::addBoneAnimation(const BoneAnimation& anim) noexcept
		{
			_boneAnimation.push_back(anim);
		}

		BoneAnimation& AnimationProperty::getBoneAnimation(std::size_t index) noexcept
		{
			return _boneAnimation[index];
		}

		const BoneAnimation& AnimationProperty::getBoneAnimation(std::size_t index) const noexcept
		{
			return _boneAnimation[index];
		}

		std::size_t AnimationProperty::getNumBoneAnimation() const noexcept
		{
			return _boneAnimation.size();
		}

		void AnimationProperty::addMorphAnimation(const MorphAnimation& anim) noexcept
		{
			_morphAnimation.push_back(anim);
		}

		MorphAnimation& AnimationProperty::getMorphAnimation(std::size_t index) noexcept
		{
			return _morphAnimation[index];
		}

		const MorphAnimation& AnimationProperty::getMorphAnimation(std::size_t index) const noexcept
		{
			return _morphAnimation[index];
		}

		std::size_t AnimationProperty::getNumMorphAnimation() const noexcept
		{
			return _morphAnimation.size();
		}

		void AnimationProperty::setBoneArray(const Bones& bones) noexcept
		{
			_bones = bones;
			this->updateBones(bones);
		}

		void AnimationProperty::setBoneArray(Bones&& bones) noexcept
		{
			_bones = std::move(bones);
			this->updateBones(_bones);
		}

		const Bones& AnimationProperty::getBoneArray() const noexcept
		{
			return _bones;
		}

		void AnimationProperty::setIKArray(const InverseKinematics& ik) noexcept
		{
			_iks = ik;
		}

		void AnimationProperty::setIKArray(InverseKinematics&& ik) noexcept
		{
			_iks = std::move(ik);
		}

		const InverseKinematics& AnimationProperty::getIKArray() const noexcept
		{
			return _iks;
		}

		AnimationPropertyPtr AnimationProperty::clone() noexcept
		{
			auto anim = std::make_shared<AnimationProperty>();
			anim->_name = this->_name;
			anim->_boneAnimation = this->_boneAnimation;
			anim->_morphAnimation = this->_morphAnimation;
			anim->_frame = this->_frame;
			return anim;
		}

		void AnimationProperty::updateFrame(float delta) noexcept
		{
			_delta += delta;

			while (_delta > (1.0f / _fps))
			{
				_frame++;
				_delta -= 1.0f / _fps;
			}
		}

		void AnimationProperty::updateMotion() noexcept
		{
			this->updateBoneMotion();
			this->updateBoneMatrix();
			this->updateIK();
		}

		void AnimationProperty::updateBones(const Bones& bones) noexcept
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
				numFrame = std::max((std::int32_t)numFrame, _boneAnimation[i].getFrameNo());
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

		bool AnimationProperty::updateBoneMotion(std::size_t index) noexcept
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
					m.make_translate(position);
					bone.setLocalTransform(m);
				}
				else
				{
					float4x4 m;
					m.make_translate(bone.getPosition());
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

		void AnimationProperty::updateBoneMotion() noexcept
		{
			for (std::size_t i = 0; i < _bones.size(); i++)
				this->updateBoneMotion(i);
		}

		void AnimationProperty::updateBoneMatrix() noexcept
		{
			std::size_t size = _bones.size();
			for (std::size_t i = 0; i < size; i++)
			{
				std::intptr_t  parent = _bones[i].getParent();
				if ((std::size_t)parent > size)
					_bones[i].setTransform(_bones[i].getLocalTransform());
				else
					_bones[i].setTransform(transform_multiply(_bones[parent].getTransform(), _bones[i].getLocalTransform()));
			}
		}

		void AnimationProperty::updateBoneMatrix(Bone& bone) noexcept
		{
			if (bone.getParent() != (-1))
			{
				auto& parent = _bones.at(bone.getParent());
				updateBoneMatrix(parent);
				bone.setTransform(transform_multiply(parent.getTransform(), bone.getLocalTransform()));
			}
			else
			{
				bone.setTransform(bone.getLocalTransform());
			}
		}

		void AnimationProperty::updateIK() noexcept
		{
			for (auto& ik : _iks)
				this->updateIK(_bones, ik);
		}

		void AnimationProperty::updateIK(Bones& bones, const IKAttr& ik) noexcept
		{
			auto& effector = bones.at(ik.boneIndex);
			auto& target = bones.at(ik.targetBoneIndex);

			const Vector3& effectPos = effector.getTransform().get_translate();

			for (std::uint32_t i = 0; i < ik.iterations; i++)
			{
				for (std::uint32_t j = 0; j < ik.chainLength; j++)
				{
					auto& bone = bones[ik.child[j].boneIndex];

					Vector3 targetPos = target.getTransform().get_translate();
					if (math::distance(effectPos, targetPos) < EPSILON)
						return;

					Vector3 dstLocal = math::inv_translate_vector3(bone.getTransform(), targetPos);
					Vector3 srcLocal = math::inv_translate_vector3(bone.getTransform(), effectPos);

					srcLocal = math::normalize(srcLocal);
					dstLocal = math::normalize(dstLocal);

					float rotationDotProduct = math::dot(dstLocal, srcLocal);
					float rotationAngle = math::safe_acos(rotationDotProduct) * ik.child[j].angleWeight;

					if (rotationAngle < EPSILON_E5)
						continue;

					Vector3 rotationAxis = math::cross(dstLocal, srcLocal);
					rotationAxis = math::normalize(rotationAxis);

					Quaternion q0(rotationAxis, degress(rotationAngle));

					if (ik.child[j].rotateLimited)
					{
						float3 euler(math::euler_angles(q0));
						euler.x = std::min(ik.child[j].minimumDegrees.x, euler.x);
						euler.y = std::min(ik.child[j].minimumDegrees.y, euler.y);
						euler.z = std::min(ik.child[j].minimumDegrees.z, euler.z);
						euler.x = std::max(ik.child[j].maximumDegrees.x, euler.x);
						euler.y = std::max(ik.child[j].maximumDegrees.y, euler.y);
						euler.z = std::max(ik.child[j].maximumDegrees.z, euler.z);

						q0.make_rotation(euler);
					}

					Quaternion qq = math::cross(bone.getRotation(), q0);
					updateTransform(bone, bone.getLocalTransform().get_translate(), qq);

					this->updateBoneMatrix(target);
				}
			}
		}

		void AnimationProperty::updateTransform(Bone& bone, const float3& translate, const Quaternion& rotate) noexcept
		{
			float4x4 transform;
			transform.make_rotation(rotate);
			transform.set_translate(translate);

			bone.setRotation(rotate);
			bone.setLocalTransform(transform);
		}

		MotionSegment AnimationProperty::findMotionSegment(int frame, const std::vector<std::size_t>& motions) noexcept
		{
			MotionSegment ms;
			ms.m0 = 0;
			ms.m1 = motions.size() - 1;

			auto& anim1 = this->getBoneAnimation(motions[ms.m1]);

			if (frame >= anim1.getFrameNo())
			{
				ms.m0 = ms.m1;
				ms.m1 = -1;
				return ms;
			}

			for (;;)
			{
				int middle = (ms.m0 + ms.m1) / 2;
				auto& anim2 = this->getBoneAnimation(motions[middle]);

				if (middle == ms.m0)
					return ms;

				if (anim2.getFrameNo() == frame)
				{
					ms.m0 = middle;
					ms.m1 = -1;
					return ms;
				}

				if (anim2.getFrameNo() > frame)
					ms.m1 = middle;
				else
					ms.m0 = middle;
			}
		}

		static float BezierEval(const std::uint8_t* ip, float t) noexcept
		{
			float xa = ip[0] / 256.0f;
			float xb = ip[2] / 256.0f;
			float ya = ip[1] / 256.0f;
			float yb = ip[3] / 256.0f;

			float min = 0.0f;
			float max = 1.0f;

			float ct = t;
			for (;;)
			{
				float x11 = xa * ct;
				float x12 = xa + (xb - xa) * ct;
				float x13 = xb + (1.0 - xb) * ct;

				float x21 = x11 + (x12 - x11) * ct;
				float x22 = x12 + (x13 - x12) * ct;

				float x3 = x21 + (x22 - x21) * ct;

				if (std::fabs(x3 - t) < 0.0001f)
				{
					float y11 = ya * ct;
					float y12 = ya + (yb - ya) * ct;
					float y13 = yb + (1.0 - yb) * ct;

					float y21 = y11 + (y12 - y11) * ct;
					float y22 = y12 + (y13 - y12) * ct;

					float y3 = y21 + (y22 - y21) * ct;

					return y3;
				}
				else if (x3 < t)
				{
					min = ct;
				}
				else
				{
					max = ct;
				}

				ct = min * 0.5f + max * 0.5f;
			}
		}

		void AnimationProperty::interpolateMotion(Quaternion& rotation, Vector3& position, const std::vector<std::size_t>& motions, std::size_t frame) noexcept
		{
			auto ms = findMotionSegment(frame, motions);

			if (ms.m1 == -1)
			{
				position = this->getBoneAnimation(motions[ms.m0]).getPosition();
				rotation = this->getBoneAnimation(motions[ms.m0]).getRotation();
			}
			else
			{
				auto& anim0 = this->getBoneAnimation(motions[ms.m0]);
				auto& anim1 = this->getBoneAnimation(motions[ms.m1]);

				int diff = anim1.getFrameNo() - anim0.getFrameNo();
				float a0 = frame - anim0.getFrameNo();
				float ratio = a0 / diff;

				float tx = BezierEval(anim0.getInterpolation().interpX, ratio);
				float ty = BezierEval(anim0.getInterpolation().interpY, ratio);
				float tz = BezierEval(anim0.getInterpolation().interpZ, ratio);
				float tr = BezierEval(anim0.getInterpolation().interpW, ratio);

				position = Vector3(1 - tx, 1 - ty, 1 - tz) * anim0.getPosition();
				position += Vector3(tx, ty, tz) * anim1.getPosition();

				Quaternion r0 = anim0.getRotation();
				Quaternion r1 = anim1.getRotation();

				rotation = math::slerp(r0, r1, tr);
			}
		}
	}
}