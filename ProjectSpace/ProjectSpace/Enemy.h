#include "GameObject.h"
#include "mysoundengine.h"
#include "GameObject.h"

class Spaceship; // Need to forward declare it.

// Class specifically made for the Enemy.
// Is used to handle the enemies in the game.
// Contains things like health and fuel for the
// enemies.
class Enemy : public GameObject
{

private:
	Rectangle2D m_collisionShape; // The shape of the spaceship collision.
	MySoundEngine* m_soundEngine =
		MySoundEngine::GetInstance(); // Pointer to sound engine.
	SoundIndex m_explosionSound =
		m_soundEngine->LoadWav(L"explosion.wav"); // Explosion sound index.

protected:
	Spaceship* m_pSpaceship;

public:

	Enemy() : GameObject(ENEMY)
	{
		
	}

	IShape2D& GetCollisionShape();

	void ProcessCollision(GameObject& other);

	void Update(float time);

	void HandleEvent(Event evt);

	void Initialise(Vector2D position, Spaceship* player);

	void Explode(DeathType type);

	int EnemyType();

}; // Enemy

// This is an inherited class for 
// a different type of enemy.
class EnemyScroller : public Enemy
{

public:

	void Initialise(Vector2D position, Spaceship* player);

	void Update(float time);
};

// This is an inherited class for 
// a different type of enemy.
class EnemyShooter : public Enemy
{
private:
	//Spaceship* m_pSpaceship;
	float m_bulletTime = 0;
public:

	void Initialise(Vector2D position, Spaceship* player);

	void Update(float time);

	void FireBullet();
};