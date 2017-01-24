#include "entities.h"
#include "gamecode.h"
#include "myinputs.h"
#include <time.h>

const float SHIPTURN = 3.0f; // Turn rate of the ships angle.
const float SHIPACC = 1200.0f; // Acceleration of the ship.
const float FRICTION = 1.0f; // Friction applied to ships velocity.
const float WORLDGRAVITY = 450.0f; // Gravity applied to all objects.
const float CAMERAOFFSET = 1200.0f; // The offset of the camera.
const float DEATHSPEED = 800.0f; // The speed of instant death collision.
const float PI = 3.142f; // Accurate used for angles.

// Sets up the initial member variables required to function.
void Spaceship::Initialise(Vector2D startPos)
{
	m_position = startPos; 
	m_velocity.set(0,0);
	m_health = 150.0f; 
	m_fuel = 150.0f; 
	LoadImage(L"spaceship.bmp");
}

// Returns the collision shape of the ship.
IShape2D& Spaceship::GetCollisionShape()
{
	m_collisionShape.PlaceAt(m_position.YValue + 64, m_position.XValue
		- 64, m_position.YValue - 64, m_position.XValue + 64);
	return m_collisionShape;
}

// Handles collision with numerous types of objects.
void Spaceship::ProcessCollision(GameObject& other)
{

	// If collision with enemy then deal damage.
	if (other.GetType() == ENEMY)
	{
		Enemy* pEnemy = (Enemy*)&other;
		pEnemy->Explode(NOTPLAYER);

		Damage(50.0f * Game::instance.GetDifficulty());
	}

	// If collision with pickup then check for specific
	// pickup and then apply relevant effects.
	if (other.GetType() == PICKUP)
	{
		Pickup* pPickup = (Pickup*)&other;
		PowerupType powerUp = pPickup->GetPickupType();

		if (powerUp == HEALTH)
		{
			if (m_health + 25.0f > 150.0f)
			{
				m_health = 150.0f;
			}
			else
			{
				m_health += 25.0f;
			}
		}

		if (powerUp == AMMO)
		{
			if (m_ammo + 5 > 15)
			{
				m_ammo = 15;
			}
			else
			{
				m_ammo += 5;
			}
		}

		if (powerUp == PSHIELD)
		{
			Shield* pShield = new Shield;
			pShield->Initialise(this);
			Game::instance.m_objects.AddItem(pShield, true);
		}
		pPickup->OnPickup();
	}

	// If collision with building then deal damage
	// with the collision pushing the ship away.
	if (other.GetType() == BUILDING)
	{
		// Get access to the Building class.
		Building* pOtherBuilding = (Building*)&other;

		// Need to check if it's a fuel building or not.
		BType buildingType = pOtherBuilding->GetBuildType();

		// If the player is accelerating more than the DEATHSPEED
		// value then they need to instantly be killed.
		if (m_velocity.XValue > DEATHSPEED || m_velocity.YValue < -DEATHSPEED)
		{
			Explode();
		}

		// Needs to be barely moving forward whilst also 
		// not falling at a speed to not receive damage.
		if (m_velocity.XValue < 10 && m_velocity.YValue > -250)
		{
			// If the ship is at the right angle and follows the 
			// rules of the speed on XValue and YValue then refuel.
			// Otherwise damage the ship like normal.
			if (m_angle >= (0 - PI / 4) && m_angle <= (0 + PI / 4))
			{
				m_velocity.YValue *= -1.0f;
				m_position.YValue += 0.5f;

				if (m_velocity.YValue < 60)
				{
					if (m_angle != 0.0f && m_angle < 0.0f)
					{
						m_angle += 0.01f;
					}
					else if (m_angle != 0.0f && m_angle > 0.0f)
					{
						m_angle -= 0.01f;
					}

					if ((m_angle < 0.1f && m_angle > 0) || (m_angle > -0.1f && m_angle < 0))
					{
						m_angle = 0.0f;
					}
					m_angleLocked = true;
					if (m_ammo < 15 && buildingType == FUEL)
					{
						m_ammo = m_ammo + 1;
					}

					if (m_fuel < 150 && buildingType == FUEL)
					{
						m_fuel = m_fuel + 10;
						if (m_fuel > 150)
						{
							m_fuel = 150.0f;
						}
						m_soundEngine->Play(m_refuelSound);
					}
				}
			}
			else
			{
				m_velocity.YValue *= -1.25f;
				m_position.YValue += 0.5f;

				m_health = m_health - m_velocity.YValue / 10;

				Explosion* pExplosion = new Explosion;
				pExplosion->Initialise(m_position, Vector2D(300.0f, 0.0f), 32.0f, 2.0f);
				Game::instance.m_objects.AddItem(pExplosion, false);


				m_soundEngine->SetVolume(m_explosionSound, -2500);
				m_soundEngine->Play(m_explosionSound);
			}
		}
		else
		{
			m_velocity.YValue *= -1.25f;
			m_position.YValue += 0.5f;

			m_health = m_health - m_velocity.YValue / 10;

			Explosion* pExplosion = new Explosion;
			pExplosion->Initialise(m_position, Vector2D(300.0f, 0.0f), 32.0f, 2.0f);
			Game::instance.m_objects.AddItem(pExplosion, false);

			if (IsDead() != true)
			{
				Particle* pParticle = new Particle;
				pParticle->Initialise(m_position, Vector2D(0.0f, -250.0f -WORLDGRAVITY), 8.0f, 1.5);
				Game::instance.m_objects.AddItem(pParticle, false);

				m_soundEngine->SetVolume(m_explosionSound, -2500);
				m_soundEngine->Play(m_explosionSound);
			}
		}

	}
}

