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
	float fps_decay = 0.7f, fps_interval_ms = 50.f;

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
				buttons.resize(1);
		}

		BOOL result = K32GetProcessMemoryInfo(GetCurrentProcess(), &mem_counter, sizeof(mem_counter));
		mem_text1 << "Total memory: " << mem_counter.PagefileUsage / 1000000 << "MB";
		mem_text2 << "Working set: " << mem_counter.WorkingSetSize / 1000000 << "MB";
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

//int main()
//{
//	// Init
//	sf::RenderWindow win(sf::VideoMode::getFullscreenModes()[0], "title", sf::Style::Default);
//	win.setFramerateLimit(60u);
//	sf::Event event;
//	WindowState state(win);
//	set_state(state);
//
//	// Colors
//	sf::Color grey = sf::Color(0x7f, 0x7f, 0x7f, 255);
//	sf::Color green = sf::Color::Green;
//	sf::Color orange = sf::Color(255, 200, 0);
//	sf::Color blue = sf::Color::Blue;
//	sf::Color magenta = sf::Color::Magenta;
//
//	sf::Color c12[3] =
//	{
//		grey,
//		green,
//		orange };
//	sf::Color c3[3] =
//	{
//		blue,
//		green,
//		orange };
//	sf::Color c4[3] = 
//	{
//		magenta,
//		green,
//		orange };
//
//	// Make the 4 pages of 4x9 textbuttons (4 objvecs and show 1 at a time)
//
//	Menu m1(4, 9);
//	m1.set_padding({ 5.f, 5.f });
//	m1.set_size({ 200.f, 100.f });
//	m1.set_position_by_bounds(get_window_bounds(Bounds::CENTER), Bounds::CENTER);
//
//	Menu m2(4, 9);
//	m2.set_padding({ 5.f, 5.f });
//	m2.set_size({ 200.f, 100.f });
//	m2.set_position_by_bounds(get_window_bounds(Bounds::CENTER), Bounds::CENTER);
//
//	Menu m3(4, 9);
//	m3.set_padding({ 5.f, 5.f });
//	m3.set_size({ 200.f, 100.f });
//	m3.set_position_by_bounds(get_window_bounds(Bounds::CENTER), Bounds::CENTER);
//
//	Menu m4(4, 9);
//	m4.set_padding({ 5.f, 5.f });
//	m4.set_size({ 200.f, 100.f });
//	m4.set_position_by_bounds(get_window_bounds(Bounds::CENTER), Bounds::CENTER);
//
//
//
//	TextButton page1, page2, page3, page4;
//	page1.set_size({ 200.f, 100.f });
//	page1.button.set_color(sf::Color::Cyan);
//	page1.button.set_position_by_bounds(m1.get_bounds(Bounds::BOTTOM_LEFT) + sf::Vector2f(0.f, 100.f), Bounds::TOP_LEFT);
//	page1.text << "Page 1";
//
//	page2.button.set_padding({ 5.f, 5.f });
//	page2.set_size({ 200.f, 100.f });
//	page2.button.set_color(sf::Color::Cyan);
//	page2.button.set_position_by_bounds(page1.button.get_bounds(Bounds::RIGHT), Bounds::LEFT);
//	page2.text << "Page 2";
//
//	page3.button.set_padding({ 5.f, 5.f });
//	page3.set_size({ 200.f, 100.f });
//	page3.button.set_color(sf::Color::Cyan);
//	page3.button.set_position_by_bounds(page2.button.get_bounds(Bounds::RIGHT), Bounds::LEFT);
//	page3.text << "Page 3";
//
//	page4.button.set_padding({ 5.f, 5.f });
//	page4.set_size({ 200.f, 100.f });
//	page4.button.set_color(sf::Color::Cyan);
//	page4.button.set_position_by_bounds(page3.button.get_bounds(Bounds::RIGHT), Bounds::LEFT);
//	page4.text << "Page 4";
//
//	//b.set_position_by_bounds(get_window_bounds(Bounds::TOP), Bounds::TOP);
//
//	// Loops
//	state.hide(m1);
//	state.hide(m2);
//	state.hide(m3);
//	state.hide(m4);
//	for (int i = 0; i < m1.buttons.size(); i++)
//	{
//		auto& b = m1.buttons[i];
//		state.hide(b.button);
//		state.hide(b.text);
//		b.text << i;
//	}
//	for (int i = 0; i < m2.buttons.size(); i++)
//	{
//		auto& b = m2.buttons[i];
//		state.hide(b.button);
//		state.hide(b.text);
//		b.text << i;
//	}
//	for (int i = 0; i < m3.buttons.size(); i++)
//	{
//		auto& b = m3.buttons[i];
//		state.hide(b.button);
//		state.hide(b.text);
//		b.text << i;
//	}
//	for (int i = 0; i < m4.buttons.size(); i++)
//	{
//		auto& b = m4.buttons[i];
//		state.hide(b.button);
//		state.hide(b.text);
//		b.text << i;
//	}
//
//	for (int i = 0; i < m1.buttons.size(); i++)
//	{
//		TextButton& button = m1.buttons[i];
//		button.button.set_color(grey);
//	}
//	for (int i = 0; i < m2.buttons.size(); i++)
//	{
//		TextButton& button = m2.buttons[i];
//		button.button.set_color(grey);
//	}
//	for (int i = 0; i < m3.buttons.size(); i++)
//	{
//		TextButton& button = m3.buttons[i];
//		button.button.set_color(blue);
//	}
//	for (int i = 0; i < m4.buttons.size(); i++)
//	{
//		TextButton& button = m4.buttons[i];
//		button.button.set_color(magenta);
//	}
//
//			
//			
//	int page_active = 2;
//	Menu* current_menu = &m1;
//	Text toggle;
//
//	TextButton reset;
//	reset.button.set_position_by_bounds(get_window_bounds(Bounds::TOP), Bounds::TOP);
//	reset.text << "Reset";
//
//	toggle.keep_text = 0;
//
//	while (win.isOpen())
//	{
//		while (win.pollEvent(event))
//		{
//			state.get_events(event);
//
//			switch (event.type)
//			{
//			case sf::Event::Closed:
//				win.close();
//				break;
//			}
//		}
//
//		state.get_state();
//
//		
//		if (page1.button.activated)
//			page_active = 1;
//		if (page2.button.activated)
//			page_active = 2;
//		if (page3.button.activated)
//			page_active = 3;
//		if (page4.button.activated)
//			page_active = 4;
//
//		//page1.text << page_active;
//
//		switch (page_active)
//		{
//		case 1:
//			for (auto& b : current_menu->buttons)
//			{
//				state.hide(b.button);
//				state.hide(b.text);
//			}
//			for (auto& b : m1.buttons)
//			{
//				state.show(b.button);
//				state.show(b.text);
//			}
//
//			current_menu = &m1;
//			break;
//		case 2:
//			for (auto& b : current_menu->buttons)
//			{
//				state.hide(b.button);
//				state.hide(b.text);
//			}
//			for (auto& b : m2.buttons)
//			{
//				state.show(b.button);
//				state.show(b.text);
//			}
//
//			current_menu = &m2;
//			break;
//		case 3:
//			for (auto& b : current_menu->buttons)
//			{
//				state.hide(b.button);
//				state.hide(b.text);
//			}
//			for (auto& b : m3.buttons)
//			{
//				state.show(b.button);
//				state.show(b.text);
//			}
//
//			current_menu = &m3;
//			break;
//		case 4:
//			for (auto& b : current_menu->buttons)
//			{
//				state.hide(b.button);
//				state.hide(b.text);
//			}
//			for (auto& b : m4.buttons)
//			{
//				state.show(b.button);
//				state.show(b.text);
//			}
//
//			current_menu = &m4;
//			break;
//		}
//
//		// Color cycle
//		if (current_menu == &m1)
//			toggle << "1";
//		if (current_menu == &m2)
//			toggle << "2";
//		if (current_menu == &m3)
//			toggle << "3";
//		if (current_menu == &m4)
//			toggle << "4";
//
//
//		if (reset.button.activated)
//		{
//			for (int i = 0; i < current_menu->buttons.size(); i++)
//			{
//				TextButton& button = current_menu->buttons[i];
//				switch (page_active)
//				{
//				case 1:
//				case 2:
//					button.button.set_color(grey);
//					break;
//				case 3:
//					button.button.set_color(blue);
//					break;
//				case 4:
//					button.button.set_color(magenta);
//					break;
//				}
//			}
//		}
//		for (int i = 0; i < current_menu->buttons.size(); i++)
//		{
//			TextButton& button = current_menu->buttons[i];
//			if (button.button.activated)
//			{
//				switch (page_active)
//				{
//				case 1:
//				case 2:
//					if (button.button.rect.getFillColor() == grey)
//						button.button.set_color(green);
//					else if (button.button.rect.getFillColor() == green)
//						button.button.set_color(orange);
//					else if (button.button.rect.getFillColor() == orange)
//						button.button.set_color(grey);
//					break;
//				case 3:
//					if (button.button.rect.getFillColor() == blue)
//						button.button.set_color(green);
//					else if (button.button.rect.getFillColor() == green)
//						button.button.set_color(orange);
//					else if (button.button.rect.getFillColor() == orange)
//						button.button.set_color(blue);
//					break;
//				case 4:
//					if (button.button.rect.getFillColor() == magenta)
//						button.button.set_color(green);
//					else if (button.button.rect.getFillColor() == green)
//						button.button.set_color(orange);
//					else if (button.button.rect.getFillColor() == orange)
//						button.button.set_color(magenta);
//					break;
//				}
//			}
//		}
//		//else
//		//	button.text << "deactivated";
//
//		win.clear();
//
//		state.draw_objects(state.objects);
//
//		win.display();
//	}
//}