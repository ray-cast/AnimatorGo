#include <octoon/rigidbody_component.h>
#include <octoon/game_app.h>


namespace octoon
{
    OctoonImplementSubClass(Rigidbody, GameComponent, "Rigidbody")

    Rigidbody::Rigidbody() noexcept
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
    }

    float Rigidbody::getAngularVelocity() const noexcept
    {
        return angularVelocity;
    }

    void Rigidbody::setGravityScale(float scale) noexcept
    {
        gravityScale = scale;
    }

    float Rigidbody::getGravityScale() const noexcept
    {
        return gravityScale;
    }

    void Rigidbody::setMass(float m) noexcept
    {
        mass = m;
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
        }
        else if(sleepMode == RigidbodySleepMode::StartAsleep)
        {
        }
        else if(sleepMode == RigidbodySleepMode::StartAwake)
        {
        }
    }

    RigidbodySleepMode Rigidbody::getSleepMode() const noexcept
    {
        return sleepMode;
    }

    void Rigidbody::setBodyType(RigidbodyType type) noexcept
    {
        bodyType = type;
    }

    RigidbodyType Rigidbody::getBodyType() const noexcept
    {
        return bodyType;
    }

    void Rigidbody::setPosition(math::Vector2 pos) noexcept
    {
        position = pos;
    }

    math::Vector2 Rigidbody::getPosition() const noexcept
    {
        return position;
    }

    void Rigidbody::setRotation(float delta) noexcept
    {
        rotation = delta;
    }

    float Rigidbody::getRotation() const noexcept
    {
        return rotation;
    }

    void Rigidbody::onAttach() except
    {
        addComponentDispatch(GameDispatchType::MoveAfter);
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
    }

    void Rigidbody::rigidbodyChange() noexcept
    {
    }

    void Rigidbody::rigidbodyExit() noexcept
    {
    }
}