// Draws the heads up display of score, health, fuel etc.
void Spaceship::DrawHud()
{
	// Make sure that the HUD is drawn on the ReverseTransform of the position.
	Vector2D hudPos = MyDrawEngine::GetInstance()->
		theCamera.ReverseTransform(Vector2D(100, 0));

	// Draw the ammunition for the spaceship.
	MyDrawEngine::GetInstance()->DrawAt(hudPos + Vector2D(800, -50), 
		MyDrawEngine::GetInstance()->LoadPicture(L"ammobar.png"));

	int i = 0;
	for (i; i < m_ammo; i++)
	{
		MyDrawEngine::GetInstance()->DrawAt(hudPos + Vector2D(760 + (10.0f * i)
			, -50), MyDrawEngine::GetInstance()->LoadPicture(L"bulletnum.png"));
	}

	// Draw the fuel bar for the spaceship.
	MyDrawEngine::GetInstance()->DrawAt(hudPos + Vector2D(450, -50)
		, MyDrawEngine::GetInstance()->LoadPicture(L"fuelbar.png"));

	int j = 0;
	for (j; j < (m_fuel / 10); j++)
	{
		MyDrawEngine::GetInstance()->DrawAt(hudPos + Vector2D(410 + (10.0f * j), -50)
			, MyDrawEngine::GetInstance()->LoadPicture(L"fuelnum.png"));
	}

	// Draw the health bar for the spaceship.
	MyDrawEngine::GetInstance()->DrawAt(hudPos + Vector2D(100, -50)
		, MyDrawEngine::GetInstance()->LoadPicture(L"hpbar.png"));

	int k = 0;
	for (k; k < (m_health / 10); k++)
	{
		MyDrawEngine::GetInstance()->DrawAt(hudPos + Vector2D(60 + (10.0f * k), -50)
			, MyDrawEngine::GetInstance()->LoadPicture(L"fuelnum.png"));
	}

	unsigned int color = MyDrawEngine::WHITE;

	// Draw the score for the player.
	MyDrawEngine::GetInstance()->WriteText(hudPos + Vector2D(1850, -50), L"Score: ", color);
	MyDrawEngine::GetInstance()->WriteInt(hudPos + Vector2D(2000, -50), m_score, color);

}

// Draws the flames behind the ships engines.
void Spaceship::DrawTrail()
{
	// Have to create two trails because of two engines.
	// Create left engine flame.

	Explosion* pTrail = new Explosion;
	Vector2D flameDirection; // Makes it fire outside the engines
	Vector2D flameOffset = Vector2D(-20.0f, -75.0f);
	flameOffset = flameOffset.rotatedBy(-m_angle);

	flameDirection.setBearing(m_angle + 3.141f, 600);
	Vector2D flamePos;
	flamePos = Vector2D(m_position + flameOffset);
	pTrail->Initialise(flamePos, flameDirection, 40.0f + 
		float(rand() % 200 / 10.0f), 0.3f);
	Game::instance.m_objects.AddItem(pTrail, false);

	// Create right engine flame.

	Explosion* pTrail2 = new Explosion;
	Vector2D flameDirection2; // Makes it fire outside the engines
	Vector2D flameOffset2 = Vector2D(+20.0f, -75.0f);
	flameOffset2 = flameOffset2.rotatedBy(-m_angle);

	flameDirection2.setBearing(m_angle + 3.141f, 600);
	Vector2D flamePos2;
	flamePos2 = Vector2D(m_position + flameOffset2);
	pTrail2->Initialise(flamePos2, flameDirection2, 40.0f 
		+ float(rand() % 200 / 10.0f), 0.3f);
	Game::instance.m_objects.AddItem(pTrail2, false);
}

// Returns true if dead.
bool Spaceship::IsDead()
{
	return m_health <= 0;
}

