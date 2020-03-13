#include "gl20_descriptor_set.h"
#include "gl20_texture.h"
#include "gl20_shader.h"
#include "gl20_sampler.h"

namespace octoon
{
	namespace hal
	{
		OctoonImplementSubClass(GL20GraphicsUniformSet, GraphicsUniformSet, "GL20GraphicsUniformSet")
		OctoonImplementSubClass(GL20DescriptorSet, GraphicsDescriptorSet, "GL20DescriptorSet")
		OctoonImplementSubClass(GL20DescriptorSetLayout, GraphicsDescriptorSetLayout, "GL20DescriptorSetLayout")
		OctoonImplementSubClass(GL20DescriptorPool, GraphicsDescriptorPool, "GL20DescriptorPool")

		GL20GraphicsUniformSet::GL20GraphicsUniformSet() noexcept
		{
		}

		GL20GraphicsUniformSet::~GL20GraphicsUniformSet() noexcept
		{
		}

		const std::string&
		GL20GraphicsUniformSet::getName() const noexcept
		{
			assert(_param);
			return _param->getName();
		}

		void
		GL20GraphicsUniformSet::uniform1b(bool value) noexcept
		{
			_variant.uniform1b(value);
		}

		void
		GL20GraphicsUniformSet::uniform1i(std::int32_t i1) noexcept
		{
			_variant.uniform1i(i1);
		}

		void
		GL20GraphicsUniformSet::uniform2i(const int2& value) noexcept
		{
			_variant.uniform2i(value);
		}

		void
		GL20GraphicsUniformSet::uniform2i(std::int32_t i1, std::int32_t i2) noexcept
		{
			_variant.uniform2i(i1, i2);
		}

		void
		GL20GraphicsUniformSet::uniform3i(const int3& value) noexcept
		{
			_variant.uniform3i(value);
		}

		void
		GL20GraphicsUniformSet::uniform3i(std::int32_t i1, std::int32_t i2, std::int32_t i3) noexcept
		{
			_variant.uniform3i(i1, i2, i3);
		}

		void
		GL20GraphicsUniformSet::uniform4i(const int4& value) noexcept
		{
			_variant.uniform4i(value);
		}

		void
		GL20GraphicsUniformSet::uniform4i(std::int32_t i1, std::int32_t i2, std::int32_t i3, std::int32_t i4) noexcept
		{
			_variant.uniform4i(i1, i2, i3, i4);
		}

		void
		GL20GraphicsUniformSet::uniform1ui(std::uint32_t ui1) noexcept
		{
			_variant.uniform1ui(ui1);
		}

		void
		GL20GraphicsUniformSet::uniform2ui(const uint2& value) noexcept
		{
			_variant.uniform2ui(value);
		}

		void
		GL20GraphicsUniformSet::uniform2ui(std::uint32_t ui1, std::uint32_t ui2) noexcept
		{
			_variant.uniform2ui(ui1, ui2);
		}

		void
		GL20GraphicsUniformSet::uniform3ui(const uint3& value) noexcept
		{
			_variant.uniform3ui(value);
		}

		void
		GL20GraphicsUniformSet::uniform3ui(std::uint32_t ui1, std::uint32_t ui2, std::uint32_t ui3) noexcept
		{
			_variant.uniform3ui(ui1, ui2, ui3);
		}

		void
		GL20GraphicsUniformSet::uniform4ui(const uint4& value) noexcept
		{
			_variant.uniform4ui(value);
		}

		void
		GL20GraphicsUniformSet::uniform4ui(std::uint32_t ui1, std::uint32_t ui2, std::uint32_t ui3, std::uint32_t ui4) noexcept
		{
			_variant.uniform4ui(ui1, ui2, ui3, ui4);
		}

		void
		GL20GraphicsUniformSet::uniform1f(float f1) noexcept
		{
			_variant.uniform1f(f1);
		}

		void
		GL20GraphicsUniformSet::uniform2f(const float2& value) noexcept
		{
			_variant.uniform2f(value);
		}

