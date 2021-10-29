#pragma once

namespace cs {

	class PhysicsSystem;

	class PhysicsLocator
	{
	public:
		static PhysicsSystem* GetPhysicsSystem();
		static void Provide(PhysicsSystem* service);
	private:
		static PhysicsSystem* system;
	};
}

