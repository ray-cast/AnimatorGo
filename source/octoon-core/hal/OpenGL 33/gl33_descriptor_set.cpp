#include "gl33_descriptor_set.h"
#include "gl33_texture.h"
#include "gl33_shader.h"
#include "gl33_sampler.h"
#include "gl33_graphics_data.h"

namespace octoon
{
	namespace hal
	{
		OctoonImplementSubClass(GL33DescriptorSet, GraphicsDescriptorSet, "GL33DescriptorSet")
		OctoonImplementSubClass(GL33GraphicsUniformSet, GraphicsUniformSet, "GL33GraphicsUniformSet")
		OctoonImplementSubClass(GL33DescriptorSetLayout, GraphicsDescriptorSetLayout, "GL33DescriptorSetLayout")
		OctoonImplementSubClass(GL33DescriptorPool, GraphicsDescriptorPool, "GL33DescriptorPool")

		GL33GraphicsUniformSet::GL33GraphicsUniformSet() noexcept
		{
		}

		GL33GraphicsUniformSet::~GL33GraphicsUniformSet() noexcept
		{
		}

		const std::string&
		GL33GraphicsUniformSet::getName() const noexcept
		{
			assert(_param);
			return _param->getName();
		}

		void
		GL33GraphicsUniformSet::uniform1b(bool value) noexcept
		{
			_variant.uniform1b(value);
		}

		void
		GL33GraphicsUniformSet::uniform1i(std::int32_t i1) noexcept
		{
			_variant.uniform1i(i1);
		}

		void
		GL33GraphicsUniformSet::uniform2i(const int2& value) noexcept
		{
			_variant.uniform2i(value);
		}

		void
		GL33GraphicsUniformSet::uniform2i(std::int32_t i1, std::int32_t i2) noexcept
		{
			_variant.uniform2i(i1, i2);
		}

		void
		GL33GraphicsUniformSet::uniform3i(const int3& value) noexcept
		{
			_variant.uniform3i(value);
		}

		void
		GL33GraphicsUniformSet::uniform3i(std::int32_t i1, std::int32_t i2, std::int32_t i3) noexcept
		{
			_variant.uniform3i(i1, i2, i3);
		}

		void
		GL33GraphicsUniformSet::uniform4i(const int4& value) noexcept
		{
			_variant.uniform4i(value);
		}

		void
		GL33GraphicsUniformSet::uniform4i(std::int32_t i1, std::int32_t i2, std::int32_t i3, std::int32_t i4) noexcept
		{
			_variant.uniform4i(i1, i2, i3, i4);
		}

		void
		GL33GraphicsUniformSet::uniform1ui(std::uint32_t ui1) noexcept
		{
			_variant.uniform1ui(ui1);
		}

		void
		GL33GraphicsUniformSet::uniform2ui(const uint2& value) noexcept
		{
			_variant.uniform2ui(value);
		}

		void
		GL33GraphicsUniformSet::uniform2ui(std::uint32_t ui1, std::uint32_t ui2) noexcept
		{
			_variant.uniform2ui(ui1, ui2);
		}

		void
		GL33GraphicsUniformSet::uniform3ui(const uint3& value) noexcept
		{
			_variant.uniform3ui(value);
		}

		void
		GL33GraphicsUniformSet::uniform3ui(std::uint32_t ui1, std::uint32_t ui2, std::uint32_t ui3) noexcept
		{
			_variant.uniform3ui(ui1, ui2, ui3);
		}

		void
		GL33GraphicsUniformSet::uniform4ui(const uint4& value) noexcept
		{
			_variant.uniform4ui(value);
		}

		void
		GL33GraphicsUniformSet::uniform4ui(std::uint32_t ui1, std::uint32_t ui2, std::uint32_t ui3, std::uint32_t ui4) noexcept
		{
			_variant.uniform4ui(ui1, ui2, ui3, ui4);
		}

		void
		GL33GraphicsUniformSet::uniform1f(float f1) noexcept
		{
			_variant.uniform1f(f1);
		}

		void
		GL33GraphicsUniformSet::uniform2f(const float2& value) noexcept
		{
			_variant.uniform2f(value);
		}