		void
		GL20GraphicsUniformSet::uniform2f(float f1, float f2) noexcept
		{
			_variant.uniform2f(f1, f2);
		}

		void
		GL20GraphicsUniformSet::uniform3f(const float3& value) noexcept
		{
			_variant.uniform3f(value);
		}

		void
		GL20GraphicsUniformSet::uniform3f(float f1, float f2, float f3) noexcept
		{
			_variant.uniform3f(f1, f2, f3);
		}

		void
		GL20GraphicsUniformSet::uniform4f(const float4& value) noexcept
		{
			_variant.uniform4f(value);
		}

		void
		GL20GraphicsUniformSet::uniform4f(float f1, float f2, float f3, float f4) noexcept
		{
			_variant.uniform4f(f1, f2, f3, f4);
		}

		void
		GL20GraphicsUniformSet::uniform2fmat(const float2x2& value) noexcept
		{
			_variant.uniform2fmat(value);
		}

		void
		GL20GraphicsUniformSet::uniform2fmat(const float* mat2) noexcept
		{
			_variant.uniform2fmat(mat2);
		}

		void
		GL20GraphicsUniformSet::uniform3fmat(const float3x3& value) noexcept
		{
			_variant.uniform3fmat(value);
		}

		void
		GL20GraphicsUniformSet::uniform3fmat(const float* mat3) noexcept
		{
			_variant.uniform3fmat(mat3);
		}

		void
		GL20GraphicsUniformSet::uniform4fmat(const float4x4& value) noexcept
		{
			_variant.uniform4fmat(value);
		}

		void
		GL20GraphicsUniformSet::uniform4fmat(const float* mat4) noexcept
		{
			_variant.uniform4fmat(mat4);
		}

		void
		GL20GraphicsUniformSet::uniform1iv(const std::vector<int1>& value) noexcept
		{
			_variant.uniform1iv(value);
		}

		void
		GL20GraphicsUniformSet::uniform1iv(std::size_t num, const std::int32_t* i1v) noexcept
		{
			_variant.uniform1iv(num, i1v);
		}

		void
		GL20GraphicsUniformSet::uniform2iv(const std::vector<int2>& value) noexcept
		{
			_variant.uniform2iv(value);
		}

		void
		GL20GraphicsUniformSet::uniform2iv(std::size_t num, const std::int32_t* i2v) noexcept
		{
			_variant.uniform2iv(num, i2v);
		}

		void
		GL20GraphicsUniformSet::uniform3iv(const std::vector<int3>& value) noexcept
		{
			_variant.uniform3iv(value);
		}

		void
		GL20GraphicsUniformSet::uniform3iv(std::size_t num, const std::int32_t* i3v) noexcept
		{
			_variant.uniform3iv(num, i3v);
		}

		void
		GL20GraphicsUniformSet::uniform4iv(const std::vector<int4>& value) noexcept
		{
			_variant.uniform4iv(value);
		}

		void
		GL20GraphicsUniformSet::uniform4iv(std::size_t num, const std::int32_t* i4v) noexcept
		{
			_variant.uniform4iv(num, i4v);
		}

		void
		GL20GraphicsUniformSet::uniform1uiv(const std::vector<uint1>& value) noexcept
		{
			_variant.uniform1uiv(value);
		}

		void
		GL20GraphicsUniformSet::uniform1uiv(std::size_t num, const std::uint32_t* ui1v) noexcept
		{
			_variant.uniform1uiv(num, ui1v);
		}

		void
		GL20GraphicsUniformSet::uniform2uiv(const std::vector<uint2>& value) noexcept
		{
			_variant.uniform2uiv(value);
		}

		void
		GL20GraphicsUniformSet::uniform2uiv(std::size_t num, const std::uint32_t* ui2v) noexcept
		{
			_variant.uniform2uiv(num, ui2v);
		}

		void
		GL20GraphicsUniformSet::uniform3uiv(const std::vector<uint3>& value) noexcept
		{
			_variant.uniform3uiv(value);
		}

