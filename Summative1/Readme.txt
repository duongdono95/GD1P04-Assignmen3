GD1P04 - OPENGL Technical Demo 2
Author: Duong Bach

1. Description:
	- This project is an OpenGL-based application designed to demonstrate the interaction within a 3D environment using GLFW and GLEW. It features basic scene manipulation where users can interact with different objects in the scene through mouse inputs. The scene includes a textured quad that changes texture on hover and can toggle a repeated texture pattern on click.

2. Features:
	- Dynamic Camera Controls: Navigate through the scene with keyboard inputs to explore different viewpoints.
	- Texture Interaction: Hover over the quad to change its texture. Click to toggle a repeated texture pattern.
	- Real-time Rendering: The application renders a statue and palm trees, demonstrating instanced rendering and basic texture management.

3. Key Functions:
	- InitialSetup(): Handles the initial setup of the shaders, textures, and models.
	- Update(float deltaTime): Manages the updates needed per frame, including input handling and scene updates.
	- Render(): Responsible for drawing the objects in the scene.
	- mouse_button_callback(GLFWwindow* window, int button, int action, int mods): Handles mouse button interactions, specifically for toggling texture repetition on the quad.

4. Installation and Setup:
	- Prerequisites:
		+ GLEW
		+ GLFW
		+ GLM
		+ STB Image Library
	- Running the Application
		+ Ensure all dependencies are installed and properly configured in your development environment.
		+ Compile the code using a suitable C++ compiler and link against the above libraries.
		+ Run the executable to launch the application.

5. Controls:
	- W/A/S/D/Q/E: Move the camera around the scene.
	- Shift: Push up camera movement speed (both Manual/Auto Camera Movement).
	- Space: Toggle Auto/ Manual Camera Movement:
		+ Click: Toggle On/Off Manual Camera Movement.
		+ Press: Control Manual Camera Movement.
	- Mouse Movement: Navigate the camera's view direction.
	- Left Mouse Button: Click on the quad to toggle the texture repetition effect.
	- Z: Toggle cursor visibility.
	- X: Toggle wireframe mode.
	- C: Output current cursor coordinates to the console.

6. Project Structure:
	- main.cpp: Contains the main application logic, rendering loop, and input handling.
	- ShaderLoader.cpp/h: Utilities to load and compile shaders from files.
	- MeshModel.cpp/h: Handles loading and rendering of 3D models.
	- CCamera.cpp/h: Manages the camera movement and perspective projection.

7. Future Enhancements:
	- Adding more interactive elements to the scene.
	- Implementing additional effects like lighting and shadows.
	- Enhancing the user interface for better interaction handling.
	- For further information on how to expand or modify this project, consult the source files and accompanying documentation in the codebase.