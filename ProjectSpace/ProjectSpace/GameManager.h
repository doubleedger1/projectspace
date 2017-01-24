#include "entities.h"

const int NUM_OF_BUILDINGS = 10; // Sets the number of buildings to have in the game.
const int MAX_ENEMIES = 10; // This is the max amount of enemies that can be spawned.

// This is the GameManager class.
// It's used to handle all of the
// objects used in the game.
class GameManager : public GameObject
{
private:
	Building* m_buildingHolder[NUM_OF_BUILDINGS];
	Enemy* m_listOfEnemies[MAX_ENEMIES];
	Spaceship* m_pSpaceship;
	Circle2D m_collisionShape;
	int m_playerScore;

public:

	GameManager() : GameObject(UNKNOWN)
	{

	}

	IShape2D& GetCollisionShape();

	void ProcessCollision(GameObject& other);

	void Update(float time);

	void HandleEvent(Event evt);

	void Initialise();

	void SetScore(int i);

	int GetScore();

}; // GameManager

