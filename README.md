# China Speedrun Engine

The China Speedrun Engine is a bullethell engine, designed to make games in the shmup genre.<br>
It's lightweight, fast and have a great bullet capacity.
Foreas, it will use GPU collisions for the bullets.<br>
It's component based.<br>
The bullets are also verse in its features.


This readme file is a work in progress.<br>
Just to quickly sum up what one has to do in order to compile-and-run this code, you need the following:


Vulkan 1.2.182.0 (Or later) (If you use a newer version, you must update some of the paths in the project solution.)<br>
Visual Studio 2019 (we used this, but I am certain you can use 2017 as well)

Once downloaded you MUST go into the Resource/shaders directory; run a file named compile.bat (if Linux compile.sh)
If you don't run this file; when running the compiled progarm, it will crash.

Libraries used:<br>
GLFW<br>
GLM<br>
The stb collection<br>
