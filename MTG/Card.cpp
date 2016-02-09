#include "Card.h"

glm::vec3 * Card::deckPosition = NULL;
glm::vec3 * Card::deckDirection = NULL;
glm::vec3 * Card::deckTapDirection = NULL;
list<Card *> * Card::cardList = NULL;
int Card::graveyardOffset = 0;
int Card::exileOffset = 0;
set<string> Card::landNames;
int * Card::playerID = NULL;
int * Card::numPlayers = NULL;
bool * Card::showHand = NULL;

Card::Card(string texturePath, int owner) : GameObject("card.obj", texturePath, "Shaders\\card.glsl")
{
	init();
	ownerIndex = owner;
	targetPosition = position = deckPosition[owner];//glm::vec3(0,0,0);
	targetDirection = direction = glm::vec3(0,-1,0);
	targetUp = up = deckDirection[owner];
}

Card::Card(string modelPath, string texturePath, string shaderPath, int owner) : GameObject(modelPath, texturePath, shaderPath)
{
	init();
	ownerIndex = owner;
	targetPosition = position = glm::vec3(0,0,0);
	targetDirection = direction = glm::vec3(0,0,1);
	targetUp = up = glm::vec3(0,1,0);
}

Card::Card(string modelPath, string texturePath, string shaderPath, glm::vec3 position, glm::vec3 direction, glm::vec3 up, int owner) : GameObject(modelPath, texturePath, shaderPath, position, direction, up)
{
	init();
	ownerIndex = owner;
	targetPosition = this->position = position;
	targetDirection = this->direction = direction;
	targetUp = this->up = up;
}

void Card::init() 
{
	cardRegion = region_hand;
	Card * parentCard = NULL;
	back = NULL;
	unknown = NULL;
	backString = "Textures\\cardback.jpg";
	unknownString = "Textures\\unknown.jpg";
	lighting = false;
	cardList->push_back(this);
}

Card::~Card()
{
	cardList->remove(this);
}

