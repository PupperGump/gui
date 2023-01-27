#include <gui.hpp>



int main()
{
	sf::VideoMode mode({ 2560, 1440 }, 32);
	sf::RenderWindow win(mode, "title", sf::Style::Default);
	win.setFramerateLimit(144u);

	sf::Event event;

	WindowState state(win);
	set_state(state);
	RectView rv(get_window_bounds(Bounds::TOP_LEFT), get_window_bounds(Bounds::CENTER), get_window_bounds(Bounds::BOTTOM_RIGHT));

	TextInput hi;
	hi.set_color(sf::Color::Black);
	//hi.limit_lines_to_rect = 0;
	hi.bind(rv);
	//hi.set_alignment(Align::RIGHT);

	hi.set_position_by_bounds(rv.get_bounds(Bounds::TOP_LEFT), Bounds::TOP_LEFT);

	//view.setViewport(sf::FloatRect( 0, 0, 1, 0.1 ));
	// todo: scrollbar, get events better, sliders, color pickers, cursor change, basic file menu, realtime gui change and save gui state, graphs

	// Menus can be made using a defined width and height, then taking in a bunch of strings that can be used as identifiers for the given textbutton. Although menus may require views for scrolling, the view should not extend outside the bounding box so it should be fine for now
	
	// Now that I've found a way to apply views to RectFields, I can start making scroll bars. First I'll fix the TextInput, then I'll practice by making sliders. Then the scrollbars can take in scroll wheel input with a customizable step size (minimum 1 pixel) and of course both vertical and horizontal scrollbars will be optional for every object using a view.

	// So for the TextInput I'll make a master string that will refresh the text objects every time it's modified. It sounds cpu intensive, but it's better than limiting line wrapping to only typing and backspacing, preventing any arrow key action. 

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
			rv.move_view({ 0.f, -1.f });
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
			rv.move_view({ 0.f, 1.f });
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
			rv.move_view({ -1.f, 0.f });
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
			rv.move_view({ 1.f, 0.f });

		win.clear({ 0, 0, 0, 255 });
		state.draw_objects();

		
		win.display();
	}
}
