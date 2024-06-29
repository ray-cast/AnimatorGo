#include <octoon/video/scriptable_render_buffer.h>
#include <octoon/video/renderer.h>
#include <octoon/hal/graphics_input_layout.h>
#include <octoon/runtime/profiling_scope.h>

namespace octoon
{
	ScriptableRenderBuffer::ScriptableRenderBuffer() noexcept
	{
	}

	ScriptableRenderBuffer::ScriptableRenderBuffer(const GraphicsDevicePtr& context, const std::shared_ptr<Mesh>& mesh) noexcept(false)
	{
		this->updateData(context, mesh);
	}

	ScriptableRenderBuffer::~ScriptableRenderBuffer() noexcept
	{
	}

	const GraphicsDataPtr&
	ScriptableRenderBuffer::getVertexBuffer() const noexcept
	{
		return vertices_;
	}

	const GraphicsDataPtr&
	ScriptableRenderBuffer::getIndexBuffer() const noexcept
	{
		return indices_;
	}

	std::size_t
	ScriptableRenderBuffer::getNumVertices() const noexcept
	{
		return mesh_->getVertexArray().size();
	}

	std::size_t
	ScriptableRenderBuffer::getNumSubsets() const noexcept
	{
		return mesh_->getNumSubsets();
	}

	std::size_t
	ScriptableRenderBuffer::getNumIndicesAll() const noexcept
	{
		return mesh_->getNumIndices();
	}

	std::size_t
	ScriptableRenderBuffer::getNumIndices(std::size_t n) const noexcept
	{
		return mesh_->getIndicesArray(n).size();
	}

	std::size_t
	ScriptableRenderBuffer::getStartIndices(std::size_t n) const noexcept
	{
		return this->startIndice_[n];
	}

	void
	ScriptableRenderBuffer::updateData(const GraphicsDevicePtr& context, const std::shared_ptr<Mesh>& mesh) noexcept(false)
	{
		if (mesh)
		{
			std::size_t numVertices = mesh->getNumVertices();
			std::size_t numIndices = mesh->getNumIndices();

			if (numVertices > 0 && (!this->vertices_ || mesh_ && mesh_->getNumVertices() != mesh->getNumVertices()))
			{
				GraphicsInputLayoutDesc inputLayout;
				inputLayout.addVertexLayout(GraphicsVertexLayout(0, "POSITION", 0, GraphicsFormat::R32G32B32SFloat));
				inputLayout.addVertexLayout(GraphicsVertexLayout(0, "NORMAL", 0, GraphicsFormat::R32G32B32SFloat));
				inputLayout.addVertexLayout(GraphicsVertexLayout(0, "TEXCOORD", 0, GraphicsFormat::R32G32SFloat));
				inputLayout.addVertexLayout(GraphicsVertexLayout(0, "TEXCOORD", 1, GraphicsFormat::R32G32SFloat));

				inputLayout.addVertexBinding(GraphicsVertexBinding(0, inputLayout.getVertexSize()));

				GraphicsDataDesc dataDesc;
				dataDesc.setType(GraphicsDataType::StorageVertexBuffer);
				dataDesc.setStream((std::uint8_t*)nullptr);
				dataDesc.setStreamSize(numVertices * inputLayout.getVertexSize());
				dataDesc.setUsage(GraphicsUsageFlagBits::WriteBit);

				this->vertices_ = context->createGraphicsData(dataDesc);
			}

			if (numIndices > 0 && (!this->indices_ || mesh_ && mesh_->getNumIndices() != mesh->getNumIndices()))
			{
				GraphicsDataDesc indiceDesc;
				indiceDesc.setType(GraphicsDataType::StorageIndexBuffer);
				indiceDesc.setStream(nullptr);
				indiceDesc.setStreamSize(numIndices * sizeof(std::uint32_t));
				indiceDesc.setUsage(GraphicsUsageFlagBits::WriteBit);

				this->indices_ = context->createGraphicsData(indiceDesc);
			}

			if (numVertices > 0 && this->vertices_)
			{
				auto streamSize = this->vertices_->getDataDesc().getStreamSize();

				void* data = nullptr;
				if (this->vertices_->map(0, streamSize, &data))
				{
					auto& vertices = mesh->getVertexArray();
					auto& texcoord = mesh->getTexcoordArray();
					auto& texcoord1 = mesh->getTexcoordArray(1);
					auto& normals = mesh->getNormalArray();

					auto vertexSize = streamSize / numVertices / sizeof(float);

					if (!vertices.empty())
					{
						auto dst = (float*)data;

						for (std::size_t i = 0; i < numVertices; i++, dst += vertexSize)
						{
							auto& v = vertices[i];
							dst[0] = v.x;
							dst[1] = v.y;
							dst[2] = v.z;
						}
					}

					if (!normals.empty())
					{
						auto dst = (float*)data;

						for (std::size_t i = 0; i < numVertices; i++, dst += vertexSize)
						{
							auto& n = normals[i];
							dst[3] = n.x;
							dst[4] = n.y;
							dst[5] = n.z;
						}
					}

					if (!texcoord.empty())
					{
						auto dst = (float*)data;

						for (std::size_t i = 0; i < numVertices; i++, dst += vertexSize)
						{
							auto& uv = texcoord[i];
							dst[6] = uv.x;
							dst[7] = uv.y;
						}
					}

					if (!texcoord1.empty())
					{
						auto dst = (float*)data;

						for (std::size_t i = 0; i < numVertices; i++, dst += vertexSize)
						{
							auto& uv = texcoord1[i];
							dst[8] = uv.x;
							dst[9] = uv.y;
						}
					}
				}

				this->vertices_->unmap();
			}

			if (numIndices > 0 && this->indices_)
			{
				this->startIndice_.resize(mesh->getNumSubsets());

				void* data = nullptr;
				if (this->indices_->map(0, numIndices * sizeof(std::uint32_t), &data))
				{
					for (std::size_t i = 0, streamOffset = 0; i < mesh->getNumSubsets(); i++)
					{
						auto& indices = mesh->getIndicesArray(i);
						if (!indices.empty())
						{
							std::memcpy((std::uint32_t*)data + streamOffset, indices.data(), indices.size() * sizeof(std::uint32_t));

							this->startIndice_[i] = streamOffset;
							streamOffset += indices.size();
						}
					}
				}

				this->indices_->unmap();
			}
		}
		else
		{
			this->vertices_.reset();
			this->indices_.reset();
		}

		this->mesh_ = mesh;
	}
}