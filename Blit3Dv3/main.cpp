/*
	This game implements an all-in-one 2D game environment
	@author David Lopez Barcenas
	@audio WWise 
	@physics Box2D //not used in this game, but you can use them
	@Graphics Blit3D 
	@Script Lua
	*/

#include <lua.hpp>		//lua
#include "Blit3D.h"
#include <random>
#include "Physics.h"
#include "Particle.h"	//particles, yay!
#include "Camera.h"		//Screenshake, more yay!
#include <stdlib.h>
#include <crtdbg.h>
#include "AudioEngine.h"
#include "DieRoller.h"
#include "Dice.h"
#include "Ghost.h"
#include "Card.h"

Blit3D *blit3D = NULL;

//the Lua interpreter
lua_State* L;

DieRoller dieRoller;

//memory leak detection
//Can't do the following if using Wwise in debug mode
/*
#define CRTDBG_MAP_ALLOC
#ifdef _DEBUG
	#ifndef DBG_NEW
		#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
		#define new DBG_NEW
	#endif
#endif  // _DEBUG
*/

//GLOBAL DATA
std::mt19937 rng;
std::uniform_real_distribution<float> plusMinus5Degrees(-5, +5);
std::uniform_real_distribution<float> plusMinus70Degrees(-70, +70);
std::uniform_real_distribution<float> plusMinus90Degrees(-90, +90);
std::uniform_real_distribution<float> random360Degrees(0, 360);
std::uniform_real_distribution<float> powerUpPercentage(0, 100);


b2Vec2 gravity; //defines our gravity vector
b2World* world; //our physics engine

// Prepare for simulation. Typically we use a time step of 1/60 of a
// second (60Hz) and ~10 iterations. This provides a high quality simulation
// in most game scenarios.
int32 velocityIterations = 8;
int32 positionIterations = 3;
float timeStep = 1.f / 60.f; //one 60th of a second
float elapsedTime = 0; //used for calculating time passed

////contact listener to handle collisions between important objects
//MyContactListener* contactListener;
//
float cursorX = 0;
//PaddleEntity* paddleEntity = NULL;

enum GameState { START, PLAYING, GHOSTTURN, GAMEOVER };
GameState gameState = START;
bool attachedBall = true; //is the ball ready to be launched from the paddle?



int playerHP = 10;
int playerMagicPoints = 0;
bool canRoll = true;
bool damageDone = false;
bool hitByGhost = false;

int rollsLeft = 3;

Ghost* ghost = NULL;
Card* card = NULL;

int ghostToFight = 0;
int cardToUse = 0;

int ghostHP = 10;
int ghostEnergy = 5;

Dice* dados[3];

//std::vector<Entity*> brickEntityList; //bricks go here
//std::vector<Entity*> ballEntityList; //track the balls seperately from everything else
//std::vector<Entity*> entityList; //other entities in our game go here
//std::vector<Entity*> deadEntityList; //dead entities

float currentBallSpeed = 60; //defaultball speed
int level = 1; //current level of play

//Sprites 
Sprite* logo = NULL;
Sprite* logoBG = NULL;
Sprite* logoGameOver = NULL;
Sprite* logoYouWin = NULL;
Sprite* logoYouLoseHP = NULL;
Sprite* logoYouLoseEnergy = NULL;

Sprite* labelGhostTurn = NULL;
Sprite* labelYourTurn = NULL;
Sprite* labelUseCard = NULL;
Sprite* labelRollAgain = NULL;
Sprite* labelUseDice = NULL;
Sprite* labelSpaceBar = NULL;

std::vector<Sprite*> cardsSprites;
std::vector<Sprite*> ghostSprites;
std::vector<Sprite*> diceSprites;
//Sprite* diceSprites[8];

int amountGhost = 0;
int amountCards = 0;

Sprite* ballSprite = NULL;
Sprite* paddleSprite = NULL;


Sprite* blueBrickSprite = NULL;
Sprite* greenBrickSprite = NULL;
Sprite* greyBrickSprite = NULL;
Sprite* purpleBrickSprite = NULL;
Sprite* redBrickSprite = NULL;
Sprite* yellowBrickSprite = NULL;


Sprite* multiBallSprites[6];
Sprite* attachedBallSprites[6];

