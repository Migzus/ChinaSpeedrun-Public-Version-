#include "Scene.h"
#include "SceneManager.h"

#include "Editor.h"

#include "imgui.h"
#include "GameObject.h"
#include "ChinaEngine.h"
#include "VulkanEngineRenderer.h"

#include "Script.h"
#include "BulletManagerComponent.h"
#include "Transform.h"
#include "MeshRenderer.h"
#include "Camera.h"
#include "CameraComponent.h"
#include "AudioSystem.h"
#include "AudioComponent.h"
#include "PhysicsServer.h"
#include "Rigidbody.h"

#include "Debug.h"

cs::Scene::Scene() :
	audioSystem{ new AudioSystem }, physicsServer{ new PhysicsServer }
{}

void cs::Scene::Initialize()
{
	Debug::LogWarning("Init ", name);
}

void cs::Scene::Start()
{
	Debug::LogSuccess("Start ", name);

	auto _scriptableObjects{ registry.view<ScriptComponent>() };
	for (auto e : _scriptableObjects)
	{
		auto& _script{ registry.get<ScriptComponent>(e) };

		_script.Start();
	}

	auto _physicsSimulations{ registry.view<RigidbodyComponent>() };
	for (auto e : _physicsSimulations)
	{
		auto& _rigidbody{ registry.get<RigidbodyComponent>(e) };

		_rigidbody.ShootStartVelocity();
	}
}

void cs::Scene::Update()
{
	switch (ChinaEngine::editor.GetPlaymodeState())
	{
	case cs::editor::EngineEditor::Playmode::EDITOR:
		UpdateEditorComponents();
		break;
	case cs::editor::EngineEditor::Playmode::PLAY:
		UpdateComponents();
		break;
	case cs::editor::EngineEditor::Playmode::PAUSE:
		break;
	}
}

void cs::Scene::Exit()
{
	Debug::LogInfo("Exiting ", name);

	auto _scriptableObjects{ registry.view<ScriptComponent>() };
	for (auto e : _scriptableObjects)
	{
		auto& _script{ registry.get<ScriptComponent>(e) };

		_script.Exit();
	}
}

void cs::Scene::Free()
{
	ClearScene();
	QueueExit();

	delete this;
}

void cs::Scene::AddGameObject(GameObject* newObject)
{
    if (newObject == nullptr)
    {
        newObject = new GameObject(this);
        newObject->name = "Game Object (" + std::to_string(gameObjects.size()) + ")";
    }
    
	newObject->scene = this;

    gameObjects.push_back(newObject);
}

void cs::Scene::RemoveGameObject()
{

}

void cs::Scene::ClearScene()
{
	//DestroyDescriptorPools();

	// currently the resources will just not be deleted (they are stored in ResourceManager)
    for (GameObject* object : gameObjects)
        delete object;

    gameObjects.clear();
	renderableObjects.clear();
}

void cs::Scene::QueueExit()
{
	SceneManager::Unload(this);
}

cs::PhysicsServer* cs::Scene::GetPhysicsServer() const
{
	return physicsServer;
}

cs::AudioSystem* cs::Scene::GetAudioSystem() const
{
	return audioSystem;
}

void cs::Scene::AddToRenderQueue(RenderComponent* renderer)
{
	renderableObjects.push_back(renderer);
}

void cs::Scene::RemoveFromRenderQueue(RenderComponent* renderer)
{
	auto _it{ std::find(renderableObjects.begin(), renderableObjects.end(), renderer) };

	if (_it != renderableObjects.end())
		renderableObjects.erase(_it);
}

uint32_t cs::Scene::GetUBOOffset()
{
	return UniformBufferObject::GetByteSize() * (uint32_t)renderableObjects.size();
}

void cs::Scene::DestroyDescriptorPools()
{
	for (auto* object : renderableObjects)
		ChinaEngine::renderer.DestroyDescriptorPool(object->descriptorPool);
}