		void
		GL33GraphicsUniformSet::uniform2f(float f1, float f2) noexcept
		{
			_variant.uniform2f(f1, f2);
		}

		void
		GL33GraphicsUniformSet::uniform3f(const float3& value) noexcept
		{
			_variant.uniform3f(value);
		}

		void
		GL33GraphicsUniformSet::uniform3f(float f1, float f2, float f3) noexcept
		{
			_variant.uniform3f(f1, f2, f3);
		}

		void
		GL33GraphicsUniformSet::uniform4f(const float4& value) noexcept
		{
			_variant.uniform4f(value);
		}

		void
		GL33GraphicsUniformSet::uniform4f(float f1, float f2, float f3, float f4) noexcept
		{
			_variant.uniform4f(f1, f2, f3, f4);
		}

		void
		GL33GraphicsUniformSet::uniform2fmat(const float2x2& value) noexcept
		{
			_variant.uniform2fmat(value);
		}

		void
		GL33GraphicsUniformSet::uniform2fmat(const float* mat2) noexcept
		{
			_variant.uniform2fmat(mat2);
		}

		void
		GL33GraphicsUniformSet::uniform3fmat(const float3x3& value) noexcept
		{
			_variant.uniform3fmat(value);
		}

		void
		GL33GraphicsUniformSet::uniform3fmat(const float* mat3) noexcept
		{
			_variant.uniform3fmat(mat3);
		}

		void
		GL33GraphicsUniformSet::uniform4fmat(const float4x4& value) noexcept
		{
			_variant.uniform4fmat(value);
		}

		void
		GL33GraphicsUniformSet::uniform4fmat(const float* mat4) noexcept
		{
			_variant.uniform4fmat(mat4);
		}

		void
		GL33GraphicsUniformSet::uniform1iv(const std::vector<int1>& value) noexcept
		{
			_variant.uniform1iv(value);
		}

		void
		GL33GraphicsUniformSet::uniform1iv(std::size_t num, const std::int32_t* i1v) noexcept
		{
			_variant.uniform1iv(num, i1v);
		}

		void
		GL33GraphicsUniformSet::uniform2iv(const std::vector<int2>& value) noexcept
		{
			_variant.uniform2iv(value);
		}

		void
		GL33GraphicsUniformSet::uniform2iv(std::size_t num, const std::int32_t* i2v) noexcept
		{
			_variant.uniform2iv(num, i2v);
		}

		void
		GL33GraphicsUniformSet::uniform3iv(const std::vector<int3>& value) noexcept
		{
			_variant.uniform3iv(value);
		}

		void
		GL33GraphicsUniformSet::uniform3iv(std::size_t num, const std::int32_t* i3v) noexcept
		{
			_variant.uniform3iv(num, i3v);
		}

		void
		GL33GraphicsUniformSet::uniform4iv(const std::vector<int4>& value) noexcept
		{
			_variant.uniform4iv(value);
		}

		void
		GL33GraphicsUniformSet::uniform4iv(std::size_t num, const std::int32_t* i4v) noexcept
		{
			_variant.uniform4iv(num, i4v);
		}

		void
		GL33GraphicsUniformSet::uniform1uiv(const std::vector<uint1>& value) noexcept
		{
			_variant.uniform1uiv(value);
		}

		void
		GL33GraphicsUniformSet::uniform1uiv(std::size_t num, const std::uint32_t* ui1v) noexcept
		{
			_variant.uniform1uiv(num, ui1v);
		}

		void
		GL33GraphicsUniformSet::uniform2uiv(const std::vector<uint2>& value) noexcept
		{
			_variant.uniform2uiv(value);
		}

		void
		GL33GraphicsUniformSet::uniform2uiv(std::size_t num, const std::uint32_t* ui2v) noexcept
		{
			_variant.uniform2uiv(num, ui2v);
		}

		void
		GL33GraphicsUniformSet::uniform3uiv(const std::vector<uint3>& value) noexcept
		{
			_variant.uniform3uiv(value);
		}

		void
		GL33GraphicsUniformSet::uniform3uiv(std::size_t num, const std::uint32_t* ui3v) noexcept
		{
			_variant.uniform3uiv(num, ui3v);
		}