// Handles all the checks, movement,
// firing, trails etc etc.
void Spaceship::Update(float time)
{
	// Make sure that the HUD is drawn every frame.
	DrawHud();

	if (m_position.XValue > m_lastXPos + 1)
	{
		m_score += (1 * Game::instance.GetDifficulty());
	}

	if (m_lastXPos <= m_position.XValue)
	{
		m_lastXPos = m_position.XValue;
	}

	// Apply acceleration to the ship through temp var.

	Vector2D accVar;

	// Checks if the ship is thrusting.

	static bool isThrusting = false; 

	// Makes sure that the keyboard is sampled for detection.

	MyInputs* pInputs = MyInputs::GetInstance();
	pInputs->SampleKeyboard();

	// Check if the player is lower than 30 hp.

	if (m_health <= 30.0f && m_lastLowhp <= 0.0f)
	{
		m_soundEngine->SetVolume(m_lowhpSound, -3000);
		m_soundEngine->Play(m_lowhpSound);
		m_lastLowhp = 1.0f;
	}

	// Check that the player is not dead
	if (IsDead() == true)
	{
		Explode();
	}

	// Used to make an accurate angle movement depending on collision.
	// Basically makes it look like the ship has realistically collided.
	// We use 5.9 and -5.9 as a full turn is either -6 or 6.

	if (m_angle > 5.9999 || m_angle < -5.9999)
	{
		m_angle = 0.0;
	}

	// Allow the player to kill themselves.
	// Useful for when the player runs out of fuel.
	if (pInputs->KeyPressed(DIK_K))
	{
		m_health = 0;
	}

	// This is where movement is handled.
	// When the player presses UP make it move.

	if (pInputs->KeyPressed(DIK_UP) && m_fuel > 0 && m_lastCutoff < 0)
	{

		// Movement of the ship itself.
		accVar.setBearing(m_angle, SHIPACC);
		m_velocity = m_velocity + accVar * time;
		m_fuel = m_fuel - 3 * time * Game::instance.GetDifficulty();

		if (m_angleLocked)
		{
			m_angleLocked = false;
		}

		DrawTrail();
		isThrusting = true;
	}

	// Handles the thrusting sound.
	// Stops playing when the player
	// is no longer thrusting.

	if (!pInputs->KeyPressed(DIK_UP))
	{
		isThrusting = false;
		m_soundEngine->Stop(m_thrustSound);
	}
	else if (m_lastCutoff < 0 && IsDead() == false)
	{
		if (m_velocity.XValue < 300.0f)
		{
			m_soundEngine->SetVolume(m_thrustSound, -3500);
			m_soundEngine->Play(m_thrustSound, true);
		}
		else if (m_velocity.XValue < 900.0f)
		{
			m_soundEngine->SetVolume(m_thrustSound, -2500);
			m_soundEngine->Play(m_thrustSound, true);
		}
		else
		{
			m_soundEngine->SetVolume(m_thrustSound, -1500);
			m_soundEngine->Play(m_thrustSound, true);
		}
	}

	// This is where shooting is handled.
	// Fires projectiles when space is pressed.

	if (pInputs->KeyPressed(DIK_SPACE) && m_bulletTime < 0 && m_ammo > 0)
	{
		FireBullet();
	}

	// When the player goes left turn left.

	if (pInputs->KeyPressed(DIK_LEFT) && !m_angleLocked)
	{
		m_angle -= SHIPTURN * time;
	}

	// When the player goes right turn right.

	if (pInputs->KeyPressed(DIK_RIGHT) && !m_angleLocked)
	{
		m_angle += SHIPTURN * time;
	}

	// When the player is in a situation where they can't fly
	// and they don't land on a fuel station; press k to suicide.
	if (pInputs->KeyPressed(DIK_K))
	{
		Explode();
	}

	// Makes sure the camera follows the spaceship
	// If the player goes too high the camera doesn't follow them.

	MyDrawEngine::GetInstance()->theCamera.PlaceAt(Vector2D(m_xPosMax + 
		CAMERAOFFSET, -m_position.YValue + 200.0f));

	if (m_position.YValue > 500)
	{
		m_soundEngine->Stop(m_thrustSound);
		m_lastCutoff = 1.0f;
	}
	if (m_position.YValue <= 200.0f)
	{
		MyDrawEngine::GetInstance()->theCamera.PlaceAt(Vector2D(m_xPosMax 
			+ CAMERAOFFSET, 0.0f));
	}

	// Takes the frametime away from the
	// already set time on the variable.
	// Basically it causes delay per shot.
	// And causes delay before thrust.

	m_bulletTime = m_bulletTime - time;
	m_lastCutoff = m_lastCutoff - time;

	// Applies the calculations to the velocity of the ship.
	// After the acceleration has changed it is then immediately
	// to the position of the spaceship.

	m_velocity = m_velocity - FRICTION * time * m_velocity + Vector2D(0,
		-WORLDGRAVITY) * time;

	m_position = m_position + m_velocity * time;
	m_lastLowhp -= time;

	// Set the max X pos to the current position.
	// Used to make sure they can't go back too far.

	if (m_position.XValue > m_xPosMax)
	{
		m_xPosMax = m_position.XValue;
	}

	// If they try to go too far back then
	// push them back the way they have to go.

	if (m_position.XValue < m_xPosMax -500)
	{
		m_position.XValue = m_xPosMax- 475;
		m_velocity = -m_velocity;
	}

	// Check if they are getting to where the road is.
	// If they are simulate a landing; apply damage if necessary.

	if (m_position.YValue <= -665)
	{		
		
		m_velocity.YValue *= -1.0f;
		m_position.YValue += 0.5f;
	
		if (m_velocity.YValue > 200)
		{
			m_health = m_health - (m_velocity.YValue / 10);

			Explosion* pExplosion = new Explosion;
			pExplosion->Initialise(m_position, Vector2D(300.0f, 0.0f), 32.0f, 2.0f);
			Game::instance.m_objects.AddItem(pExplosion, false);


			m_soundEngine->SetVolume(m_explosionSound, -2500);
			m_soundEngine->Play(m_explosionSound);
		}
	}
} 

// Not used.
void Spaceship::HandleEvent(Event evt)
{

} 

// Creates particles and explosion effects.
void Spaceship::Explode()
{
	// Stop the thrust sound and play explosion sound.
	m_soundEngine->SetVolume(m_explosionSound, -3500);
	m_soundEngine->Play(m_explosionSound);
	m_soundEngine->Stop(m_thrustSound);

	// Draws an explosion at the ships position.
	Explosion* pExplosion = new Explosion;
	pExplosion->Initialise(m_position, Vector2D(300.0f, 0.0f), 68.0f, 3.0f);

	// Draws particle effects at the ships position that fall.
	Particle* pParticle = new Particle;
	pParticle->Initialise(m_position, Vector2D(0.0f, -300.0f -WORLDGRAVITY), 8.0f, 1.5f);

	// Make sure to add them to the object list.
	Game::instance.m_objects.AddItem(pExplosion, false);
	Game::instance.m_objects.AddItem(pParticle, false);

	// Sets the ship to inactive and deletes the object.
	Deactivate();

	Game::instance.SetScore(m_score);
	Game::instance.SetGameOver();


} 

