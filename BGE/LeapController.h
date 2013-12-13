#pragma once
#include "GameComponent.h"
#include "Model.h"
#include "Leap.h"
using namespace std;
using namespace Leap;
namespace BGE
{
	class LeapController :
		public GameComponent
	{
	private:
		void LeapController::CalculateInertiaTensor();
		shared_ptr<Model> model;
	public:
		LeapController(shared_ptr<Model> model);
		~LeapController(void);

		bool Initialise();
		void Update(float timeDelta);
		void Draw();

		float mass;
		glm::vec3 velocity, force, acceleration;
		glm::vec3 angularVelocity, angularAcceleration, torque;
		glm::mat3 inertialTensor;

		void AddForce(glm::vec3);
		void AddTorque(glm::vec3);
		void AddForceAtPoint(glm::vec3, glm::vec3);
	};

}