		void
		GL33GraphicsUniformSet::uniform4uiv(const std::vector<uint4>& value) noexcept
		{
			_variant.uniform4uiv(value);
		}

		void
		GL33GraphicsUniformSet::uniform4uiv(std::size_t num, const std::uint32_t* ui4v) noexcept
		{
			_variant.uniform4uiv(num, ui4v);
		}

		void
		GL33GraphicsUniformSet::uniform1fv(const std::vector<float1>& value) noexcept
		{
			_variant.uniform1fv(value);
		}

		void
		GL33GraphicsUniformSet::uniform1fv(std::size_t num, const float* f1v) noexcept
		{
			_variant.uniform1fv(num, f1v);
		}

		void
		GL33GraphicsUniformSet::uniform2fv(const std::vector<float2>& value) noexcept
		{
			_variant.uniform2fv(value);
		}

		void
		GL33GraphicsUniformSet::uniform2fv(std::size_t num, const float* f2v) noexcept
		{
			_variant.uniform2fv(num, f2v);
		}

		void
		GL33GraphicsUniformSet::uniform3fv(const std::vector<float3>& value) noexcept
		{
			_variant.uniform3fv(value);
		}

		void
		GL33GraphicsUniformSet::uniform3fv(std::size_t num, const float* f3v) noexcept
		{
			_variant.uniform3fv(num, f3v);
		}

		void
		GL33GraphicsUniformSet::uniform4fv(const std::vector<float4>& value) noexcept
		{
			_variant.uniform4fv(value);
		}

		void
		GL33GraphicsUniformSet::uniform4fv(std::size_t num, const float* f4v) noexcept
		{
			_variant.uniform4fv(num, f4v);
		}

		void
		GL33GraphicsUniformSet::uniform2fmatv(const std::vector<float2x2>& value) noexcept
		{
			_variant.uniform2fmatv(value);
		}

		void
		GL33GraphicsUniformSet::uniform2fmatv(std::size_t num, const float* mat2) noexcept
		{
			_variant.uniform2fmatv(num, mat2);
		}

		void
		GL33GraphicsUniformSet::uniform3fmatv(const std::vector<float3x3>& value) noexcept
		{
			_variant.uniform3fmatv(value);
		}

		void
		GL33GraphicsUniformSet::uniform3fmatv(std::size_t num, const float* mat3) noexcept
		{
			_variant.uniform3fmatv(num, mat3);
		}

		void
		GL33GraphicsUniformSet::uniform4fmatv(const std::vector<float4x4>& value) noexcept
		{
			_variant.uniform4fmatv(value);
		}

		void
		GL33GraphicsUniformSet::uniform4fmatv(std::size_t num, const float* mat4) noexcept
		{
			_variant.uniform4fmatv(num, mat4);
		}

		void
		GL33GraphicsUniformSet::uniformTexture(GraphicsTexturePtr texture, GraphicsSamplerPtr sampler) noexcept
		{
			_variant.uniformTexture(texture, sampler);
		}

		void
		GL33GraphicsUniformSet::uniformBuffer(GraphicsDataPtr ubo) noexcept
		{
			_variant.uniformBuffer(ubo);
		}

		bool
		GL33GraphicsUniformSet::getBool() const noexcept
		{
			return _variant.getBool();
		}

		int
		GL33GraphicsUniformSet::getInt() const noexcept
		{
			return _variant.getInt();
		}

		const int2&
		GL33GraphicsUniformSet::getInt2() const noexcept
		{
			return _variant.getInt2();
		}

		const int3&
		GL33GraphicsUniformSet::getInt3() const noexcept
		{
			return _variant.getInt3();
		}

		const int4&
		GL33GraphicsUniformSet::getInt4() const noexcept
		{
			return _variant.getInt4();
		}

		uint1
		GL33GraphicsUniformSet::getUInt() const noexcept
		{
			return _variant.getUInt();
		}

		const uint2&
		GL33GraphicsUniformSet::getUInt2() const noexcept
		{
			return _variant.getUInt2();
		}

		const uint3&
		GL33GraphicsUniformSet::getUInt3() const noexcept
		{
			return _variant.getUInt3();
		}