// Fires a bullet whenever the player presses space.
void Spaceship::FireBullet()
{
	// Set up all the calculation variables.
	// We need velocity and positions with
	// offset for the spaceship firing pos.
	Vector2D bulletPos;
	Vector2D bulletVel;
	Vector2D bulletOffset = Vector2D(0, 100.0f);
	bulletOffset = bulletOffset.rotatedBy(-m_angle);

	// Do the calculation.
	// Apply angle of ship to the bullet.
	// Apply the position of the ship to the bullet with offset.

	bulletPos.setBearing(m_angle, 1.0f);
	bulletPos = Vector2D(m_position + bulletOffset);
	bulletVel.setBearing(m_angle, 1000.0f);
	bulletVel = bulletVel + m_velocity;

	// Create the bullet and apply calculations.
	// Add it to the objects list at run time.
	// Remove a bullet from the players munitions.
	Bullet* pBullet = new Bullet;
	pBullet->Initialise(bulletPos, bulletVel, PLAYER);
	Game::instance.m_objects.AddItem(pBullet, true); 
	m_bulletTime = 0.5f; 
	m_ammo = m_ammo - 1;

} 

// Returns the ships position.
Vector2D Spaceship::GetPos()
{
	return m_position;
}

// Adds points to the player's score.
void Spaceship::AddScore(int i)
{
	m_score += i;
}

// Takes health from the ship.
void Spaceship::Damage(float f)
{

	// Remove dealt damage.
	m_health -= f;

	// Set the volume of explosion and play it.
	m_soundEngine->SetVolume(m_explosionSound, -1500);
	m_soundEngine->Play(m_explosionSound);

	// Draw an explosion effect at the ships position.
	Explosion* pExplosion = new Explosion;
	pExplosion->Initialise(m_position, Vector2D(300.0f, 0.0f), 68.0f, 3.0f);

	// Draw particle effects at the ships position that fall.
	Particle* pParticle = new Particle;
	pParticle->Initialise(m_position, Vector2D(0.0f, -300.0f - WORLDGRAVITY), 8.0f, 1.5f);

	// Add them to the object list.
	Game::instance.m_objects.AddItem(pExplosion, false);
	Game::instance.m_objects.AddItem(pParticle, false);
}

// Chooses a random building image.
int Building::ChooseBuilding()
{

	// Load all the images.

	LoadImage(L"building1.png");
	LoadImage(L"building2.png");
	LoadImage(L"building3.png");
	LoadImage(L"building4.png");
	LoadImage(L"building5.png");
	LoadImage(L"building6.png");
	LoadImage(L"building7.png");
	LoadImage(L"building8.png");
	LoadImage(L"building9.png");

	// Select an image at random and return.
	int rndNum = rand() % 9;
	return rndNum;
}

// Sets up the initial member variables required to function.
void Building::Initialise(Vector2D startPos, BType buildType, Spaceship* player)
{
	// Setup the variables with the values from the arguments.
	m_pSpaceship = player;
	m_position = startPos;
	int rndSize = rand() % 3 + 1;

	// Randomly scale the buildings.
	// Make sure to place them correctly.
	if (rndSize == 1)
	{
		m_imageScale = 1.5f;
		m_position.YValue -= 32;
	}
	else if (rndSize == 2)
	{
		m_imageScale = 2.0f;
	}
	else
	{
		m_imageScale = 2.5f;
		m_position.YValue += 32;
	}

	// Set up the types.
	m_buildingTypes[0] = BType::FUEL;
	m_buildingTypes[1] = BType::NOFUEL;

	// Choose a building to start off with.
	int rndBuild = ChooseBuilding();

	// Make sure type is fuel if building is.
	if (rndBuild == 4)
	{
		m_buildingType = BType::FUEL;
	}

	m_imageNumber = rndBuild;
}

// Returns the collisionshape of the building.
IShape2D& Building::GetCollisionShape()
{
	m_collisionShape.PlaceAt(m_position.YValue + 64 * m_imageScale,
		m_position.XValue - 64 * m_imageScale, 
		m_position.YValue - 64 * m_imageScale,
		m_position.XValue + 64 * m_imageScale);
	return m_collisionShape;
}

// Not used.
void Building::ProcessCollision(GameObject& other)
{

}

// Not used.
void Building::Update(float time)
{
	
}

// Returns the buildings position.
Vector2D Building::GetPos()
{
	return m_position;
}

// Returns the buildings building type.
BType Building::GetBuildType()
{
	return m_buildingType;
}

// Not used.
void Building::HandleEvent(Event evt)
{

}

// Sets the images number.
// Used by GameManager class.
void Building::SetImageNumber(int i)
{
	m_imageNumber = i;
}

// Get's the image number.
// Used by GameManager class.
int Building::GetImageNumber()
{
	return m_imageNumber;
}

// Set's the buildings building type.
// Used by GameManager class.
void Building::SetBuildType(BType b)
{
	m_buildingType = b;
}

// Set's the buildings off the screen.
// Used by GameManager class.
void Building::SetOffScreen(float xPos)
{
	m_position.XValue = xPos;
}

// Sets up the initial member variables required to function.
void Explosion::Initialise(Vector2D position, Vector2D velocity, 
	float animationSpeed, float scale)
{

	// Setup the initial member variables.
	m_velocity = velocity;
	m_position = position;
	m_animationSpeed = animationSpeed;
	m_currentAnimation = 0.0f;
	m_imageScale = scale;
	m_drawDepth = 1;

	// Load all the images to simulate an explosion.
	LoadImage(L"explosion1.png");
	LoadImage(L"explosion2.png");
	LoadImage(L"explosion3.png");
	LoadImage(L"explosion4.png");
	LoadImage(L"explosion5.png");
	LoadImage(L"explosion6.png");
	LoadImage(L"explosion7.png");
	LoadImage(L"explosion8.png");
}

// Handles movement and deactivation.
void Explosion::Update(float frametime)
{
	// Goes through the images one by one at frametime.
	m_currentAnimation += m_animationSpeed* frametime;

	// Removes the explosion once the transition has ended.
	if (m_currentAnimation >= 8.0f)
	{
		Deactivate();
		m_currentAnimation = 0;
	}

	// Makes the explosion move.
	m_position = m_position + m_velocity*frametime;

	// Sets the image to the number it has reached so far.
	m_imageNumber = int(m_currentAnimation);
}

