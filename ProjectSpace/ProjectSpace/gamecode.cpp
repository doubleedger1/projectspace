// GameCode.cpp		


// Version  13.03	Draw Engine no longer requires a height / width
// Version 13.0   Update to wchar_t and wstring

//Version 11			5/3/08	
// These three functions form the basis of a game loop in the window created in the
// wincode.cpp file

#include "gamecode.h"
#include "mydrawengine.h"
#include "mysoundengine.h"
#include "myinputs.h"
#include <time.h>
#include <iostream>
#include <fstream>
#include "gametimer.h"
#include "errorlogger.h"
#include <math.h>
#include "shapes.h"
#include "ObjectManager.h"
#include "entities.h"

using namespace std;

ErrorType Game::Main()
// Called repeatedly - the game loop
{
	//Flip and clear the back buffer
	MyDrawEngine* pTheDrawEngine= MyDrawEngine::GetInstance();
	MyDrawEngine::GetInstance()->AddFont(L"Impact", 24, false, false);
	pTheDrawEngine->Flip();
	pTheDrawEngine->ClearBackBuffer();

	ErrorType err;

	switch(m_currentState)
	{
	case MENU:

		err= MainMenu();

		break;
	case PAUSED:

		err = PauseMenu();
		break;
	case RUNNING:


		err= Update();
		break;

	case GAMEOVER:

		err = FAILURE;
		break;
	case DIFFICULTY:

		err = DifficultyMenu();
		break;
	case HIGHSCORES:
		
		err = HighScores();
		break;
	case HOWTOPLAY:

		err = HowToPlay();
		break;

	default:
		// Not a valid state
		err = FAILURE;

	}

	return err;
}

void Game::ChangeState(GameState newState)
{
	// Very crude state system
	// Close old state
	switch(m_currentState)
	{
	case MENU:

		break;
	case PAUSED:

		break;
	case RUNNING:

		break;
	case GAMEOVER:

		break;
	case DIFFICULTY:
		
		break;
	case HIGHSCORES:
		break;
	case HOWTOPLAY:
		break;
	}

	// Change the state
	m_currentState = newState;
	m_menuOption = 0;

	// Transition to new state
	switch(m_currentState)
	{
	case MENU:

		break;
	case PAUSED:

		break;
	case RUNNING:

		break;
	case GAMEOVER:

		break;
	case DIFFICULTY:

		break;
	case HIGHSCORES:
		break;
	case HOWTOPLAY:
		break;
	}
}

ErrorType Game::Setup(bool bFullScreen, HWND hwnd, HINSTANCE hinstance)
// Called once before entering game loop. 
{
	// Create the engines - this should be done before creating other DDraw objects
	if(FAILED(MyDrawEngine::Start(hwnd, bFullScreen)))
	{
		ErrorLogger::Writeln(L"Failed to start MyDrawEngine");
		return FAILURE;
	}
	if(FAILED(MySoundEngine::Start(hwnd)))
	{
		ErrorLogger::Writeln(L"Failed to start MySoundEngine");
		return FAILURE;
	}
	if(FAILED(MyInputs::Start(hinstance, hwnd)))
	{
		ErrorLogger::Writeln(L"Failed to start MyInputs");
		return FAILURE;
	}
	return (SUCCESS);
}

void Game::Shutdown()
// Called once before entering game loop. 
{
	m_objects.DeleteAllObjects();

	// (engines must be terminated last)
	MyDrawEngine::Terminate();
	MySoundEngine::Terminate();
	MyInputs::Terminate();
}
// **********************************************************************************************
// The game !!! *********************************************************************************
// **********************************************************************************************