		const uint4&
		GL33GraphicsUniformSet::getUInt4() const noexcept
		{
			return _variant.getUInt4();
		}

		float
		GL33GraphicsUniformSet::getFloat() const noexcept
		{
			return _variant.getFloat();
		}

		const float2&
		GL33GraphicsUniformSet::getFloat2() const noexcept
		{
			return _variant.getFloat2();
		}

		const float3&
		GL33GraphicsUniformSet::getFloat3() const noexcept
		{
			return _variant.getFloat3();
		}

		const float4&
		GL33GraphicsUniformSet::getFloat4() const noexcept
		{
			return _variant.getFloat4();
		}

		const float2x2&
		GL33GraphicsUniformSet::getFloat2x2() const noexcept
		{
			return _variant.getFloat2x2();
		}

		const float3x3&
		GL33GraphicsUniformSet::getFloat3x3() const noexcept
		{
			return _variant.getFloat3x3();
		}

		const float4x4&
		GL33GraphicsUniformSet::getFloat4x4() const noexcept
		{
			return _variant.getFloat4x4();
		}

		const std::vector<int1>&
		GL33GraphicsUniformSet::getIntArray() const noexcept
		{
			return _variant.getIntArray();
		}

		const std::vector<int2>&
		GL33GraphicsUniformSet::getInt2Array() const noexcept
		{
			return _variant.getInt2Array();
		}

		const std::vector<int3>&
		GL33GraphicsUniformSet::getInt3Array() const noexcept
		{
			return _variant.getInt3Array();
		}

		const std::vector<int4>&
		GL33GraphicsUniformSet::getInt4Array() const noexcept
		{
			return _variant.getInt4Array();
		}

		const std::vector<uint1>&
		GL33GraphicsUniformSet::getUIntArray() const noexcept
		{
			return _variant.getUIntArray();
		}

		const std::vector<uint2>&
		GL33GraphicsUniformSet::getUInt2Array() const noexcept
		{
			return _variant.getUInt2Array();
		}

		const std::vector<uint3>&
		GL33GraphicsUniformSet::getUInt3Array() const noexcept
		{
			return _variant.getUInt3Array();
		}

		const std::vector<uint4>&
		GL33GraphicsUniformSet::getUInt4Array() const noexcept
		{
			return _variant.getUInt4Array();
		}

		const std::vector<float1>&
		GL33GraphicsUniformSet::getFloatArray() const noexcept
		{
			return _variant.getFloatArray();
		}

		const std::vector<float2>&
		GL33GraphicsUniformSet::getFloat2Array() const noexcept
		{
			return _variant.getFloat2Array();
		}

		const std::vector<float3>&
		GL33GraphicsUniformSet::getFloat3Array() const noexcept
		{
			return _variant.getFloat3Array();
		}

		const std::vector<float4>&
		GL33GraphicsUniformSet::getFloat4Array() const noexcept
		{
			return _variant.getFloat4Array();
		}

		const std::vector<float2x2>&
		GL33GraphicsUniformSet::getFloat2x2Array() const noexcept
		{
			return _variant.getFloat2x2Array();
		}

		const std::vector<float3x3>&
		GL33GraphicsUniformSet::getFloat3x3Array() const noexcept
		{
			return _variant.getFloat3x3Array();
		}

		const std::vector<float4x4>&
		GL33GraphicsUniformSet::getFloat4x4Array() const noexcept
		{
			return _variant.getFloat4x4Array();
		}

		const GraphicsTexturePtr&
		GL33GraphicsUniformSet::getTexture() const noexcept
		{
			return _variant.getTexture();
		}

		const GraphicsSamplerPtr&
		GL33GraphicsUniformSet::getTextureSampler() const noexcept
		{
			return _variant.getTextureSampler();
		}

		const GraphicsDataPtr&
		GL33GraphicsUniformSet::getBuffer() const noexcept
		{
			return _variant.getBuffer();
		}

		void
		GL33GraphicsUniformSet::setGraphicsParam(GraphicsParamPtr param) noexcept
		{
			assert(param);
			_param = param;
			_variant.setType(param->getType());
		}

		const GraphicsParamPtr&
		GL33GraphicsUniformSet::getGraphicsParam() const noexcept
		{
			return _param;
		}