//font
AngelcodeFont* debugFont = NULL;
AngelcodeFont* infoFont = NULL;

//particle stuff
std::vector<Particle*> particleList;

Sprite* cloudSprite1 = NULL;
Sprite* cloudSprite2 = NULL;
Sprite* cloudSprite3 = NULL;

Sprite* explosionSprite1 = NULL;
Sprite* explosionSprite2 = NULL;
Sprite* explosionSprite3 = NULL;
Sprite* explosionSprite4 = NULL;
Sprite* explosionSprite5 = NULL;
Sprite* explosionSprite6 = NULL;
Sprite* explosionSprite7 = NULL;

Sprite* boinkSprite1 = NULL;
Sprite* boinkSprite2 = NULL;
Sprite* boinkSprite3 = NULL;

Camera2D* camera = NULL; //for screenshake

AudioEngine * audioE = NULL;
AkGameObjectID mainGameID = 1;
AkPlayingID BrickHitID, FloorHitID, PaddleHitID, PowerUpID, WallHitID,GameOverID, musicID;


//error handling function for Lua calls
//ErrorCheck() will print out some error info
//DO NOT CLEAR THE ERROR STRING OFF THE STACK
bool ErrorCheck(int errorNum)
{
	if (errorNum)
	{
		//There was an error!
		std::cout << "ERROR!\n";
		std::cout << "There was an error calling Lua.\n";
		switch (errorNum)
		{
		case LUA_ERRRUN:
			std::cout << "There was a runtime error.\n";
			break;

		case LUA_ERRMEM:
			std::cout << "There was a memory allocation error (this should be unusual!).\n";
			break;

		case LUA_ERRERR:
			std::cout << "There was a error-handler error (this should be unusual!).\n";
			break;

		case LUA_ERRSYNTAX:
			std::cout << "There was a syntax error.\n";
			break;

		default:
			std::cout << "There was an unknown error.\n";
		}

		std::cout << "Error string from Lua:\n";

		//get error string off the stack
		std::string errorString = lua_tostring(L, -1);
		std::cout << errorString << "\n";

		return false;
	}

	return true;
}


/// <summary>
/// Luas the test.
/// </summary>
/// <returns>Text informing Lua status</returns>
std::string luaTest()
{
	std::string someText;

	//the Lua function name we want to call
	lua_getglobal(L, "TestLoad");

	//call the function with 0 argument, return 1 result
	lua_call(L, 0, 1);

	//get the result
	someText.append(lua_tostring(L, -1));

	//remove the result from the lua stack
	lua_pop(L, 1);

	return someText;
}


/// <summary>
/// Rolls the dice for Lua.
/// </summary>
/// <param name="Lstate">The Lua state.</param>
/// <returns></returns>
static int rollDN(lua_State* Lstate)
{
	//get number of arguments
	int n = lua_gettop(Lstate);
	int rand = lua_tonumber(Lstate, 1);

	//push the raand
	lua_pushnumber(Lstate, dieRoller.RollDN(rand));

	//return the number of results
	return 1;
}


/// <summary>
/// Luas the size of the card or ghost list.
/// </summary>
/// <param name="cardOrGhost">The card or ghost. 1 for cards and 2 for ghost</param>
/// <returns>The size of the list on the Lua script</returns>
int luaCardOrGhostSize(int cardOrGhost)
{
	int value;

	//the function name
	lua_getglobal(L, "getArraySize");

	//the first argument
	lua_pushnumber(L, cardOrGhost);

	//call the function with 2 arguments, return 1 result
	lua_call(L, 1, 1);

	//get the result
	value = (int)lua_tointeger(L, -1);

	//remove the result from the lua stack
	lua_pop(L, 1);

	return value;
}

/// <summary>
/// Get the Filename of the Sprite associated to the Ghost or Card.
/// </summary>
/// <param name="elementNumber">The element number.</param>
/// <param name="cardOrGhost">The card or ghost. 1 for card, 2 for ghost</param>
/// <returns></returns>
std::string luaCOGFileName(int elementNumber, int cardOrGhost)
{
	std::string fileName;

	//the Lua function name we want to call
	lua_getglobal(L, "getFileName");

	//the first argument
	lua_pushnumber(L, elementNumber);

	//the first argument
	lua_pushnumber(L, cardOrGhost);

	//call the function with 2 argument, return 1 result
	lua_call(L, 2, 1);

	//get the result
	fileName.append(lua_tostring(L, -1));

	//remove the result from the lua stack
	lua_pop(L, 1);

	return fileName;
}


