# China Speedrun Engine

* This readme file is a work in progress.<br>

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
GLFW<br>
GLM<br>
The stb collection<br>
imgui<br>
OpenAL<br>
TinyOBJLoader<br>
Box2D<br>
entt<br>
gifdec<br>
ImGuizmo<br>
<br>
(Links to their github pages will be added soon)