void Card::setStatics(list<Card *> * CardList, int * PlayerID, int * NumPlayers, bool * ShowHand)
{
	cardList = CardList; 
	playerID = PlayerID;
	numPlayers = NumPlayers;
	showHand = ShowHand;

	string tempLandNames[] = {
		"Arena","Bazaar of Baghdad","Dark Depths","Diamond Valley","Eye of Ugin","Glacial Chasm","Halls of Mist","Ice Floe",
		"Island of Wak-Wak","Maze of Ith","Oasis","Safe Haven","Sorrow's Path","The Tabernacle at Pendrell Vale",

		"Ancient Tomb","Blasted Landscape","Blinkmoth Nexus","Blinkmoth Well","Buried Ruin","Cathedral of War","City of Shadows",
		"City of Traitors","Cloudpost","Contested War Zone","Crystal Vein","Darksteel Citadel","Desert","Deserted Temple","Dread Statuary",
		"Dust Bowl","Eldrazi Temple","Elephant Graveyard","Encroaching Wastes","Gargoyle Castle","Ghost Quarter","Ghost Town","Glimmerpost",
		"Gods' Eye, Gate to the Reikai","Griffin Canyon","Hall of the Bandit Lord","Haunted Fengraf","High Market","Homeward Path","Inkmoth Nexus",
		"Library of Alexandria BANNED IN EDH","Maze's End","Maze of Shadows","Mikokoro, Center of the Sea","Miren, the Moaning Well","Mishra's Factory",
		"Mishra's Workshop","Mutavault","Mystifying Maze","Petrified Field","Phyrexia's Core","Quicksand","Rath's Edge","Reliquary Tower","Rishadan Port",
		"Rogue's Passage","Scorched Ruins","Seraph Sanctuary","Sheltered Valley","Stalking Stones","Strip Mine","Swarmyard","Tectonic Edge","Temple of the False God",
		"Thespian's Stage","Tower of the Magistrate","Untaidake, the Cloud Keeper","Urza's Factory","Urza's Mine","Urza's Power Plant","Urza's Tower","Wasteland",
		"Winding Canyons","Wintermoon Mesa","Zoetic Cavern",
	
		"Mouth of Ronom","Scrying Sheets",
	
		"Abandoned Outpost","Ancient Ziggurat","Archaeological Dig","Bog Wreckage","Cavern of Souls","City of Ass","City of Brass","Command Tower","Crystal Quarry",
		"Evolving Wilds","Exotic Orchard","Forbidden Orchard","Forsaken City","Gemstone Caverns","Gemstone Mine","Glimmervoid","Grand Coliseum","Henge of Ramos","Lotus Vale",
		"Meteor Crater","Mirrodin's Core","Nykthos, Shrine to Nyx","Opal Palace","Pillar of the Paruns","Primal Beyond","Rainbow Vale","Ravaged Highlands","Reflecting Pool",
		"Rhystic Cave","Rupture Spire","School of the Unseen","Seafloor Debris","Shimmering Grotto","Springjack Pasture","Tarnished Citadel","Tendo Ice Bridge","Terminal Moraine",
		"Terrain Generator","Terramorphic Expanse","Thawing Glaciers","Thran Quarry","Timberland Ruins","Transguild Promenade","Undiscovered Paradise","Unknown Shores",
		"Unstable Frontier","Vesuva","Vivid Crag","Vivid Creek","Vivid Grove","Vivid Marsh","Vivid Meadow",

		"Ancient Den","Cathedral of Serra","Daru Encampment","Drifting Meadow","Eiganjo Castle","Emeria, the Sky Ruin","Flagstones of Trokair","Forbidding Watchtower","Fountain of Cho",
		"Icatian Store","Kabira Crossroads","Karakas","Karoo","Kjeldoran Outpost","Kor Haven","Mistveil Plains","New Benalia","Nomad Stadium","Plains","Plains1","Plains2","Plains3",
		"Plains4","Plains5","Remote Farm","Ruins of Trokair","Rustic Clachan","Secluded Steppe","Sejiri Steppe","Serra's Sanctum","Snow-Covered Plains","Windbrisk Heights",

		"Academy Ruins","Cephalid Coliseum","Coral Atoll","Faerie Conclave","Halimar Depths","Island","Island1","Island2","Island3","Island4","Island5","Lonely Sandbar","Magosi, the Waterveil",
		"Minamo, School at Water's Edge","Moonring Island","Oboro, Palace in the Clouds","Remote Isle","Riptide Laboratory","Sand Silos","Saprazzan Cove","Saprazzan Skerry","Seafarer's Quay",
		"Seat of the Synod","Shelldock Isle","Snow-Covered Island","Soaring Seacliff","Soldevi Excavations","Svyelunite Temple","Teferi's Isle","Tolaria","Tolaria West","Tolarian Academy",

		"Barren Moor","Bojuka Bog","Bottomless Vault","Cabal Coffers","Cabal Pit","Crypt of Agadeem","Dakmor Salvage","Ebon Stronghold","Everglades","Howltooth Hollow","Lake of the Dead",
		"Leechridden Swamp","Peat Bog","Phyrexian Tower","Piranha Marsh","Polluted Mire","Shizo, Death's Storehouse","Snow-Covered Swamp","Spawning Pool","Subterranean Hangar","Swamp",
		"Swamp1","Swamp2","Swamp3","Swamp4","Swamp5","Tomb of Urami","Unholy Citadel","Unholy Grotto","Urborg","Urborg, Tomb of Yawgmoth","Vault of Whispers","Volrath's Stronghold",

		"Balduvian Trading Post","Barbarian Ring","Dormant Volcano","Dwarven Hold","Dwarven Ruins","Forgotten Cave","Ghitu Encampment","Goblin Burrows","Great Furnace","Hammerheim",
		"Hellion Crucible","Keldon Megaliths","Keldon Necropolis","Kher Keep","Madblind Mountain","Mercadian Bazaar","Mountain","Mountain1","Mountain2","Mountain3","Mountain4",
		"Mountain5","Mountain Stronghold","Sandstone Needle","Shinka, the Bloodsoaked Keep","Shivan Gorge","Smoldering Crater","Smoldering Spires","Snow-Covered Mountain",
		"Spinerock Knoll","Teetering Peaks","Valakut, the Molten Pinnacle",

		"Adventurers' Guildhouse","Centaur Garden","Dryad Arbor","Forest","Forest1","Forest2","Forest3","Forest4","Forest5","Gaea's Cradle","Havenwood Battleground","Heart of Yavimaya",
		"Hickory Woodlot","Hollow Trees","Jungle Basin","Khalni Garden","Llanowar Reborn","Mosswort Bridge","Okina, Temple to the Grandfathers","Oran-Rief, the Vastwood","Pendelhaven",
		"Rushwood Grove","Sapseep Forest","Slippery Karst","Snow-Covered Forest","Tranquil Thicket","Tree of Tales","Treetop Village","Turntimber Grove","Wirewood Lodge","Yavimaya Hollow",
		
		"Adarkar Wastes","Azorius Chancery","Azorius Guildgate","Boreal Shelf","Calciform Pools","Celestial Colonnade","Cloudcrest Lake","Coastal Tower","Flood Plain","Flooded Strand",
		"Glacial Fortress","Hallowed Fountain","Land Cap","Moorland Haunt","Mystic Gate","Nimbus Maze","Prahv, Spires of Order","Seachrome Coast","Seaside Haven","Sejiri Refuge",
		"Skycloud Expanse","Temple of Enlightenment","Thalakos Lowlands","Tundra","Wanderwine Hub",

		"Caves of Koilos","Fetid Heath","Godless Shrine","Isolated Chapel","Marsh Flats","Orzhov Basilica","Orzhov Guildgate","Orzhova, the Church of Deals","Salt Flats","Scrubland",
		"Starlit Sanctum","Tainted Field","Temple of Silence","Vault of the Archangel",
		
		"Ancient Amphitheater","Arid Mesa","Battlefield Forge","Boros Garrison","Boros Guildgate","Clifftop Retreat","Plateau","Rugged Prairie","Sacred Foundry","Scabland",
		"Slayers' Stronghold","Sunhome, Fortress of the Legion","Temple of Triumph",

		"Arctic Flats","Brushland","Elfhame Palace","Gavony Township","Grasslands","Graypelt Refuge","Grove of the Guardian","Horizon Canopy","Krosan Verge","Nantuko Monastery",
		"Razorverge Thicket","Riftstone Portal","Saltcrusted Steppe","Savannah","Selesnya Guildgate","Selesnya Sanctuary","Stirring Wildwood","Sungrass Prairie","Sunpetal Grove",
		"Temple Garden","Temple of Plenty","Tranquil Garden","Vec Townships","Veldt","Vitu-Ghazi, the City-Tree","Windswept Heath","Wooded Bastion",

		"Bad River","Creeping Tar Pit","Darkslick Shores","Darkwater Catacombs","Dimir Aqueduct","Dimir Guildgate","Dreadship Reef","Drowned Catacomb","Duskmantle, House of Shadow",
		"Frost Marsh","Jwar Isle Refuge","Nephalia Drownyard","Polluted Delta","River Delta","River of tears","Rootwater Depths","Salt Marsh","Secluded Glen","Sunken Ruins","Tainted Isle",
		"Temple of Deceit","Underground River","Underground Sea","Waterveil Cavern","Watery Grave",
		
		"Caldera Lake","Cascade Bluffs","Desolate Lighthouse","Izzet Boilerworks","Izzet Guildgate","Nivix, Aerie of the Firemind","Scalding Tarn","Shivan Reef","Steam Vents","Sulfur Falls",
		"Volcanic Island",
		
		"Alchemist's Refuge","Breeding Pool","Flooded Grove","Hinterland Harbor","Misty Rainforest","Novijen, Heart of Progress","Simic Growth Chamber","Simic Guildgate","Skyshroud Forest",
		"Temple of Mystery","Tropical Island","Yavimaya Coast",
		
		"Akoum Refuge","Auntie's Hovel","Badlands","Blackcleave Cliffs","Blood Crypt","Bloodstained Mire","Cinder Marsh","Dragonskull Summit","Graven Cairns","Lantern-lit Graveyard","Lava Tubes",
		"Lavaclaw Reaches","Molten Slagheap","Rakdos Carnarium","Rakdos Guildgate","Rix Maadi, Dungeon Palace","Rocky Tar Pit","Shadowblood Ridge","Stensia Bloodhall","Sulfurous Springs",
		"Tainted Peak","Temple of Malice","Tresserhorn Sinks","Urborg Volcano",

		"Bayou","Gilt-Leaf Palace","Golgari Guildgate","Golgari Rot Farm","Grim Backwoods","Llanowar Wastes","Overgrown Tomb","Pine Barrens","Svogthos, the Restless Tomb","Tainted Wood",
		"Twilight Mire","Verdant Catacombs","Woodland Cemetery",
		
		"Contested Cliffs","Copperline Gorge","Fire-lit Thicket","Fungal Reaches","Grove of the Burnwillows","Gruul Guildgate","Gruul Turf","Highland Weald","Karplusan Forest","Kazandu Refuge",
		"Kessig Wolf Run","Mogg Hollows","Mossfire Valley","Pinecrest Ridge","Mountain Valley","Raging Ravine","Rootbound Crag","Shivan Oasis","Skarrg, the Rage Pits","Stomping Ground","Taiga",
		"Temple of Abandon","Timberline Ridge","Wooded Foothills",
		
		"Aysen Abbey","Bant Panorama","Irrigation Ditch","Seaside Citadel","Treva's Ruins"
	};

	for (int i = 0; i < 485; i++) {
		for (int i2 = 0; i2 < tempLandNames[i].size(); i2++) {
			tempLandNames[i][i2] = tolower(tempLandNames[i][i2]);
		}
	}

	landNames = set<string>(tempLandNames, tempLandNames+485);
	int x = 2;
}

