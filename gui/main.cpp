#include <gui.hpp>



int main()
{
	sf::VideoMode mode({ 2560, 1440 }, 32);
	sf::RenderWindow win(mode, "title", sf::Style::Default);
	win.setFramerateLimit(144u);

	sf::Event event;

	WindowState state(win);
	set_state(state);
	//state.set_window(win);
	//Button butt;
	//Button butt2;
	//butt2.set_color_impl(sf::Color::Red);
	//butt.padding = { 10.f, 10.f };
	//butt2.padding = { 10.f, 10.f };
	TextInput in({ 0.f, 0.f }, { 50.f, 70.f });
	in.set_position_by_bounds(get_window_bounds(Bounds::CENTER), Bounds::CENTER);
	//in.text.set_string("a");
	ObjVec vec;
	//Text test;
	//in.set_vector(vec, 1);
	//in.ignore_focus = 1;

	//std::vector<Object*> vec;
	//butt.set_vector(vec);
	//TextButton tb;
	//Menu menu(2, 3);

	//menu.set_padding({ 10.f, 10.f });
	
	
	//butt.set_position_by_bounds(get_window_bounds(Bounds::LEFT), Bounds::LEFT);

	//butt.set_position({ 400, 400 });
	//butt2.move_vector(butt.get_bind_vector(), butt);
	//Text button_text;
	//button_text.set_position_by_bounds(butt.get_bounds(Bounds::BOTTOM), Bounds::TOP);
	//button_text.set_string("button");

	//Text text({ 200, 200 }, 40u);
	//text.set_string("something really big");
	//text.set_position_by_bounds(get_window_bounds(Bounds::TOP), Bounds::TOP);


	//Menu menu(1, 3);
	//menu.set_position({ 800, 800 });

	// todo: menu creation, scrollbar, text input, return self for chaining maybe, get events better, sliders, color pickers, cursor change, basic file menu, realtime gui change and save gui state, graphs, make all gui elements part of a 'menu' that controls whether they all show or not
	// Problem: any updating outside of the update functions will cause objects to be updated when hidden
	// Solution: store keyboard input in a string in get_events(), then clear/set the string in update()
	// Word wrapping can be done by adding newlines at the last space whenever the text's width is greater than its bounding box
	// Menus can be made using a defined width and height, then taking in a bunch of strings that can be used as identifiers for the given textbutton. Although menus may require views for scrolling, the view should not extend outside the bounding box so it should be fine for now

	// A "TextButton" is a class that takes a text and sticks it to the center of a button, right? But my binding system won't work with the menus. So far it's basically associating by group and not by object. This means that re-binding the items inside a TextButton will make them lose their own bindings. Although a text saves the position and bounds, it's only for alignment. 
	// Binding system fixed, can bind bound things to things that are bound to other things and even bind them in a big infinite circle without any problems so that doing anything with one of them affects all of them. Not advised though.
	sf::String userInput;
	//hide(state.objects);
	while (win.isOpen())
	{
		while (win.pollEvent(event))
		{
			state.get_events(event);
			//std::cout << state.keyboard_input.toAnsiString() << "\n";

			switch (event.type)
			{
			case sf::Event::Closed:
				win.close();
				break;
			//case sf::Event::TextEntered:
			//	if (event.text.unicode != (short)8) // Backspace character, I think
			//	{
			//		userInput += event.text.unicode;
			//		//text.set_string(userInput);
			//	}
			//	break;

			case sf::Event::KeyPressed:
				if (event.key.code == sf::Keyboard::Key::Backspace) // delete the last character
				{
					if (userInput.getSize() != 0)
					{
						userInput.erase(userInput.getSize() - 1);
						//text.set_string(userInput);
					}
				}
				break;
			}
		}
		//test.set_position_by_bounds(get_window_bounds(Bounds::CENTER), Bounds::CENTER);
		state.get_state();
		state.update(vec);
		
		
		//if (state.mouse_clicked)
		//{
			if (in.has_user_focus)
				in.set_color_impl(sf::Color::Green);
			if (!in.has_user_focus)
				in.set_color_impl(sf::Color::Red);
		//}
		

		//if (butt.pressed)
		//	butt.rect.setFillColor(sf::Color::Green);
		//else
		//	butt.rect.setFillColor(sf::Color::Red);


		win.clear({ 0, 0, 0, 255 });
		state.draw_objects();
		state.draw_objects(vec);
		
		
		
		win.display();
	}
}
