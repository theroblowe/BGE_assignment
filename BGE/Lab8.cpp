#include "Lab8.h"
#include "Content.h"
#include "VectorDrawer.h"
#include "LazerBeam.h"
#include "FountainEffect.h"
#include "Steerable3DController.h"
#include "Leap.h"
#include "LineDrawer.h"
#include "Utils.h"
#include "LeapMath.h"

using namespace BGE;
using namespace Leap;

Lab8::Lab8(void)
{
	elapsed = 10000;
}


Lab8::~Lab8(void)
{
}

shared_ptr<Steerable3DController> shipController;
bool Lab8::Initialise()
{
	std::shared_ptr<GameComponent> ground = make_shared<Ground>();
	Attach(ground);	

	ship1 = make_shared<GameComponent>();
	shared_ptr<Model> model = Content::LoadModel("cobramk3", glm::rotate(glm::mat4(1), 180.0f, glm::vec3(0,1,0))); 
	shipController = make_shared<Steerable3DController>(model);

	ship1->Attach(shipController);
	ship1->Attach(model);
	ship1->GetController()->position = glm::vec3(-10, 2, -10);
	ship1->Attach(make_shared<VectorDrawer>());
	Attach(ship1);

	
	
	riftEnabled = false;
	fullscreen = false;
	width = 800;
	height = 600;

	mass = 1.0f;
	ship1->velocity = glm::vec3(0,0,0);

	Game::Initialise();

	camera->GetController()->position = glm::vec3(0, 4, 20);
	return true;
}

