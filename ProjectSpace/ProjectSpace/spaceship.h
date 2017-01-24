#include "GameObject.h"
#include "mysoundengine.h"
#include "GameObject.h"

// Class specifically made for the spaceship.
// Is used to handle the spaceship in the game.
// Contains things like health and fuel for the
// spaceship.
class Spaceship : public GameObject
{
private:
	Rectangle2D m_collisionShape; // The shape of the spaceship collision.
	Vector2D m_velocity; // The speed at which the ship is travelling.
	int m_score = -1; // Holds the score of the spaceship.
	float m_health; // Holds the health of the ship.
	float m_fuel; // Holds the fuel of the ship.
	float m_bulletTime = 0; // Causes delay per shot rather than constant.
	float m_lastLowhp = 0; // Delay between playing lowhp sound when lower than 30hp.
	float m_lastCutoff = 0; // Cuts off the thrust if too high.
	float m_lastXPos = 0; // Stores last XPos; used to ensure no point hoarding.
	int m_ammo = 15; // Starts off with ammunition.
	float m_camPosX = 0; // Keeps track of camera movement along the axis.
	float m_xPosMax = 0; // Makes the spaceship bounce if it goes too far back.
	bool m_angleLocked = false; // Locks angle turning when landed.
	MySoundEngine* m_soundEngine =
		MySoundEngine::GetInstance(); // Pointer to sound engine.
	SoundIndex m_bgMusic =
		m_soundEngine->LoadWav(L"backgroundmusic.wav"); // BG Music index.
	SoundIndex m_thrustSound =
		m_soundEngine->LoadWav(L"thrust.wav"); // Thrust sound index.
	SoundIndex m_explosionSound =
		m_soundEngine->LoadWav(L"explosion.wav"); // Explosion sound index.
	SoundIndex m_refuelSound =
		m_soundEngine->LoadWav(L"refuel.wav"); // Refuel sound index.
	SoundIndex m_lowhpSound =
		m_soundEngine->LoadWav(L"lowhp.wav"); // Low hp sound index.

public:

	Spaceship() : GameObject(SPACESHIP)
	{

	}

	IShape2D& GetCollisionShape();

	void ProcessCollision(GameObject& other);

	void Update(float time);

	void HandleEvent(Event evt);

	void Initialise(Vector2D position);

	void Explode();

	Vector2D GetPos();

	void FireBullet();

	void DrawHud();

	void DrawTrail();

	bool IsDead();
	
	void AddScore(int num);

	void Damage(float num);

}; // Spaceship

// This is the shield class.
// Used to protect the ship
// when it is picked up.
class Shield : public GameObject
{
private:
	float m_damageCap = 150.0f; // Amount of damage taken before removal.
	float m_shieldDuration = 15.0f; // Duration of the shield.
	Circle2D m_collisionShape; // Collision shape of the shield.
	float m_animationSpeed; // Speed at which is cycles through images.
	float m_currentAnimation;  // Holds the current image for that frame.

protected:
	Spaceship* m_pSpaceship;

public:
	Shield() : GameObject(SHIELD)
	{

	}

	IShape2D& GetCollisionShape();

	void ProcessCollision(GameObject& other);

	void Update(float time);

	void HandleEvent(Event evt);

	void Initialise(Spaceship* player);

	void Damage(float num);

	void DrawShieldHud();

	void Draw();
}; // Shield
