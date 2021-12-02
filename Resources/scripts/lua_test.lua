max_speed = 1.0
slow_speed = 0.3

local c = true

-- Called when the program starts
function start()
	print("Start Script")
	get_component("transform")
end

-- Called whenever a key was updated
function input(event)
	--print(event.keycode, " | ", event.scancode, " | ", event.action, " | ", event.mods)
	
	if (event.keycode == KEY_LEFT_ARROW) then
		print("we got here!")
	end
end

-- Updates every frame
function update(deltaTime)
	
end

-- Called once the program / scene ends
function exit()
	print("Exit script")
end

-- Called only through a Physics Object Component and a collision enters
function on_collision_enter()
	print("Collision Enter")
end

-- Called only through a Physics Object Component and a collision exits
function on_collision_exit()
	print("Collision Exit")
end