/// <summary>
/// Get the name of the Ghost or Card.
/// </summary>
/// <param name="elementNumber">The element number.</param>
/// <param name="cardOrGhost">The card or ghost. 1 for card, 2 for ghost</param>
/// <returns></returns>
std::string luaCOGName(int elementNumber, int cardOrGhost)
{
	std::string name;

	//the Lua function name we want to call
	lua_getglobal(L, "getName");

	//the first argument
	lua_pushnumber(L, elementNumber);

	//the first argument
	lua_pushnumber(L, cardOrGhost);

	//call the function with 2 argument, return 1 result
	lua_call(L, 2, 1);

	//get the result
	name.append(lua_tostring(L, -1));

	//remove the result from the lua stack
	lua_pop(L, 1);

	return name;
}

/// <summary>
/// Get description using the lua data.
/// </summary>
/// <param name="elementNumber">The element number.</param>
/// <param name="cardOrGhost">The card or ghost. 1 for card, 2 for ghost</param>
/// <returns></returns>
std::string luaCOGDescription(int elementNumber, int cardOrGhost)
{
	std::string description;

	//the Lua function name we want to call
	lua_getglobal(L, "getDescription");

	//the first argument
	lua_pushnumber(L, elementNumber);

	//the first argument
	lua_pushnumber(L, cardOrGhost);

	//call the function with 2 argument, return 1 result
	lua_call(L, 2, 1);

	//get the result
	description.append(lua_tostring(L, -1));

	//remove the result from the lua stack
	lua_pop(L, 1);

	return description;
}

/// <summary>
/// Luas to get random ghost.
/// </summary>
/// <returns> A random ghost ID</returns>
int luaGetRandomGhost()
{
	int value;

	//the function name
	lua_getglobal(L, "randomGhost");


	//call the function with 2 arguments, return 1 result
	lua_call(L, 0, 1);

	//get the result
	value = (int)lua_tointeger(L, -1);

	//remove the result from the lua stack
	lua_pop(L, 1);

	return value;
}

/// <summary>
/// Luas to get random card.
/// </summary>
/// <returns>A random card</returns>
int luaGetRandomCard()//send 1 for cards and 2 for ghost
{
	int value;

	//the function name
	lua_getglobal(L, "randomCard");


	//call the function with 2 arguments, return 1 result
	lua_call(L, 0, 1);

	//get the result
	value = (int)lua_tointeger(L, -1);

	//remove the result from the lua stack
	lua_pop(L, 1);

	return value;
}


/// <summary>
/// Luas the get card atribbute.
/// </summary>
/// <param name="attribute">The attribute. 1 for hpPlayer, 2 for energyPlayer, 3 for damageHP, 4 for damageEnergy, 5 for cost; </param>
/// <param name="cardID">The card identifier.</param>
/// <returns>The value from  the desired atribbute</returns>
int luaGetCardAtribbute(int attribute, int cardID)
{
	int value;

	//the function name
	lua_getglobal(L, "getCardAttribute");

	//the first argument
	lua_pushnumber(L, attribute);

	//the first argument
	lua_pushnumber(L, cardID);

	//call the function with 2 arguments, return 1 result
	lua_call(L, 2, 1);

	//get the result
	value = (int)lua_tointeger(L, -1);

	//remove the result from the lua stack
	lua_pop(L, 1);

	return value;
}

/// <summary>
/// Get the ghost atribbute.
/// </summary>
/// <param name="attribute">The attribute. 1 for ghostHP, 2 for energyGhost</param>
/// <param name="ghostID">The ghost identifier. </param>
/// <returns>The value from  the desired atribbute</returns>
int luaGetGhostAtribbute(int attribute, int ghostID)
{
	int value;

	//the function name
	lua_getglobal(L, "getGhostAttribute");

	//the first argument
	lua_pushnumber(L, attribute);

	//the first argument
	lua_pushnumber(L, ghostID);

	//call the function with 2 arguments, return 1 result
	lua_call(L, 2, 1);

	//get the result
	value = (int)lua_tointeger(L, -1);

	//remove the result from the lua stack
	lua_pop(L, 1);

	return value;
}


