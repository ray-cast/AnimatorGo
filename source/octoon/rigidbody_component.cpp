#include <octoon/rigidbody_component.h>
#include <octoon/game_app.h>


namespace octoon
{
    OctoonImplementSubClass(Rigidbody, GameComponent, "Rigidbody")

    Rigidbody::Rigidbody() noexcept
        :body(nullptr)
    {
    }

	Rigidbody::~Rigidbody()
    {

    }

	GameComponentPtr Rigidbody::clone() const noexcept
    {
		auto instance = std::make_shared<Rigidbody>();
		return instance;
    }

    void Rigidbody::setAngularVelocity(float v) noexcept
    {
        angularVelocity = v;
        if(body)
            body->SetAngularVelocity(angularVelocity);
    }

    float Rigidbody::getAngularVelocity() const noexcept
    {
        return angularVelocity;
    }

    void Rigidbody::setGravityScale(float scale) noexcept
    {
        gravityScale = scale;
        if(body)
            body->SetGravityScale(scale);
    }

    float Rigidbody::getGravityScale() const noexcept
    {
        return gravityScale;
    }

    void Rigidbody::setMass(float m) noexcept
    {
        mass = m;
        if(body)
        {
            b2MassData data;
            body->GetMassData(&data);
            data.mass = m;
            body->SetMassData(&data);
        }
    }

    float Rigidbody::getMass() const noexcept
    {
        return mass;
    }

    void Rigidbody::setSleepMode(RigidbodySleepMode mode) noexcept
    {
        sleepMode = mode;
        if(sleepMode == RigidbodySleepMode::NeverSleep)
        {
            body->SetSleepingAllowed(false);
        }
        else if(sleepMode == RigidbodySleepMode::StartAsleep)
        {
            body->SetSleepingAllowed(true);
        }
        else if(sleepMode == RigidbodySleepMode::StartAwake)
        {
            body->SetSleepingAllowed(true);
        }
    }

    RigidbodySleepMode Rigidbody::getSleepMode() const noexcept
    {
        return sleepMode;
    }

    void Rigidbody::setBodyType(RigidbodyType type) noexcept
    {
        bodyType = type;
        if(body)
            body->SetType(static_cast<b2BodyType>(type));
    }

    RigidbodyType Rigidbody::getBodyType() const noexcept
    {
        return bodyType;
    }

    void Rigidbody::setPosition(math::Vector2 pos) noexcept
    {
        position = pos;
        if(body)
            body->SetTransform(b2Vec2(pos.x, pos.y), rotation);
    }

    math::Vector2 Rigidbody::getPosition() const noexcept
    {
        return position;
    }

    void Rigidbody::setRotation(float delta) noexcept
    {
        rotation = delta;
        if(body)
            body->SetTransform(b2Vec2(position.x, position.y), rotation);
    }

    float Rigidbody::getRotation() const noexcept
    {
        return rotation;
    }

    void Rigidbody::onAttach() except
    {
        addComponentDispatch(GameDispatchType::MoveAfter);
        if(body == nullptr)
            rigidbodyEnter();
    }

    void Rigidbody::onDetach() noexcept
    {
        removeComponentDispatch(GameDispatchType::MoveAfter);
    }

    void Rigidbody::onAttachComponent(const GameComponentPtr& component) noexcept
    {
        //if (component->isA<Collider>())
		//    component->downcast<Collider>()->addShapeChangeListener(&OnCollisionChange);
    }

    void Rigidbody::onDetachComponent(const GameComponentPtr& component) noexcept
    {
        //if (component->isA<Collider>())
		//    component->downcast<Collider>()->removeShapeChangeListener(&OnCollisionChange);
    }

    void Rigidbody::rigidbodyEnter() noexcept
    {
        auto world = runtime::Singleton<GameApp>::instance()->getFeature<PhysicsFeature>()->getWorld();

        b2BodyDef bodyDef;
        bodyDef.type = (b2BodyType)getBodyType();
        bodyDef.position.Set(getPosition().x, getPosition().y);
        bodyDef.angularVelocity = angularVelocity;
        bodyDef.linearVelocity = b2Vec2(velocity.x, velocity.y);
        bodyDef.gravityScale = gravityScale;
        if(sleepMode == RigidbodySleepMode::NeverSleep)
        {
            bodyDef.allowSleep = false;
        }
        else if(sleepMode == RigidbodySleepMode::StartAsleep)
        {
            bodyDef.allowSleep = true;
            bodyDef.awake = false;
        }
        else if(sleepMode == RigidbodySleepMode::StartAwake)
        {
            bodyDef.allowSleep = false;
            bodyDef.awake = true;
        }
        body = world->CreateBody(&bodyDef);
    }

    void Rigidbody::rigidbodyChange() noexcept
    {
    }

    void Rigidbody::rigidbodyExit() noexcept
    {
		auto world = runtime::Singleton<GameApp>::instance()->getFeature<PhysicsFeature>()->getWorld();

        if(body != nullptr)
            world->DestroyBody(body);
    }
}