void Card::initDecks(int numPlayers) {
	if (deckPosition != NULL)		delete [] deckPosition;
	if (deckDirection != NULL)		delete [] deckDirection;
	if (deckTapDirection != NULL)	delete [] deckTapDirection;
	deckPosition =		new glm::vec3[numPlayers];
	deckDirection =		new glm::vec3[numPlayers];
	deckTapDirection =	new glm::vec3[numPlayers];

	graveyardOffset = 0;
	exileOffset = 0;

	for (int i = 0; i < numPlayers; i++) {
		float angleDiff = 360.0f/numPlayers;
		glm::vec3 dir = glm::rotate(glm::vec3(0,0,1), angleDiff*i, glm::vec3(0, 1, 0));
		glm::vec3 sideDir = glm::normalize(glm::cross(dir, glm::vec3(0, 1, 0)));
		deckPosition[i] = -dir*landZ + sideDir * deckX;
		deckDirection[i] = dir;
		deckTapDirection[i] = sideDir;
		
	}
}

bool Card::isLand(string cardPath) {
	string name;
	int numSlash = 3;
	int slashCount = 0;
	int i = 0;
	while (slashCount < numSlash) {
		if (cardPath[i] == '\\') slashCount++;
		i++;
	}
	while (i < cardPath.size()) {
		name += cardPath[i];
		i++;
	}
	while (name.back() != '.') name.pop_back();
	name.pop_back();
	while (name.back() != '.') name.pop_back();
	name.pop_back();

	set<string>::iterator it;
	it = landNames.find(name);

	return (it != landNames.end());
}