/// <summary>
/// Loads a ghost info.
/// </summary>
void LoadGhost()
{
	ghostToFight = luaGetRandomGhost();
	ghost = new Ghost(ghostToFight,ghostSprites[ghostToFight-1]);
	ghost->name = luaCOGName(ghostToFight, 2);
	ghost->description = luaCOGDescription(ghostToFight, 2);
	ghost->hp = luaGetGhostAtribbute(1, ghostToFight);
	ghost->energy = luaGetGhostAtribbute(2, ghostToFight);
	ghostHP = ghost->hp;
	ghostEnergy = ghost->energy;
}


/// <summary>
/// Loads a card info.
/// </summary>
void LoadCard()
{
	cardToUse = luaGetRandomCard();
	card = new Card(cardToUse, cardsSprites[cardToUse-1]);
	card->name = luaCOGName(cardToUse, 1);
	card->description = luaCOGDescription(cardToUse, 1);
	card->hpPlayer = luaGetCardAtribbute(1,cardToUse);
	card->energyPlayer = luaGetCardAtribbute(2, cardToUse);
	card->damageHP = luaGetCardAtribbute(3, cardToUse);
	card->damageEnergy = luaGetCardAtribbute(4, cardToUse);
	card->cost = luaGetCardAtribbute(5, cardToUse);
}


/// <summary>
/// Applies the card effect.
/// </summary>
void ApplyCardEffect()
{

	playerHP = playerHP + card->hpPlayer;
	playerMagicPoints = playerMagicPoints + card->energyPlayer;

	ghostHP = ghostHP - card->damageHP;
	ghostEnergy = ghostEnergy - card->damageEnergy;

	playerMagicPoints -= card->cost;
	
	LoadCard();
}

/// <summary>
/// Rolls the dice.
/// </summary>
void rollDice()
{
	rollsLeft--;
	for (auto d : dados)
		d->value = dieRoller.RollDN(6);
	if (rollsLeft < 1)canRoll = false;
}


/// <summary>
/// Uses the dices.
/// </summary>
void useDices()
{
	for (auto d : dados) {
		switch (d->value) {
			case 1:
				playerHP++;
				break;
			case 2:
			case 3:
			case 4:
				playerMagicPoints++;
				break;
			case 5:
			case 6:
				ghostHP--;
				break;
			default:
				break;
		}
	}
	rollsLeft = 3;
	canRoll = true;
	damageDone = true;
	for (auto d : dados) d->value = 0;
	gameState = GHOSTTURN;
}


/// <summary>
/// The ghost uses the dice.
/// </summary>
void useDicesGhost()
{
	for (auto d : dados)
		d->value = dieRoller.RollDN(6);
}

/// <summary>
/// Applies the damage the ghost made to the player.
/// </summary>
void applyGhostDamage() {
	for (auto d : dados) {
		if (d->value == 2 || d->value == 3 || d->value == 4) ghostEnergy++;
		if (d->value == 5 || d->value == 6) playerHP--;
	}
	damageDone = true;
	gameState = PLAYING;
}


