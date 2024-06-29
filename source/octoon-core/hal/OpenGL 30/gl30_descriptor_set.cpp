#include "gl30_descriptor_set.h"
#include "gl30_texture.h"
#include "gl30_shader.h"
#include "gl30_sampler.h"

namespace octoon
{
	namespace hal
	{
		OctoonImplementSubClass(GL30GraphicsUniformSet, GraphicsUniformSet, "GL30GraphicsUniformSet")
		OctoonImplementSubClass(GL30DescriptorSet, GraphicsDescriptorSet, "GL30DescriptorSet")
		OctoonImplementSubClass(GL30DescriptorSetLayout, GraphicsDescriptorSetLayout, "GL30DescriptorSetLayout")
		OctoonImplementSubClass(GL30DescriptorPool, GraphicsDescriptorPool, "GL30DescriptorPool")

		GL30GraphicsUniformSet::GL30GraphicsUniformSet() noexcept
		{
		}

		GL30GraphicsUniformSet::~GL30GraphicsUniformSet() noexcept
		{
		}

		const std::string&
		GL30GraphicsUniformSet::getName() const noexcept
		{
			assert(_param);
			return _param->getName();
		}

		void
		GL30GraphicsUniformSet::uniform1b(bool value) noexcept
		{
			_variant.uniform1b(value);
		}

		void
		GL30GraphicsUniformSet::uniform1i(std::int32_t i1) noexcept
		{
			_variant.uniform1i(i1);
		}

		void
		GL30GraphicsUniformSet::uniform2i(const math::int2& value) noexcept
		{
			_variant.uniform2i(value);
		}

		void
		GL30GraphicsUniformSet::uniform2i(std::int32_t i1, std::int32_t i2) noexcept
		{
			_variant.uniform2i(i1, i2);
		}

		void
		GL30GraphicsUniformSet::uniform3i(const math::int3& value) noexcept
		{
			_variant.uniform3i(value);
		}

		void
		GL30GraphicsUniformSet::uniform3i(std::int32_t i1, std::int32_t i2, std::int32_t i3) noexcept
		{
			_variant.uniform3i(i1, i2, i3);
		}

		void
		GL30GraphicsUniformSet::uniform4i(const math::int4& value) noexcept
		{
			_variant.uniform4i(value);
		}

		void
		GL30GraphicsUniformSet::uniform4i(std::int32_t i1, std::int32_t i2, std::int32_t i3, std::int32_t i4) noexcept
		{
			_variant.uniform4i(i1, i2, i3, i4);
		}

		void
		GL30GraphicsUniformSet::uniform1ui(std::uint32_t ui1) noexcept
		{
			_variant.uniform1ui(ui1);
		}

		void
		GL30GraphicsUniformSet::uniform2ui(const math::uint2& value) noexcept
		{
			_variant.uniform2ui(value);
		}

		void
		GL30GraphicsUniformSet::uniform2ui(std::uint32_t ui1, std::uint32_t ui2) noexcept
		{
			_variant.uniform2ui(ui1, ui2);
		}

		void
		GL30GraphicsUniformSet::uniform3ui(const math::uint3& value) noexcept
		{
			_variant.uniform3ui(value);
		}

		void
		GL30GraphicsUniformSet::uniform3ui(std::uint32_t ui1, std::uint32_t ui2, std::uint32_t ui3) noexcept
		{
			_variant.uniform3ui(ui1, ui2, ui3);
		}

		void
		GL30GraphicsUniformSet::uniform4ui(const math::uint4& value) noexcept
		{
			_variant.uniform4ui(value);
		}

		void
		GL30GraphicsUniformSet::uniform4ui(std::uint32_t ui1, std::uint32_t ui2, std::uint32_t ui3, std::uint32_t ui4) noexcept
		{
			_variant.uniform4ui(ui1, ui2, ui3, ui4);
		}

		void
		GL30GraphicsUniformSet::uniform1f(float f1) noexcept
		{
			_variant.uniform1f(f1);
		}

		void
		GL30GraphicsUniformSet::uniform2f(const math::float2& value) noexcept
		{
			_variant.uniform2f(value);
		}

		void
		GL30GraphicsUniformSet::uniform2f(float f1, float f2) noexcept
		{
			_variant.uniform2f(f1, f2);
		}

