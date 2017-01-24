#pragma once


// List of object types available in the game
// Add a name for each possible type of entity in your game
// Still using old enums until C++11 is available

enum ObjectType{UNKNOWN, SHIP, PICKUP, BUILDING, SPACESHIP, FLAME,
	BULLET, EXPLOSION, FRAGMENT, ENEMY, SHIELD};

enum BType{FUEL, NOFUEL};

enum FiredBy{PLAYER, BYENEMY};

enum EnemyType{HELI, ESHIP, ROCKETSHIP};

enum PowerupType{HEALTH, PSHIELD, AMMO};

enum DeathType{ISPLAYER, NOTPLAYER};