		void
		GL20GraphicsUniformSet::uniform3uiv(std::size_t num, const std::uint32_t* ui3v) noexcept
		{
			_variant.uniform3uiv(num, ui3v);
		}

		void
		GL20GraphicsUniformSet::uniform4uiv(const std::vector<uint4>& value) noexcept
		{
			_variant.uniform4uiv(value);
		}

		void
		GL20GraphicsUniformSet::uniform4uiv(std::size_t num, const std::uint32_t* ui4v) noexcept
		{
			_variant.uniform4uiv(num, ui4v);
		}

		void
		GL20GraphicsUniformSet::uniform1fv(const std::vector<float1>& value) noexcept
		{
			_variant.uniform1fv(value);
		}

		void
		GL20GraphicsUniformSet::uniform1fv(std::size_t num, const float* f1v) noexcept
		{
			_variant.uniform1fv(num, f1v);
		}

		void
		GL20GraphicsUniformSet::uniform2fv(const std::vector<float2>& value) noexcept
		{
			_variant.uniform2fv(value);
		}

		void
		GL20GraphicsUniformSet::uniform2fv(std::size_t num, const float* f2v) noexcept
		{
			_variant.uniform2fv(num, f2v);
		}

		void
		GL20GraphicsUniformSet::uniform3fv(const std::vector<float3>& value) noexcept
		{
			_variant.uniform3fv(value);
		}

		void
		GL20GraphicsUniformSet::uniform3fv(std::size_t num, const float* f3v) noexcept
		{
			_variant.uniform3fv(num, f3v);
		}

		void
		GL20GraphicsUniformSet::uniform4fv(const std::vector<float4>& value) noexcept
		{
			_variant.uniform4fv(value);
		}

		void
		GL20GraphicsUniformSet::uniform4fv(std::size_t num, const float* f4v) noexcept
		{
			_variant.uniform4fv(num, f4v);
		}

		void
		GL20GraphicsUniformSet::uniform2fmatv(const std::vector<float2x2>& value) noexcept
		{
			_variant.uniform2fmatv(value);
		}

		void
		GL20GraphicsUniformSet::uniform2fmatv(std::size_t num, const float* mat2) noexcept
		{
			_variant.uniform2fmatv(num, mat2);
		}

		void
		GL20GraphicsUniformSet::uniform3fmatv(const std::vector<float3x3>& value) noexcept
		{
			_variant.uniform3fmatv(value);
		}

		void
		GL20GraphicsUniformSet::uniform3fmatv(std::size_t num, const float* mat3) noexcept
		{
			_variant.uniform3fmatv(num, mat3);
		}

		void
		GL20GraphicsUniformSet::uniform4fmatv(const std::vector<float4x4>& value) noexcept
		{
			_variant.uniform4fmatv(value);
		}

		void
		GL20GraphicsUniformSet::uniform4fmatv(std::size_t num, const float* mat4) noexcept
		{
			_variant.uniform4fmatv(num, mat4);
		}

		void
		GL20GraphicsUniformSet::uniformTexture(GraphicsTexturePtr texture, GraphicsSamplerPtr sampler) noexcept
		{
			_variant.uniformTexture(texture, sampler);
		}

		void
		GL20GraphicsUniformSet::uniformBuffer(GraphicsDataPtr ubo) noexcept
		{
			_variant.uniformBuffer(ubo);
		}

		bool
		GL20GraphicsUniformSet::getBool() const noexcept
		{
			return _variant.getBool();
		}

		int
		GL20GraphicsUniformSet::getInt() const noexcept
		{
			return _variant.getInt();
		}

		const int2&
		GL20GraphicsUniformSet::getInt2() const noexcept
		{
			return _variant.getInt2();
		}

		const int3&
		GL20GraphicsUniformSet::getInt3() const noexcept
		{
			return _variant.getInt3();
		}

		const int4&
		GL20GraphicsUniformSet::getInt4() const noexcept
		{
			return _variant.getInt4();
		}