// Not used.
void Explosion::ProcessCollision(GameObject& other)
{
	
}

// Sets up the initial member variables required to function.
void Particle::Initialise(Vector2D position, Vector2D velocity,
	float animationSpeed, float scale)
{
	// Set up the initial member variables.
	m_hasLooped = false;
	m_velocity = velocity;
	m_position = position;
	m_animationSpeed = animationSpeed;
	m_currentAnimation = 0.0f;
	m_imageScale = scale;
	m_angle = 3.0f;

	// Load all the images to simulate particle effect.
	LoadImage(L"particle0.png");
	LoadImage(L"particle1.png");
	LoadImage(L"particle2.png");
	LoadImage(L"particle3.png");
	LoadImage(L"particle4.png");
	LoadImage(L"particle5.png");
	LoadImage(L"particle6.png");
	LoadImage(L"particle7.png");
	LoadImage(L"particle8.png");
}

// Handles movement and deactivation.
void Particle::Update(float frametime)
{
	// Goes through the images one by one at frametime.
	m_currentAnimation += m_animationSpeed* frametime;

	// Removes the particle once the transition has ended.
	if (m_currentAnimation >= 9.0f)
	{
		if (m_hasLooped)
		{
			Deactivate();
			m_currentAnimation = 0;
		}
		else
		{
			m_currentAnimation = 0;
			m_hasLooped = true;
		}
	}

	// Makes the particle move.
	m_position = m_position + m_velocity*frametime;

	// Sets the image to the number it has reached so far.
	m_imageNumber = int(m_currentAnimation);
}

// Not used.
void Particle::ProcessCollision(GameObject& other)
{

}

// Sets up the initial member variables required to function.
void Bullet::Initialise(Vector2D position, Vector2D velocity, FiredBy attacker)
{
	// Set up the initial member variables.
	m_firedBy = attacker;
	m_position = position;
	m_velocity = velocity;
	m_timer = 20.0;
	LoadImage(L"bullet.png");
	m_soundEngine->SetVolume(m_bulletSound, -1500);
	m_soundEngine->Play(m_bulletSound);
}

// Checks if the bullet is still active.
// Removes the bullet if it has not 
// collides with anything in given time.
void Bullet::Update(float frametime)
{
	// Remove a second from the time
	// Change its position.
	m_timer -= frametime;
	m_position = m_position + m_velocity * frametime;
	m_velocity.YValue -= WORLDGRAVITY * frametime;
	if (m_timer <= 0)
	{
		Deactivate();
	}
}

// Returns who fired the bullet.
FiredBy Bullet::GetFiredBy()
{
	return m_firedBy;
}

// Handles collision with multiple object types.
void Bullet::ProcessCollision(GameObject& other)
{
	// If collision is with a building 
	// then create explosion, particles 
	// and play explosion sound file.
	// Then remove the bullet from play.
	if (other.GetType() == BUILDING)
	{
		m_soundEngine->SetVolume(m_explodeSound, -1500);
		m_soundEngine->Play(m_explodeSound);

		Deactivate();

		Explosion* pExplosion = new Explosion;
		pExplosion->Initialise(m_position, Vector2D(300.0f, 0.0f), 68.0f,
			0.3f);

		Particle* pParticle = new Particle;
		pParticle->Initialise(m_position, Vector2D(0.0f, -300.0f -WORLDGRAVITY), 8.0f, 1.5f);

		Game::instance.m_objects.AddItem(pExplosion, false);
		Game::instance.m_objects.AddItem(pParticle, false);
	}

	// If collision with a spaceship 
	// then deal damage and remove bullet
	// from play. The sounds will be 
	// handles from the ships damage function.
	if (other.GetType() == SPACESHIP)
	{
		if (GetFiredBy() != PLAYER)
		{
			Deactivate();
			Spaceship* pSpaceship = (Spaceship*)&other;
			pSpaceship->Damage(25.0f * Game::instance.GetDifficulty());
		}
	}
	
	// If collision with a enemy 
	// then explode enemy and 
	// remove bullet from play.
	// the enemies explode function
	// will play the sounds.
	if (other.GetType() == ENEMY)
	{
		if (GetFiredBy() != BYENEMY)
		{
			Deactivate();
			Enemy* pEnemy = (Enemy*)&other;
			pEnemy->Explode(ISPLAYER);
		}
	}

	// If the bullet collides with 
	// another bullet then it will
	// explode with sound and effects.
	if (other.GetType() == BULLET)
	{

		Explode();
	}
}

// Causes an explosion where the bullet was.
void Bullet::Explode()
{
	// Plays the explosion sound.
	m_soundEngine->SetVolume(m_explodeSound, -1500);
	m_soundEngine->Play(m_explodeSound);

	// Draws an explosion effect at position of bullet.
	Explosion* pExplosion = new Explosion;
	pExplosion->Initialise(m_position, Vector2D(300.0f, 0.0f), 68.0f, 1.0f);

	// Draws particle effects at position of bullet and falls.
	Particle* pParticle = new Particle;
	pParticle->Initialise(m_position, Vector2D(0.0f, -300.0f - WORLDGRAVITY), 8.0f, 1.0f);

	// Add the objects to the object list.
	Game::instance.m_objects.AddItem(pExplosion, false);
	Game::instance.m_objects.AddItem(pParticle, false);
	
	// Remove the bullet from play.
	Deactivate();
}

// Not really used just required by superclass.
IShape2D& GameManager::GetCollisionShape()
{
	return m_collisionShape;
}

// Not really used just required by superclass.
void GameManager::ProcessCollision(GameObject& other)
{

}

