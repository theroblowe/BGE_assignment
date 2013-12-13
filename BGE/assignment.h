#pragma once
#include "Game.h"
#include "GameComponent.h"
#include "FountainEffect.h"
#include "Leap.h"

using namespace std;
using namespace Leap;

namespace BGE
{
	class assignment :
		public Game
	{
	public:
		assignment();
		~assignment(void);

		shared_ptr<GameComponent> spaceShip;
		shared_ptr<FountainEffect> fountain;
		float elapsed;
		bool Initialise();
		void Update(float timeDelta);

		glm::vec3 force;
		float mass;
		Controller controller;
	};
}
