#include "Scene.h"
#include "SceneManager.h"

#include "Editor.h"

#include "imgui.h"
#include "GameObject.h"
#include "ChinaEngine.h"

#include "Debug.h"

#include "Transform.h"
#include "MeshRenderer.h"
#include "Camera.h"
#include "CameraComponent.h"
#include "AudioSystem.h"
#include "AudioComponent.h"
#include "PhysicsServer.h"
#include "Rigidbody.h"

cs::Scene::Scene() :
	audioSystem{ new AudioSystem }, physicsServer{ new PhysicsServer }
{}

void cs::Scene::Initialize()
{

}

void cs::Scene::Start()
{

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
	ClearScene();

	delete this;
}

void cs::Scene::AddGameObject(GameObject* newObject)
{
    if (newObject == nullptr)
    {
        newObject = new GameObject;
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
	// currently the resources will just not be deleted (they are stored in ResourceManager)
    for (GameObject* object : gameObjects)
        delete object;

    gameObjects.clear();
}

void cs::Scene::QueueExit()
{
	SceneManager::Unload(this);
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

void cs::Scene::ImGuiDrawGameObjects()
{
    if (ImGui::TreeNode(name.c_str()))
    {
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
}

void cs::Scene::UpdateComponents()
{
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