string Card::getCardName(string cardPath)
{
	string name;
	int numSlash = 3;
	int slashCount = 0;
	int i = 0;
	while (slashCount < numSlash) {
		if (cardPath[i] == '\\') slashCount++;
		i++;
	}
	while (i < cardPath.size()) {
		name += cardPath[i];
		i++;
	}
	while (name.back() != '.') name.pop_back();
	name.pop_back();
	while (name.back() != '.') name.pop_back();
	name.pop_back();

	if (name == "forest1" || name == "forest2" || name == "forest3" || name == "forest4" || name == "forest5")
		return "forest";
	else if (name == "island1" || name == "island2" || name == "island3" || name == "island4" || name == "island5")
		return "island";
	else if (name == "mountain1" || name == "mountain2" || name == "mountain3" || name == "mountain4" || name == "mountain5")
		return "mountain";
	else if (name == "plains1" || name == "plains2" || name == "plains3" || name == "plains4" || name == "plains5")
		return "plains";
	else if (name == "swamp1" || name == "swamp2" || name == "swamp3" || name == "swamp4" || name == "swamp5")
		return "swamp";
	else
		return name;
}

POINT Card::getPointRegion(glm::vec2 worldPos) //x is cardRegion, y is playerID
{
	POINT p;
	glm::vec2 corner1, corner2, corner3;

	for (int i = 0; i < *numPlayers; i++) {
		if (*showHand) {
			//check hand
			corner1 = glm::vec2(-deckDirection[i].x, -deckDirection[i].z) * (handZ - 0.5f - sectionSpacing/2) + glm::vec2(deckTapDirection[i].x, deckTapDirection[i].z) * (deckX - 0.5f - sectionSpacing/2);
			corner2 = glm::vec2(-deckDirection[i].x, -deckDirection[i].z) * (handZ - 0.5f - sectionSpacing/2) + glm::vec2(-deckTapDirection[i].x, -deckTapDirection[i].z) * (deckX - 0.5f - sectionSpacing/2);
			corner3 = glm::vec2(-deckDirection[i].x, -deckDirection[i].z) * (handZ + 0.5f + sectionSpacing/2) + glm::vec2(deckTapDirection[i].x, deckTapDirection[i].z) * (deckX - 0.5f - sectionSpacing/2);
			if (Utilities::pointInRect(worldPos, corner1, corner2, corner3)) { p.x = region_hand; p.y = i; return p; }
		}

		//check combat
		corner1 = glm::vec2(-deckDirection[i].x, -deckDirection[i].z) * (combatZ - 0.5f - sectionSpacing/2) + glm::vec2(deckTapDirection[i].x, deckTapDirection[i].z) * (deckX - 0.5f - sectionSpacing/2);
		corner2 = glm::vec2(-deckDirection[i].x, -deckDirection[i].z) * (combatZ - 0.5f - sectionSpacing/2) + glm::vec2(-deckTapDirection[i].x, -deckTapDirection[i].z) * (deckX - 0.5f - sectionSpacing/2);
		corner3 = glm::vec2(-deckDirection[i].x, -deckDirection[i].z) * (combatZ + 0.5f + sectionSpacing/2) + glm::vec2(deckTapDirection[i].x, deckTapDirection[i].z) * (deckX - 0.5f - sectionSpacing/2);
		if (Utilities::pointInRect(worldPos, corner1, corner2, corner3)) { p.x = region_combat; p.y = i; return p; }

		//check permanents
		corner1 = glm::vec2(-deckDirection[i].x, -deckDirection[i].z) * (permanentZ - 0.5f - sectionSpacing/2) + glm::vec2(deckTapDirection[i].x, deckTapDirection[i].z) * (deckX - 0.5f - sectionSpacing/2);
		corner2 = glm::vec2(-deckDirection[i].x, -deckDirection[i].z) * (permanentZ - 0.5f - sectionSpacing/2) + glm::vec2(-deckTapDirection[i].x, -deckTapDirection[i].z) * (deckX - 0.5f - sectionSpacing/2);
		corner3 = glm::vec2(-deckDirection[i].x, -deckDirection[i].z) * (permanentZ + 0.5f + sectionSpacing/2) + glm::vec2(deckTapDirection[i].x, deckTapDirection[i].z) * (deckX - 0.5f - sectionSpacing/2);
		if (Utilities::pointInRect(worldPos, corner1, corner2, corner3)) { p.x = region_permanent; p.y = i; return p; }

		//check lands
		corner1 = glm::vec2(-deckDirection[i].x, -deckDirection[i].z) * (landZ - 0.5f - sectionSpacing/2) + glm::vec2(deckTapDirection[i].x, deckTapDirection[i].z) * (deckX - 0.5f - sectionSpacing/2);
		corner2 = glm::vec2(-deckDirection[i].x, -deckDirection[i].z) * (landZ - 0.5f - sectionSpacing/2) + glm::vec2(-deckTapDirection[i].x, -deckTapDirection[i].z) * (deckX - 0.5f - sectionSpacing/2);
		corner3 = glm::vec2(-deckDirection[i].x, -deckDirection[i].z) * (landZ + 0.5f + sectionSpacing/2) + glm::vec2(deckTapDirection[i].x, deckTapDirection[i].z) * (deckX - 0.5f - sectionSpacing/2);
		if (Utilities::pointInRect(worldPos, corner1, corner2, corner3)) { p.x = region_land; p.y = i; return p; }

		//check deck
		corner1 = glm::vec2(-deckDirection[i].x, -deckDirection[i].z) * (landZ - 0.5f - sectionSpacing/2) + glm::vec2(deckTapDirection[i].x, deckTapDirection[i].z) * (deckX - 0.5f - sectionSpacing/2);
		corner2 = glm::vec2(-deckDirection[i].x, -deckDirection[i].z) * (landZ - 0.5f - sectionSpacing/2) + glm::vec2(deckTapDirection[i].x, deckTapDirection[i].z) * (deckX + 0.5f + sectionSpacing/2);
		corner3 = glm::vec2(-deckDirection[i].x, -deckDirection[i].z) * (landZ + 0.5f + sectionSpacing/2) + glm::vec2(deckTapDirection[i].x, deckTapDirection[i].z) * (deckX - 0.5f - sectionSpacing/2);
		if (Utilities::pointInRect(worldPos, corner1, corner2, corner3)) { p.x = region_topDeck; p.y = i; return p; }

		//check graveyard
		corner1 = glm::vec2(-deckDirection[i].x, -deckDirection[i].z) * (landZ - 0.5f - sectionSpacing/2) + glm::vec2(deckTapDirection[i].x, deckTapDirection[i].z) * (graveyardX - 0.5f - sectionSpacing/2);
		corner2 = glm::vec2(-deckDirection[i].x, -deckDirection[i].z) * (landZ - 0.5f - sectionSpacing/2) + glm::vec2(deckTapDirection[i].x, deckTapDirection[i].z) * (graveyardX + 0.5f + sectionSpacing/2);
		corner3 = glm::vec2(-deckDirection[i].x, -deckDirection[i].z) * (landZ + 0.5f + sectionSpacing/2) + glm::vec2(deckTapDirection[i].x, deckTapDirection[i].z) * (graveyardX - 0.5f - sectionSpacing/2);
		if (Utilities::pointInRect(worldPos, corner1, corner2, corner3)) { p.x = region_topGraveyard; p.y = i; return p; }

		//check exile
		corner1 = glm::vec2(-deckDirection[i].x, -deckDirection[i].z) * (landZ - 0.5f - sectionSpacing/2) + glm::vec2(deckTapDirection[i].x, deckTapDirection[i].z) * (exileX - 0.5f - sectionSpacing/2);
		corner2 = glm::vec2(-deckDirection[i].x, -deckDirection[i].z) * (landZ - 0.5f - sectionSpacing/2) + glm::vec2(deckTapDirection[i].x, deckTapDirection[i].z) * (exileX + 0.5f + sectionSpacing/2);
		corner3 = glm::vec2(-deckDirection[i].x, -deckDirection[i].z) * (landZ + 0.5f + sectionSpacing/2) + glm::vec2(deckTapDirection[i].x, deckTapDirection[i].z) * (exileX - 0.5f - sectionSpacing/2);
		if (Utilities::pointInRect(worldPos, corner1, corner2, corner3)) { p.x = region_topExile; p.y = i; return p; }
	}

	p.x = -1; p.y = -1;
	return p;
}