		void
		GL30GraphicsUniformSet::uniform3f(const math::float3& value) noexcept
		{
			_variant.uniform3f(value);
		}

		void
		GL30GraphicsUniformSet::uniform3f(float f1, float f2, float f3) noexcept
		{
			_variant.uniform3f(f1, f2, f3);
		}

		void
		GL30GraphicsUniformSet::uniform4f(const math::float4& value) noexcept
		{
			_variant.uniform4f(value);
		}

		void
		GL30GraphicsUniformSet::uniform4f(float f1, float f2, float f3, float f4) noexcept
		{
			_variant.uniform4f(f1, f2, f3, f4);
		}

		void
		GL30GraphicsUniformSet::uniform2fmat(const math::float2x2& value) noexcept
		{
			_variant.uniform2fmat(value);
		}

		void
		GL30GraphicsUniformSet::uniform2fmat(const float* mat2) noexcept
		{
			_variant.uniform2fmat(mat2);
		}

		void
		GL30GraphicsUniformSet::uniform3fmat(const math::float3x3& value) noexcept
		{
			_variant.uniform3fmat(value);
		}

		void
		GL30GraphicsUniformSet::uniform3fmat(const float* mat3) noexcept
		{
			_variant.uniform3fmat(mat3);
		}

		void
		GL30GraphicsUniformSet::uniform4fmat(const math::float4x4& value) noexcept
		{
			_variant.uniform4fmat(value);
		}

		void
		GL30GraphicsUniformSet::uniform4fmat(const float* mat4) noexcept
		{
			_variant.uniform4fmat(mat4);
		}

		void
		GL30GraphicsUniformSet::uniform1iv(const std::vector<math::int1>& value) noexcept
		{
			_variant.uniform1iv(value);
		}

		void
		GL30GraphicsUniformSet::uniform1iv(std::size_t num, const std::int32_t* i1v) noexcept
		{
			_variant.uniform1iv(num, i1v);
		}

		void
		GL30GraphicsUniformSet::uniform2iv(const std::vector<math::int2>& value) noexcept
		{
			_variant.uniform2iv(value);
		}

		void
		GL30GraphicsUniformSet::uniform2iv(std::size_t num, const std::int32_t* i2v) noexcept
		{
			_variant.uniform2iv(num, i2v);
		}

		void
		GL30GraphicsUniformSet::uniform3iv(const std::vector<math::int3>& value) noexcept
		{
			_variant.uniform3iv(value);
		}

		void
		GL30GraphicsUniformSet::uniform3iv(std::size_t num, const std::int32_t* i3v) noexcept
		{
			_variant.uniform3iv(num, i3v);
		}

		void
		GL30GraphicsUniformSet::uniform4iv(const std::vector<math::int4>& value) noexcept
		{
			_variant.uniform4iv(value);
		}

		void
		GL30GraphicsUniformSet::uniform4iv(std::size_t num, const std::int32_t* i4v) noexcept
		{
			_variant.uniform4iv(num, i4v);
		}

		void
		GL30GraphicsUniformSet::uniform1uiv(const std::vector<math::uint1>& value) noexcept
		{
			_variant.uniform1uiv(value);
		}

		void
		GL30GraphicsUniformSet::uniform1uiv(std::size_t num, const std::uint32_t* ui1v) noexcept
		{
			_variant.uniform1uiv(num, ui1v);
		}

		void
		GL30GraphicsUniformSet::uniform2uiv(const std::vector<math::uint2>& value) noexcept
		{
			_variant.uniform2uiv(value);
		}

		void
		GL30GraphicsUniformSet::uniform2uiv(std::size_t num, const std::uint32_t* ui2v) noexcept
		{
			_variant.uniform2uiv(num, ui2v);
		}

		void
		GL30GraphicsUniformSet::uniform3uiv(const std::vector<math::uint3>& value) noexcept
		{
			_variant.uniform3uiv(value);
		}

		void
		GL30GraphicsUniformSet::uniform3uiv(std::size_t num, const std::uint32_t* ui3v) noexcept
		{
			_variant.uniform3uiv(num, ui3v);
		}