// Creates all the objects such as buildings
// and the player ship and keeps track of buildings.
void GameManager::Initialise()
{
	// Set the position of this entity off the screen.
	// Simply used to make sure "No Image" doesn't appear
	// half way through the game.
	m_position = Vector2D(-1343245, -142523);

	// Set up the player for the game.
	// Also set the m_pSpaceship to point
	// to the players spaceship for enemies.
	Spaceship* pShip = new Spaceship;
	pShip->Initialise(Vector2D(10, 10));
	Game::instance.m_objects.AddItem(pShip, true);

	// Assign the spaceship pointer variable to the player ship.	
	m_pSpaceship = pShip;

	// Loops NUM_OF_BUILDINGS times to create that many
	// The GameManager will handle their positions
	// as well as their building types and images later.
	for (int i = 0; i < NUM_OF_BUILDINGS; i++)
	{
		m_buildingHolder[i] = new Building;
		int posNum = 400 * i;
		m_buildingHolder[i]->Initialise(Vector2D(-450.0F + posNum, -600.0f), NOFUEL, pShip);
		Game::instance.m_objects.AddItem(m_buildingHolder[i], true);
	}
}

// Spawns in enemies and moves buildings when need be.
// Also spawns in pickups with building movements randomly.
void GameManager::Update(float time)
{
	// Get the position of the world co-ordinates.
	float camPos = MyDrawEngine::GetInstance()->
		theCamera.ReverseTransform(Vector2D(0, 0)).XValue;

	// Draw the space
	MyDrawEngine::GetInstance()->DrawAt(Vector2D(camPos + (CAMERAOFFSET * 1.5f)
		, -860.0f), MyDrawEngine::GetInstance()->LoadPicture(L"bg.jpg"), 2);

	// Draw the sky 
	MyDrawEngine::GetInstance()->DrawAt(Vector2D(camPos + (CAMERAOFFSET * 1.5f)
		, 0), MyDrawEngine::GetInstance()->LoadPicture(L"earthbg.jpg"), 2);

	// Draw the road
	MyDrawEngine::GetInstance()->DrawAt(Vector2D(camPos + (CAMERAOFFSET * 1.5f)
		, -865.0f), MyDrawEngine::GetInstance()->LoadPicture(L"road2.png"), 2.1f);

	int pickChance = rand() % 100; // Chance to spawn a pickup.
	int enemySpawnChance = rand() % 100; // Chance to spawn an enemy.
	int enemySpawnRate = 1; // Changes the rate of which the enemies spawn.
	int enemyType = rand() % 3 + 1; // Chooses between 3 types of enemy.

	// Loop through all of the buildings to check if any are off screen.
	for (int i = 0; i < NUM_OF_BUILDINGS; i++)
	{
		if (m_buildingHolder[i]->GetPos().XValue < camPos - 100)
		{

			// Set up a new building and change type accordingly.
			// Move it to the other side of the screen.
			m_buildingHolder[i]->SetImageNumber(m_buildingHolder[i]->ChooseBuilding());
			if (m_buildingHolder[i]->GetImageNumber() == 4)
			{
				m_buildingHolder[i]->SetBuildType(BType::FUEL);
			}
			else
			{
				m_buildingHolder[i]->SetBuildType(BType::NOFUEL);
			}
			m_buildingHolder[i]->SetOffScreen(camPos + MyDrawEngine::GetInstance()->GetScreenWidth() * 2);
		
			// Depending on how far along the player is adjust spawn rate if necessary.
			if (enemySpawnRate < 3 && m_buildingHolder[i]->GetPos().XValue > 12000)
			{
				enemySpawnRate = 3;
			}
			else if (enemySpawnRate < 2 && m_buildingHolder[i]->GetPos().XValue > 6000)
			{
				enemySpawnRate = 2;
			}

			// Chance to spawn a pickup above the building.
			if (pickChance > 90)
			{
				Pickup* newPickup = new Pickup;
				newPickup->Initialise(Vector2D(m_buildingHolder[i]->GetPos().XValue 
					+ 32, m_buildingHolder[i]->GetPos().YValue + 200));
				Game::instance.m_objects.AddItem(newPickup, true);
			}

			// Chance to spawn an enemy above the building.
			if (enemySpawnChance > 180 / enemySpawnRate)
			{
				if (enemyType < 2)
				{
					Enemy* pEnemy = new Enemy;
					pEnemy->Initialise(Vector2D(m_buildingHolder[i]->GetPos().XValue
						+ 32, m_buildingHolder[i]->GetPos().YValue + 400), m_pSpaceship);
					Game::instance.m_objects.AddItem(pEnemy, true);
				}
				else if (enemyType < 3)
				{
					EnemyShooter* pEnemy = new EnemyShooter;
					pEnemy->Initialise(Vector2D(m_buildingHolder[i]->GetPos().XValue
						+ 32, m_buildingHolder[i]->GetPos().YValue + 400), m_pSpaceship);
					Game::instance.m_objects.AddItem(pEnemy, true);
				}
				else
				{
					EnemyScroller* pEnemy = new EnemyScroller;
					pEnemy->Initialise(Vector2D(m_buildingHolder[i]->GetPos().XValue
						+ 32, m_buildingHolder[i]->GetPos().YValue + 400), m_pSpaceship);
					Game::instance.m_objects.AddItem(pEnemy, true);
				}
			}
		}
	}
}

// Not used just required by superclass.
void GameManager::HandleEvent(Event evt)
{

}

// Not used.
int GameManager::GetScore()
{
	return m_playerScore;
}

// Returns the collisionshape of the pickup.
IShape2D& Pickup::GetCollisionShape()
{
	m_collisionShape.PlaceAt(m_position.YValue + 16 * m_imageScale,
		m_position.XValue - 16 * m_imageScale,
		m_position.YValue - 16 * m_imageScale, 
		m_position.XValue + 16 * m_imageScale);
	return m_collisionShape;
}