void cs::Scene::CreateDescriptorPools()
{
	for (auto* object : renderableObjects)
		ChinaEngine::renderer.MakeDescriptorPool(*object);
}

void cs::Scene::Input(int keycode, int scancode, int action, int mods)
{
	auto _scriptableObjects{ registry.view<ScriptComponent>() };
	for (auto e : _scriptableObjects)
	{
		auto& _script{ registry.get<ScriptComponent>(e) };

		_script.Input(keycode, scancode, action, mods);
	}
}

bool cs::Scene::ImGuiDrawGameObjects()
{
	bool _isClicked{ false };

    if (ImGui::TreeNode(name.c_str()))
    {
		_isClicked = ImGui::IsItemClicked();

        ImGui::SameLine();
        if (ImGui::Button("X"))
            SceneManager::Unload(this);

        for (GameObject* object : gameObjects)
        {
            ImGui::Text(object->name.c_str());
            if (ImGui::IsItemClicked())
                ChinaEngine::editor.SetSelectedGameObject(object);
        }
        ImGui::TreePop();
    }

	return _isClicked;
}

void cs::Scene::UpdateEditorComponents()
{
	auto _transformComponentView{ registry.view<TransformComponent>() };
	for (auto e : _transformComponentView)
	{
		auto& _transformComponent{ registry.get<TransformComponent>(e) };
		Transform::CalculateMatrix(_transformComponent);
	}

	auto _renderableObjects{ registry.view<MeshRendererComponent, TransformComponent>() };
	for (auto e : _renderableObjects)
	{
		auto& _transform{ registry.get<TransformComponent>(e) };
		auto& _meshRenderer{ registry.get<MeshRendererComponent>(e) };

		MeshRenderer::UpdateUBO(_meshRenderer, _transform, *SceneManager::mainCamera);
	}

	auto _bulletManagerObjects{ registry.view<BulletManagerComponent>() };
	for (auto e : _bulletManagerObjects)
	{
		auto& _bulletManagerComponent{ registry.get<BulletManagerComponent>(e) };

		_bulletManagerComponent.Update();
	}
}

void cs::Scene::UpdateComponents()
{
	auto _scriptableObjects{ registry.view<ScriptComponent>() };
	for (auto e : _scriptableObjects)
	{
		auto& _script{ registry.get<ScriptComponent>(e) };

		_script.Update();
	}

	auto _audioComponentView{ registry.view<AudioComponent>() };
	for (auto e : _audioComponentView)
	{
		auto& _audioComponent{ registry.get<AudioComponent>(e) };
		audioSystem->Update(_audioComponent);
	}

	auto _transformComponentView{ registry.view<TransformComponent>() };
	for (auto e : _transformComponentView)
	{
		auto& _transformComponent{ registry.get<TransformComponent>(e) };
		Transform::CalculateMatrix(_transformComponent);
	}

	auto _cameras{ registry.view<CameraComponent, TransformComponent>() };
	for (auto e : _cameras)
	{
		auto& _transform{ registry.get<TransformComponent>(e) };
		auto& _camera{ registry.get<CameraComponent>(e) };

		Camera::UpdateCameraTransform(_camera, _transform);
	}

	auto _renderableObjects{ registry.view<MeshRendererComponent, TransformComponent>() };
	for (auto e : _renderableObjects)
	{
		auto& _transform{ registry.get<TransformComponent>(e) };
		auto& _meshRenderer{ registry.get<MeshRendererComponent>(e) };

		MeshRenderer::UpdateUBO(_meshRenderer, _transform, *SceneManager::mainCamera);
	}

	physicsServer->Step();

	auto _physicsSimulations{ registry.view<RigidbodyComponent, TransformComponent>() };
	for (auto e : _physicsSimulations)
	{
		auto& _transform{ registry.get<TransformComponent>(e) };
		auto& _rigidbody{ registry.get<RigidbodyComponent>(e) };

		Rigidbody::CalculatePhysics(_rigidbody, _transform);
	}
}