		uint1
		GL20GraphicsUniformSet::getUInt() const noexcept
		{
			return _variant.getUInt();
		}

		const uint2&
		GL20GraphicsUniformSet::getUInt2() const noexcept
		{
			return _variant.getUInt2();
		}

		const uint3&
		GL20GraphicsUniformSet::getUInt3() const noexcept
		{
			return _variant.getUInt3();
		}

		const uint4&
		GL20GraphicsUniformSet::getUInt4() const noexcept
		{
			return _variant.getUInt4();
		}

		float
		GL20GraphicsUniformSet::getFloat() const noexcept
		{
			return _variant.getFloat();
		}

		const float2&
		GL20GraphicsUniformSet::getFloat2() const noexcept
		{
			return _variant.getFloat2();
		}

		const float3&
		GL20GraphicsUniformSet::getFloat3() const noexcept
		{
			return _variant.getFloat3();
		}

		const float4&
		GL20GraphicsUniformSet::getFloat4() const noexcept
		{
			return _variant.getFloat4();
		}

		const float2x2&
		GL20GraphicsUniformSet::getFloat2x2() const noexcept
		{
			return _variant.getFloat2x2();
		}

		const float3x3&
		GL20GraphicsUniformSet::getFloat3x3() const noexcept
		{
			return _variant.getFloat3x3();
		}

		const float4x4&
		GL20GraphicsUniformSet::getFloat4x4() const noexcept
		{
			return _variant.getFloat4x4();
		}

		const std::vector<int1>&
		GL20GraphicsUniformSet::getIntArray() const noexcept
		{
			return _variant.getIntArray();
		}

		const std::vector<int2>&
		GL20GraphicsUniformSet::getInt2Array() const noexcept
		{
			return _variant.getInt2Array();
		}

		const std::vector<int3>&
		GL20GraphicsUniformSet::getInt3Array() const noexcept
		{
			return _variant.getInt3Array();
		}

		const std::vector<int4>&
		GL20GraphicsUniformSet::getInt4Array() const noexcept
		{
			return _variant.getInt4Array();
		}

		const std::vector<uint1>&
		GL20GraphicsUniformSet::getUIntArray() const noexcept
		{
			return _variant.getUIntArray();
		}

		const std::vector<uint2>&
		GL20GraphicsUniformSet::getUInt2Array() const noexcept
		{
			return _variant.getUInt2Array();
		}

		const std::vector<uint3>&
		GL20GraphicsUniformSet::getUInt3Array() const noexcept
		{
			return _variant.getUInt3Array();
		}

		const std::vector<uint4>&
		GL20GraphicsUniformSet::getUInt4Array() const noexcept
		{
			return _variant.getUInt4Array();
		}

		const std::vector<float1>&
		GL20GraphicsUniformSet::getFloatArray() const noexcept
		{
			return _variant.getFloatArray();
		}

		const std::vector<float2>&
		GL20GraphicsUniformSet::getFloat2Array() const noexcept
		{
			return _variant.getFloat2Array();
		}

		const std::vector<float3>&
		GL20GraphicsUniformSet::getFloat3Array() const noexcept
		{
			return _variant.getFloat3Array();
		}

		const std::vector<float4>&
		GL20GraphicsUniformSet::getFloat4Array() const noexcept
		{
			return _variant.getFloat4Array();
		}

		const std::vector<float2x2>&
		GL20GraphicsUniformSet::getFloat2x2Array() const noexcept
		{
			return _variant.getFloat2x2Array();
		}

		const std::vector<float3x3>&
		GL20GraphicsUniformSet::getFloat3x3Array() const noexcept
		{
			return _variant.getFloat3x3Array();
		}

		const std::vector<float4x4>&
		GL20GraphicsUniformSet::getFloat4x4Array() const noexcept
		{
			return _variant.getFloat4x4Array();
		}

		const GraphicsTexturePtr&
		GL20GraphicsUniformSet::getTexture() const noexcept
		{
			return _variant.getTexture();
		}