// Not used just required by the superclass.
void Pickup::ProcessCollision(GameObject& other)
{

}

// Sets up the initial member variables required to function.
void Pickup::Initialise(Vector2D pos)
{
	// Load the required images.
	LoadImage(L"pickup_ammo.png");
	LoadImage(L"pickup_hp.png");
	LoadImage(L"shield_pickup.png");

	// Choose a random pickup.
	// Declate temp type.
	int rndNum = ChoosePickup();
	PowerupType type;
	
	// Assign the type accordingly.
	if (rndNum == 0)
	{
		type = AMMO;
	}
	else if (rndNum == 1)
	{
		type = HEALTH;
	}
	else
	{
		type = PSHIELD;
	}

	// Set up the initial member variables.
	// Also provide powerup and image.
	m_position = pos;
	m_angle = 0.0f;
	m_powerUp = type;
	m_imageScale = 2.0f;
	m_imageNumber = rndNum;
}

// Not used just required by the superclass.
void Pickup::Update(float time)
{
	
}

// Returns the pickup type.
PowerupType Pickup::GetPickupType()
{
	return m_powerUp;
}

// Removes the object from the game when picked up by player.
void Pickup::OnPickup()
{
	// Play sound and set inactive.
	m_soundEngine->Play(m_pickupSound);
	m_active = false;
}

// Chooses a random pickup.
int Pickup::ChoosePickup()
{
	int rndNum = rand() % 3;
	return rndNum;
}

// Sets up the initial member variables required to function.
void Enemy::Initialise(Vector2D position, Spaceship* playerref)
{
	// Set up the initial member variables.
	m_position = position;
	m_pSpaceship = playerref;
	m_imageScale = 2.0f;
	LoadImage(L"enemy1.png");
}


// Returns the collisionshape of the enemy.
IShape2D& Enemy::GetCollisionShape()
{
	m_collisionShape.PlaceAt(m_position.YValue + 32 * m_imageScale,
	m_position.XValue - 32 * m_imageScale,
	m_position.YValue - 32 * m_imageScale,
	m_position.XValue + 32 * m_imageScale);
	return m_collisionShape;
}

// Not used just required by superclass.
void Enemy::ProcessCollision(GameObject& other)
{

}

// Handles movement of the enemy.
void Enemy::Update(float time)
{
	// Get the world coordinates.
	float camPos = MyDrawEngine::GetInstance()->
		theCamera.ReverseTransform(Vector2D(0, 0)).XValue;

	// If the enemy is off the screen on the left
	// remove it from the game as it is passed.
	if (m_position.XValue < camPos - 100)
	{
		Deactivate();
	}

	// If the spaceship is still alive
	// Then mimic the ship to attempt to
	// stop the player from getting past.
	if (m_pSpaceship != nullptr && m_pSpaceship->IsDead() == false)

	{
		float playerPos = m_pSpaceship->GetPos().YValue;
		if (m_position.YValue < playerPos)
		{
			m_position.YValue += 1.0f * Game::instance.GetDifficulty();
		}
		else if (m_position.YValue > playerPos && playerPos > -200)
		{
			m_position.YValue -= 1.0f * Game::instance.GetDifficulty();
		}
	}
	else
	{
		if (m_position.YValue < 600 && m_position.YValue <= -200)
		{
			m_position.YValue += 1.0f * Game::instance.GetDifficulty();
		}
		else 
		{
			m_position.YValue -= 1.0f * Game::instance.GetDifficulty();
		}
	}

}

// Not used just required by the superclass.
void Enemy::HandleEvent(Event evt)
{

}

// Creates an explosion and removes the object.
void Enemy::Explode(DeathType type)
{
	// If the player exploded it
	// provide score depending
	// on the game difficulty.
	if (type == ISPLAYER && m_pSpaceship != nullptr)
	{
		m_pSpaceship->AddScore(250 * Game::instance.GetDifficulty());
	}

	// Play explosion sound.
	m_soundEngine->SetVolume(m_explosionSound, -1500);
	m_soundEngine->Play(m_explosionSound);

	// Draws an explosion at the position of the enemy.
	Explosion* pExplosion = new Explosion;
	pExplosion->Initialise(m_position, Vector2D(300.0f, 0.0f), 68.0f, 3.0f);

	// Draws particle effects at the position of the enemy that fall.
	Particle* pParticle = new Particle;
	pParticle->Initialise(m_position, Vector2D(0.0f, -300.0f - WORLDGRAVITY), 8.0f, 1.5f);

	// Add the objects to the object list.
	Game::instance.m_objects.AddItem(pExplosion, false);
	Game::instance.m_objects.AddItem(pParticle, false);
	
	// Remove enemy from the game.
	Deactivate();
}

// Sets up the initial member variables required to function.
void EnemyScroller::Initialise(Vector2D position, Spaceship* player)
{
	// Set up initial member variables.
	m_drawDepth = 1;
	m_position = position;
	m_pSpaceship = player;
	m_imageScale = 2.0f;
	LoadImage(L"enemy2.png");
}

// Handles movement of the enemy scroller.
void EnemyScroller::Update(float time)
{
	// Get world coordinates.
	float camPos = MyDrawEngine::GetInstance()->
		theCamera.ReverseTransform(Vector2D(0, 0)).XValue;

	// If the player is off the screen 
	// then remove from the game.
	if (m_position.XValue < camPos - 100)
	{
		Deactivate();
	}

	// Follow movement of the player ship
	// whilst speeding to collide with them.
	// Moves faster depending on difficulty.
	if (m_pSpaceship != nullptr)
	{
		float playerPos = m_pSpaceship->GetPos().YValue;
		if (m_position.YValue < playerPos)
		{
			m_position.YValue += 1.0f * Game::instance.GetDifficulty();
		}
		else if (m_position.YValue > playerPos && playerPos > -200)
		{
			m_position.YValue -= 1.0f * Game::instance.GetDifficulty();
		}
	}

	m_position.XValue += -180.0f * Game::instance.GetDifficulty() * time;


}
// Sets up the initial member variables required to function.
void EnemyShooter::Initialise(Vector2D position, Spaceship* player)
{
	// Set initial member variables.
	m_drawDepth = 1;
	m_position = position;
	m_pSpaceship = player;
	m_imageScale = 2.0f;
	LoadImage(L"enemy3.png");
}

