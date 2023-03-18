#include <gui.hpp>

int main()
{
	//sf::VideoMode mode({ 100, 100 }, 32);
	sf::RenderWindow win(sf::VideoMode::getFullscreenModes()[0], "title", sf::Style::Default);
	//win.setFramerateLimit(1u);

	sf::Event event;

	WindowState state(win);
	set_state(state);

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
	

	//s.set_size({ 1000.f, 20.f });

	Text fps, t, mouse, cpos;
	TextButton al, ac, ar;
	al.text << "Left";
	ac.text << "Center";
	ar.text << "Right";
	al.button.stick(mouse, Bounds::TOP, Bounds::BOTTOM);
	ac.button.stick(al.button, Bounds::LEFT, Bounds::RIGHT);
	ar.button.stick(ac.button, Bounds::LEFT, Bounds::RIGHT);

	ObjVec test;
	t.set_position_by_bounds(get_window_bounds(Bounds::TOP), Bounds::TOP);
	fps.set_position_by_bounds(get_window_bounds(Bounds::TOP_RIGHT), Bounds::TOP_RIGHT);
	cpos.set_position_by_bounds(get_window_bounds(Bounds::TOP), Bounds::TOP);

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
		//std::cout << t.get_bounds(Bounds::BOTTOM_RIGHT).x << '\n';
		
		mouse << state.mouse_coord_position.x;

		if (al.button.activated)
			alignment = sf::Align::LEFT;
		if (ac.button.activated)
			alignment = sf::Align::CENTER;
		if (ar.button.activated)
			alignment = sf::Align::RIGHT;

		t.text.align(alignment, s.val);
		t.set_size(s2.val);

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