ErrorType Game::DifficultyMenu()
{
	MyDrawEngine::GetInstance()->DrawAt(Vector2D(0, 0), MyDrawEngine::GetInstance()->LoadPicture(L"bg.jpg"), 1.75f);
	MyDrawEngine::GetInstance()->DrawAt(Vector2D(-400, 600), MyDrawEngine::GetInstance()->LoadPicture(L"logo.png"), 2.0f);

	const int DIFFICULTYOPTIONS = 3;
	wchar_t numoptions[DIFFICULTYOPTIONS][10] = { L"Easy", L"Medium", L"Hard" };

	for (int i = 0; i < DIFFICULTYOPTIONS; i++)
	{
		int colour = MyDrawEngine::GREY;
		if (i == m_menuOption)
		{
			colour = MyDrawEngine::WHITE;
		}
		MyDrawEngine::GetInstance()->WriteText(450, 300 + 50 * i, numoptions[i], colour, 1);
	}
	MyInputs* pInputs = MyInputs::GetInstance();

	pInputs->SampleKeyboard();
	if (pInputs->NewKeyPressed(DIK_UP))
	{
		m_menuOption--;
		MySoundEngine::GetInstance()->Play(MySoundEngine::GetInstance()->LoadWav(L"menuclick.wav"));
	}
	if (pInputs->NewKeyPressed(DIK_DOWN))
	{
		m_menuOption++;
		MySoundEngine::GetInstance()->Play(MySoundEngine::GetInstance()->LoadWav(L"menuclick.wav"));
	}
	if (m_menuOption<0)
	{
		m_menuOption = 0;
	}
	else if (m_menuOption >= DIFFICULTYOPTIONS)
	{
		m_menuOption = DIFFICULTYOPTIONS - 1;
	}

	if (pInputs->NewKeyPressed(DIK_RETURN))
	{
		MySoundEngine::GetInstance()->Play(MySoundEngine::GetInstance()->LoadWav(L"menuclick.wav"));
		if (m_menuOption == 0)
		{
			m_difficulty = 1;
			StartOfGame();
			ChangeState(RUNNING);
		}

		if (m_menuOption == 1)
		{
			m_difficulty = 2;
			StartOfGame();
			ChangeState(RUNNING);
		}
		if (m_menuOption == 2)
		{
			m_difficulty = 3;
			StartOfGame();
			ChangeState(RUNNING);
		}
	}

	return SUCCESS;

}


ErrorType Game::PauseMenu()
{
	// Code for a basic pause menu

	MyDrawEngine::GetInstance()->AddFont(L"Comic Sans MS", 64, false, false);

	const int NUMOPTIONS = 2;
	wchar_t options[NUMOPTIONS][15] = {L"Resume", L"Main menu"};

	for(int i=0;i<NUMOPTIONS;i++)
	{
		int colour = MyDrawEngine::GREY;
		if(i == m_menuOption)
		{
			colour = MyDrawEngine::WHITE;
		}
		MyDrawEngine::GetInstance()->WriteText(450,300+50*i, options[i], colour);
	}

	MyInputs* pInputs = MyInputs::GetInstance();

	pInputs->SampleKeyboard();
	if(pInputs->NewKeyPressed(DIK_UP))
	{
		m_menuOption--;
	}
	if(pInputs->NewKeyPressed(DIK_DOWN))
	{
		m_menuOption++;
	}
	if(m_menuOption<0)
	{
		m_menuOption=0;
	}
	else if(m_menuOption>=NUMOPTIONS)
	{
		m_menuOption=NUMOPTIONS-1;
	}

	if(pInputs->NewKeyPressed(DIK_RETURN))
	{
		if(m_menuOption ==0)
		{
			ChangeState(RUNNING);
		}
		if(m_menuOption ==1)
		{
			EndOfGame();
			ChangeState(MENU);
		}

	}

	return SUCCESS;
}

