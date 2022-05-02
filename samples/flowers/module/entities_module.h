#ifndef FLOWER_ENTITIES_MODULE_H_
#define FLOWER_ENTITIES_MODULE_H_

#include <flower_model.h>
#include <octoon/game_object.h>

namespace flower
{
	class EntitiesModule final : public FlowerModule
	{
	public:
		EntitiesModule() noexcept;
		virtual ~EntitiesModule() noexcept;

		virtual void reset() noexcept override;

		virtual void load(octoon::runtime::json& reader) noexcept override;
		virtual void save(octoon::runtime::json& reader) noexcept override;

	private:
		EntitiesModule(const EntitiesModule&) = delete;
		EntitiesModule& operator=(const EntitiesModule&) = delete;

	public:
		octoon::GameObjects objects;
		octoon::GameObjectPtr camera;
		octoon::GameObjectPtr sound;
		octoon::GameObjectPtr sunLight;
		octoon::GameObjectPtr enviromentLight;
	};
}

#endif