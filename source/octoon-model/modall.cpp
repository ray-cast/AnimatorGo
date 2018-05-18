#include <octoon/model/model.h>
#include <octoon/model/modall.h>
#include <octoon/model/pmx_loader.h>

namespace octoon
{
	namespace model
	{
#if OCTOON_BUILD_PMX_MODEL
		std::shared_ptr<ModelLoader> pmx = std::make_shared<PmxLoader>();
#endif

		std::vector<ModelLoaderPtr> _handlers = {
#if OCTOON_BUILD_PMX_MODEL
			pmx
#endif
		};


		bool emptyLoader() noexcept
		{
			return _handlers.empty();
		}

		bool addHandler(ModelLoaderPtr&& handler) noexcept
		{
			assert(handler);
			auto it = std::find(_handlers.begin(), _handlers.end(), handler);
			if (it == _handlers.end())
			{
				_handlers.push_back(std::move(handler));
				return true;
			}

			return false;
		}

		bool addHandler(const ModelLoaderPtr& handler) noexcept
		{
			assert(handler);
			auto it = std::find(_handlers.begin(), _handlers.end(), handler);
			if (it == _handlers.end())
			{
				_handlers.push_back(handler);
				return true;
			}

			return false;
		}

		bool removeHandler(const ModelLoaderPtr& handler) noexcept
		{
			assert(handler);
			auto it = std::find(_handlers.begin(), _handlers.end(), handler);
			if (it != _handlers.end())
			{
				_handlers.erase(it);
				return true;
			}

			return false;
		}

		ModelLoaderPtr findHandler(const char* type) noexcept
		{
			if (type)
			{
				for (auto& it : _handlers)
					if (it->doCanRead(type))
						return it;
			}

			return nullptr;
		}

		ModelLoaderPtr findHandler(istream& stream) noexcept
		{
			if (!stream.good())
				return nullptr;

			for (auto it : _handlers)
			{
				stream.seekg(0, std::ios_base::beg);

				if (it->doCanRead(stream))
				{
					stream.seekg(0, std::ios_base::beg);
					return it;
				}
			}

			return nullptr;
		}

		ModelLoaderPtr findHandler(istream& stream, const char* type) noexcept
		{
			ModelLoaderPtr result = findHandler(type);
			if (result)
				return result;
			return findHandler(stream);
		}
	}
}