		void
		GL30GraphicsUniformSet::uniform4uiv(const std::vector<math::uint4>& value) noexcept
		{
			_variant.uniform4uiv(value);
		}

		void
		GL30GraphicsUniformSet::uniform4uiv(std::size_t num, const std::uint32_t* ui4v) noexcept
		{
			_variant.uniform4uiv(num, ui4v);
		}

		void
		GL30GraphicsUniformSet::uniform1fv(const std::vector<math::float1>& value) noexcept
		{
			_variant.uniform1fv(value);
		}

		void
		GL30GraphicsUniformSet::uniform1fv(std::size_t num, const float* f1v) noexcept
		{
			_variant.uniform1fv(num, f1v);
		}

		void
		GL30GraphicsUniformSet::uniform2fv(const std::vector<math::float2>& value) noexcept
		{
			_variant.uniform2fv(value);
		}

		void
		GL30GraphicsUniformSet::uniform2fv(std::size_t num, const float* f2v) noexcept
		{
			_variant.uniform2fv(num, f2v);
		}

		void
		GL30GraphicsUniformSet::uniform3fv(const std::vector<math::float3>& value) noexcept
		{
			_variant.uniform3fv(value);
		}

		void
		GL30GraphicsUniformSet::uniform3fv(std::size_t num, const float* f3v) noexcept
		{
			_variant.uniform3fv(num, f3v);
		}

		void
		GL30GraphicsUniformSet::uniform4fv(const std::vector<math::float4>& value) noexcept
		{
			_variant.uniform4fv(value);
		}

		void
		GL30GraphicsUniformSet::uniform4fv(std::size_t num, const float* f4v) noexcept
		{
			_variant.uniform4fv(num, f4v);
		}

		void
		GL30GraphicsUniformSet::uniform2fmatv(const std::vector<math::float2x2>& value) noexcept
		{
			_variant.uniform2fmatv(value);
		}

		void
		GL30GraphicsUniformSet::uniform2fmatv(std::size_t num, const float* mat2) noexcept
		{
			_variant.uniform2fmatv(num, mat2);
		}

		void
		GL30GraphicsUniformSet::uniform3fmatv(const std::vector<math::float3x3>& value) noexcept
		{
			_variant.uniform3fmatv(value);
		}

		void
		GL30GraphicsUniformSet::uniform3fmatv(std::size_t num, const float* mat3) noexcept
		{
			_variant.uniform3fmatv(num, mat3);
		}

		void
		GL30GraphicsUniformSet::uniform4fmatv(const std::vector<math::float4x4>& value) noexcept
		{
			_variant.uniform4fmatv(value);
		}

		void
		GL30GraphicsUniformSet::uniform4fmatv(std::size_t num, const float* mat4) noexcept
		{
			_variant.uniform4fmatv(num, mat4);
		}

		void
		GL30GraphicsUniformSet::uniformTexture(std::shared_ptr<GraphicsTexture> texture, GraphicsSamplerPtr sampler) noexcept
		{
			_variant.uniformTexture(texture, sampler);
		}

		void
		GL30GraphicsUniformSet::uniformBuffer(GraphicsDataPtr ubo) noexcept
		{
			_variant.uniformBuffer(ubo);
		}

		bool
		GL30GraphicsUniformSet::getBool() const noexcept
		{
			return _variant.getBool();
		}

		int
		GL30GraphicsUniformSet::getInt() const noexcept
		{
			return _variant.getInt();
		}

		const math::int2&
		GL30GraphicsUniformSet::getInt2() const noexcept
		{
			return _variant.getInt2();
		}

		const math::int3&
		GL30GraphicsUniformSet::getInt3() const noexcept
		{
			return _variant.getInt3();
		}

		const math::int4&
		GL30GraphicsUniformSet::getInt4() const noexcept
		{
			return _variant.getInt4();
		}

		math::uint1
		GL30GraphicsUniformSet::getUInt() const noexcept
		{
			return _variant.getUInt();
		}

		const math::uint2&
		GL30GraphicsUniformSet::getUInt2() const noexcept
		{
			return _variant.getUInt2();
		}

		const math::uint3&
		GL30GraphicsUniformSet::getUInt3() const noexcept
		{
			return _variant.getUInt3();
		}

