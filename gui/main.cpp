#include <gui.hpp>

int main()
{
	sf::VideoMode mode({ 2560, 1440 }, 32);
	sf::RenderWindow win(mode, "title", sf::Style::Default);
	//win.setFramerateLimit(144u);

	sf::Event event;

	WindowState state(win);
	set_state(state);
	Text fps;
	fps.set_position_by_bounds(get_window_bounds(Bounds::TOP_RIGHT), Bounds::TOP_RIGHT);
	sf::Clock fps_clock;

	//r.set_position_by_bounds(get_window_bounds(Bounds::CENTER), Bounds::CENTER);

	//t.move_vector(state.objects, 2);
	//t.move_vector(state.objects, c);
	//TextButton tb({ 0.f, 0.f }, { 70.f, 40.f });
	TextInput in({ 1000.f, 100.f }, { 500.f, 500.f });
	//in.ignore_focus = 1;
	//tb.button.set_position_by_bounds(get_window_bounds(Bounds::TOP_RIGHT), Bounds::TOP_RIGHT);
	Slider s({ 0.f, 0.f }, { 2000.f, 10.f }, -1000.f, 1000.f);
	//s.max = 1;
	s.set_position_by_bounds(get_window_bounds(Bounds::CENTER), Bounds::CENTER);
	// todo: scrollbar, get events better, sliders, color pickers, cursor change, basic file menu, realtime gui change and save gui state, graphs

	// Menus can be made using a defined width and height, then taking in a bunch of strings that can be used as identifiers for the given textbutton. Although menus may require views for scrolling, the view should not extend outside the bounding box so it should be fine for now
	
	// Now that I've found a way to apply views to RectFields, I can start making scroll bars. First I'll fix the TextInput, then I'll practice by making sliders. Then the scrollbars can take in scroll wheel input with a customizable step size (minimum 1 pixel) and of course both vertical and horizontal scrollbars will be optional for every object using a view.

	// So for the TextInput I'll make a master string that will refresh the text objects every time it's modified. It sounds cpu intensive, but it's better than limiting line wrapping to only typing and backspacing, preventing any arrow key action. 

	// I've basically perfected the text input and now simply need to improve the keyboard cursor. After that, I need to find a way to turn its RectField into a RectView. However, I'm getting errors so not today. Also need to handle copy and paste and cut and tab and delete and arrow keys and home and end and highlighting. After I do that, I will create basic sliders, slap those onto the view and call them scrollbars. Then I can move the view around using those. Another thing to consider is the vertical wiggle room for the bottom line so it doesn't go over the border, but double the charactersize seems to work for now. After that it's cleanup and menus, color pickers, 

	// RectView error is probably to do with the draw function. I'm going to check if object-specific functions like binding will stop working properly with things that inherit from other derived classes.

	// TextInput with view is fixed, but adding too many lines (more than 50) is extremely performance heavy. As a possible optimization, I can hide lines that don't intersect with the viewport or something.

	// Oops, TextInput runs fine in release. Debug slows this program down by about 20-100 times.

	// Slider time. Should take a position, size, min and max. Just adjust x-axis or whatever and normalize to diff
	// Done there, now need some slider thing and maybe options for whether the rectfield or slider itself will handle hovering. In order to work as a scrollbar, it will need to have a slider thing in the middle and an invisible rectfield for hovering. Will also have to calculate some unit measurement based on view size to see what the step size for scrolling will be.

	unsigned int fps_counter = 0;
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


		win.clear({ 0, 0, 0, 255 });

		state.draw_objects();

		
		win.display();

		if (fps_clock.getElapsedTime().asMilliseconds() >= 100.f)
		{
			fps.set_string(std::to_string(fps_counter * 10.f));
			fps_clock.restart();
			fps_counter = 0;
		}
		fps_counter++;
	}
}
