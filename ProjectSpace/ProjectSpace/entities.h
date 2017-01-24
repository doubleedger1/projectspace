#pragma once

#include "GameObject.h"
#include "mysoundengine.h"
#include "bullet.h"
#include "building.h"
#include "Enemy.h"
#include "GameManager.h"
#include "spaceship.h"

// Class to handle an explosion
class Explosion : public GameObject
{
private:
	Vector2D m_velocity;
	Circle2D m_collisionShape; // Not used.
	float m_animationSpeed; // Speed at which is cycles through images.
	float m_currentAnimation;  // Holds the current image for that frame.
public:
	Explosion() : GameObject(EXPLOSION)
	{

	}

	void Initialise(Vector2D position, Vector2D velocity, 
		float animationSpeed, float scale);
	void Update(float frametime);
	IShape2D& GetCollisionShape()
	{
		m_collisionShape.PlaceAt(m_position, 32.0f* m_imageScale);
		return m_collisionShape;
	}
	void ProcessCollision(GameObject& other);
}; // Explosion


// Class to handle particles.
class Particle : public GameObject
{
private:
	Vector2D m_velocity;
	Circle2D m_collisionShape; // Not used.
	float m_animationSpeed; // Speed at which is cycles through images.
	float m_currentAnimation;  // Holds the current image for that frame.
	bool m_hasLooped; // Used to prolong life of particle effect.
public:
	Particle() : GameObject(FRAGMENT)
	{

	}

	void Initialise(Vector2D position, Vector2D velocity,
		float animationSpeed, float scale);
	void Update(float frametime);
	IShape2D& GetCollisionShape()
	{
		m_collisionShape.PlaceAt(m_position, 32.0f* m_imageScale);
		return m_collisionShape;
	}
	void ProcessCollision(GameObject& other);
}; // Particle

// Class to handle pickups for the ship.
class Pickup : public GameObject
{
private:
	Rectangle2D m_collisionShape;
	PowerupType m_powerUp;
	MySoundEngine* m_soundEngine = MySoundEngine::GetInstance();
	SoundIndex m_pickupSound = m_soundEngine->LoadWav(L"pickup.wav");
public:
	Pickup() : GameObject(PICKUP)
	{

	}

	IShape2D& GetCollisionShape();

	void ProcessCollision(GameObject& other);

	void Initialise(Vector2D pos);

	void Update(float time);

	PowerupType GetPickupType();

	void OnPickup();

	int ChoosePickup();
}; // Pickup