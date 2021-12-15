max_speed = 1.0
slow_speed = 0.3

local transform
local velocity = { 0.0, 0.0 }
local currentSpeed = 0.0

-- Called when the program starts
function start()
	print("Start Script")
	--transform = get_component("transform")
	currentSpeed = max_speed
end

-- Called whenever a key was updated
function input(event)
	print(event.keycode, " | ", event.scancode, " | ", event.action, " | ", event.mods)
	
	--[[if (event.keycode == KEY_SHIFT && event.action == ACTION_DOWN) then
		currentSpeed = slow_speed
	else if (event.keycode == KEY_SHIFT && event.action == ACTION_UP) then
		currentSpeed = max_speed
	end

	--Left action
	if (event.keycode == KEY_LEFT_ARROW && event.action == ACTION_DOWN) then
		velocity.x = -1.0
	else if (event.keycode == KEY_LEFT_ARROW && event.action == ACTION_UP) then
		velocity.x = 0.0
	end

	--Right action
	if (event.keycode == KEY_RIGHT_ARROW && event.action == ACTION_DOWN) then
		velocity.x = 1.0
	else if (event.keycode == KEY_RIGHT_ARROW && event.action == ACTION_UP) then
		velocity.x = 0.0
	end

	--Up action
	if (event.keycode == KEY_UP_ARROW && event.action == ACTION_DOWN) then
		velocity.y = 1.0
	else if (event.keycode == KEY_UP_ARROW && event.action == ACTION_UP) then
		velocity.y = 0.0
	end

	--Down action
	if (event.keycode == KEY_DOWN_ARROW && event.action == ACTION_DOWN) then
		velocity.y = -1.0
	else if (event.keycode == KEY_DOWN_ARROW && event.action == ACTION_UP) then
		velocity.y = 0.0
	end]]--
end

-- Updates every frame
function update(deltaTime)
	--transform.position += velocity * currentSpeed * deltaTime
end

-- Called once the program / scene ends
function exit()
	print("Exit script")
end

-- Called only through a Physics Object Component and a collision enters
function on_collision_enter()
	print("Collision Enter")
	kill_player()
end

-- Called only through a Physics Object Component and a collision exits
function on_collision_exit()
	print("Collision Exit")
end

function kill_player()
	--queue_free()
end