void Card::updateWorldMatrix()
{
	worldMatrix = glm::inverse(glm::lookAt(position, position+direction, up));
	normalMatrix = glm::transpose(glm::inverse(worldMatrix));

	backWorldMatrix = glm::inverse(glm::lookAt(position, position-direction, up)); 
	backNormalMatrix = glm::transpose(glm::inverse(worldMatrix));
}

void Card::flipX(float x)
{
	glm::vec3 sideVector = deckDirection[ownerIndex];
	direction = glm::rotate(direction, x, sideVector);
}

void Card::setOwnerIndex(int i)
{
	bool tapped = (targetUp == deckTapDirection[ownerIndex]);
	ownerIndex = i;
	if (tapped) 
		targetUp = deckTapDirection[ownerIndex];
	else
		targetUp = deckDirection[ownerIndex];
}

int Card::update(long elapsedTime)
{
	GameObject::update(elapsedTime);
	if (back == NULL) {
		back = assetManager->getTexture(backString);
		if (back == NULL) {
			assetManager->forceLoadTexture(backString);
			back = assetManager->getTexture(backString);
		}
	}
	if (unknown == NULL) {
		unknown = assetManager->getTexture(unknownString);
		if (unknown == NULL) {
			assetManager->forceLoadTexture(unknownString);
			unknown = assetManager->getTexture(unknownString);
		}
	}
	updateWorldMatrix();

	#define tapSpeed 0.65f
	#define angleBuffer (elapsedTime*tapSpeed/180.0f*M_PI)
	//Flip
	float angle = acos(glm::dot(direction, targetDirection));
	if (angle <= angleBuffer) direction = glm::normalize(targetDirection);
	else {
		glm::vec3 sideVector = glm::normalize(deckDirection[ownerIndex]);
		direction = glm::rotate(direction, elapsedTime*tapSpeed, sideVector);
	}

	//Tap
	angle = acos(glm::dot(up, targetUp));
	if (angle <= angleBuffer) 
		up = glm::normalize(targetUp); 
	else {
		glm::vec3 sideVector = glm::normalize(glm::cross(up, targetUp));
		up = glm::rotate(up, elapsedTime*tapSpeed, sideVector);
	}

	//Move
	#define moveSpeed 0.02f
	#define moveBuffer moveSpeed*elapsedTime
	if (abs(Utilities::magnitude(position - targetPosition)) <= moveBuffer)
		position = targetPosition;
	else {
		glm::vec3 dir = glm::normalize((targetPosition - position));
		position += dir*(float)(elapsedTime*moveSpeed);
	}
	
	return 0;
}

