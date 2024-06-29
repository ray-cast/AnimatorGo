#include <octoon/hal/graphics_descriptor.h>

namespace octoon
{
	OctoonImplementSubInterface(GraphicsUniformSet, Object, "GraphicsUniformSet")
	OctoonImplementSubInterface(GraphicsDescriptorPool, GraphicsResource, "GraphicsDescriptorPool")
	OctoonImplementSubInterface(GraphicsDescriptorSet, GraphicsResource, "GraphicsDescriptorSet")
	OctoonImplementSubInterface(GraphicsDescriptorSetLayout, GraphicsResource, "GraphicsDescriptorSetLayout")

	void
	GraphicsDescriptorSetLayoutDesc::setUniformComponents(const GraphicsParams& component) noexcept
	{
		_params = component;
	}

	const GraphicsParams&
	GraphicsDescriptorSetLayoutDesc::getUniformComponents() const noexcept
	{
		return _params;
	}

	void
	GraphicsDescriptorSetLayoutDesc::addUniformComponent(GraphicsParamPtr component) noexcept
	{
		assert(component);
		auto it = std::find(_params.begin(), _params.end(), component);
		if (it == _params.end())
			_params.erase(it);
	}

	void
	GraphicsDescriptorSetLayoutDesc::removeUniformComponent(GraphicsParamPtr component) noexcept
	{
		assert(component);
		auto it = std::find(_params.begin(), _params.end(), component);
		if (it != _params.end())
			_params.erase(it);
	}

	GraphicsDescriptorPoolComponent::GraphicsDescriptorPoolComponent() noexcept
		: _type(UniformAttributeFormat::Null)
		, _descriptorNums(0)
	{
	}

	GraphicsDescriptorPoolComponent::GraphicsDescriptorPoolComponent(UniformAttributeFormat type, std::uint32_t nums) noexcept
		: _type(type)
		, _descriptorNums(nums)
	{
	}

	GraphicsDescriptorPoolComponent::~GraphicsDescriptorPoolComponent() noexcept
	{
	}

	void
	GraphicsDescriptorPoolComponent::setDescriptorType(UniformAttributeFormat type) noexcept
	{
		_type = type;
	}

	UniformAttributeFormat
	GraphicsDescriptorPoolComponent::getDescriptorType() const noexcept
	{
		return _type;
	}

	void
	GraphicsDescriptorPoolComponent::setDescriptorNums(std::uint32_t nums) noexcept
	{
		_descriptorNums = nums;
	}

	std::uint32_t
	GraphicsDescriptorPoolComponent::getDescriptorNums() const noexcept
	{
		return _descriptorNums;
	}

	GraphicsDescriptorPoolDesc::GraphicsDescriptorPoolDesc() noexcept
		: _maxSets(1)
	{
	}

	GraphicsDescriptorPoolDesc::GraphicsDescriptorPoolDesc(std::uint32_t maxSets) noexcept
		: _maxSets(maxSets)
	{
	}

	GraphicsDescriptorPoolDesc::~GraphicsDescriptorPoolDesc() noexcept
	{
	}

	void
	GraphicsDescriptorPoolDesc::setMaxSets(std::uint32_t maxsets) noexcept
	{
		_maxSets = maxsets;
	}

	std::uint32_t
	GraphicsDescriptorPoolDesc::getMaxSets() const noexcept
	{
		return _maxSets;
	}

	const GraphicsDescriptorPoolComponents&
	GraphicsDescriptorPoolDesc::getGraphicsDescriptorPoolComponents() const noexcept
	{
		return _poolSizes;
	}

	void
	GraphicsDescriptorPoolDesc::addGraphicsDescriptorPoolComponent(const GraphicsDescriptorPoolComponent& component) noexcept
	{
		auto it = _poolSizes.begin();
		auto end = _poolSizes.begin();
		for (; it != end; ++it)
		{
			if (it->getDescriptorType() == component.getDescriptorType())
			{
				_poolSizes.erase(it);
				return;
			}
		}

		_poolSizes.push_back(component);
	}

	GraphicsDescriptorSetDesc::GraphicsDescriptorSetDesc() noexcept
	{
	}

	GraphicsDescriptorSetDesc::~GraphicsDescriptorSetDesc() noexcept
	{
	}

	void
	GraphicsDescriptorSetDesc::setDescriptorSetLayout(GraphicsDescriptorSetLayoutPtr layout) noexcept
	{
		_layout = layout;
	}

	GraphicsDescriptorSetLayoutPtr
	GraphicsDescriptorSetDesc::getDescriptorSetLayout() const noexcept
	{
		return _layout;
	}

	void
	GraphicsDescriptorSetDesc::setGraphicsDescriptorPool(GraphicsDescriptorPoolPtr pool) noexcept
	{
		_pool = pool;
	}

	GraphicsDescriptorPoolPtr
	GraphicsDescriptorSetDesc::getGraphicsDescriptorPool() const noexcept
	{
		return _pool;
	}
}