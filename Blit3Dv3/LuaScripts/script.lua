--Lua script for Exorcism_TheCardGame
--@author David Lopez Barcenas @ Frederico Bekerdoff

--Ghost data for the game, this can be updated with new ghost information
ghostData = 
{
	{
		name="Skull Meister",
		sprite="skull_meister.png",
		description="Master of Skelletons",
		hp=10,
		energy=5;
	},
	{
		name="Demonic Regan",
		sprite="demonic_regan.png",
		description="A girl possesed by an ancient demon",
		hp=10,
		energy=11;
	},
	{
		name="A regular ghost",
		sprite="a_regular_ghost.png",
		description="The classic ghost you read on books",
		hp=10,
		energy=6;
	}
}
--Card data for the game, this can be updated with new Card information
cardData = 
{
	{
		name="Super Bless",
		sprite="super_bless.png",
		description="Use to deal +3 Damage to ghost",
		hpPlayer=0,
		energyPlayer=0,
		damageHP=3,
		damageEnergy=0,
		cost=4;
	},
	{
		name="Holly Molly",
		sprite="holly_molly.png",
		description="Use to gain +2 HP",
		hpPlayer=2,
		energyPlayer=0,
		damageHP=0,
		damageEnergy=0,
		cost=3;
	},
	{
		name="Bless of heaven",
		sprite="bless_of_heaven.png",
		description="Use to gain +5 Magic Points",
		hpPlayer=0,
		energyPlayer=5,
		damageHP=0,
		damageEnergy=0,
		cost=2;
	},
	{
		name="Pray for your soul",
		sprite="pray_for_soul.png",
		description="Use to reduce ghost energy -4",
		hpPlayer=0,
		energyPlayer=0,
		damageHP=0,
		damageEnergy=4,
		cost=4;
	}
}

--function that returns a given element from an array -- works inside lua
function random_from_array( varray )
	local r = rollDN( #varray )
	return varray[ r ]
end

--function that returns a random number between 1 and tghe size of ghostData
function randomGhost()
	local r = rollDN(#ghostData)
	return r
end


--function that returns a random number between 1 and tghe size of cardData
function randomCard()
	local r = rollDN(#cardData)
	return r
end

--function to get the quantity of elements from a list x==1 cardData list, x == 2 for ghostData list
function getArraySize(x)
	if x == 1 then 	return #cardData end
	if x == 2 then 	return #ghostData 
	else return (0) end
end

--function to get the fileNames for sprite generation
function getFileName(x, y)
	if y == 1 then 	card = cardData[x] 	return card.sprite end
	if y == 2 then 	ghost = ghostData[x] 	return ghost.sprite 
	else noName = "empty.png" return noName end
end

--fuction to get the names of the ghost or cards
function getName(x, y)
	if y == 1 then 	card = cardData[x] 	return card.name end
	if y == 2 then 	ghost = ghostData[x] 	return ghost.name 
	else noName = "no name" return noName end
end

--fuction to get the names of the ghost or cards
function getDescription(x, y)
	if y == 1 then 	card = cardData[x] 	return card.description end
	if y == 2 then 	ghost = ghostData[x] 	return ghost.description 
	else noDescription = "no description" return noDescription end
end

--function to get card attributes 1
function getCardAttribute(attribute, cardID)
	if attribute == 1 then return cardData[cardID].hpPlayer end
	if attribute == 2 then return cardData[cardID].energyPlayer end
	if attribute == 3 then return cardData[cardID].damageHP end
	if attribute == 4 then return cardData[cardID].damageEnergy end
	if attribute == 5 then return cardData[cardID].cost
	else return (- 1) end
end


--function to get card attributes 1
function getGhostAttribute(attribute, ghostID)
	if attribute == 1 then return ghostData[ghostID].hp end
	if attribute == 2 then return ghostData[ghostID].energy 
	else return (- 1) end
end

--create a name using Markov Chain Generator
function NameCreation()
	firstName = MarkovName()
	lastName = MarkovName()
	title = RandomString()
	return firstName.." "..lastName.." "..title
end

--function to generate a random class and race, no need to define if the character is avegare or heroic
function GenerateClassAndRace()
	job = rollDN(6)							--class
	race = rollDN(5)						--race
	return job, race
end

--function to generate a single stat value x == 1 means heroic, x == 2 for average
function GenerateSingleStatValue(x)
	power = 1							--the times the value would increment, here we can buff or nerf NPC
	if x < 2 then power = 2 else power = 1 end
	value = power *  Roll4D6DropLowest()
	return value
end

--generate basic stats using dieRoll calls
function GenerateStatsBasic(x)
	increment = 1
	if x < 2 then increment = 2 else increment = 1 end
	INT = increment * Roll4D6DropLowest()	--intelligence
	STR = increment * Roll4D6DropLowest()	--strength
	HP = increment * Roll4D6DropLowest()	--hit points
	job = rollDN(6)							--class
	race = rollDN(5)						--race
	return INT, STR, HP, job, race   --SADLY, lua can only handle up to three returns 
end

--generate extended stats using dieRoll calls
function GenerateStatsExtended(x)
	increment = 1
	if x < 2 then increment = 2 else increment = 1 end
	CON = increment * Roll4D6DropLowest()	--constitution
	DEX = increment * Roll4D6DropLowest()	--dexterity
	CHA = increment * Roll4D6DropLowest()	--charisma
	WIS = increment * Roll4D6DropLowest()	--wisdom
	AC = increment * Roll4D6DropLowest()	--armour class
	return CON, DEX, CHA, WIS, AC
end


function Gibberish()
-- get a random word from a C++ function
	global_counter = 0
	aWord = RandomString()
	global_counter = global_counter + 1
	return aWord .. " " .. global_counter
end

function TestLoad()
	aWord = "Lua 5.1 Loaded "	
	return aWord
end

--
--element = random_from_array(cardData)
--nameOfcard = element.name.. "  --- from Lua"
--print(nameOfcard)