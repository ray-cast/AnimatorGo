#include <octoon/box_collider_component.h>
#include <octoon/rigidbody_component.h>


namespace octoon
{
    OctoonImplementSubClass(BoxCollider, Collider, "BoxCollider")

    BoxCollider::BoxCollider() noexcept
        :isRegistered(false)
    {

    }

    BoxCollider::~BoxCollider()
    {

    }

    GameComponentPtr BoxCollider::clone() const noexcept
    {
        return std::make_shared<BoxCollider>();
    }

    void BoxCollider::setAutoTiling(bool isAutoTilling) noexcept
    {
        autoTiling = isAutoTilling;
        onCollisionChange();
    }

    bool BoxCollider::getAutoTiling() const noexcept
    {
        return autoTiling;
    }

    void BoxCollider::setEdgeRadius(float r) noexcept
    {
        edgeRadius = r;
        onCollisionChange();
    }

    float BoxCollider::getEdgeRadius() const noexcept
    {
        return edgeRadius;
    }

    void BoxCollider::setSize(const math::Vector2& s) noexcept
    {
        size = s;
        onCollisionChange();
    }

    math::Vector2 BoxCollider::getSize() const noexcept
    {
        return size;
    }

    void BoxCollider::onCollisionChange() noexcept
    {
        auto rigidBody = getComponent<Rigidbody>();
        if (!rigidBody)
            return;

        if(!isRegistered)
            return;
        
        b2PolygonShape shapeDef;
        shapeDef.SetAsBox(size.x, size.y);
        shapeDef.m_radius = edgeRadius;

        b2FixtureDef fixtureDef;
        fixtureDef.shape = &shapeDef;
        
        rigidBody->body->DestroyFixture(collider);
        collider = rigidBody->body->CreateFixture(&fixtureDef);
    }

    void BoxCollider::onCollisionEnter() noexcept
    {
        auto rigidBody = getComponent<Rigidbody>();
        if (!rigidBody)
            return;
        
        b2PolygonShape shapeDef;
        shapeDef.SetAsBox(size.x, size.y);
        shapeDef.m_radius = edgeRadius;

        b2FixtureDef fixtureDef;
        fixtureDef.shape = &shapeDef;
        
        collider = rigidBody->body->CreateFixture(&fixtureDef);

        isRegistered = true;
    }

    void BoxCollider::onCollisionExit() noexcept
    {
        auto rigidBody = getComponent<Rigidbody>();
        if (!rigidBody)
            return;

        if(!isRegistered)
            return;

        rigidBody->body->DestroyFixture(collider);
    }

    void BoxCollider::onCollisionStay() noexcept
    {

    }

    void BoxCollider::onAttach() except
    {
        auto rigidBody = getComponent<Rigidbody>();
        if (!rigidBody)
            return;

        onCollisionEnter();
    }

    void BoxCollider::onDetach() noexcept
    {
        onCollisionExit();
    }

    void BoxCollider::onAttachComponent(const GameComponentPtr& component) except
    {
        if (component->isA<Rigidbody>())
        {
            onCollisionEnter();
        }
    }

    void BoxCollider::onDetachComponent(const GameComponentPtr& component) noexcept
    {
    }
}