		const GraphicsSamplerPtr&
		GL20GraphicsUniformSet::getTextureSampler() const noexcept
		{
			return _variant.getTextureSampler();
		}

		const GraphicsDataPtr&
		GL20GraphicsUniformSet::getBuffer() const noexcept
		{
			return _variant.getBuffer();
		}

		void
		GL20GraphicsUniformSet::setGraphicsParam(GraphicsParamPtr param) noexcept
		{
			assert(param);
			_param = param;
			_variant.setType(param->getType());
		}

		const GraphicsParamPtr&
		GL20GraphicsUniformSet::getGraphicsParam() const noexcept
		{
			return _param;
		}

		GL20DescriptorPool::GL20DescriptorPool() noexcept
		{
		}

		GL20DescriptorPool::~GL20DescriptorPool() noexcept
		{
			this->close();
		}

		bool
		GL20DescriptorPool::setup(const GraphicsDescriptorPoolDesc& desc) noexcept
		{
			return true;
		}

		void
		GL20DescriptorPool::close() noexcept
		{
		}

		const GraphicsDescriptorPoolDesc&
		GL20DescriptorPool::getDescriptorPoolDesc() const noexcept
		{
			return _descriptorPoolDesc;
		}

		void
		GL20DescriptorPool::setDevice(GraphicsDevicePtr device) noexcept
		{
			_device = device;
		}

		GraphicsDevicePtr
		GL20DescriptorPool::getDevice() noexcept
		{
			return _device.lock();
		}

		GL20DescriptorSetLayout::GL20DescriptorSetLayout() noexcept
		{
		}

		GL20DescriptorSetLayout::~GL20DescriptorSetLayout() noexcept
		{
			this->close();
		}

		bool
		GL20DescriptorSetLayout::setup(const GraphicsDescriptorSetLayoutDesc& descriptorSetDesc) noexcept
		{
			_descriptorSetDesc = descriptorSetDesc;
			return true;
		}

		void
		GL20DescriptorSetLayout::close() noexcept
		{
		}

		const GraphicsDescriptorSetLayoutDesc&
		GL20DescriptorSetLayout::getDescriptorSetLayoutDesc() const noexcept
		{
			return _descriptorSetDesc;
		}

		void
		GL20DescriptorSetLayout::setDevice(GraphicsDevicePtr device) noexcept
		{
			_device = device;
		}

		GraphicsDevicePtr
		GL20DescriptorSetLayout::getDevice() noexcept
		{
			return _device.lock();
		}

		GL20DescriptorSet::GL20DescriptorSet() noexcept
		{
		}

		GL20DescriptorSet::~GL20DescriptorSet() noexcept
		{
			this->close();
		}

		bool
		GL20DescriptorSet::setup(const GraphicsDescriptorSetDesc& descriptorSetDesc) noexcept
		{
			assert(descriptorSetDesc.getDescriptorSetLayout());

			auto& descriptorSetLayoutDesc = descriptorSetDesc.getDescriptorSetLayout()->getDescriptorSetLayoutDesc();
			auto& uniforms = descriptorSetLayoutDesc.getUniformComponents();
			for (auto& uniform : uniforms)
			{
				auto uniformSet = std::make_shared<GL20GraphicsUniformSet>();
				uniformSet->setGraphicsParam(uniform);
				_activeUniformSets.push_back(uniformSet);
			}

			_descriptorSetDesc = descriptorSetDesc;
			return true;
		}

		void
		GL20DescriptorSet::close() noexcept
		{
			_activeUniformSets.clear();
		}

