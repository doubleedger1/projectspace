#include "GameObject.h"
#include "mysoundengine.h"
#include "GameObject.h"


// This is the bullet class.
// The bullet class is used 
// by the ship and the enemies.
// It will be used when the player
// presses the spacebar to fire.


class Bullet : public GameObject
{
private:
	Circle2D m_collisionShape; // The shape of the bullet collision.
	Vector2D m_velocity; // Used for bullet travelling.
	float m_damageMultiplier; // Sets the damage of the bullet.
	float m_timer; // Used to remove a bullet after certain amount of travelling.
	MySoundEngine* m_soundEngine = MySoundEngine::GetInstance(); // Give it its own pointer to the sound engine.
	SoundIndex m_explodeSound = m_soundEngine->LoadWav(L"explosion.wav"); // Bullet explosion sound.
	SoundIndex m_bulletSound = m_soundEngine->LoadWav(L"bullet.wav"); // Bullet shooting sound.

protected: 
	FiredBy m_firedBy;

public:

	Bullet() : GameObject(BULLET)
	{

	}

	IShape2D& GetCollisionShape()
	{
		m_collisionShape.PlaceAt(m_position, 5.0f);
		return m_collisionShape;

	}


	void ProcessCollision(GameObject& other);


	void Update(float time);

	IShape2D& GetCollisionOjbect();

	void Initialise(Vector2D position, Vector2D velocity, FiredBy attacker);

	void Update();

	void Explode();

	FiredBy GetFiredBy();
}; // Bullet