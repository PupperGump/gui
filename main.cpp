#include <gui.hpp>



int main()
{
	sf::VideoMode mode({ 2560, 1440 }, 32);
	sf::RenderWindow win(mode, "title", sf::Style::Default);
	win.setFramerateLimit(144u);

	sf::Event event;

	WindowState state(win);
	set_state(state);

	TextInput in({ 0.f, 0.f }, { 50.f, 70.f });

	in.set_position_by_bounds(get_window_bounds(Bounds::CENTER), Bounds::CENTER);


	// todo: menu creation, scrollbar, text input, return self for chaining maybe, get events better, sliders, color pickers, cursor change, basic file menu, realtime gui change and save gui state, graphs

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
		
		
		if (in.focus_toggled)
		{
			if (in.has_user_focus)
				in.set_color_impl(sf::Color::Green);
			if (!in.has_user_focus)
				in.set_color_impl(sf::Color::Red);
		}

	


		win.clear({ 0, 0, 0, 255 });
		state.draw_objects();
		
		
		
		win.display();
	}
}