void Init()
{

	//seed random generator
	std::random_device rd;
	rng.seed(rd());

	//turn off the cursor
	//blit3D->ShowCursor(false);

	//setup camera
	camera = new Camera2D();

	//set it's valid pan area
	camera->minX = -blit3D->screenWidth / 2;
	camera->minY = -blit3D->screenHeight / 2;
	camera->maxX = blit3D->screenWidth / 2;
	camera->maxY = blit3D->screenHeight / 2;

	camera->PanTo(blit3D->screenWidth / 2, blit3D->screenHeight / 2);

	debugFont = blit3D->MakeAngelcodeFontFromBinary32("Media\\DebugFont_24.bin");
	infoFont = blit3D->MakeAngelcodeFontFromBinary32("Media\\Pamplona46.bin");

	//load the sprites
	blueBrickSprite = blit3D->MakeSprite(0, 0, 64, 32, "Media\\element_blue_rectangle_glossy.png");
	
	amountCards = luaCardOrGhostSize(1);
	amountGhost = luaCardOrGhostSize(2);
	//std::cout << "ghost on list: " << amountGhost << " cards on list: " << amountCards << std::endl;	//debugg

	Sprite* ghostSprite;
	for (int i = 1; i <= amountGhost; i++) {
		std::string filePath = "Media\\"+luaCOGFileName(i,2);
		ghostSprite = blit3D->MakeSprite(0, 0, 400, 400, filePath);
		ghostSprites.push_back(ghostSprite);
	}

	Sprite* cardSprite;
	for (int i = 1; i <= amountCards; i++) {
		std::string filePath = "Media\\" + luaCOGFileName(i, 1);
		cardSprite = blit3D->MakeSprite(0, 0, 400, 400, filePath);
		cardsSprites.push_back(cardSprite);
	}


	diceSprites.push_back(blit3D->MakeSprite(128,64,64,64,"Media\\dice.png"));
	diceSprites.push_back(blit3D->MakeSprite(0, 0, 64, 64, "Media\\dice.png"));
	diceSprites.push_back(blit3D->MakeSprite(64, 0, 64, 64, "Media\\dice.png"));
	diceSprites.push_back(blit3D->MakeSprite(128, 0, 64, 64, "Media\\dice.png"));
	diceSprites.push_back(blit3D->MakeSprite(192, 0, 64, 64, "Media\\dice.png"));
	diceSprites.push_back(blit3D->MakeSprite(0, 64, 64, 64, "Media\\dice.png"));
	diceSprites.push_back(blit3D->MakeSprite(64, 64, 64, 64, "Media\\dice.png"));
	diceSprites.push_back(blit3D->MakeSprite(192, 64, 64, 64, "Media\\dice.png"));


	for (int i = 0; i < 3; i++)
	{
		dados[i] = new Dice(i, diceSprites);
		dados[i]->posY = 400;
		dados[i]->posX = 840 + (i*120); 
	}
		

	//power up animated sprite lists
	for (int i = 0; i < 6; ++i)
		multiBallSprites[i] = blit3D->MakeSprite(38, 193 + i * 32, 21, 10, "Media\\arinoid_master.png");
	
	logo = blit3D->MakeSprite(0, 0, 1920, 1080, "Media\\START.png");

	logoBG = blit3D->MakeSprite(0, 0, 1920, 1080, "Media\\BG.png");

	logoGameOver = blit3D->MakeSprite(0, 0, 1920, 1080, "Media\\GAMEOVER.png");

	logoYouWin = blit3D->MakeSprite(0, 0, 1920, 1080, "Media\\YOU_WIN.png");

	logoYouLoseHP = blit3D->MakeSprite(0, 0, 1920, 1080, "Media\\YOU_LOSEHP.png");
	
	logoYouLoseEnergy = blit3D->MakeSprite(0, 0, 1920, 1080, "Media\\YOU_LOSEENERGY.png");

	labelGhostTurn = blit3D->MakeSprite(0, 0, 400, 60, "Media\\button_ghost-turn.png");

	labelYourTurn = blit3D->MakeSprite(0, 0, 400, 60, "Media\\button_your-turn.png");

	labelUseCard = blit3D->MakeSprite(0, 0, 400, 60, "Media\\button_use-card.png");

	labelRollAgain = blit3D->MakeSprite(0, 0, 400, 60, "Media\\button_roll-again.png");

	labelUseDice = blit3D->MakeSprite(0, 0, 400, 60, "Media\\button_use-dice.png");
	
	labelSpaceBar = blit3D->MakeSprite(0, 0, 400, 60, "Media\\button_space-bar-to-continue.png");
	
	//particle sprites
	cloudSprite1 = blit3D->MakeSprite(355, 419, 26, 26, "Media\\arinoid_master.png");
	cloudSprite2 = blit3D->MakeSprite(386, 419, 28, 29, "Media\\arinoid_master.png");
	cloudSprite3 = blit3D->MakeSprite(421, 423, 23, 21, "Media\\arinoid_master.png");

	explosionSprite1 = blit3D->MakeSprite(3, 165, 25, 24, "Media\\arinoid_master.png");
	explosionSprite2 = blit3D->MakeSprite(37, 163, 23, 24, "Media\\arinoid_master.png");
	explosionSprite3 = blit3D->MakeSprite(69, 166, 21, 20, "Media\\arinoid_master.png");
	explosionSprite4 = blit3D->MakeSprite(102, 168, 19, 17, "Media\\arinoid_master.png");
	explosionSprite5 = blit3D->MakeSprite(136, 170, 15, 14, "Media\\arinoid_master.png");
	explosionSprite6 = blit3D->MakeSprite(170, 172, 11, 11, "Media\\arinoid_master.png");
	explosionSprite7 = blit3D->MakeSprite(204, 174, 7, 7, "Media\\arinoid_master.png");

	boinkSprite1 = blit3D->MakeSprite(420, 388, 27, 26, "Media\\arinoid_master.png");
	boinkSprite2 = blit3D->MakeSprite(388, 388, 27, 26, "Media\\arinoid_master.png");
	boinkSprite3 = blit3D->MakeSprite(356, 388, 27, 26, "Media\\arinoid_master.png");

	//paddle
	cursorX = blit3D->screenWidth / 2;
	//paddleEntity = MakePaddle(blit3D->screenWidth / 2, 30, paddleSprite);//padle is 46 px tall, so top padel is 53px
	//entityList.push_back(paddleEntity);

	//create audio engine
	audioE = new AudioEngine;
	audioE->Init();
	audioE->SetBasePath("Media\\");

	//load banks
	audioE->LoadBank("Init.bnk");
	audioE->LoadBank("MainBank.bnk");

	//register our game objects
	audioE->RegisterGameObject(mainGameID);

	//load a ghost
	LoadGhost();
	//load a card
	LoadCard();
}