		const math::uint4&
		GL30GraphicsUniformSet::getUInt4() const noexcept
		{
			return _variant.getUInt4();
		}

		float
		GL30GraphicsUniformSet::getFloat() const noexcept
		{
			return _variant.getFloat();
		}

		const math::float2&
		GL30GraphicsUniformSet::getFloat2() const noexcept
		{
			return _variant.getFloat2();
		}

		const math::float3&
		GL30GraphicsUniformSet::getFloat3() const noexcept
		{
			return _variant.getFloat3();
		}

		const math::float4&
		GL30GraphicsUniformSet::getFloat4() const noexcept
		{
			return _variant.getFloat4();
		}

		const math::float2x2&
		GL30GraphicsUniformSet::getFloat2x2() const noexcept
		{
			return _variant.getFloat2x2();
		}

		const math::float3x3&
		GL30GraphicsUniformSet::getFloat3x3() const noexcept
		{
			return _variant.getFloat3x3();
		}

		const math::float4x4&
		GL30GraphicsUniformSet::getFloat4x4() const noexcept
		{
			return _variant.getFloat4x4();
		}

		const std::vector<math::int1>&
		GL30GraphicsUniformSet::getIntArray() const noexcept
		{
			return _variant.getIntArray();
		}

		const std::vector<math::int2>&
		GL30GraphicsUniformSet::getInt2Array() const noexcept
		{
			return _variant.getInt2Array();
		}

		const std::vector<math::int3>&
		GL30GraphicsUniformSet::getInt3Array() const noexcept
		{
			return _variant.getInt3Array();
		}

		const std::vector<math::int4>&
		GL30GraphicsUniformSet::getInt4Array() const noexcept
		{
			return _variant.getInt4Array();
		}

		const std::vector<math::uint1>&
		GL30GraphicsUniformSet::getUIntArray() const noexcept
		{
			return _variant.getUIntArray();
		}

		const std::vector<math::uint2>&
		GL30GraphicsUniformSet::getUInt2Array() const noexcept
		{
			return _variant.getUInt2Array();
		}

		const std::vector<math::uint3>&
		GL30GraphicsUniformSet::getUInt3Array() const noexcept
		{
			return _variant.getUInt3Array();
		}

		const std::vector<math::uint4>&
		GL30GraphicsUniformSet::getUInt4Array() const noexcept
		{
			return _variant.getUInt4Array();
		}

		const std::vector<math::float1>&
		GL30GraphicsUniformSet::getFloatArray() const noexcept
		{
			return _variant.getFloatArray();
		}

		const std::vector<math::float2>&
		GL30GraphicsUniformSet::getFloat2Array() const noexcept
		{
			return _variant.getFloat2Array();
		}

		const std::vector<math::float3>&
		GL30GraphicsUniformSet::getFloat3Array() const noexcept
		{
			return _variant.getFloat3Array();
		}

		const std::vector<math::float4>&
		GL30GraphicsUniformSet::getFloat4Array() const noexcept
		{
			return _variant.getFloat4Array();
		}

		const std::vector<math::float2x2>&
		GL30GraphicsUniformSet::getFloat2x2Array() const noexcept
		{
			return _variant.getFloat2x2Array();
		}

		const std::vector<math::float3x3>&
		GL30GraphicsUniformSet::getFloat3x3Array() const noexcept
		{
			return _variant.getFloat3x3Array();
		}

		const std::vector<math::float4x4>&
		GL30GraphicsUniformSet::getFloat4x4Array() const noexcept
		{
			return _variant.getFloat4x4Array();
		}

		const std::shared_ptr<GraphicsTexture>&
		GL30GraphicsUniformSet::getTexture() const noexcept
		{
			return _variant.getTexture();
		}

		const GraphicsSamplerPtr&
		GL30GraphicsUniformSet::getTextureSampler() const noexcept
		{
			return _variant.getTextureSampler();
		}

		const GraphicsDataPtr&
		GL30GraphicsUniformSet::getBuffer() const noexcept
		{
			return _variant.getBuffer();
		}

		void
		GL30GraphicsUniformSet::setGraphicsParam(GraphicsParamPtr param) noexcept
		{
			assert(param);
			_param = param;
			_variant.setType(param->getType());
		}

