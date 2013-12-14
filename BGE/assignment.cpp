#include "assignment.h"
#include "Content.h"
#include "VectorDrawer.h"
#include "LazerBeam.h"
#include "FountainEffect.h"
#include "LeapController.h"
#include "Leap.h"
#include "LineDrawer.h"
#include "Utils.h"
#include "LeapMath.h"
#include "SteeringControler.h"
#include "Params.h"

using namespace BGE;
using namespace Leap;

shared_ptr<LeapController> spaceshipController;

assignment::assignment(void)
{

	elapsed = 10000;
}


assignment::~assignment(void)
{
}

bool assignment::Initialise()
{
	Params::Load("default");
	std::shared_ptr<GameComponent> ground = make_shared<Ground>();
	Attach(ground);	

	spaceShip = make_shared<GameComponent>();
	shared_ptr<Model> model = Content::LoadModel("cobramk3", glm::rotate(glm::mat4(1), 180.0f, glm::vec3(0,1,0))); 
	spaceshipController = make_shared<LeapController>(model);

	spaceShip->Attach(spaceshipController);
	spaceShip->Attach(model);
	spaceShip->GetController()->position = glm::vec3(-10, 2, -10);
	spaceShip->Attach(make_shared<VectorDrawer>());
	Attach(spaceShip);
	//camera->Attach(spaceshipController);

	fountain = make_shared<FountainEffect>(200);
	//fountain->worldMode = world_modes::from_parent;
	spaceShip->Attach(fountain);
	fountain->diffuse = glm::vec3(1,0,0);

	riftEnabled = false;
	fullscreen = false;
	width = 800;
	height = 600;

	mass = 1.0f;
	spaceShip->velocity = glm::vec3(0,0,0);

	Game::Initialise();
	shared_ptr<SteeringController> sc = make_shared<SteeringController>();
	sc->TurnOn(SteeringController::offset_pursuit);
	sc->leader = spaceshipController;
	sc->offset = glm::vec3(0,1,5);
	sc->position = spaceshipController->position + sc->offset;
	camera->Attach(sc);
	//camera->GetController()->position = glm::vec3(0, 4, 20);
	return true;
}

void assignment::Update(float timeDelta)
{

	Leap::Vector start[5] = {};
	Leap::Vector end[5] = {};

	glm::vec3 Glstart[5] = {};
	glm::vec3 Glend[5] = {};

	fountain->position = spaceShip->position;
	glm::vec3 acceleration;
	float newtons = 10.0f;
	float epsilon = glm::epsilon<float>();
	const Frame frame = controller.frame();

	if (!frame.hands().isEmpty()) 
	{
		// Get the first hand
		const Hand hand = frame.hands()[0];
		// Check if the hand has any fingers
		const FingerList fingers = hand.fingers();
		if (!fingers.isEmpty()) 
		{
			Game::Instance()->PrintFloat("PALM PITCH",hand.palmPosition().pitch());
			Game::Instance()->PrintFloat("PALM ROLL",hand.palmPosition().roll());
			Game::Instance()->PrintFloat("PALM YAW",hand.palmPosition().yaw());

			Game::Instance()->PrintFloat("HAND PITCH",hand.direction().pitch()*RAD_TO_DEG);
			Game::Instance()->PrintFloat("HAND ROLL",hand.palmNormal().roll()*RAD_TO_DEG);
			Game::Instance()->PrintFloat("HAND YAW",hand.direction().yaw()*RAD_TO_DEG);

			//spaceshipController->Pitch((hand.direction().pitch()*RAD_TO_DEG)/75.0);
			//spaceshipController->Roll(-(hand.palmNormal().roll()*RAD_TO_DEG)/50.0);
			//spaceshipController->Yaw(-(hand.direction().yaw()*RAD_TO_DEG)/50.0);

			for (int i = 0; i < fingers.count(); ++i) 
			{
        	  
			  //start[i].x = fingers[i].tipPosition().x - fingers[i].length();
			  //start[i].y = fingers[i].tipPosition().y - fingers[i].length();
			  //start[i].z = fingers[i].tipPosition().z - fingers[i].length();

			  start[i].x = hand.palmPosition().x;
			  start[i].z = hand.palmPosition().z;
			  start[i].y = hand.palmPosition().y;

			  end[i] = fingers[i].tipPosition();
			  //std::cout << i << start[i] << end[i] <<endl;

			  Glstart[i] = BGE::LeapToGLVector(start[i]);
			  Glend[i] = BGE::LeapToGLVector(end[i]);

			  Glstart[i].x = Glstart[i].x/10;
			  Glstart[i].z = Glstart[i].z/10;
			  Glstart[i].y = Glstart[i].y/10;

			  Glend[i].x = Glend[i].x/10;
			  Glend[i].z = Glend[i].z/10;
			  Glend[i].y = Glend[i].y/10;
		  
		  }

			  for (int i = 0; i < fingers.count(); ++i) 
		  {
			  glm::vec3 colour = glm::vec3(255,255,0);
		  
			  LineDrawer::DrawLine(Glstart[i],Glend[i],colour);
		  
		  }

			//camera->GetController()->position = spaceShip->position;
			spaceShip->velocity *= 0.99f;
		}
	}

	//camera->GetController()->position.x = spaceShip->GetController()->position.x;
	//camera->GetController()->look = spaceShip->GetController()->look;
	//camera->GetController()->orientation = spaceShip->GetController()->orientation;




	Game::Update(timeDelta);
}