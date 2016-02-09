#ifndef CARD_H
#define CARD_H

#include "GameObject.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"	
#include "glm/gtx/rotate_vector.hpp"

#include <set>
using std::set;

#define sectionSpacing	0.25f
#define combatZ			(0.5f+0.0f+1.0f*sectionSpacing)
#define permanentZ		(0.5f+1.0f+2.0f*sectionSpacing)
#define landZ			(0.5f+2.0f+3.0f*sectionSpacing)
#define handZ			(0.5f+4.5f+4.0f*sectionSpacing)
#define deckX			5.0f
#define graveyardX		6.0f
#define exileX			7.0f

#define region_deck				0 //For drawing
#define region_topDeck			1 //return to top
#define region_bottomDeck		2 //return to bottom
#define region_hand				3
#define region_land				4
#define region_permanent		5
#define region_combat			6
#define region_topGraveyard		7
#define region_graveyard		8
#define region_graveyardView	9
#define region_topExile			10
#define region_exile			11
#define region_exileView		12

class Card : public GameObject
{
public:
	Card(string, int owner = 0);
	Card(string, string, string, int owner = 0);
	Card(string modelPath, string texturePath, string shaderPath, glm::vec3 position, glm::vec3 direction, glm::vec3 up, int owner = 0);
	virtual ~Card();

	void flipX(float x);
	void tap() { targetUp = deckTapDirection[ownerIndex]; }
	void unTap() { targetUp = deckDirection[ownerIndex]; }
	void toggleTap() { 
		if (cardRegion != region_deck && cardRegion != region_graveyard && cardRegion != region_exile) {
			if (targetUp == deckTapDirection[ownerIndex]) unTap(); 
			else if (targetUp == deckDirection[ownerIndex]) tap();
		}
	}

	void forceFlip() { targetDirection = -targetDirection; }
	void flip() { if (cardRegion != region_deck && cardRegion != region_graveyard && cardRegion != region_exile) targetDirection = -targetDirection; }
	bool isFlipped() { return targetDirection != glm::vec3(0, 1, 0); }
	bool isSelected() { return selected; }
	void setRegion(int x)		{ cardRegion = x; }
	void moveToTopDeck()		{ if (cardRegion != region_deck && cardRegion != region_graveyard && cardRegion != region_exile) cardRegion = region_topDeck; }
	void moveToBottomDeck()		{ if (cardRegion != region_deck && cardRegion != region_graveyard && cardRegion != region_exile) cardRegion = region_bottomDeck; }
	void moveToHand()			{ if (cardRegion != region_deck && cardRegion != region_graveyard && cardRegion != region_exile) cardRegion = region_hand; if (isFlipped()) flip(); }
	void moveToLands()			{ if (cardRegion != region_deck && cardRegion != region_graveyard && cardRegion != region_exile) cardRegion = region_land; }
	void moveToPermanents()		{ if (cardRegion != region_deck && cardRegion != region_graveyard && cardRegion != region_exile) cardRegion = region_permanent; }
	void moveToCombat()			{ if (cardRegion != region_deck && cardRegion != region_graveyard && cardRegion != region_exile) cardRegion = region_combat; }
	void moveToGraveyard()		{ if (cardRegion != region_deck && cardRegion != region_graveyard && cardRegion != region_exile) cardRegion = region_topGraveyard; }
	void moveToExile()			{ if (cardRegion != region_deck && cardRegion != region_graveyard && cardRegion != region_exile) cardRegion = region_topExile; }
	
	glm::vec3 getDeckPosition() { return deckPosition[ownerIndex]; }
	glm::vec3 getDeckDirection() { return glm::normalize(deckDirection[ownerIndex]); }
	glm::vec3 getTapDirection() { return glm::normalize(deckTapDirection[ownerIndex]); }
	glm::vec3 getGraveyardPosition() { return deckPosition[ownerIndex] + deckTapDirection[ownerIndex] * (graveyardX-deckX); }
	glm::vec3 getExilePosition() { return deckPosition[ownerIndex] + deckTapDirection[ownerIndex] * (exileX-deckX); }
	int getGraveyardOffset() { return graveyardOffset; }
	int getExileOffset() { return exileOffset; }
	int getCardRegion() { return cardRegion; }
	int getOwnerIndex() { return ownerIndex; }
	Card * getParentCard() { return parentCard; }
	string getCardName() { return getCardName(texturePath); }
	virtual Texture * getTexture() { 
		if (*playerID == ownerIndex) return texture; 
		else {
			if (isFlipped() || cardRegion == region_hand) return unknown; 
			return texture;
		}
	}
	void setParentCard(Card * p) { parentCard = p; }
	void setTargetPosition(float x, float y, float z) { targetPosition = glm::vec3(x,y,z); }
	void setTargetPosition(glm::vec3 v) { targetPosition = v; }
	void setOwnerIndex(int i);

	static void setStatics(list<Card *> * CardList, int * PlayerID, int * NumPlayers, bool * ShowHand);
	static void initDecks(int numPlayers = 2);
	static bool isLand(string cardName);
	static string getCardName(string cardPath);
	static void incGraveyardOffset() { graveyardOffset++; }
	static void incExileOffset() { exileOffset++; }
	static void decGraveyardOffset() { graveyardOffset--; }
	static void decExileOffset() { exileOffset--; }
	static POINT getPointRegion(glm::vec2 worldPos); //x is cardRegion, y is playerID

	virtual void updateWorldMatrix();
	virtual int update(long elapsedTime);
	virtual int render(long totalElapsedTime);
private:
	static bool * showHand;
	static int * numPlayers;
	static int * playerID;
	static glm::vec3 * deckPosition;
	static glm::vec3 * deckDirection;
	static glm::vec3 * deckTapDirection;
	static int graveyardOffset;
	static int exileOffset;
	static list<Card *> * cardList;
	static set<string> landNames;

	void init();

	int ownerIndex;
	int cardRegion;
	Card * parentCard;

	Texture * back;
	Texture * unknown;
	glm::mat4 backWorldMatrix, backNormalMatrix;
	string backString, unknownString;
	glm::vec3 targetPosition, targetDirection, targetUp;
};

#endif