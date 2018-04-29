#include <octoon/sphere_collider_component.h>
#include <octoon/rigidbody_component.h>


namespace octoon
{
    OctoonImplementSubClass(CircleCollider, Collider, "CircleCollider")

    CircleCollider::CircleCollider() noexcept
        :isRegistered(false)
    {

    }

    CircleCollider::~CircleCollider()
    {

    }

    GameComponentPtr CircleCollider::clone() const noexcept
    {
        return std::make_shared<CircleCollider>();
    }

    void CircleCollider::setRadius(float r) noexcept
    {
        radius = r;
        onCollisionChange();
    }

    float CircleCollider::getRadius() const noexcept
    {
        return radius;
    }

    void CircleCollider::onCollisionChange() noexcept
    {
        auto rigidBody = getComponent<Rigidbody>();
        if (!rigidBody)
            return;

        if(!isRegistered)
            return;
        
        b2CircleShape shapeDef;
        shapeDef.m_p = b2Vec2(rigidBody->getPosition().x, rigidBody->getPosition().y);
        shapeDef.m_radius = this->getRadius();

        b2FixtureDef fixtureDef;
        fixtureDef.shape = &shapeDef;
        
        rigidBody->body->DestroyFixture(collider);
        collider = rigidBody->body->CreateFixture(&fixtureDef);
    }

    void CircleCollider::onCollisionEnter() noexcept
    {
        auto rigidBody = getComponent<Rigidbody>();
        if (!rigidBody)
            return;
        
        b2CircleShape shapeDef;
        shapeDef.m_p = b2Vec2(rigidBody->getPosition().x, rigidBody->getPosition().y);
        shapeDef.m_radius = this->getRadius();

        b2FixtureDef fixtureDef;
        fixtureDef.shape = &shapeDef;
        
        collider = rigidBody->body->CreateFixture(&fixtureDef);

        isRegistered = true;
    }

    void CircleCollider::onCollisionExit() noexcept
    {
        auto rigidBody = getComponent<Rigidbody>();
        if (!rigidBody)
            return;

        if(!isRegistered)
            return;

        rigidBody->body->DestroyFixture(collider);
    }

    void CircleCollider::onCollisionStay() noexcept
    {

    }

    void CircleCollider::onAttach() except
    {
        auto rigidBody = getComponent<Rigidbody>();
        if (!rigidBody)
            return;

        onCollisionEnter();
    }

    void CircleCollider::onDetach() noexcept
    {
        onCollisionExit();
    }

    void CircleCollider::onAttachComponent(const GameComponentPtr& component) except
    {
        if (component->isA<Rigidbody>())
        {
            onCollisionEnter();
        }
    }

    void CircleCollider::onDetachComponent(const GameComponentPtr& component) noexcept
    {
    }
}