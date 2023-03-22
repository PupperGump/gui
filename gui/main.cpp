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

	//TextInput in({ 1000.f, 100.f }, { 200.f, 500.f });
	//in.set_size({ 400.f, 500.f });
	//in.line_limit = 3;
	Slider s({ 0.f, 0.f }, { 1000.f, 20.f }, 0.f, 1000.f);
	s.precision = 1;
	s.set_position_by_bounds(get_window_bounds(Bounds::CENTER), Bounds::CENTER);
	s.set_color(sf::Color::Green, 0);
	s.knob.set_color(sf::Color::Yellow);
	Slider s2({ 0.f, 0.f }, { 1000.f, 20.f }, 0.f, 1000.f);
	s2.precision = 0;
	s2.set_color(sf::Color::Blue, 0);
	s2.knob.set_color(sf::Color::Green);
	s2.set_position_by_bounds(s.get_bounds(Bounds::BOTTOM), Bounds::TOP, 1.f, 15.f);
	Slider s3({ 0.f, 0.f }, { 1000.f, 20.f }, 0.f, 1000.f);
	s3.precision = 0;
	s3.set_color(sf::Color::Red, 0);
	s3.knob.set_color(sf::Color::Cyan);
	s3.set_position_by_bounds(s2.get_bounds(Bounds::BOTTOM), Bounds::TOP, 1.f, 15.f);

	//s.set_size({ 1000.f, 20.f });

	Text fps, t, mem_text, t2, t3, t4, t5;
	TextButton al, ac, ar;
	al.text << "Left";
	ac.text << "Center";
	ar.text << "Right";
	al.button.stick(mem_text, Bounds::TOP, Bounds::BOTTOM);
	ac.button.stick(al.button, Bounds::LEFT, Bounds::RIGHT);
	ar.button.stick(ac.button, Bounds::LEFT, Bounds::RIGHT);

	ObjVec test;
	t.set_position_by_bounds(get_window_bounds(Bounds::TOP), Bounds::TOP);
	t2.set_position_by_bounds(t.get_bounds(Bounds::BOTTOM), Bounds::TOP);
	t3.set_position_by_bounds(t2.get_bounds(Bounds::BOTTOM), Bounds::TOP);
	t4.set_position_by_bounds(t3.get_bounds(Bounds::BOTTOM), Bounds::TOP);
	t5.set_position_by_bounds(t4.get_bounds(Bounds::BOTTOM), Bounds::TOP);
	fps.set_position_by_bounds(get_window_bounds(Bounds::TOP_RIGHT), Bounds::TOP_RIGHT);

	t2.text.setFont(state.fonts[1]);

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

	set_vector(test, s, s2, fps);
	//state.hide(test);
	//state.show(test);
	unsigned int fps_counter = 0, fps_average = 1;
	float fps_decay = 0.7f, fps_interval_ms = 50.f;

	t << "It's a beautiful day outside, with the sun shining brightly and the birds chirping happily. I can hear the gentle rustling of leaves in the trees and feel the soft breeze on my skin. However, my stomach is growling loudly, so I think it's time to grab a bite to eat! I'm craving a juicy burger with all the fixings - lettuce, tomato, pickles, cheese, and a generous dollop of ketchup. Maybe I'll even treat myself to some crispy french fries and a refreshing soda. Yum!";
	RectField tbox;
	tbox.set_color({ 0, 0, 0, 0 });
	tbox.rect.setOutlineColor(sf::Color::Yellow);
	tbox.rect.setOutlineThickness(2.f);

	
	t.text.props[0].fill_color = sf::Color::Red;
	t.text.props[1].fill_color = sf::Color::Green;
	t.text.props[2].fill_color = sf::Color::Blue;

	//t.set_size(50);

	sf::Align alignment = sf::Align::LEFT;

	PROCESS_MEMORY_COUNTERS mem_counter;


	unsigned short fontsize = 0;
	while (win.isOpen())
	{
		while (win.pollEvent(event))
		{
			state.get_events(event);

			switch (event.type)
			{
			case sf::Event::KeyPressed:
				//if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
				//{
				//	switch (event.key.code)
				//	{
				//	case sf::Keyboard::R:
				//		in.set_alignment(Align::RIGHT);
				//		break;
				//	case sf::Keyboard::L:
				//		in.set_alignment(Align::LEFT);
				//		break;
				//	case sf::Keyboard::C:
				//		in.set_alignment(Align::CENTER);
				//		break;
				//	}
				//}
				break;
			case sf::Event::Closed:
				win.close();
				break;
			}
		}
		state.get_state();

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
			t.set_position(t.get_position() + sf::Vector2f(0.f, -1.f));
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
			t.set_position(t.get_position() + sf::Vector2f(0.f, 1.f));
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
			t.set_position(t.get_position() + sf::Vector2f(-1.f, 0.f));
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
			t.set_position(t.get_position() + sf::Vector2f(1.f, 0.f));

		tbox.stick(t);
		tbox.set_size(t.get_bounds(Bounds::BOTTOM_RIGHT) - t.get_position());
		
		BOOL result = K32GetProcessMemoryInfo(GetCurrentProcess(), &mem_counter, sizeof(mem_counter));
		mem_text << mem_counter.WorkingSetSize;

		if (al.button.activated)
			alignment = sf::Align::LEFT;
		if (ac.button.activated)
			alignment = sf::Align::CENTER;
		if (ar.button.activated)
			alignment = sf::Align::RIGHT;

		t.text.align(alignment, s.val);
		t.set_size(s2.val);
		fps.set_size(s3.val);
		
		t2.set_size(fontsize++);

		if (fontsize > 400)
			fontsize = 0;

		win.clear({ 0, 0, 0, 255 });

		//t << state.keyboard_input;
		

		//state.update(s.vec);
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