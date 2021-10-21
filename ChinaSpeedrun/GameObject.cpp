#include "GameObject.h"
#include "Signal.h"

cs::GameObject::GameObject() :
	name{ "Object" }, active{ true }
{
	ComponentInit();
}

void cs::GameObject::Test(int number, std::string someString)
{

}

class Foo
{
public:
	void kill(int number, std::string kill)
	{

	}
};

void cs::GameObject::EditorDrawComponents()
{
	//Foo a;
	//Signal::Connect("timeout", &a, &Foo::kill);
	//Signal::Connect("timeout", this, &GameObject::Test);
	//Signal::EmitSignal("timeout", &a, 2, "lol");
}

void cs::GameObject::Init()
{

}

void cs::GameObject::Start()
{

}

void cs::GameObject::Update()
{

}

void cs::GameObject::FixedUpdate()
{

}

void cs::GameObject::ExitTree()
{

}

void cs::GameObject::QueueFree()
{

}

cs::GameObject::~GameObject()
{
	
}

void cs::GameObject::ComponentInit()
{

}

void cs::GameObject::RemoveAllComponents()
{

}