		void
		GL20DescriptorSet::apply(const GL20Program& shaderObject) noexcept
		{
			for (auto& it : _activeUniformSets)
			{
				auto type = it->getGraphicsParam()->getType();
				auto location = it->getGraphicsParam()->getBindingPoint();
				switch (type)
				{
				case GraphicsUniformType::Boolean:
					GL_CHECK(glUniform1i(location, it->getBool()));
					break;
				case GraphicsUniformType::Int:
					GL_CHECK(glUniform1i(location, it->getInt()));
					break;
				case GraphicsUniformType::Int2:
					GL_CHECK(glUniform2iv(location, 1, (GLint*)it->getInt2().ptr()));
					break;
				case GraphicsUniformType::Int3:
					GL_CHECK(glUniform3iv(location, 1, (GLint*)it->getInt3().ptr()));
					break;
				case GraphicsUniformType::Int4:
					GL_CHECK(glUniform4iv(location, 1, (GLint*)it->getInt4().ptr()));
					break;
				case GraphicsUniformType::Float:
					GL_CHECK(glUniform1f(location, it->getFloat()));
					break;
				case GraphicsUniformType::Float2:
					GL_CHECK(glUniform2fv(location, 1, it->getFloat2().ptr()));
					break;
				case GraphicsUniformType::Float3:
					GL_CHECK(glUniform3fv(location, 1, it->getFloat3().ptr()));
					break;
				case GraphicsUniformType::Float4:
					GL_CHECK(glUniform4fv(location, 1, it->getFloat4().ptr()));
					break;
				case GraphicsUniformType::Float3x3:
					GL_CHECK(glUniformMatrix3fv(location, 1, GL_FALSE, it->getFloat3x3().ptr()));
					break;
				case GraphicsUniformType::Float4x4:
					GL_CHECK(glUniformMatrix4fv(location, 1, GL_FALSE, it->getFloat4x4().ptr()));
					break;
				case GraphicsUniformType::IntArray:
					GL_CHECK(glUniform1iv(location, (GLsizei)it->getIntArray().size(), it->getIntArray().data()));
					break;
				case GraphicsUniformType::Int2Array:
					GL_CHECK(glUniform2iv(location, (GLsizei)it->getInt2Array().size(), (GLint*)it->getInt2Array().data()));
					break;
				case GraphicsUniformType::Int3Array:
					GL_CHECK(glUniform3iv(location, (GLsizei)it->getInt3Array().size(), (GLint*)it->getInt3Array().data()));
					break;
				case GraphicsUniformType::Int4Array:
					GL_CHECK(glUniform4iv(location, (GLsizei)it->getInt4Array().size(), (GLint*)it->getInt4Array().data()));
					break;
				case GraphicsUniformType::FloatArray:
					GL_CHECK(glUniform1fv(location, (GLsizei)it->getFloatArray().size(), (GLfloat*)it->getFloatArray().data()));
					break;
				case GraphicsUniformType::Float2Array:
					GL_CHECK(glUniform2fv(location, (GLsizei)it->getFloat2Array().size(), (GLfloat*)it->getFloat2Array().data()));
					break;
				case GraphicsUniformType::Float3Array:
					GL_CHECK(glUniform3fv(location, (GLsizei)it->getFloat3Array().size(), (GLfloat*)it->getFloat3Array().data()));
					break;
				case GraphicsUniformType::Float4Array:
					GL_CHECK(glUniform4fv(location, (GLsizei)it->getFloat4Array().size(), (GLfloat*)it->getFloat4Array().data()));
					break;
				case GraphicsUniformType::Float3x3Array:
					GL_CHECK(glUniformMatrix3fv(location, (GLsizei)it->getFloat3x3Array().size(), GL_FALSE, (GLfloat*)it->getFloat3x3Array().data()));
					break;
				case GraphicsUniformType::Float4x4Array:
					GL_CHECK(glUniformMatrix4fv(location, (GLsizei)it->getFloat4x4Array().size(), GL_FALSE, (GLfloat*)it->getFloat4x4Array().data()));
					break;
				case GraphicsUniformType::Sampler:
					assert(false);
					break;
				case GraphicsUniformType::CombinedImageSampler:
				case GraphicsUniformType::StorageImage:
				case GraphicsUniformType::SamplerImage:
					{
						auto& texture = it->getTexture();
						if (texture)
						{
							auto gltexture = texture->downcast<GL20Texture>();
							GL_CHECK(glActiveTexture(GL_TEXTURE0 + location));
							GL_CHECK(glBindTexture(gltexture->getTarget(), gltexture->getInstanceID()));
							gltexture->setSampler(it->getTextureSampler());
						}
					}
					break;
				case GraphicsUniformType::StorageTexelBuffer:
				case GraphicsUniformType::UniformTexelBuffer:
					GL_PLATFORM_LOG("Can't support TBO.");
					break;
				case GraphicsUniformType::StorageBuffer:
				case GraphicsUniformType::StorageBufferDynamic:
				case GraphicsUniformType::UniformBuffer:
				case GraphicsUniformType::UniformBufferDynamic:
					GL_PLATFORM_LOG("Can't support UBO.");
					break;
				case GraphicsUniformType::InputAttachment:
					break;
				case GraphicsUniformType::UInt:
				case GraphicsUniformType::UInt2:
				case GraphicsUniformType::UInt3:
				case GraphicsUniformType::UInt4:
				case GraphicsUniformType::UIntArray:
				case GraphicsUniformType::UInt2Array:
				case GraphicsUniformType::UInt3Array:
				case GraphicsUniformType::UInt4Array:
					GL_PLATFORM_LOG("Can't support unsigned type.");
					break;
				default:
					break;
				}
			}
		}

