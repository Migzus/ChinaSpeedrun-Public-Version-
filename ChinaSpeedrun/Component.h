#pragma once

namespace cs
{
	class Component
	{
	public:
		bool enabled{ true };
		class GameObject* gameObject{ nullptr };
		
		virtual void ImGuiDrawComponent() = 0;
	};
}