void DeInit(void)
{
	if (camera != NULL) delete camera;

	//delete all particles
	for (auto& p : particleList) delete p;
	particleList.clear();

	////delete all the entities
	//for (auto& e : entityList) delete e;
	//entityList.clear();

	//for (auto& b : ballEntityList) delete b;
	//ballEntityList.clear();

	//for (auto& b : brickEntityList) delete b;
	//brickEntityList.clear();

	////delete the contact listener
	//delete contactListener;

	//Free all physics game data we allocated
	delete world;

	//any sprites/fonts still allocated are freed automatcally by the Blit3D object when we destroy it
	if (audioE != NULL) delete audioE;
	//any sprites still allocated are freed automatcally by the Blit3D object when we destroy it
}

void Update(double seconds)
{
	//must always update audio in our game loop
	audioE->ProcessAudio();
	if (gameState == START || gameState == GAMEOVER) return;
	//if (gameState == GHOSTTURN) useDicesGhost();

	//stop it from lagging hard if more than a small amount of time has passed
	if (seconds > 1.0 / 30) elapsedTime += 1.f / 30;
	else elapsedTime += (float)seconds;

	camera->Update(timeStep);

	//shake the screen!
	//camera->Shake(1.5);		//we want to shake camera
	//BrickHitID = audioE->PlayEvent("BrickHit", mainGameID); //if we want to play sound

	if (rollsLeft == 0 || damageDone)
	{

		//add a particle effect
		Particle* p = new Particle();
		p->spriteList.push_back(explosionSprite4);
		p->spriteList.push_back(explosionSprite1);
		p->spriteList.push_back(explosionSprite1);
		p->spriteList.push_back(explosionSprite2);
		p->spriteList.push_back(explosionSprite3);
		p->spriteList.push_back(explosionSprite4);
		p->spriteList.push_back(explosionSprite5);
		p->spriteList.push_back(explosionSprite6);
		p->spriteList.push_back(explosionSprite7);
		p->spriteList.push_back(cloudSprite3);
		p->spriteList.push_back(cloudSprite2);
		p->spriteList.push_back(cloudSprite1);
		p->spriteList.push_back(cloudSprite1);
		p->spriteList.push_back(cloudSprite2);
		p->spriteList.push_back(cloudSprite3);
		p->rotationSpeed = plusMinus90Degrees(rng);
		p->angle = random360Degrees(rng);
		p->direction = b2Vec2(0.f, 0.f);;
		p->startingSpeed = 0;
		p->targetSpeed = 0;
		p->totalTimeToLive = 0.5f;
		//get coords of ball
		p->coords = b2Vec2((blit3D->screenWidth / 2), blit3D->screenHeight - 250.f);
		if (hitByGhost)
		{
			p->coords = b2Vec2((blit3D->screenWidth / 2), 250.f);
			hitByGhost = false;
		}
		particleList.push_back(p);

		if (damageDone) {
			camera->Shake(25.5);		//we want to shake camera
			WallHitID = audioE->PlayEvent("WallHit", mainGameID);
			damageDone = false;
		} 
	}


		
	//update the particle list and remove dead particles
	for (int i = particleList.size() - 1; i >= 0; --i)
	{
		if (particleList[i]->Update(timeStep))
		{
			//time to die!
			delete particleList[i];
			particleList.erase(particleList.begin() + i);
		}
	}

	if (playerHP < 1 || ghostHP < 1 || ghostEnergy >= 20)
		gameState = GAMEOVER;
}

