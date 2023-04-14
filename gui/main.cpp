#include <gui.hpp>
#include <windows.h>
#include <Psapi.h>

int main()
{
	//sf::VideoMode mode({ 100, 100 }, 32);
	sf::RenderWindow win(sf::VideoMode::getFullscreenModes()[0], "title", sf::Style::Default);
	//win.setFramerateLimit(1u);

	sf::Event event;

	WindowState state(win);
	set_state(state);

	state.fonts.resize(5);
	
	state.fonts[1].loadFromFile("C:\\Windows\\Fonts\\brushsci.ttf");

	sf::Clock fps_clock;

	//s.set_size({ 1000.f, 20.f });

	Text fps, mem_text1, mem_text2, objects_text;

	mem_text2.stick(mem_text1, Bounds::TOP_LEFT, Bounds::BOTTOM_LEFT);
	objects_text.stick(mem_text2, Bounds::TOP_LEFT, Bounds::BOTTOM_LEFT);

	ObjVec test;
	fps.set_position_by_bounds(state.get_window_bounds(Bounds::TOP_RIGHT), Bounds::TOP_RIGHT);

	// todo: scrollbar, tree/dropdown elements, get events better, color pickers, cursor change, basic file menu, realtime gui change and save gui state, graphs, allow multiple windows and states

	// Now I'm gonna change TextInput. Which means changing sf::Text. I will not use external libraries, but may copy code if they give me what I need. I need:
	// 1. Letters that can each have separate properties such as font, fill color, outline color, and character size
	// 2. The ability to provide the number of and dimensions of all "lines"
	// 3. The ability to accept different escape sequences than just \n and \t. Formatting is not necessary since the stringstream handles that.
	// 
	// Due to the way sfml draws things, I can't use a different font size for each letter. However, I can individualize everything that addGlyph uses, such as, well, just color for now
	// Since I'm about to go insane, I will congratulate myself on my current achievements. This is pretty well built and organized, and it's very usable in its current state. It's at the point where I no longer have to care about the system and can simply add what I need to it. The cpp file currently has 1872 lines of code and every bit of it was thought long and hard about. I will definitely be better than ImGui which only gives you the bare minimum jack-of-all-trades crap.

	// I broke TextInput somehow, but I was gonna change it anyways

	// Formatting floats without rounding sometimes causes -0 to appear in slider val
	// Hovering over slider knob gains user focus before first click

	// Potential issue: Objects retain user focus after being hidden (fixed)

	// I'm gonna change set_size() to work with ratios of the given videomode by default. 

	set_vector(test, fps);
	//state.hide(test);
	//state.show(test);
	unsigned int fps_counter = 0, fps_average = 1;
	float fps_decay = 0.9f, fps_interval_ms = 50.f;

	//int arr[2048];

	PROCESS_MEMORY_COUNTERS mem_counter;

	std::vector<Button> buttons;

	while (win.isOpen())
	{
		while (win.pollEvent(event))
		{
			state.get_events(event);

			switch (event.type)
			{
			case sf::Event::KeyPressed:
				break;
			case sf::Event::Closed:
				win.close();
				break;
			}
		}
		state.get_state();

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
		{
			buttons.push_back(Button(state.mouse_coord_position));
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Backspace))
		{
			if (buttons.size() > 0)
				buttons.clear();
		}

		

		BOOL result = K32GetProcessMemoryInfo(GetCurrentProcess(), &mem_counter, sizeof(mem_counter));
		mem_text1 << "Total memory: " << mem_counter.PagefileUsage / 1000000.f << "MB";
		mem_text2 << "Working set: " << mem_counter.WorkingSetSize / 1000000.f << "MB";
		objects_text << "state.objects size: " << state.objects.size();

		win.clear({ 0, 0, 0, 255 });

		state.draw_objects(test);


		win.display();

		if (fps_clock.getElapsedTime().asMilliseconds() >= fps_interval_ms)
		{	
			fps_average = fps_decay * fps_average + (1.f - fps_decay) * (fps_counter * (1000.f / fps_interval_ms));
			fps << fps_average;
			fps_clock.restart();


			fps_counter = 0;
		}
		fps_counter++;

	}
}

