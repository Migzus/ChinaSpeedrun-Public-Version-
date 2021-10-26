#pragma once

namespace cs
{
	class Component
	{
	public:
		class GameObject* gameObject{ nullptr };
		bool enabled{ true };
		
		virtual void ImGuiDrawComponent() = 0;
	};
}