		const GraphicsParamPtr&
		GL30GraphicsUniformSet::getGraphicsParam() const noexcept
		{
			return _param;
		}

		GL30DescriptorPool::GL30DescriptorPool() noexcept
		{
		}

		GL30DescriptorPool::~GL30DescriptorPool() noexcept
		{
			this->close();
		}

		bool
		GL30DescriptorPool::setup(const GraphicsDescriptorPoolDesc& desc) noexcept
		{
			return true;
		}

		void
		GL30DescriptorPool::close() noexcept
		{
		}

		const GraphicsDescriptorPoolDesc&
		GL30DescriptorPool::getDescriptorPoolDesc() const noexcept
		{
			return _descriptorPoolDesc;
		}

		void
		GL30DescriptorPool::setDevice(GraphicsDevicePtr device) noexcept
		{
			_device = device;
		}

		GraphicsDevicePtr
		GL30DescriptorPool::getDevice() const noexcept
		{
			return _device.lock();
		}

		GL30DescriptorSetLayout::GL30DescriptorSetLayout() noexcept
		{
		}

		GL30DescriptorSetLayout::~GL30DescriptorSetLayout() noexcept
		{
			this->close();
		}

		bool
		GL30DescriptorSetLayout::setup(const GraphicsDescriptorSetLayoutDesc& descriptorSetLayoutDesc) noexcept
		{
			_descripotrSetLayoutDesc = descriptorSetLayoutDesc;
			return true;
		}

		void
		GL30DescriptorSetLayout::close() noexcept
		{
		}

		const GraphicsDescriptorSetLayoutDesc&
		GL30DescriptorSetLayout::getDescriptorSetLayoutDesc() const noexcept
		{
			return _descripotrSetLayoutDesc;
		}

		void
		GL30DescriptorSetLayout::setDevice(GraphicsDevicePtr device) noexcept
		{
			_device = device;
		}

		GraphicsDevicePtr
		GL30DescriptorSetLayout::getDevice() const noexcept
		{
			return _device.lock();
		}

		GL30DescriptorSet::GL30DescriptorSet() noexcept
		{
		}

		GL30DescriptorSet::~GL30DescriptorSet() noexcept
		{
			this->close();
		}

		bool
		GL30DescriptorSet::setup(const GraphicsDescriptorSetDesc& descriptorSetDesc) noexcept
		{
			assert(descriptorSetDesc.getDescriptorSetLayout());

			auto& descriptorSetLayoutDesc = descriptorSetDesc.getDescriptorSetLayout()->getDescriptorSetLayoutDesc();

			auto& params = descriptorSetLayoutDesc.getUniformComponents();
			for (auto& param : params)
			{
				auto uniformSet = std::make_shared<GL30GraphicsUniformSet>();
				uniformSet->setGraphicsParam(param);
				_activeUniformSets.push_back(uniformSet);
			}

			_descriptorSetDesc = descriptorSetDesc;
			return true;
		}

		void
		GL30DescriptorSet::close() noexcept
		{
			_activeUniformSets.clear();
		}

