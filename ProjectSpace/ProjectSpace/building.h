#include "GameObject.h"
#include "mysoundengine.h"
#include "GameObject.h"

// Class specifically made for buildings.
// Is used to handle buildings in the game.
// Holds different types for the buildings
// such as being able to land to refuel.
class Spaceship;
class Building : public GameObject
{
private:

	Rectangle2D m_collisionShape; // The collision shape of the building.
	BType m_buildingType; // The type of building it will be; defined in initialise/ReInitialise
	wchar_t* m_buildingImages[10]; // Holds all the strings for images.
	BType m_buildingTypes[2]; // Holds all the BTypes for the buildings.
	Spaceship* m_pSpaceship; // Holds a pointer to the player.

public:

	Building() : GameObject(BUILDING)
	{

	}

	IShape2D& GetCollisionShape();

	void ProcessCollision(GameObject& other);

	void Update(float time);

	void HandleEvent(Event evt);

	void Initialise(Vector2D position, BType type, Spaceship* player);

	Vector2D GetPos();

	int ChooseBuilding();

	BType GetBuildType();

	void SetImageNumber(int i);

	int GetImageNumber();

	void SetBuildType(BType b);

	void SetOffScreen(float xPos);

}; // Building

// This is the road class.
// Used as an invisible barrier
// for the road image so it 
// simulates the player landing.
class Road : public GameObject
{

private:
	Rectangle2D m_collisionShape; // The collision shape of the building.
public:

	Road() : GameObject(UNKNOWN)
	{

	}

	IShape2D& GetCollisionShape();

	void ProcessCollision(GameObject& other);

	void Update(float time);

	void HandleEvent(Event evt);

	void Initialise(Vector2D pos);

}; // Road
