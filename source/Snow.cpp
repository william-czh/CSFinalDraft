#include "Snow.hpp"
#include "Shader.hpp"
#include "SnowScene.hpp"
#include <atlas/utils/Application.hpp>

int Snow::amountSnow = 0;

Snow::Snow() :
    m_size(0.0002)
{
    ++Snow::amountSnow;

    m_NormDistr = std::normal_distribution<float>(0.0f, m_size * 18.0f);
    m_UniDistr = std::uniform_real_distribution<float>(0.0f, (float)(2.0 * M_PI));
}

Snow::~Snow()
{
    auto currScene = atlas::utils::Application::getInstance().getCurrentScene();
    ((SnowScene*)currScene)->getSnowAccum().refreshNearestVert(mPosition);
    --Snow::amountSnow;
}

void Snow::setMass(float mass)
{
    m_size = mass;
}

float Snow::getMass() const
{
    return m_size;
}

void Snow::setPos(glm::vec3 const &pos)
{
    mPosition = pos;
}

glm::vec3 Snow::getPos() const
{
    return mPosition;
}

void Snow::setVeloc(glm::vec3 const &velocity)
{
    mVelocity = velocity;
}

glm::vec3 Snow::getVeloc() const
{
    return mVelocity;
}

void Snow::setAccel(glm::vec3 const &acceleration)
{
    mAcceleration = acceleration;
}

glm::vec3 Snow::getAccel() const
{
    return mAcceleration;
}

void Snow::updateGeometry(atlas::core::Time<> const &t)
{
    glm::vec3 newPosition;
    glm::vec3 newVelocity;

    float deltaTime = t.deltaTime;

    // Compute the new pos by Runge-Kutta Order 4.
    glm::vec3 v1 = deltaTime * computeVelocity(0);
    glm::vec3 v2 = deltaTime * computeVelocity(0.5 * deltaTime);
    glm::vec3 v3 = deltaTime * computeVelocity(0.5 * deltaTime);
    glm::vec3 v4 = deltaTime * computeVelocity(deltaTime);
    newPosition = mPosition + (1.0f / 6.0f) * (v1 + 2.0f * v2 + 2.0f * v3 + v4);

    // Compute the new velocity by Runge-Kutta Order 4.
    glm::vec3 a1 = deltaTime * computeAcceleration(0, mVelocity);
    glm::vec3 a2 = deltaTime * computeAcceleration(0.5 * deltaTime, mVelocity + 0.5f * a1);
    glm::vec3 a3 = deltaTime * computeAcceleration(0.5 * deltaTime, mVelocity + 0.5f * a2);
    glm::vec3 a4 = deltaTime * computeAcceleration(deltaTime, mVelocity + a3);
    newVelocity = mVelocity + (1.0f / 6.0f) * (a1 + 2.0f * a2 + 2.0f * a3 + a4);

    // Update acceleration so that we use it to compute velocity
    // the next time we execute Runge-Kutta.
    mAcceleration = (newVelocity - mVelocity) / deltaTime;

    mPosition = newPosition;
    mVelocity = newVelocity;

    applyTransformations();
}

void Snow::applyTransformations()
{
    setModel(glm::mat4(1.0f));
    transformGeometry(glm::translate(glm::mat4(1.0f), mPosition));
}

glm::vec3 Snow::computeVelocity(float deltaTime, glm::vec3 const &velocity)
{
    return velocity + mAcceleration * deltaTime;
}

glm::vec3 Snow::computeVelocity(float deltaTime)
{
    return computeVelocity(deltaTime, mVelocity);
}

glm::vec3 Snow::computeAcceleration(float deltaTime, glm::vec3 const &velocity)
{
    glm::vec3 pos = mPosition + velocity * deltaTime;
    return computeAcceleration(pos, velocity);
}

glm::vec3 Snow::computeAcceleration(glm::vec3 const &pos, glm::vec3 const &velocity)
{
    const float g = 9.81;
    glm::vec3 gforce(0.0f, -g * m_size, 0.0f);

    const float viscosity_k = 7.5f;
    glm::vec3 f_Viscosity = -viscosity_k * m_size * mVelocity;

    glm::vec3 f_Wind = m_size * ((SnowScene*)atlas::utils::Application::getInstance().getCurrentScene())->getForceWind();

    float radOffset = std::fabs(m_NormDistr(m_Gen));
    float theta = m_UniDistr(m_Gen);
    glm::vec3 offset = radOffset * glm::vec3(cos(theta), 0.0f, sin(theta));

    glm::vec3 nForce = gforce + f_Viscosity + f_Wind + offset;

    return nForce / m_size;
}

void Snow::setModel(glm::mat4 const &model)
{
    mModel = model;
}

glm::mat4 Snow::getRotation() const
{
    return mRotMat;
}

void Snow::setRotation(glm::mat4 const &rot)
{
    mRotMat = rot;
}

int Snow::getSnowAmount()
{
    return Snow::amountSnow;
}