		void
		GL30DescriptorSet::apply(const GL30Program& shaderObject) noexcept
		{
			for (auto& it : _activeUniformSets)
			{
				auto type = it->getGraphicsParam()->getType();
				auto location = it->getGraphicsParam()->getBindingPoint();
				switch (type)
				{
				case UniformAttributeFormat::Boolean:
					GL_CHECK(glUniform1i(location, it->getBool()));
					break;
				case UniformAttributeFormat::Int:
					GL_CHECK(glUniform1i(location, it->getInt()));
					break;
				case UniformAttributeFormat::Int2:
					GL_CHECK(glUniform2iv(location, 1, (GLint*)it->getInt2().ptr()));
					break;
				case UniformAttributeFormat::Int3:
					GL_CHECK(glUniform3iv(location, 1, (GLint*)it->getInt3().ptr()));
					break;
				case UniformAttributeFormat::Int4:
					GL_CHECK(glUniform4iv(location, 1, (GLint*)it->getInt4().ptr()));
					break;
				case UniformAttributeFormat::UInt:
					GL_CHECK(glUniform1ui(location, it->getUInt()));
					break;
				case UniformAttributeFormat::UInt2:
					GL_CHECK(glUniform2uiv(location, 1, (GLuint*)it->getUInt2().ptr()));
					break;
				case UniformAttributeFormat::UInt3:
					GL_CHECK(glUniform3uiv(location, 1, (GLuint*)it->getUInt3().ptr()));
					break;
				case UniformAttributeFormat::UInt4:
					GL_CHECK(glUniform4uiv(location, 1, (GLuint*)it->getUInt4().ptr()));
					break;
				case UniformAttributeFormat::Float:
					GL_CHECK(glUniform1f(location, it->getFloat()));
					break;
				case UniformAttributeFormat::Float2:
					GL_CHECK(glUniform2fv(location, 1, it->getFloat2().ptr()));
					break;
				case UniformAttributeFormat::Float3:
					GL_CHECK(glUniform3fv(location, 1, it->getFloat3().ptr()));
					break;
				case UniformAttributeFormat::Float4:
					GL_CHECK(glUniform4fv(location, 1, it->getFloat4().ptr()));
					break;
				case UniformAttributeFormat::Float3x3:
					GL_CHECK(glUniformMatrix3fv(location, 1, GL_FALSE, it->getFloat3x3().ptr()));
					break;
				case UniformAttributeFormat::Float4x4:
					GL_CHECK(glUniformMatrix4fv(location, 1, GL_FALSE, it->getFloat4x4().ptr()));
					break;
				case UniformAttributeFormat::IntArray:
					GL_CHECK(glUniform1iv(location, (GLsizei)it->getIntArray().size(), it->getIntArray().data()));
					break;
				case UniformAttributeFormat::Int2Array:
					GL_CHECK(glUniform2iv(location, (GLsizei)it->getInt2Array().size(), (GLint*)it->getInt2Array().data()));
					break;
				case UniformAttributeFormat::Int3Array:
					GL_CHECK(glUniform3iv(location, (GLsizei)it->getInt3Array().size(), (GLint*)it->getInt3Array().data()));
					break;
				case UniformAttributeFormat::Int4Array:
					GL_CHECK(glUniform4iv(location, (GLsizei)it->getInt4Array().size(), (GLint*)it->getInt4Array().data()));
					break;
				case UniformAttributeFormat::UIntArray:
					GL_CHECK(glUniform1uiv(location, (GLsizei)it->getUIntArray().size(), it->getUIntArray().data()));
					break;
				case UniformAttributeFormat::UInt2Array:
					GL_CHECK(glUniform2uiv(location, (GLsizei)it->getUInt2Array().size(), (GLuint*)it->getUInt2Array().data()));
					break;
				case UniformAttributeFormat::UInt3Array:
					GL_CHECK(glUniform3uiv(location, (GLsizei)it->getUInt3Array().size(), (GLuint*)it->getUInt3Array().data()));
					break;
				case UniformAttributeFormat::UInt4Array:
					GL_CHECK(glUniform4uiv(location, (GLsizei)it->getUInt4Array().size(), (GLuint*)it->getUInt4Array().data()));
					break;
				case UniformAttributeFormat::FloatArray:
					GL_CHECK(glUniform1fv(location, (GLsizei)it->getFloatArray().size(), (GLfloat*)it->getFloatArray().data()));
					break;
				case UniformAttributeFormat::Float2Array:
					GL_CHECK(glUniform2fv(location, (GLsizei)it->getFloat2Array().size(), (GLfloat*)it->getFloat2Array().data()));
					break;
				case UniformAttributeFormat::Float3Array:
					GL_CHECK(glUniform3fv(location, (GLsizei)it->getFloat3Array().size(), (GLfloat*)it->getFloat3Array().data()));
					break;
				case UniformAttributeFormat::Float4Array:
					GL_CHECK(glUniform4fv(location, (GLsizei)it->getFloat4Array().size(), (GLfloat*)it->getFloat4Array().data()));
					break;
				case UniformAttributeFormat::Float3x3Array:
					GL_CHECK(glUniformMatrix3fv(location, (GLsizei)it->getFloat3x3Array().size(), GL_FALSE, (GLfloat*)it->getFloat3x3Array().data()));
					break;
				case UniformAttributeFormat::Float4x4Array:
					GL_CHECK(glUniformMatrix4fv(location, (GLsizei)it->getFloat4x4Array().size(), GL_FALSE, (GLfloat*)it->getFloat4x4Array().data()));
					break;
				break;
				case UniformAttributeFormat::Sampler:
				case UniformAttributeFormat::SamplerImage:
				case UniformAttributeFormat::CombinedImageSampler:
				case UniformAttributeFormat::StorageImage:
				{
					auto& texture = it->getTexture();
					if (texture)
					{
						auto gltexture = texture->downcast<GL30Texture>();
						GL_CHECK(glActiveTexture(GL_TEXTURE0 + location));
						GL_CHECK(glBindTexture(gltexture->getTarget(), gltexture->getInstanceID()));

						auto sampler = it->getTextureSampler();
						if (sampler)
						{
							auto instance = sampler->downcast<GL30Sampler>()->getInstanceID();
							glBindSampler(location, instance);
						}
					}
					else
					{
						GL_CHECK(glActiveTexture(GL_TEXTURE0 + location));
						GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));
					}
				}
				break;
				case UniformAttributeFormat::StorageTexelBuffer:
					break;
				case UniformAttributeFormat::StorageBuffer:
					break;
				case UniformAttributeFormat::StorageBufferDynamic:
					break;
				case UniformAttributeFormat::UniformTexelBuffer:
					break;
				case UniformAttributeFormat::UniformBuffer:
					break;
				case UniformAttributeFormat::UniformBufferDynamic:
					break;
				case UniformAttributeFormat::InputAttachment:
					break;
				default:
					break;
				}
			}
		}

		void
		GL30DescriptorSet::copy(std::uint32_t descriptorCopyCount, const GraphicsDescriptorSetPtr descriptorCopies[]) noexcept
		{
			for (std::size_t i = 0; i < descriptorCopyCount; i++)
			{
				if (!descriptorCopies[i])
					continue;

				auto descriptorCope = descriptorCopies[i]->downcast<GL30DescriptorSet>();
				for (auto& activeUniformSet : descriptorCope->_activeUniformSets)
				{
					auto it = std::find_if(_activeUniformSets.begin(), _activeUniformSets.end(), [&](GraphicsUniformSetPtr& it) { return it->getGraphicsParam() == activeUniformSet->getGraphicsParam(); });
					if (it == _activeUniformSets.end())
						continue;

					auto type = activeUniformSet->getGraphicsParam()->getType();
					switch (type)
					{
					case UniformAttributeFormat::Boolean:
						(*it)->uniform1b(activeUniformSet->getBool());
						break;
					case UniformAttributeFormat::Int:
						(*it)->uniform1i(activeUniformSet->getInt());
						break;
					case UniformAttributeFormat::Int2:
						(*it)->uniform2i(activeUniformSet->getInt2());
						break;
					case UniformAttributeFormat::Int3:
						(*it)->uniform3i(activeUniformSet->getInt3());
						break;
					case UniformAttributeFormat::Int4:
						(*it)->uniform4i(activeUniformSet->getInt4());
						break;
					case UniformAttributeFormat::UInt:
						(*it)->uniform1ui(activeUniformSet->getUInt());
						break;
					case UniformAttributeFormat::UInt2:
						(*it)->uniform2ui(activeUniformSet->getUInt2());
						break;
					case UniformAttributeFormat::UInt3:
						(*it)->uniform3ui(activeUniformSet->getUInt3());
						break;
					case UniformAttributeFormat::UInt4:
						(*it)->uniform4ui(activeUniformSet->getUInt4());
						break;
					case UniformAttributeFormat::Float:
						(*it)->uniform1f(activeUniformSet->getFloat());
						break;
					case UniformAttributeFormat::Float2:
						(*it)->uniform2f(activeUniformSet->getFloat2());
						break;
					case UniformAttributeFormat::Float3:
						(*it)->uniform3f(activeUniformSet->getFloat3());
						break;
					case UniformAttributeFormat::Float4:
						(*it)->uniform4f(activeUniformSet->getFloat4());
						break;
					case UniformAttributeFormat::Float2x2:
						(*it)->uniform2fmat(activeUniformSet->getFloat2x2());
						break;
					case UniformAttributeFormat::Float3x3:
						(*it)->uniform3fmat(activeUniformSet->getFloat3x3());
						break;
					case UniformAttributeFormat::Float4x4:
						(*it)->uniform4fmat(activeUniformSet->getFloat4x4());
						break;
					case UniformAttributeFormat::IntArray:
						(*it)->uniform1iv(activeUniformSet->getIntArray());
						break;
					case UniformAttributeFormat::Int2Array:
						(*it)->uniform2iv(activeUniformSet->getInt2Array());
						break;
					case UniformAttributeFormat::Int3Array:
						(*it)->uniform3iv(activeUniformSet->getInt3Array());
						break;
					case UniformAttributeFormat::Int4Array:
						(*it)->uniform4iv(activeUniformSet->getInt4Array());
						break;
					case UniformAttributeFormat::UIntArray:
						(*it)->uniform1uiv(activeUniformSet->getUIntArray());
						break;
					case UniformAttributeFormat::UInt2Array:
						(*it)->uniform2uiv(activeUniformSet->getUInt2Array());
						break;
					case UniformAttributeFormat::UInt3Array:
						(*it)->uniform3uiv(activeUniformSet->getUInt3Array());
						break;
					case UniformAttributeFormat::UInt4Array:
						(*it)->uniform4uiv(activeUniformSet->getUInt4Array());
						break;
					case UniformAttributeFormat::FloatArray:
						(*it)->uniform1fv(activeUniformSet->getFloatArray());
						break;
					case UniformAttributeFormat::Float2Array:
						(*it)->uniform2fv(activeUniformSet->getFloat2Array());
						break;
					case UniformAttributeFormat::Float3Array:
						(*it)->uniform3fv(activeUniformSet->getFloat3Array());
						break;
					case UniformAttributeFormat::Float4Array:
						(*it)->uniform4fv(activeUniformSet->getFloat4Array());
						break;
					case UniformAttributeFormat::Float2x2Array:
						(*it)->uniform2fmatv(activeUniformSet->getFloat2x2Array());
						break;
					case UniformAttributeFormat::Float3x3Array:
						(*it)->uniform3fmatv(activeUniformSet->getFloat3x3Array());
						break;
					case UniformAttributeFormat::Float4x4Array:
						(*it)->uniform4fmatv(activeUniformSet->getFloat4x4Array());
						break;
					case UniformAttributeFormat::Sampler:
						(*it)->uniformTexture(activeUniformSet->getTexture(), activeUniformSet->getTextureSampler());
						break;
					case UniformAttributeFormat::SamplerImage:
						(*it)->uniformTexture(activeUniformSet->getTexture(), activeUniformSet->getTextureSampler());
						break;
					case UniformAttributeFormat::CombinedImageSampler:
						(*it)->uniformTexture(activeUniformSet->getTexture(), activeUniformSet->getTextureSampler());
						break;
					case UniformAttributeFormat::StorageImage:
						(*it)->uniformTexture(activeUniformSet->getTexture(), activeUniformSet->getTextureSampler());
						break;
					case UniformAttributeFormat::StorageTexelBuffer:
						break;
					case UniformAttributeFormat::StorageBuffer:
						break;
					case UniformAttributeFormat::StorageBufferDynamic:
						break;
					case UniformAttributeFormat::UniformTexelBuffer:
						break;
					case UniformAttributeFormat::UniformBuffer:
						(*it)->uniformBuffer(activeUniformSet->getBuffer());
						break;
					case UniformAttributeFormat::UniformBufferDynamic:
						break;
					case UniformAttributeFormat::InputAttachment:
						break;
					default:
						break;
					}
				}
			}
		}

		const GraphicsUniformSets&
		GL30DescriptorSet::getUniformSets() const noexcept
		{
			return _activeUniformSets;
		}

		const GraphicsDescriptorSetDesc&
		GL30DescriptorSet::getDescriptorSetDesc() const noexcept
		{
			return _descriptorSetDesc;
		}

		void
		GL30DescriptorSet::setDevice(GraphicsDevicePtr device) noexcept
		{
			_device = device;
		}

		GraphicsDevicePtr
		GL30DescriptorSet::getDevice() const noexcept
		{
			return _device.lock();
		}
	}
}