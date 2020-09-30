#ifndef RABBIT_ENTITIES_MODULE_H_
#define RABBIT_ENTITIES_MODULE_H_

#include <rabbit_model.h>
#include <octoon/game_object.h>

namespace rabbit
{
	class EntitiesModule final : public RabbitModule
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
		octoon::GameObjects rigidbodies;
		octoon::GameObjectPtr camera;
		octoon::GameObjectPtr sound;
		octoon::GameObjectPtr sunLight;
		octoon::GameObjectPtr enviromentLight;
	};
}

#endif