//  *********************************************************************************
#pragma once
#include "errortype.h"
#include "windows.h"
#include "ObjectManager.h"
#include "event.h"

// For reading keyboard
#define KEYPRESSED(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)



//typedef unsigned int SoundIndex;
class Game
{
private:
	enum GameState{ MENU, PAUSED, RUNNING, DIFFICULTY, GAMEOVER, HIGHSCORES, HOWTOPLAY };
	GameState m_currentState;
	int m_difficulty = 1; // Default difficulty is 1.
	void ChangeState(GameState newState);
	int m_menuOption;
	int m_finalScore;
	bool m_hasStarted = false;
	bool m_loadedScores = false;
	bool m_loadedHelp = false;
	int m_highScores[5];
	Game();
	~Game();
	Game(Game& other);
public:
	static Game instance;
	ObjectManager m_objects;
	ErrorType Setup(bool bFullScreen, HWND hwnd, HINSTANCE hinstance);
	void Shutdown();
	ErrorType Main();

	ErrorType PauseMenu();
	ErrorType MainMenu();
	ErrorType DifficultyMenu();
	ErrorType HighScores();
	ErrorType HowToPlay();
	ErrorType StartOfGame();
	ErrorType Update();
	ErrorType EndOfGame();
	void Insert();
	int GetDifficulty();
	void SetGameOver();
	void SetScore(int score);
	void NotifyEvent(Event evt);
};