// Handles firing of enemy shooter.
void EnemyShooter::Update(float time)
{
	// Get world coordinates.
	float camPos = MyDrawEngine::GetInstance()->
		theCamera.ReverseTransform(Vector2D(0, 0)).XValue;

	// If the enemy shooter is off screen then remove.
	if (m_position.XValue < camPos - 100)
	{
		Deactivate();
	}

	// Get the distance between and set angle accordingly.
	if (m_pSpaceship != nullptr)
	{
		Vector2D targetPlayer = m_pSpaceship->GetPos() - m_position;
		m_angle = targetPlayer.angle();
	}

	// If the player isn't dead and the bullet isn't on cooldown
	// fire a bullet at the angle towards the players ship.
	if (m_bulletTime < 0 && m_pSpaceship != nullptr && m_pSpaceship->IsDead() == false)
	{
		FireBullet();
	}

	m_bulletTime -= time;
}

// Fires a bullet.
void EnemyShooter::FireBullet()
{
	// Set up all the calculation variables.

	Vector2D bulletPos;
	Vector2D bulletVel;
	Vector2D bulletOffset = Vector2D(0, 100.0f);
	bulletOffset = bulletOffset.rotatedBy(-m_angle);

	// Do the calculation.
	// Apply angle of ship to the bullet.
	// Apply the position of the ship to the bullet with offset.

	bulletPos.setBearing(m_angle, 1.0f);
	bulletPos = Vector2D(m_position + bulletOffset);
	bulletVel.setBearing(m_angle, 800.0f);

	// Create the bullet and apply calculations.
	// Add it to the objects list at run time.
	// Remove a bullet from the players munitions.
	Bullet* pBullet = new Bullet;
	pBullet->Initialise(bulletPos, bulletVel, BYENEMY);
	Game::instance.m_objects.AddItem(pBullet, true);
	m_bulletTime = 3.0f / Game::instance.GetDifficulty();
}

// Sets up the initial member variables required to function.
void Shield::Initialise(Spaceship* player)
{
	// Set up the initial member variables.
	m_pSpaceship = player;
	m_drawDepth = -1;
	m_animationSpeed = 64.0f;
	m_currentAnimation = 0.0f;

	LoadImage(L"shield.png");
	LoadImage(L"shield1.png");
	LoadImage(L"shield2.png");
	LoadImage(L"shield3.png");

	m_position = m_pSpaceship->GetPos();
}

// Draws the HUD and handles movement
// Also checks for expiration of self.
void Shield::Update(float time)
{
	if (m_pSpaceship != nullptr)
	{
		// Goes through the images one by one at frametime.
		m_currentAnimation += m_animationSpeed * time;

		// Resets the transition
		if (m_currentAnimation >= 4.0f)
		{
			m_currentAnimation = 0;
		}

		// Sets the image to the number it has reached so far.
		m_imageNumber = int(m_currentAnimation);

		m_position = m_pSpaceship->GetPos();

		DrawShieldHud();

		m_shieldDuration -= time;

		if (m_shieldDuration < 0.0f || m_damageCap <= 0.0f)
		{
			Deactivate();
		}
	}
}

// Returns the collisionshape of the shield.
IShape2D& Shield::GetCollisionShape()
{
	m_collisionShape.PlaceAt(m_position, 100.0f);
	return m_collisionShape;
}

// Handles collisions with bullets.
void Shield::ProcessCollision(GameObject&other)
{
	if (other.GetType() == BULLET)
	{
		Bullet* pBullet = (Bullet*)&other;
		if (pBullet->GetFiredBy() != PLAYER)
		{
			Damage(15.0f);
			pBullet->Explode();
		}
	}
}

// Not used just required by the superclass.
void Shield::HandleEvent(Event evt)
{

}

// Applies damage to the shield.
void Shield::Damage(float f)
{
	m_damageCap -= f; 
}

// Draws the HUD of the shield.
void Shield::DrawShieldHud()
{
	// Make sure that the HUD is drawn on the ReverseTransform of the position.
	Vector2D hudPos = MyDrawEngine::GetInstance()->
		theCamera.ReverseTransform(Vector2D(100, 0));

	// Draw the health bar for the spaceship.
	MyDrawEngine::GetInstance()->DrawAt(hudPos + Vector2D(1200.0f, -50.0f)
		, MyDrawEngine::GetInstance()->LoadPicture(L"shieldbar.png"));

	int i = 0;
	for (i; i < (m_damageCap/ 10); i++)
	{
		MyDrawEngine::GetInstance()->DrawAt(hudPos + Vector2D(1160.0f + (10.0f * i), -50.0f)
			, MyDrawEngine::GetInstance()->LoadPicture(L"shieldnum.png"));
	}

}

void Shield::Draw()
{
	MyDrawEngine* pDE = MyDrawEngine::GetInstance();


	if (m_imageNumber >= 0 && m_imageNumber < int(m_images.size()))
	{
		pDE->DrawAt(m_position, m_images[m_imageNumber], m_imageScale, -m_angle, 1 - m_damageCap / 150.0f);
	}
	else
	{
		pDE->WriteText(m_position, L"No image", pDE->LIGHTGREEN, 0);
	}
}