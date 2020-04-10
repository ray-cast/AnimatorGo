#include <octoon/video/forward_buffer.h>
#include <octoon/video/renderer.h>

namespace octoon::video
{
	ForwardBuffer::ForwardBuffer(const std::shared_ptr<mesh::Mesh>& mesh) noexcept(false)
	{
		this->setMesh(mesh);
	}

	ForwardBuffer::~ForwardBuffer() noexcept
	{
	}

	void
	ForwardBuffer::setMesh(const std::shared_ptr<mesh::Mesh>& mesh) noexcept(false)
	{
		if (this->mesh_ != mesh)
		{
			this->updateData(mesh);
			this->mesh_ = mesh;
		}
	}

	const std::shared_ptr<mesh::Mesh>&
	ForwardBuffer::getMesh() const noexcept
	{
		return this->mesh_;
	}

	const hal::GraphicsDataPtr&
	ForwardBuffer::getVertexBuffer() const noexcept
	{
		return vertices_;
	}

	const hal::GraphicsDataPtr&
	ForwardBuffer::getIndexBuffer(std::size_t n) const noexcept
	{
		return indices_[n];
	}

	std::size_t
	ForwardBuffer::getNumVertices() const noexcept
	{
		return mesh_->getVertexArray().size();
	}

	std::size_t
	ForwardBuffer::getNumIndices(std::size_t n) const noexcept
	{
		return mesh_->getIndicesArray(n).size();
	}

	void
	ForwardBuffer::updateData(const std::shared_ptr<mesh::Mesh>& mesh) noexcept(false)
	{
		if (mesh)
		{
			auto& vertices = mesh->getVertexArray();
			auto& texcoord = mesh->getTexcoordArray();
			auto& texcoord1 = mesh->getTexcoordArray(1);
			auto& normals = mesh->getNormalArray();

			hal::GraphicsInputLayoutDesc inputLayout;
			inputLayout.addVertexLayout(hal::GraphicsVertexLayout(0, "POSITION", 0, hal::GraphicsFormat::R32G32B32SFloat));
			inputLayout.addVertexLayout(hal::GraphicsVertexLayout(0, "NORMAL", 0, hal::GraphicsFormat::R32G32B32SFloat));
			inputLayout.addVertexLayout(hal::GraphicsVertexLayout(0, "TEXCOORD", 0, hal::GraphicsFormat::R32G32SFloat));
			inputLayout.addVertexLayout(hal::GraphicsVertexLayout(0, "TEXCOORD", 1, hal::GraphicsFormat::R32G32SFloat));

			inputLayout.addVertexBinding(hal::GraphicsVertexBinding(0, inputLayout.getVertexSize()));

			auto vertexSize = inputLayout.getVertexSize() / sizeof(float);

			std::uint32_t offset = 0;
			std::vector<float> data(vertices.size() * vertexSize);

			for (auto& layout : inputLayout.getVertexLayouts())
			{
				if (layout.getSemantic() == "POSITION")
				{
					auto v = data.data() + offset;
					for (auto& it : vertices)
					{
						v[0] = it.x;
						v[1] = it.y;
						v[2] = it.z;
						v += vertexSize;
					}
				}
				else if (layout.getSemantic() == "TEXCOORD" && layout.getSemanticIndex() == 0)
				{
					auto t = data.data() + offset;
					for (auto& it : texcoord)
					{
						t[0] = it.x;
						t[1] = it.y;
						t += vertexSize;
					}
				}
				else if (layout.getSemantic() == "TEXCOORD" && layout.getSemanticIndex() == 1)
				{
					auto t = data.data() + offset;
					for (auto& it : texcoord1)
					{
						t[0] = it.x;
						t[1] = it.y;
						t += vertexSize;
					}
				}
				else if (layout.getSemantic() == "NORMAL")
				{
					auto n = data.data() + offset;
					for (auto& it : normals)
					{
						n[0] = it.x;
						n[1] = it.y;
						n[2] = it.z;
						n += vertexSize;
					}
				}

				offset += layout.getVertexCount();
			}

			hal::GraphicsDataDesc dataDesc;
			dataDesc.setType(hal::GraphicsDataType::StorageVertexBuffer);
			dataDesc.setStream((std::uint8_t*)data.data());
			dataDesc.setStreamSize(data.size() * sizeof(float));
			dataDesc.setUsage(hal::GraphicsUsageFlagBits::ReadBit);

			this->vertices_ = video::Renderer::instance()->createGraphicsData(dataDesc);

			for (std::size_t i = 0; i < mesh->getNumSubsets(); i++)
			{
				auto& indices = mesh->getIndicesArray(i);
				if (!indices.empty())
				{
					hal::GraphicsDataDesc indiceDesc;
					indiceDesc.setType(hal::GraphicsDataType::StorageIndexBuffer);
					indiceDesc.setStream((std::uint8_t*)indices.data());
					indiceDesc.setStreamSize(indices.size() * sizeof(std::uint32_t));
					indiceDesc.setUsage(hal::GraphicsUsageFlagBits::ReadBit);

					this->indices_.emplace_back(video::Renderer::instance()->createGraphicsData(indiceDesc));
				}
				else
				{
					this->indices_.emplace_back(nullptr);
				}
			}
		}
		else
		{
			this->vertices_.reset();
			this->indices_.shrink_to_fit();
		}
	}
}