		GL33DescriptorPool::GL33DescriptorPool() noexcept
		{
		}

		GL33DescriptorPool::~GL33DescriptorPool() noexcept
		{
			this->close();
		}

		bool
		GL33DescriptorPool::setup(const GraphicsDescriptorPoolDesc& desc) noexcept
		{
			return true;
		}

		void
		GL33DescriptorPool::close() noexcept
		{
		}

		const GraphicsDescriptorPoolDesc&
		GL33DescriptorPool::getDescriptorPoolDesc() const noexcept
		{
			return _descriptorPoolDesc;
		}

		void
		GL33DescriptorPool::setDevice(const GraphicsDevicePtr& device) noexcept
		{
			_device = device;
		}

		GraphicsDevicePtr
		GL33DescriptorPool::getDevice() noexcept
		{
			return _device.lock();
		}

		GL33DescriptorSetLayout::GL33DescriptorSetLayout() noexcept
		{
		}

		GL33DescriptorSetLayout::~GL33DescriptorSetLayout() noexcept
		{
			this->close();
		}

		bool
		GL33DescriptorSetLayout::setup(const GraphicsDescriptorSetLayoutDesc& descriptorSetLayoutDesc) noexcept
		{
			_descripotrSetLayoutDesc = descriptorSetLayoutDesc;
			return true;
		}

		void
		GL33DescriptorSetLayout::close() noexcept
		{
		}

		const GraphicsDescriptorSetLayoutDesc&
		GL33DescriptorSetLayout::getDescriptorSetLayoutDesc() const noexcept
		{
			return _descripotrSetLayoutDesc;
		}

		void
		GL33DescriptorSetLayout::setDevice(const GraphicsDevicePtr& device) noexcept
		{
			_device = device;
		}

		GraphicsDevicePtr
		GL33DescriptorSetLayout::getDevice() noexcept
		{
			return _device.lock();
		}

		GL33DescriptorSet::GL33DescriptorSet() noexcept
		{
		}

		GL33DescriptorSet::~GL33DescriptorSet() noexcept
		{
			this->close();
		}

		bool
		GL33DescriptorSet::setup(const GraphicsDescriptorSetDesc& descriptorSetDesc) noexcept
		{
			assert(descriptorSetDesc.getDescriptorSetLayout());

			auto& descriptorSetLayoutDesc = descriptorSetDesc.getDescriptorSetLayout()->getDescriptorSetLayoutDesc();

			auto& params = descriptorSetLayoutDesc.getUniformComponents();
			for (auto& uniform : params)
			{
				auto uniformSet = std::make_shared<GL33GraphicsUniformSet>();
				uniformSet->setGraphicsParam(uniform);
				_activeUniformSets.push_back(uniformSet);
			}

			_descriptorSetDesc = descriptorSetDesc;
			return true;
		}

		void
		GL33DescriptorSet::close() noexcept
		{
			_activeUniformSets.clear();
		}