void Draw(void)
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);	//clear colour: r,g,b,a 	
											// wipe the drawing surface clear
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	camera->Draw();

	switch (gameState)
	{
	case START:
	{
		logo->Blit(blit3D->screenWidth / 2, blit3D->screenHeight / 2);
		std::string lifeString =  luaTest();
		float width = infoFont->WidthText(lifeString);
		infoFont->BlitText(((blit3D->screenWidth) - (width )), 50, lifeString);
	}
		break;
	
	case GHOSTTURN:
	case PLAYING:
	{
		logoBG->Blit(blit3D->screenWidth / 2, blit3D->screenHeight / 2);
		
		//Ghost draw
		ghost->Draw();
		std::string gName = ghost->name;
		float width = debugFont->WidthText(gName);
		infoFont->BlitText(((blit3D->screenWidth / 2) + 300), blit3D->screenHeight - 50, gName);

		std::string data;
		data = "HP: " +std::to_string(ghostHP);// +ghost->hp;
		infoFont->BlitText(((blit3D->screenWidth / 2) +300), blit3D->screenHeight - 110, data);

		data = "Ghost energy: " + std::to_string(ghostEnergy);// +ghost->hp;
		infoFont->BlitText(((blit3D->screenWidth / 2) + 300), blit3D->screenHeight - 160, data);

		//Card Draw
		card->Draw();
		std::string cardName = card->name + " Card";
		width = infoFont->WidthText(cardName);
		infoFont->BlitText(((blit3D->screenWidth) - (width)-50), 600, cardName);
		data = card->description;
		width = infoFont->WidthText(data);
		infoFont->BlitText(((blit3D->screenWidth) - (width)-50), 550, data);
		data = "Magic cost: "+ std::to_string(card->cost);
		width = infoFont->WidthText(data);
		infoFont->BlitText(((blit3D->screenWidth) - (width)-50), 500, data);
		
		
		//Labels Draw
		if (gameState == GHOSTTURN)
		{
			labelSpaceBar->Blit(210, 300);
			labelGhostTurn->Blit(210, 400);
		}
		else
		{
			labelYourTurn->Blit(210,400);
			labelUseCard->Blit(210,100);
			labelRollAgain->Blit(210, 300);
			labelUseDice->Blit(210, 200);
			labelYourTurn->Blit(210, 400);
		}
		
		
		//Dices draw
		for(auto& d:dados)d->Draw();

		std::string lifeString = "HP: " + std::to_string(playerHP);
		infoFont->BlitText(blit3D->screenWidth / 4 - 50, 300, lifeString);
		lifeString = "Magic Points: " + std::to_string(playerMagicPoints);
		infoFont->BlitText(blit3D->screenWidth / 4 - 50,  200, lifeString);
		lifeString = "Rolls Left: " + std::to_string(rollsLeft);
		infoFont->BlitText(blit3D->screenWidth / 4 - 50, 100, lifeString);
		
		for (auto& p : particleList) p->Draw(8.f);
		
		break;
	}
	case GAMEOVER:
	{
		logoGameOver->Blit(blit3D->screenWidth / 2, blit3D->screenHeight / 2);
		if (ghostHP <= 0) 
		{
			logoYouWin->Blit(blit3D->screenWidth / 2, blit3D->screenHeight / 2);
		}
		if (playerHP <= 0)
		{
			logoYouLoseHP->Blit(blit3D->screenWidth / 2, blit3D->screenHeight / 2);
		}
		if (ghostEnergy >= 20)
		{
			logoYouLoseEnergy->Blit(blit3D->screenWidth / 2, blit3D->screenHeight / 2);
		}
		break;
	}
	}
}

