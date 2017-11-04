<h3><strong>Warning:</strong> This project has been deprecated thanks to an architectural overhaul, a newer engine project is currently in the works. Check back at <a href="https://www.mauriceheijligers.com/portfolio/hadron-game-engine/" target="_blank">my portfolio</a> in a couple weeks/months for updates.</h3>
<br />
<br />
This is an educational project!<br />
<br />
<br />
CMake Build seems to break on Assimp's zLib library, I'm currently working on a fix.
<br />
My goal is to have a functional game engine that supports DX12, Vulkan, Is crossplatform, has a multithreaded entity component system, has physics handled my Bullet or PhysX and audio through WWise. I'm looking to implement clustered forward rendering, possibly a raytracer and if time permits I will develop a clone of this engine using my univisity's facilities to port the engine to PS4 as well.
<br />
suggestions, questions and feedback are welcome.
<br />
<br />
This project requires at least Visual Studio 2015. Use CMake with Visual Studio 14 2015 Win64 or Higher. Win64 is necessary.<br />
<br />
My next goal is to overhaul my graphics pipeline since that has not seen any love and still only renders 1 object instead of the scene.<br />
<br />
Structure:
/<br />
--- bin/<br />
--- build/<br />
--- lib/<br />
--- inc/<br />
--- src/<br />
------ lcn_core/<br />
--------- inc/<br />
--------- src/<br />
------ lcn_game/<br />
------ vk_renderer/<br />
------ windowing/<br />
--- thirdparty/<br />
<br />
