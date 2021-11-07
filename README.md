# A Public Version of China Speedrun Engine

* This readme file is a work in progress. The project is also a mess...<br>

The China Speedrun Engine is a bullethell engine, designed to make games in the shmup genre.<br>
It's lightweight, fast and have a great bullet capacity.
Foreas, it will use GPU collisions for the bullets. (Potentially, if we have time to implement it)<br>
It's component based.<br>
The bullets are also verse in its features.

<br><br>
Just to quickly sum up what one has to do in order to compile-and-run this code, you need the following:


Vulkan 1.2.182.0 (Or later)<br>
Visual Studio 2019 (we used this, but I am certain you can use 2017 as well)

Once downloaded you MUST go into the Resource/shaders directory; run a file named compile.bat (if Linux compile.sh)
If you don't run this file; when running the compiled progarm, it will crash.

Libraries used:<br>
[GLFW](https://github.com/glfw/glfw)<br>
[GLM](https://github.com/g-truc/glm)<br>
[The stb collection](https://github.com/nothings/stb)<br>
[imgui](https://github.com/ocornut/imgui)<br>
[OpenAL](https://www.openal.org/)<br>
[TinyOBJLoader](https://github.com/tinyobjloader/tinyobjloader)<br>
[Box2D](https://github.com/erincatto/box2d)<br>
[entt](https://github.com/skypjack/entt)<br>
[gifdec](https://github.com/lecram/gifdec)<br>
[ImGuizmo](https://github.com/CedricGuillemet/ImGuizmo)<br>