		void
		GL20DescriptorSet::copy(std::uint32_t descriptorCopyCount, const GraphicsDescriptorSetPtr descriptorCopies[]) noexcept
		{
			for (std::size_t i = 0; i < descriptorCopyCount; i++)
			{
				if (!descriptorCopies[i])
					continue;

				auto descriptorCope = descriptorCopies[i]->downcast<GL20DescriptorSet>();
				for (auto& activeUniformSet : descriptorCope->_activeUniformSets)
				{
					auto it = std::find_if(_activeUniformSets.begin(), _activeUniformSets.end(), [&](GraphicsUniformSetPtr& it) { return it->getGraphicsParam() == activeUniformSet->getGraphicsParam(); });
					if (it == _activeUniformSets.end())
						continue;

					auto type = activeUniformSet->getGraphicsParam()->getType();
					switch (type)
					{
					case GraphicsUniformType::Boolean:
						(*it)->uniform1b(activeUniformSet->getBool());
						break;
					case GraphicsUniformType::Int:
						(*it)->uniform1i(activeUniformSet->getInt());
						break;
					case GraphicsUniformType::Int2:
						(*it)->uniform2i(activeUniformSet->getInt2());
						break;
					case GraphicsUniformType::Int3:
						(*it)->uniform3i(activeUniformSet->getInt3());
						break;
					case GraphicsUniformType::Int4:
						(*it)->uniform4i(activeUniformSet->getInt4());
						break;
					case GraphicsUniformType::UInt:
						(*it)->uniform1ui(activeUniformSet->getUInt());
						break;
					case GraphicsUniformType::UInt2:
						(*it)->uniform2ui(activeUniformSet->getUInt2());
						break;
					case GraphicsUniformType::UInt3:
						(*it)->uniform3ui(activeUniformSet->getUInt3());
						break;
					case GraphicsUniformType::UInt4:
						(*it)->uniform4ui(activeUniformSet->getUInt4());
						break;
					case GraphicsUniformType::Float:
						(*it)->uniform1f(activeUniformSet->getFloat());
						break;
					case GraphicsUniformType::Float2:
						(*it)->uniform2f(activeUniformSet->getFloat2());
						break;
					case GraphicsUniformType::Float3:
						(*it)->uniform3f(activeUniformSet->getFloat3());
						break;
					case GraphicsUniformType::Float4:
						(*it)->uniform4f(activeUniformSet->getFloat4());
						break;
					case GraphicsUniformType::Float2x2:
						(*it)->uniform2fmat(activeUniformSet->getFloat2x2());
						break;
					case GraphicsUniformType::Float3x3:
						(*it)->uniform3fmat(activeUniformSet->getFloat3x3());
						break;
					case GraphicsUniformType::Float4x4:
						(*it)->uniform4fmat(activeUniformSet->getFloat4x4());
						break;
					case GraphicsUniformType::IntArray:
						(*it)->uniform1iv(activeUniformSet->getIntArray());
						break;
					case GraphicsUniformType::Int2Array:
						(*it)->uniform2iv(activeUniformSet->getInt2Array());
						break;
					case GraphicsUniformType::Int3Array:
						(*it)->uniform3iv(activeUniformSet->getInt3Array());
						break;
					case GraphicsUniformType::Int4Array:
						(*it)->uniform4iv(activeUniformSet->getInt4Array());
						break;
					case GraphicsUniformType::UIntArray:
						(*it)->uniform1uiv(activeUniformSet->getUIntArray());
						break;
					case GraphicsUniformType::UInt2Array:
						(*it)->uniform2uiv(activeUniformSet->getUInt2Array());
						break;
					case GraphicsUniformType::UInt3Array:
						(*it)->uniform3uiv(activeUniformSet->getUInt3Array());
						break;
					case GraphicsUniformType::UInt4Array:
						(*it)->uniform4uiv(activeUniformSet->getUInt4Array());
						break;
					case GraphicsUniformType::FloatArray:
						(*it)->uniform1fv(activeUniformSet->getFloatArray());
						break;
					case GraphicsUniformType::Float2Array:
						(*it)->uniform2fv(activeUniformSet->getFloat2Array());
						break;
					case GraphicsUniformType::Float3Array:
						(*it)->uniform3fv(activeUniformSet->getFloat3Array());
						break;
					case GraphicsUniformType::Float4Array:
						(*it)->uniform4fv(activeUniformSet->getFloat4Array());
						break;
					case GraphicsUniformType::Float2x2Array:
						(*it)->uniform2fmatv(activeUniformSet->getFloat2x2Array());
						break;
					case GraphicsUniformType::Float3x3Array:
						(*it)->uniform3fmatv(activeUniformSet->getFloat3x3Array());
						break;
					case GraphicsUniformType::Float4x4Array:
						(*it)->uniform4fmatv(activeUniformSet->getFloat4x4Array());
						break;
					case GraphicsUniformType::Sampler:
						(*it)->uniformTexture(activeUniformSet->getTexture(), activeUniformSet->getTextureSampler());
						break;
					case GraphicsUniformType::SamplerImage:
						(*it)->uniformTexture(activeUniformSet->getTexture(), activeUniformSet->getTextureSampler());
						break;
					case GraphicsUniformType::CombinedImageSampler:
						(*it)->uniformTexture(activeUniformSet->getTexture(), activeUniformSet->getTextureSampler());
						break;
					case GraphicsUniformType::StorageImage:
						(*it)->uniformTexture(activeUniformSet->getTexture(), activeUniformSet->getTextureSampler());
						break;
					case GraphicsUniformType::StorageTexelBuffer:
						break;
					case GraphicsUniformType::StorageBuffer:
						break;
					case GraphicsUniformType::StorageBufferDynamic:
						break;
					case GraphicsUniformType::UniformTexelBuffer:
						break;
					case GraphicsUniformType::UniformBuffer:
						(*it)->uniformBuffer(activeUniformSet->getBuffer());
						break;
					case GraphicsUniformType::UniformBufferDynamic:
						break;
					case GraphicsUniformType::InputAttachment:
						break;
					default:
						break;
					}
				}
			}
		}

		const GraphicsUniformSets&
		GL20DescriptorSet::getUniformSets() const noexcept
		{
			return _activeUniformSets;
		}

		const GraphicsDescriptorSetDesc&
		GL20DescriptorSet::getDescriptorSetDesc() const noexcept
		{
			return _descriptorSetDesc;
		}

		void
		GL20DescriptorSet::setDevice(GraphicsDevicePtr device) noexcept
		{
			_device = device;
		}

		GraphicsDevicePtr
		GL20DescriptorSet::getDevice() noexcept
		{
			return _device.lock();
		}
	}
}