		void
		GL33DescriptorSet::apply(const GL33Program& shaderObject) noexcept
		{
			auto program = shaderObject.getInstanceID();
			for (auto& it : _activeUniformSets)
			{
				auto type = it->getGraphicsParam()->getType();
				auto location = it->getGraphicsParam()->getBindingPoint();
				switch (type)
				{
				case GraphicsUniformType::Boolean:
					glProgramUniform1i(program, location, it->getBool());
					break;
				case GraphicsUniformType::Int:
					glProgramUniform1i(program, location, it->getInt());
					break;
				case GraphicsUniformType::Int2:
					glProgramUniform2iv(program, location, 1, (GLint*)it->getInt2().ptr());
					break;
				case GraphicsUniformType::Int3:
					glProgramUniform3iv(program, location, 1, (GLint*)it->getInt3().ptr());
					break;
				case GraphicsUniformType::Int4:
					glProgramUniform4iv(program, location, 1, (GLint*)it->getInt4().ptr());
					break;
				case GraphicsUniformType::UInt:
					glProgramUniform1ui(program, location, it->getUInt());
					break;
				case GraphicsUniformType::UInt2:
					glProgramUniform2uiv(program, location, 1, (GLuint*)it->getUInt2().ptr());
					break;
				case GraphicsUniformType::UInt3:
					glProgramUniform3uiv(program, location, 1, (GLuint*)it->getUInt3().ptr());
					break;
				case GraphicsUniformType::UInt4:
					glProgramUniform4uiv(program, location, 1, (GLuint*)it->getUInt4().ptr());
					break;
				case GraphicsUniformType::Float:
					glProgramUniform1f(program, location, it->getFloat());
					break;
				case GraphicsUniformType::Float2:
					glProgramUniform2fv(program, location, 1, it->getFloat2().ptr());
					break;
				case GraphicsUniformType::Float3:
					glProgramUniform3fv(program, location, 1, it->getFloat3().ptr());
					break;
				case GraphicsUniformType::Float4:
					glProgramUniform4fv(program, location, 1, it->getFloat4().ptr());
					break;
				case GraphicsUniformType::Float2x2:
					glProgramUniformMatrix2fv(program, location, 1, GL_FALSE, it->getFloat2x2().ptr());
					break;
				case GraphicsUniformType::Float3x3:
					glProgramUniformMatrix3fv(program, location, 1, GL_FALSE, it->getFloat3x3().ptr());
					break;
				case GraphicsUniformType::Float4x4:
					glProgramUniformMatrix4fv(program, location, 1, GL_FALSE, it->getFloat4x4().ptr());
					break;
				case GraphicsUniformType::IntArray:
					assert(it->getIntArray().size() < std::numeric_limits<GLsizei>::max());
					glProgramUniform1iv(program, location, static_cast<GLsizei>(it->getIntArray().size()), it->getIntArray().data());
					break;
				case GraphicsUniformType::Int2Array:
					assert(it->getInt2Array().size() < std::numeric_limits<GLsizei>::max());
					glProgramUniform2iv(program, location, static_cast<GLsizei>(it->getInt2Array().size()), (GLint*)it->getInt2Array().data());
					break;
				case GraphicsUniformType::Int3Array:
					assert(it->getInt3Array().size() < std::numeric_limits<GLsizei>::max());
					glProgramUniform3iv(program, location, static_cast<GLsizei>(it->getInt3Array().size()), (GLint*)it->getInt3Array().data());
					break;
				case GraphicsUniformType::Int4Array:
					assert(it->getInt4Array().size() < std::numeric_limits<GLsizei>::max());
					glProgramUniform4iv(program, location, static_cast<GLsizei>(it->getInt4Array().size()), (GLint*)it->getInt4Array().data());
					break;
				case GraphicsUniformType::UIntArray:
					assert(it->getUIntArray().size() < std::numeric_limits<GLsizei>::max());
					glProgramUniform1uiv(program, location, static_cast<GLsizei>(it->getUIntArray().size()), it->getUIntArray().data());
					break;
				case GraphicsUniformType::UInt2Array:
					assert(it->getUInt2Array().size() < std::numeric_limits<GLsizei>::max());
					glProgramUniform2uiv(program, location, static_cast<GLsizei>(it->getUInt2Array().size()), (GLuint*)it->getUInt2Array().data());
					break;
				case GraphicsUniformType::UInt3Array:
					assert(it->getUInt3Array().size() < std::numeric_limits<GLsizei>::max());
					glProgramUniform3uiv(program, location, static_cast<GLsizei>(it->getUInt3Array().size()), (GLuint*)it->getUInt3Array().data());
					break;
				case GraphicsUniformType::UInt4Array:
					assert(it->getUInt4Array().size() < std::numeric_limits<GLsizei>::max());
					glProgramUniform4uiv(program, location, static_cast<GLsizei>(it->getUInt4Array().size()), (GLuint*)it->getUInt4Array().data());
					break;
				case GraphicsUniformType::FloatArray:
					assert(it->getFloatArray().size() < std::numeric_limits<GLsizei>::max());
					glProgramUniform1fv(program, location, static_cast<GLsizei>(it->getFloatArray().size()), (GLfloat*)it->getFloatArray().data());
					break;
				case GraphicsUniformType::Float2Array:
					assert(it->getFloat2Array().size() < std::numeric_limits<GLsizei>::max());
					glProgramUniform2fv(program, location, static_cast<GLsizei>(it->getFloat2Array().size()), (GLfloat*)it->getFloat2Array().data());
					break;
				case GraphicsUniformType::Float3Array:
					assert(it->getFloat3Array().size() < std::numeric_limits<GLsizei>::max());
					glProgramUniform3fv(program, location, static_cast<GLsizei>(it->getFloat3Array().size()), (GLfloat*)it->getFloat3Array().data());
					break;
				case GraphicsUniformType::Float4Array:
					assert(it->getFloat4Array().size() < std::numeric_limits<GLsizei>::max());
					glProgramUniform4fv(program, location, static_cast<GLsizei>(it->getFloat4Array().size()), (GLfloat*)it->getFloat4Array().data());
					break;
				case GraphicsUniformType::Float2x2Array:
					assert(it->getFloat2x2Array().size() < std::numeric_limits<GLsizei>::max());
					glProgramUniformMatrix2fv(program, location, static_cast<GLsizei>(it->getFloat2x2Array().size()), GL_FALSE, (GLfloat*)it->getFloat2x2Array().data());
					break;
				case GraphicsUniformType::Float3x3Array:
					assert(it->getFloat3x3Array().size() < std::numeric_limits<GLsizei>::max());
					glProgramUniformMatrix3fv(program, location, static_cast<GLsizei>(it->getFloat3x3Array().size()), GL_FALSE, (GLfloat*)it->getFloat3x3Array().data());
					break;
				case GraphicsUniformType::Float4x4Array:
					assert(it->getFloat4x4Array().size() < std::numeric_limits<GLsizei>::max());
					glProgramUniformMatrix4fv(program, location, static_cast<GLsizei>(it->getFloat4x4Array().size()), GL_FALSE, (GLfloat*)it->getFloat4x4Array().data());
					break;
				case GraphicsUniformType::Sampler:
					glBindSampler(location, it->getTextureSampler()->downcast<GL33Sampler>()->getInstanceID());
					break;
				case GraphicsUniformType::SamplerImage:
				case GraphicsUniformType::CombinedImageSampler:
				case GraphicsUniformType::StorageImage:
				{
					auto& texture = it->getTexture();
					if (texture)
					{
						auto gltexture = texture->downcast<GL33Texture>();
						GL_CHECK(glActiveTexture(GL_TEXTURE0 + location));
						GL_CHECK(glBindTexture(gltexture->getTarget(), gltexture->getInstanceID()));

						auto sampler = it->getTextureSampler();
						if (sampler)
						{
							auto instance = sampler->downcast<GL33Sampler>()->getInstanceID();
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
				case GraphicsUniformType::StorageTexelBuffer:
					break;
				case GraphicsUniformType::StorageBuffer:
					break;
				case GraphicsUniformType::StorageBufferDynamic:
					break;
				case GraphicsUniformType::UniformTexelBuffer:
					break;
				case GraphicsUniformType::UniformBuffer:
				{
					auto& buffer = it->getBuffer();
					if (buffer)
					{
						auto ubo = buffer->downcast<GL33GraphicsData>();
						glBindBufferBase(GL_UNIFORM_BUFFER, location, ubo->getInstanceID());
					}
				}
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

		void
		GL33DescriptorSet::copy(std::uint32_t descriptorCopyCount, const GraphicsDescriptorSetPtr descriptorCopies[]) noexcept
		{
			for (std::size_t i = 0; i < descriptorCopyCount; i++)
			{
				if (!descriptorCopies[i])
					continue;

				auto descriptorCope = descriptorCopies[i]->downcast<GL33DescriptorSet>();
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
		GL33DescriptorSet::getUniformSets() const noexcept
		{
			return _activeUniformSets;
		}

		const GraphicsDescriptorSetDesc&
		GL33DescriptorSet::getDescriptorSetDesc() const noexcept
		{
			return _descriptorSetDesc;
		}

		void
		GL33DescriptorSet::setDevice(const GraphicsDevicePtr& device) noexcept
		{
			_device = device;
		}

		GraphicsDevicePtr
		GL33DescriptorSet::getDevice() noexcept
		{
			return _device.lock();
		}
	}
}