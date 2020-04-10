#include "clw_scene.h"

namespace octoon::video
{
	ClwScene::ClwScene(CLWContext context, std::uint32_t w, std::uint32_t h)
		: Output(w, h)
		, context_(context)
		, data_(context.CreateBuffer<math::float4>(w * h, CL_MEM_READ_WRITE))
	{
	}

	void
	ClwScene::getData(math::float4* data) const
	{
		context_.ReadBuffer(0, data_, data, data_.GetElementCount()).Wait();
	}

	void
	ClwScene::getData(math::float4* data, std::size_t offset, std::size_t elems_count) const
	{
		context_.ReadBuffer(0, data_, data, offset, elems_count).Wait();
	}

	void
	ClwScene::clear(math::float4 const& val)
	{
		context_.FillBuffer(0, data_, val, data_.GetElementCount()).Wait();
	}

	const CLWBuffer<math::float4>&
	ClwScene::data() const  noexcept
	{
		return data_;
	}
}