#include <gui.hpp>



int main()
{
	sf::VideoMode mode({ 2560, 1440 }, 32);
	sf::RenderWindow win(mode, "title", sf::Style::Default);
	win.setFramerateLimit(144u);

	sf::Event event;

	WindowState state(win);
	set_state(state);

	Button butt;
	Button butt2;
	Button butt3;
	butt2.set_color(sf::Color::Green);
	butt3.set_color(sf::Color::Cyan);

	butt2.bind(butt);
	butt3.bind(butt2);

	butt2.set_position_by_bounds(butt.get_bounds(Bounds::RIGHT), Bounds::LEFT);
	butt3.set_position_by_bounds(butt2.get_bounds(Bounds::RIGHT), Bounds::LEFT);
	
	Text hi;
	hi.set_position_by_bounds(get_window_bounds(Bounds::TOP_RIGHT), Bounds::TOP_RIGHT);

	ObjVec hivec;
	hi.set_vector(hivec);

	//TextInput in({ 0.f, 0.f }, { 50.f, 70.f });
	//in.set_position_by_bounds(get_window_bounds(Bounds::CENTER), Bounds::CENTER);

	sf::View view({ 0.f, 0.f }, get_window_bounds(Bounds::BOTTOM_RIGHT) / 2.f);
	view.setCenter(view.getSize() / 2.f);
	sf::FloatRect stupid({ 0.f, 0.f }, { 0.5f, 0.5f });
	view.setViewport(stupid);


	//view.setViewport(sf::FloatRect( 0, 0, 1, 0.1 ));
	// todo: scrollbar, get events better, sliders, color pickers, cursor change, basic file menu, realtime gui change and save gui state, graphs

	// Menus can be made using a defined width and height, then taking in a bunch of strings that can be used as identifiers for the given textbutton. Although menus may require views for scrolling, the view should not extend outside the bounding box so it should be fine for now

	while (win.isOpen())
	{
		while (win.pollEvent(event))
		{
			state.get_events(event);

			switch (event.type)
			{
			case sf::Event::Closed:
				win.close();
				break;
			}
		}
		state.get_state();
		
		
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
			view.move({ 0.f, -1.f });
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
			view.move({ 0.f, 1.f });
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
			view.move({ -1.f, 0.f });
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
			view.move({ 1.f, 0.f });
		
		//if (in.focus_toggled)
		//{
		//	if (in.has_user_focus)
		//		in.set_color_impl(sf::Color::Green);
		//	if (!in.has_user_focus)
		//		in.set_color_impl(sf::Color::Red);
		//}

		butt2.unbind();
		if (butt.on)
			butt.set_color(sf::Color::Yellow);
		else
			butt.set_color(sf::Color::Red);
		butt2.bind(butt);
		
		win.clear({ 0, 0, 0, 255 });
		win.setView(view);
		state.draw_objects();
		win.setView(win.getDefaultView());
		state.draw_objects(hivec);

		
		win.display();
	}
}
