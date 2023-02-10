#include <gui.hpp>

int main()
{
	//sf::VideoMode mode({ 2560, 1440 }, 32);
	sf::RenderWindow win(sf::VideoMode::getFullscreenModes()[0], "title", sf::Style::Default);
	//win.setFramerateLimit(144u);

	sf::Event event;

	WindowState state(win);
	set_state(state);

	sf::Clock fps_clock;

	TextInput in({ 1000.f, 100.f }, { 200.f, 500.f });
	//in.set_size({ 400.f, 500.f });
	//in.line_limit = 3;
	Slider s({ 0.f, 0.f }, { 1000.f, 20.f });
	s.precision = 1;
	s.set_position_by_bounds(get_window_bounds(Bounds::CENTER), Bounds::CENTER);
	s.set_color(sf::Color::Green, 0);
	s.knob.set_color(sf::Color::Yellow);
	//s.set_size({ 1000.f, 20.f });

	Text fps, vec_size;
	ObjVec test;
	vec_size.set_position_by_bounds(get_window_bounds(Bounds::TOP), Bounds::TOP);
	fps.set_position_by_bounds(get_window_bounds(Bounds::TOP_RIGHT), Bounds::TOP_RIGHT);

	// Right aligned text tends to break and overlap. It appears to be taking the text below it and moving it up.

	// todo: scrollbar, tree/dropdown elements, get events better, color pickers, cursor change, basic file menu, realtime gui change and save gui state, graphs

	// One probably annoying issue is that for each ObjVec, the items inside will be updated and drawn with their specific call. This can lead to unexpected situations such as something that is being drawn first also being updated first, catching focus in the wrong order.
	// Solution 1: Create a queue for updating and use draw_objects() only for drawing
	// Solution 2: With each creating of an ObjVec, we also know how many draw calls there will be.
	set_vector(test, s, fps);
	//state.hide(test);
	//state.show(test);
	unsigned int fps_counter = 0;
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
			s.set_position(s.get_position() + sf::Vector2f(0.f, -0.1f));
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
			s.set_position(s.get_position() + sf::Vector2f(0.f, 0.1f));
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
			s.set_position(s.get_position() + sf::Vector2f(-0.1f, 0.f));
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
			s.set_position(s.get_position() + sf::Vector2f(0.1f, 0.f));

		win.clear({ 0, 0, 0, 255 });
		state.draw_objects(test);
		state.draw_objects();
		
		
		win.display();

		if (fps_clock.getElapsedTime().asMilliseconds() >= 100.f)
		{
			fps << fps_counter * 10.f;
			fps_clock.restart();
			fps_counter = 0;
		}
		fps_counter++;


	}
}