ErrorType Game::MainMenu()
{
	// Code for a basic main menu
	static float e_angle = 0;
	e_angle -= 0.005f;
	MySoundEngine* pSoundEngine = MySoundEngine::GetInstance();
	static SoundIndex menuMusic;
	if (!m_hasStarted)
	{
		menuMusic = pSoundEngine->LoadWav(L"menumusic.wav");
		pSoundEngine->Play(menuMusic, true);
		m_hasStarted = true;
	}


	MyDrawEngine::GetInstance()->DrawAt(Vector2D(0, 0), MyDrawEngine::GetInstance()->LoadPicture(L"bg.jpg"), 1.75f);
	MyDrawEngine::GetInstance()->DrawAt(Vector2D(-400, 600), MyDrawEngine::GetInstance()->LoadPicture(L"logo.png"), 2.0f);
	MyDrawEngine::GetInstance()->DrawAt(Vector2D(0, 0), MyDrawEngine::GetInstance()->LoadPicture(L"earth.png"), 1.0f, e_angle);
	MyDrawEngine::GetInstance()->DrawAt(Vector2D(0, 0), MyDrawEngine::GetInstance()->LoadPicture(L"sun.png"), 1.0f);

	MyDrawEngine::GetInstance()->theCamera.PlaceAt(Vector2D(0, 0)); // Makes sure the camera resets.
	const int NUMOPTIONS =4;
	wchar_t options[NUMOPTIONS][15] = {L"Play game", L"Go Fullscreen", L"How To Play", L"Exit"};


	if(MyDrawEngine::GetInstance()->IsWindowFullScreen())
	{
		wcscpy_s( options[1], 15, L"Go Windowed");
	}

	for(int i=0;i<NUMOPTIONS;i++)
	{
		int colour = MyDrawEngine::GREY;
		if(i == m_menuOption)
		{
			colour = MyDrawEngine::WHITE;
		}
		MyDrawEngine::GetInstance()->WriteText(450,300+50*i, options[i], colour, 1);
	}
	MyInputs* pInputs = MyInputs::GetInstance();

	pInputs->SampleKeyboard();
	if(pInputs->NewKeyPressed(DIK_UP))
	{
		m_menuOption--;
		MySoundEngine::GetInstance()->Play(MySoundEngine::GetInstance()->LoadWav(L"menuclick.wav"));
	}
	if(pInputs->NewKeyPressed(DIK_DOWN))
	{
		m_menuOption++;
		MySoundEngine::GetInstance()->Play(MySoundEngine::GetInstance()->LoadWav(L"menuclick.wav"));
	}
	if(m_menuOption<0)
	{
		m_menuOption=0;
	}
	else if(m_menuOption>=NUMOPTIONS)
	{
		m_menuOption=NUMOPTIONS-1;
	}

	if(pInputs->NewKeyPressed(DIK_RETURN))
	{
		pSoundEngine->Play(MySoundEngine::GetInstance()->LoadWav(L"menuclick.wav"));
		if(m_menuOption ==0)
		{
			pSoundEngine->Stop(menuMusic);
			m_hasStarted = false;
			DifficultyMenu();
			ChangeState(DIFFICULTY);
		}

		if(m_menuOption ==1)
		{
			if(MyDrawEngine::GetInstance()->IsWindowFullScreen())
			{
				MyDrawEngine::GetInstance()->GoWindowed();
			}
			else
			{
				MyDrawEngine::GetInstance()->GoFullScreen();
			}
		}

		if (m_menuOption == 2)
		{
			ChangeState(HOWTOPLAY);
		}

		if(m_menuOption == 3)
		{
			ChangeState(GAMEOVER);
		}
	}

	return SUCCESS;
}

// Tells the user how to play in case
// they don't read the document itself.
ErrorType Game::HowToPlay()
{
	MyInputs* pInputs = MyInputs::GetInstance();
	pInputs->SampleKeyboard();

	MyDrawEngine::GetInstance()->DrawAt(Vector2D(0, 0), MyDrawEngine::GetInstance()->LoadPicture(L"bg.jpg"), 1.75f);
	MyDrawEngine::GetInstance()->theCamera.PlaceAt(Vector2D(0, 0)); // Makes sure the camera resets.

	// Make sure we don't load it every frame.
	if (!m_loadedHelp)
	{
		wifstream loadHowText("howtoplay.txt");
		std::wstring line;
		float yPos = 600.0f;
		while (std::getline(loadHowText, line))
		{
			MyDrawEngine::GetInstance()->WriteText(Vector2D(-300, yPos), (wchar_t*)line.c_str(), MyDrawEngine::WHITE);
			yPos -= 35.0f;
		}
		loadHowText.close();
	}
	
	if (pInputs->NewKeyPressed(DIK_RETURN))
	{
		ChangeState(MENU);
		m_loadedHelp = false;
	}

	return SUCCESS;
}

// Draws the high scores at 
// the end of the game.
ErrorType Game::HighScores()
{
	MyDrawEngine::GetInstance()->DrawAt(MyDrawEngine::GetInstance()->GetViewport().GetCentre(), MyDrawEngine::GetInstance()->LoadPicture(L"bg.jpg"), 1.75f);

	wchar_t* enterTxt = L"Press Enter to return to menu.";

	float len = strlen((char*)enterTxt);

	MyDrawEngine::GetInstance()->DrawAt(MyDrawEngine::GetInstance()->GetViewport().GetCentre() - Vector2D(50, -40.0f)
		, MyDrawEngine::GetInstance()->LoadPicture(L"highscoreborder.png"));

	MyDrawEngine::GetInstance()->theCamera.Reset();
	MyDrawEngine::GetInstance()->WriteText(MyDrawEngine::GetInstance()->GetViewport().GetCentre() - Vector2D(175, -500.0f),
		L"You Died!", MyDrawEngine::RED, 1);

	MyDrawEngine::GetInstance()->WriteText(MyDrawEngine::GetInstance()->GetViewport().GetCentre() - Vector2D(250, -350.0f),
		L"Your Score: ", MyDrawEngine::RED, 1);

	MyDrawEngine::GetInstance()->WriteInt(MyDrawEngine::GetInstance()->GetViewport().GetCentre() - Vector2D(-150,  -350.0f),
		m_finalScore, MyDrawEngine::CYAN, 1);

	MyDrawEngine::GetInstance()->WriteText(MyDrawEngine::GetInstance()->GetViewport().GetCentre() - Vector2D(250, -150.0f),
		L"Top 5 scores: ", MyDrawEngine::WHITE, 1);

	for (int i = 0; i < 5; i++)
	{
		MyDrawEngine::GetInstance()->WriteInt(MyDrawEngine::GetInstance()->GetViewport().GetCentre() - Vector2D(100, 65.0f * i),
			m_highScores[i], MyDrawEngine::GREEN, 1);
	}

	MyDrawEngine::GetInstance()->WriteText(MyDrawEngine::GetInstance()->GetViewport().GetCentre() - Vector2D(500, 350.0f),
		enterTxt, MyDrawEngine::WHITE, 1);

	MyInputs* pInputs = MyInputs::GetInstance();
	pInputs->SampleKeyboard();

	if (pInputs->NewKeyPressed(DIK_RETURN))
	{
		EndOfGame();
		ChangeState(MENU);
	}
	return SUCCESS;
}