int Card::render(long totalElapsedTime)
{
	if (texture == NULL || back == NULL || unknown == NULL || model == NULL || projMatrix == NULL || viewMatrix == NULL) return -1;;
	
	Shader * temp = activeShader;
	if (!(*ignoreShader)) {
		if (shader == 0) 
			return -1;
		else shader->activate();
	}

	if (activeShader == NULL) return -1;

	activeShader->setUniformf1("time", totalElapsedTime);
	if (lighting)	activeShader->setUniformf1("enableLighting", 1.0f);
	else			activeShader->setUniformf1("enableLighting", 0.0f);
	if (selected)	activeShader->setUniformf1("selectedCard", 1.0f);
	else			activeShader->setUniformf1("selectedCard", 0.0f);

	activeShader->setUniformTexture("texS", getTexture()->getTextureID());
	glActiveTexture(GL_TEXTURE0);
	getTexture()->bindTexture();
	glUniform1i(glGetUniformLocation(activeShader->getShaderHandle(), "texS"), 0);

	activeShader->setUniformMatrixf4("worldViewProj", *projMatrix * *viewMatrix * worldMatrix);
	activeShader->setUniformMatrixf4("normalMatrix", normalMatrix);
	
	model->render();

	activeShader->setUniformTexture("texS", back->getTextureID());
	glActiveTexture(GL_TEXTURE0);
	back->bindTexture();
	glUniform1i(glGetUniformLocation(activeShader->getShaderHandle(), "texS"), 0);

	activeShader->setUniformMatrixf4("worldViewProj", *projMatrix * *viewMatrix * backWorldMatrix);
	activeShader->setUniformMatrixf4("normalMatrix", backNormalMatrix);
	
	model->render();

	if (!(*ignoreShader && temp != NULL)) temp->activate(); //restore old shader
}