void Lab8::Update(float timeDelta)
{
	
	Leap::Vector start[5] = {};
	Leap::Vector end[5] = {};

	glm::vec3 Glstart[5] = {};
	glm::vec3 Glend[5] = {};

	const Frame frame = controller.frame();

	//std::cout << ", hands: " << frame.hands().count() <<endl;
    //std::cout << ", fingers: " << frame.fingers().count() <<endl;
	//std::cout << ", tools: " << frame.tools().count() <<endl;

	if (!frame.hands().isEmpty()) 
	{
		// Get the first hand
		const Hand hand = frame.hands()[0];
		// Check if the hand has any fingers
		const FingerList fingers = hand.fingers();
		if (!fingers.isEmpty()) 
		{
			Leap::Finger first = fingers.leftmost();
			Leap::Finger last = fingers.rightmost();
			Leap::Finger front = fingers.frontmost();

			Game::Instance()->PrintFloat("HAND PITCH",hand.direction().pitch()*RAD_TO_DEG);
			Game::Instance()->PrintFloat("HAND ROLL",hand.palmNormal().roll()*RAD_TO_DEG);
			Game::Instance()->PrintFloat("HAND YAW",hand.direction().yaw()*RAD_TO_DEG);
			//Game::Instance()->PrintFloat("FIRST FINGER  ",first.tipPosition().y);
			//Game::Instance()->PrintFloat("LAST FINGER  ",last.tipPosition().y);
			//Game::Instance()->PrintFloat("PALM POSITION  ",hand.palmPosition().y);
			//Game::Instance()->PrintFloat("FRONT FINGER  ",front.tipPosition().y);
			  //For each finger calculate the finger start and end point
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

		if(first.tipPosition().y + 40.0 > last.tipPosition().y)
		{
			//Game::Instance()->PrintText("Turning Right");
			//shipController->AddTorque(glm::vec3(0,-10,0));
			//shipController->Roll(0.5);
			//shipController->Yaw(0.5);
			//shipController->AddForce(glm::vec3(0,-10,0));
		}
		if(last.tipPosition().y + 40.0 > first.tipPosition().y)
		{
			//Game::Instance()->PrintText("Turning Left");
			//shipController->AddTorque(glm::vec3(0,10,0));
			//shipController->Roll(-0.5);
			//shipController->Yaw(-0.5);
			//shipController->AddForce(glm::vec3(0,10,0));
		}
		if(hand.palmPosition().y > front.tipPosition().y + 30.0)
		{
			//Game::Instance()->PrintText("Go Forward");
			//shipController->Pitch(-0.5);	

			//shipController->AddForce(glm::vec3(0,0,-100));
			//shipController->look = shipController->look * glm::vec3(0,100,0);
			//shipController->position = shipController->velocity * timeDelta;

		}
		if(front.tipPosition().y > hand.palmPosition().y + 30.0)
		{
			//Game::Instance()->PrintText("Go Reverse");
			//shipController->Pitch(0.5);
			//shipController->AddForce(glm::vec3(0,0,100));
			//shipController->look = shipController->look * glm::vec3(0,-100,0);
		}

		  for (int i = 0; i < fingers.count(); ++i) 
		  {
			  glm::vec3 colour = glm::vec3(255,255,0);
		  
			  LineDrawer::DrawLine(Glstart[i],Glend[i],colour);
		  
		  }
		}
		
	}

	glm::vec3 acceleration;
	const Hand hand = frame.hands()[0];
	const FingerList fingers = hand.fingers();

	if (!frame.hands().isEmpty()) 
	{
		shipController->Pitch((hand.direction().pitch()*RAD_TO_DEG)/75.0);
		shipController->Roll(-(hand.palmNormal().roll()*RAD_TO_DEG)/50.0);
		//shipController->Yaw(-(hand.direction().yaw()*RAD_TO_DEG)/50.0);
	}


	// Forces on ship1
	float newtons = 10.0f;
	float epsilon = glm::epsilon<float>();
	/*
	//if (keyState[SDL_SCANCODE_UP])
	if (!frame.hands().isEmpty()) 
	{
		//if (keyState[SDL_SCANCODE_UP])
		if(hand.palmPosition().pitch()<1.2)
		{

			force += ship1->look * newtons;
			force += force * glm::vec3(2,2,2);
		}
		//if (keyState[SDL_SCANCODE_DOWN])
		if(hand.palmPosition().pitch()>2.0)
		{
			force -= ship1->look * newtons;
		}
		//if (keyState[SDL_SCANCODE_RIGHT])
		if(hand.palmPosition().roll()< 2.5 && hand.palmPosition().roll() > 0)
		{
			force += ship1->right * newtons;
		}
		//if (keyState[SDL_SCANCODE_LEFT])
		if(hand.palmPosition().roll() > (-2.5) && hand.palmPosition().roll() < 0)
		{
			force -= ship1->right * newtons;
		}
		if (keyState[SDL_SCANCODE_SPACE])
		{
			force += force * glm::vec3(10,10,10);
		}
	}
	*/
	// Now calculate the acceleration, new velocity and new position
	//acceleration = force/mass;
	//ship1->velocity += acceleration * timeDelta;
	//ship1->position += ship1->velocity * timeDelta;
	
	// Check if the velocity length is > epsilon and if so create the look vector from the velocity
	//if(glm::length(ship1->velocity) > epsilon)
	//{
	//	ship1->look = glm::normalize(ship1->velocity);
	//}
	
	// Now check to see if the |look - basis| > epsilon
	//if(glm::length(ship1->look - GameComponent::basisLook) > epsilon)
	//{
		// And if so calculate the quaternion
	//	glm::vec3 axis = glm::cross(GameComponent::basisLook, ship1->look);
	//	axis = glm::normalize(axis);
	//	float theta = glm::acos(glm::dot(ship1->look, GameComponent::basisLook));
	//	ship1->orientation = glm::angleAxis(glm::degrees(theta), axis);
	//}
	/*
	camera->GetController()->position = ship1->position;
	camera->GetController()->position.z = ship1->position.z;
	camera->GetController()->look = ship1->look;
	camera->GetController()->orientation = ship1->orientation;
	*/
	// Apply damping
	//ship1->velocity *= 0.99f;
	// Reset the force accumulator
	//force = glm::vec3(0,0,0);


	Game::Update(timeDelta);

}