ErrorType Game::StartOfGame()
{
	// Code to set up your game

	// Make sure we don't load the scores
	// every frame regardless of difference
	// in terms of efficiency.
	if (!m_loadedScores)	
	{
		ifstream loadScores("scores.txt");
		if (loadScores.is_open())
		{
			for (int i = 0; i < 5; i++)
			{
				loadScores >> m_highScores[i];
			}
			loadScores.close();
		}
		else
		{
			for (int i = 0; i < 5; i++)
			{
				m_highScores[i] = 0;
			}
		}
		m_loadedScores = true;
	}

	// Stop the music of the game.
	MySoundEngine::GetInstance()->Stop(MySoundEngine::GetInstance()->LoadWav(L"menumusic.wav"));
	
	srand(unsigned(time(NULL)));

	// Set up the manager for the buildings.
	GameManager* pGameManager = new GameManager;
	pGameManager->Initialise();

	// Add all the objects to the games with or without collisions.
	m_objects.AddItem(pGameManager, false);

	return SUCCESS;
}

ErrorType Game::Update()
{
	m_objects.UpdateAll();
	m_objects.ProcessCollisions();
	m_objects.DrawAll();

  MyDrawEngine* pDrawEngine = MyDrawEngine::GetInstance();

	// Garbage collect any objects. Possibly slow
	m_objects.DeleteInactiveItems();

	// Check for entry to pause menu
	static bool escapepressed = true;
	if(KEYPRESSED(VK_ESCAPE))
	{
		if(!escapepressed)
			ChangeState(PAUSED);
		escapepressed=true;
	}
	else
		escapepressed=false;

	// Any code here to run the game,
	// but try to do this within a game object if possible

	return SUCCESS;
}

ErrorType Game::EndOfGame()
// called when the game ends by returning to main menu
{
	// Add code here to tidy up ********************************
	
	// Save the scores before we close
	// the whole program for loading later.
	ofstream saveScores("scores.txt");
	
	if (saveScores.is_open())
	{
		for (int i = 0; i < 5; i++)
		{
			saveScores << m_highScores[i] << "\n";
		}
		saveScores.close();
	}
	
	m_objects.DeleteAllObjects();

	return SUCCESS;
}

Game::Game()
{
	// No-op
}

Game::~Game()
{
	// No-op
}


void Game::NotifyEvent(Event evt)
{
	m_objects.HandleEvent(evt);
}

// Sets the final score of the game.
void Game::SetScore(int score)
{
	m_finalScore = score;
}

// Returns the difficulty.
int Game::GetDifficulty()
{
	return m_difficulty;
}

// When the game is over
// do the high score checks.
void Game::SetGameOver()
{
	Insert();
}

// Checks for new high scores.
// If the last score is greater
// than any of the values in the
// table then move down or remove
// the old values and insert the new one.
void Game::Insert()
{

	// Loops through the high scores 1 by 1.
	// Break when it finds a value less than
	// the new score we have ended with.
	int pos;
	for (pos = 0; pos < 5; pos++)
	{
		if (m_highScores[pos] < m_finalScore)
			break;
	}

	// It has to be less than 5 to make sure
	// we don't go off the end of the array.
	if (pos < 5)
	{
		for (int i = 4; i < pos; i--)
		{
			m_highScores[i + 1] = m_highScores[i];
		}
		m_highScores[pos] = m_finalScore;
	}

	ChangeState(HIGHSCORES);


}

Game Game::instance;