//the key codes/actions/mods for DoInput are from GLFW: check its documentation for their values
void DoInput(int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		blit3D->Quit(); //start the shutdown sequence

	//suicide button is K
	if (key == GLFW_KEY_K && action == GLFW_PRESS)
	{
		if (gameState == PLAYING)
		{
			gameState = GAMEOVER;
		}
	}

	if (gameState == START) {
		if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
		{
			gameState = PLAYING;
			attachedBall = true;
			//playerHP = 3;
			level = 1;
			LoadGhost();
			LoadCard();
			musicID = audioE->PlayEvent("loungemusic", mainGameID);
		}
	}

	if (gameState == PLAYING)
	{
		if (key == GLFW_KEY_C && action == GLFW_PRESS && card->cost <= playerMagicPoints)
			ApplyCardEffect();

		if (key == GLFW_KEY_R && action == GLFW_PRESS && canRoll)
			rollDice();

		if (key == GLFW_KEY_X && action == GLFW_PRESS)
		{
			useDices();
			useDicesGhost();
		}
			
	}

	if (gameState == GHOSTTURN)
	{
		if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
		{
			
			applyGhostDamage();
			hitByGhost = true;
			for (auto d : dados) d->value = 0;
			//gameState == PLAYING;
		}
		if (key == GLFW_KEY_C && action == GLFW_PRESS && card->cost <= playerMagicPoints)
			ApplyCardEffect();

	}

	if (gameState == GAMEOVER)
	{
		if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
		{
			audioE->StopEvent("loungemusic", mainGameID, musicID, 0);
			playerHP = 10;
			playerMagicPoints = 0;
			gameState = START;
		}
	}
}

void DoCursor(double x, double y)
{
	//scale display size
	cursorX = static_cast<float>(x) * (blit3D->screenWidth / blit3D->trueScreenWidth);
}

void DoMouseButton(int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		switch (gameState)
		{
		case START:
			gameState = PLAYING;
			attachedBall = true;
			//playerHP = 3;
			level = 1;
			LoadGhost();
			LoadCard();
			musicID = audioE->PlayEvent("loungemusic", mainGameID);
			break;

		case PLAYING:
			//gameState = GAMEOVER;
			break;
		case GAMEOVER:
			audioE->StopEvent("loungemusic", mainGameID, musicID, 0);
			playerHP = 10;
			playerMagicPoints = 0;
			gameState = START;
			break;
		default:
			break;
		}
	}

}

int main(int argc, char *argv[])
{
	//memory leak detection
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	
	//initialize Lua
	L = lua_open();
	//load Lua base libraries
	luaL_openlibs(L);

	//register our C++ functions with Lua
	//lua_register(TheLuaInterpreter, "FunctionIdentifierOnLua", FunctionNameC++);
	lua_register(L, "rollDN", rollDN);
	//load the script...this loads the all functions, but
	//doesn't execute it yet.
	if (!ErrorCheck(luaL_dofile(L, "LuaScripts\\script.lua")))
	{
		//DO NOT CLEAR THE ERROR STRING OFF THE STACK
		//AFTER luaL_dofile() OR AN EXCEPTION GETS THROWN
		std::cout << "ERROR processing script.lua\n\n";
	}
	else std::cout << "Scripts Loaded Succesfully\n\n";

	blit3D = new Blit3D(Blit3DWindowModel::BORDERLESSFULLSCREEN_1080P, 720, 480);

	//set our callback funcs
	blit3D->SetInit(Init);
	blit3D->SetDeInit(DeInit);
	blit3D->SetUpdate(Update);
	blit3D->SetDraw(Draw);
	blit3D->SetDoInput(DoInput);
	blit3D->SetDoCursor(DoCursor);
	blit3D->SetDoMouseButton(DoMouseButton);

	//Run() blocks until the window is closed
	blit3D->Run(Blit3DThreadModel::SINGLETHREADED);
	if (blit3D) delete blit3D;
	//cleanup Lua
	lua_close(L);
}