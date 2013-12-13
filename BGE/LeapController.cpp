#include "LeapController.h"
#include "Leap.h"
#include "Content.h"
#include "Model.h"
#include <gtc/quaternion.hpp>
#include <gtx/quaternion.hpp>
#include <gtc/matrix_transform.hpp>
#include <sstream>
#include <string>

using namespace BGE;
using namespace std;
using namespace Leap;

LeapController::LeapController(shared_ptr<Model> model):GameComponent()
{
	mass = 10.0f;
	velocity = glm::vec3(0);
	force = glm::vec3(0);
	acceleration = glm::vec3(0);
	angularAcceleration = glm::vec3(0);
	angularVelocity = glm::vec3(0);
	torque = glm::vec3(0);
	worldMode = world_modes::to_parent;
	this->model = model;
}


LeapController::~LeapController(void)
{
}

bool LeapController::Initialise()
{
	if (!model->initialised)
	{
		model->Initialise();
	}
	CalculateInertiaTensor();
	
	GameComponent::Initialise();
	return true;
}

void LeapController::CalculateInertiaTensor() { 
	float width = model->boundingBox.max.x - model->boundingBox.min.x;
	float height = model->boundingBox.max.y - model->boundingBox.min.y;
	float depth = model->boundingBox.max.z - model->boundingBox.min.z;

	inertialTensor[0][0] = (float) (mass * (pow(height, 2) + pow(depth, 2))) / 12.0f;
    inertialTensor[1][1] = (float) (mass * (pow(width, 2) + pow(depth, 2))) / 12.0f;
    inertialTensor[2][2] = (float) (mass * (pow(width, 2) + pow(height, 2))) / 12.0f;
}

void LeapController::AddForce(glm::vec3 force)
{
    this->force += force;
}

void LeapController::AddTorque(glm::vec3 torque)
{
    this->torque += torque;
}

void LeapController::AddForceAtPoint(glm::vec3 force, glm::vec3 point)
{
	glm::vec3 to = glm::cross(force, point);
    torque += to;

    force += force;
}

void LeapController::Draw()
{
	GameComponent::Draw();
}


void LeapController::Update(float timeDelta)
{
	Leap::Controller controller;
	const Frame frame = controller.frame();
	const Hand hand = frame.hands()[0];
	const FingerList fingers = hand.fingers();

	const Uint8 * keyState = Game::Instance()->GetKeyState();

	float scale = 10000.0f;

	if (!frame.hands().isEmpty()) 
	{
		//if (keyState[SDL_SCANCODE_SPACE])
		if(hand.palmPosition().pitch() < 1.4)
		{
			AddForce((look * scale * timeDelta)/glm::vec3(2,2,2));
		}

		// Yaw
		if (keyState[SDL_SCANCODE_J])
		//if(hand.palmPosition().roll() > -2.75 && hand.palmPosition().roll() < 0)
		{

			//AddTorque((up * scale * timeDelta)/glm::vec3(10,10,10));
		}
		if (keyState[SDL_SCANCODE_L])
		//if(hand.palmPosition().roll() < 2.75 && hand.palmPosition().roll() > 0)
		{
			//AddTorque((- up * scale * timeDelta)/glm::vec3(10,10,10));
		}
		// End of Yaw

		//Pitch
		//if (keyState[SDL_SCANCODE_K])
		if ((hand.direction().pitch()*RAD_TO_DEG)> 25.0)
		{
			AddTorque((right * scale * timeDelta)/glm::vec3(100,100,100));
		}
		//if (keyState[SDL_SCANCODE_L])
		if ((hand.direction().pitch()*RAD_TO_DEG)< -25.0)
		{
			AddTorque((-right * scale * timeDelta)/glm::vec3(100,100,100));
		}
		// End of Pitch

		// Roll
		//if (keyState[SDL_SCANCODE_Y])
		if((hand.palmNormal().roll()*RAD_TO_DEG) < 25.0)
		{
			AddTorque((look * scale * timeDelta)/glm::vec3(100,100,100));
		}
		//if (keyState[SDL_SCANCODE_H])
		if((hand.palmNormal().roll()*RAD_TO_DEG) > -25.0)
		{
			AddTorque((-look * scale * timeDelta)/glm::vec3(100,100,100));
		}

		// Do the Newtonian integration
		acceleration = force / mass;
		velocity += acceleration * timeDelta;
		position += velocity * timeDelta;
	
		if (glm::length(velocity) > 0.0001f)
		{
			look = glm::normalize(velocity);
			right = glm::cross(look, up);
			velocity *= 0.99f;
		}
	}
    // Do the Hamiltonian integration
	angularAcceleration = torque * glm::inverse(inertialTensor);
    angularVelocity = angularVelocity + angularAcceleration * timeDelta;

    glm::quat w = glm::quat(0, angularVelocity.x, angularVelocity.y, angularVelocity.z);

	orientation = orientation + ((w * (timeDelta / 2.0f)) * orientation);
	orientation = glm::normalize(orientation);
    torque = glm::vec3(0);
	force = glm::vec3(0);

	angularVelocity *= 0.99f;

	look = RotateVector(basisLook, orientation);
	up = RotateVector(basisUp, orientation);
	right = RotateVector(basisRight, orientation);

	GameComponent::Update(timeDelta);
}
