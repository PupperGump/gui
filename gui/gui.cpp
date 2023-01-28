#include <gui.hpp>

WindowState* state;


bool set_state(WindowState& state_input)
{
	if (&state_input == NULL)
		return 0;
	state = &state_input;

	// init default resources (todo: add more fonts and an enum)
	sf::Font font;
	if (!font.loadFromFile("arial.ttf"))
		std::cout << "Missing default font: arial.ttf\n";
	state->fonts.push_back(font);

	return 1;
}



////////////////////////////////////////////////////////////////////////////////////////////////////
// Object
////////////////////////////////////////////////////////////////////////////////////////////////////



Object::Object() 
{
	push_vector(state->objects);
	current_vector = &state->objects;
}
Object::~Object()
{
	unbind();
	remove_vector(state->objects);
}

void Object::push_vector(ObjVec& object_vector)
{
	object_vector.push_back(this);
}

int Object::find_vector(ObjVec& object_vector)
{
	for (int i = 0; i < object_vector.size(); i++)
	{
		if (this == object_vector[i])
			return i;
	}
	return -1;
}

bool Object::remove_vector(ObjVec& object_vector)
{
	// Find and destroy the pointer to this object
	if (&object_vector == NULL)
		return 0;
	int index = find_vector(object_vector);

	if (index == -1)
		return 0;
	object_vector.erase(object_vector.begin() + index);

	return 1;
}

void Object::set_vector(ObjVec& object_vector)
{
	remove_vector(*current_vector);
	push_vector(object_vector);
	current_vector = &object_vector;
}
void Object::set_vector(ObjVec& object_vector, bool unbind_current)
{
	// Since everything is paired up nicely when bound together, I can simply iterate over the vector and stop when I find an unbound object (end of tree)
	ObjVec& from_vec = *current_vector;
	int pos = find_vector(from_vec);
	if (pos == -1)
		return;
	if (unbind_current)
		unbind();
	while (pos < from_vec.size() && (from_vec[pos]->bound() || from_vec[pos] == this))
	{
		Object& obj = *from_vec[pos];
		std::cout << obj.bound() << "\n";
		obj.remove_vector(from_vec);
		obj.push_vector(object_vector);
		obj.current_vector = &object_vector;
	}
}	

void Object::move_vector(ObjVec& object_vector, size_t position)
{
	int pos = 0;
	pos = find_vector(object_vector);
	if (pos == -1)
		return;
	object_vector.erase(object_vector.begin() + pos);
	object_vector.insert(object_vector.begin() + position, this);
}

void Object::move_vector(ObjVec& object_vector, Object& next_to, int position)
{
	int pos1 = 0, pos2 = 0;
	pos1 = find_vector(object_vector);

	pos2 = next_to.find_vector(object_vector);

	if (pos1 == -1) // Not found
		return;

	object_vector.erase(object_vector.begin() + pos1);
	object_vector.insert(object_vector.begin() + pos2 + position, this);
}


void Object::set_position(sf::Vector2f position)
{
	set_position(position, 1);
}

void Object::set_position(sf::Vector2f position, bool is_caller)
{
	if (is_caller)
	{
		state->offset = position - get_position();
		state->caller = this;
	}
	else
	{
		if (state->caller == this)
			return;
	}
	for (auto& obj : bound_objects)
	{
		if (obj->affected_by_bound)
			obj->set_position(position, 0); // position doesn't matter here since we already have the offset
	}

	set_position_impl(get_position() + state->offset);
}

void Object::set_scale(float scale_x, float scale_y)
{
	set_scale(scale_x, scale_y, 1);
}

void Object::set_scale(float scale_x, float scale_y, bool is_caller)
{
	if (is_caller)
	{
		state->caller = this;
	}
	else
	{
		if (state->caller == this)
			return;
	}
	for (auto& obj : bound_objects)
	{
		// If scale_y is 0, use scale_x for both
		if (obj->affected_by_bound)
		{
			if (scale_y == 0.f)
				obj->set_scale(scale_x, scale_x, 0);
			else
				obj->set_scale(scale_x, scale_y, 0);
		}
	}

	set_scale_impl(scale_x, scale_y);
}

void Object::set_color(sf::Color color)
{
	set_color(color, 1);
}

void Object::set_color(sf::Color color, bool is_caller)
{
	if (is_caller)
	{
		state->caller = this;
	}
	else
	{
		if (state->caller == this)
			return;
	}
	for (auto& obj : bound_objects)
	{
		if (obj->affected_by_bound)
			obj->set_color(color, 0);
	}

	set_color_impl(color);
}

void Object::set_padding(sf::Vector2f padding)
{
	set_padding(padding, 1);
}

void Object::set_padding(sf::Vector2f padding, bool is_caller)
{
	if (is_caller)
	{
		state->caller = this;
	}
	else
	{
		if (state->caller == this)
			return;
	}
	for (auto& obj : bound_objects)
	{
		if (obj->affected_by_bound)
			obj->padding = padding;
	}
	padding = padding;
}


bool Object::bound()
{
	if (bound_to == NULL)
		return 0;
	return 1;
}

void Object::bind(Object& other)
{
	if (other.bound_to == this)
	{
		// If the other object is already bound to the current object, break the binding
		other.bound_to = nullptr;
		return;
	}
	if (bound())
		unbind();
	bound_to = &other;

	Object* last = NULL; 
	auto vec = bound_to->bound_objects;
	if (vec.size() > 0)
		last = vec[vec.size() - 1];

	// Probably overkill
	if (last == this)
	{
		std::cout << "bind: current object found in bound_to->bound_objects\n";
		return;
	}

	push_vector(bound_to->bound_objects);
	if (current_vector != bound_to->current_vector)
	{
		set_vector(*bound_to->current_vector);
	}

	// Move right after "last" (pos 0 means left of, 1 means right of)
	if (last != NULL)
		move_vector(*current_vector, *last, 1);

	// Share views
	view_ptr = bound_to->view_ptr;
}


void Object::unbind()
{
	if (bound_to == NULL)
		return;	
	
	remove_vector(bound_to->bound_objects);
	set_vector(state->objects);
	bound_to = NULL;
	view_ptr = NULL;
}



////////////////////////////////////////////////////////////////////////////////////////////////////
// WindowState
////////////////////////////////////////////////////////////////////////////////////////////////////


WindowState::WindowState()
{
}

WindowState::WindowState(sf::RenderWindow& window)
{
	this->window = &window;
}

void WindowState::set_window(sf::RenderWindow& window)
{
	this->window = &window;
}

void WindowState::show_all()
{
	for (auto& obj : state->objects)
	{
		obj->hide_object = 0;
	}
}
void WindowState::hide_all()
{
	for (auto& obj : state->objects)
	{
		obj->hide_object = 1;
	}
}

// These use the recursive implementation for bound objects
void WindowState::show(Object& object, bool is_caller)
{
	if (is_caller)
	{
		state->caller = &object;
	}
	else
	{
		if (state->caller == &object)
			return;
	}
	for (auto& obj : object.bound_objects)
	{
		object.hide_object = 0;
	}

	object.hide_object = 0;
}

void WindowState::hide(Object& object, bool is_caller)
{
	//object.hide_object = 1;

	if (is_caller)
	{
		state->caller = &object;
	}
	else
	{
		if (state->caller == &object)
			return;
	}
	for (auto& obj : object.bound_objects)
	{
		object.hide_object = 1;
	}

	object.hide_object = 1;
}

void WindowState::show(ObjVec& object_vector)
{
	for (auto& obj : object_vector)
	{
		obj->hide_object = 0;
	}
}
void WindowState::hide(ObjVec& object_vector)
{
	for (auto& obj : object_vector)
	{
		obj->hide_object = 1;
	}
}

// Call inside event loop
void WindowState::get_events(sf::Event& event)
{
	switch (event.type)
	{
	case sf::Event::MouseButtonPressed:
		switch (event.mouseButton.button)
		{
		case sf::Mouse::Left:
			mouse_down = 1;
			mouse_up = 0;
			break;
		}
		break;
	case sf::Event::MouseButtonReleased:
		switch (event.mouseButton.button)
		{
		case sf::Mouse::Left:
			mouse_up = 1;
			mouse_down = 0;
			break;
		}
		break;
	case sf::Event::TextEntered:
		keyboard_input += event.text.unicode;
		break;
	}
}

// Call after event loop
void WindowState::get_state()
{
	// Moved this here because I'm making update and draw more flexible
	object_focused = 0;

	mouse_screen_position = sf::Mouse::getPosition(*state->window);

	if (mouse_clicked)
	{
		mouse_clicked = 0;
	}
	if (mouse_down)
	{
		mouse_was_down = 1;
	}

	if (mouse_up && mouse_was_down)
	{
		mouse_clicked = 1;
		mouse_was_down = 0;
	}
}

// Might be useless now
void WindowState::update()
{
	//if (objects.size() == 0)
	//	return;

	update(objects);
}


void WindowState::update(ObjVec& object_vector)
{
	if (object_vector.size() == 0)
		return;

	// Loop backwards through the object vector to "depth-check" object_vector if object.ignore_focus is 0
	for (int i = object_vector.size() - 1; i >= 0; i--)
	{
		if (object_vector[i] == NULL)
			continue;
		if (object_vector[i]->hide_object)
			continue;

		if (mouse_clicked)
		{
			if (object_vector[i]->has_user_focus)
				object_vector[i]->lost_focus = 1;
			object_vector[i]->has_user_focus = 0;

			if (object_vector[i]->focus_toggled)
				object_vector[i]->focus_toggled = 0;
			if (object_vector[i]->lost_focus || object_vector[i]->activated)
				object_vector[i]->focus_toggled = 1;
		}
		object_vector[i]->get_hovered();

		if (object_vector[i]->ignore_focus)
		{
			object_vector[i]->update();
			continue;
		}
		if (!object_focused)
		{
			if (object_vector[i]->hovered || object_vector[i]->has_user_focus)
			{
				if (object_vector[i]->has_user_focus)
					object_vector[i]->lost_focus = 0;
				object_vector[i]->update();
				object_focused = 1;
			}
		}
	}
}

void WindowState::draw_objects()
{
	// I changed it again. Bound objects will be positioned directly after the last object in bound_to->bound_objects. In other words, bound objects will appear above what they're bound to, and those that are bound later will appear above the rest.

	// Can't find a better place for this
	//update(objects);
	draw_objects(objects);
	keyboard_input.clear();
}

void WindowState::draw_objects(ObjVec& object_vector)
{
	// I think it's better to stick these together since update() and draw() will be called in the same spot anyways, but if that's a problem I can just make the user call update() instead
	update(object_vector);
	for (auto& obj : object_vector)
	{
		if (obj->hide_object)
			continue;
		if (obj->view_ptr != NULL)
			state->window->setView(*obj->view_ptr);
		obj->draw();
		state->window->setView(state->window->getDefaultView());
	}
}

sf::Vector2f get_window_bounds(unsigned int type, float scale_x, float scale_y)
{
	sf::Vector2f step = state->window->getDefaultView().getSize() / 2.f;
	sf::Vector2f pos = step; // Half width right, half height down
	step.x *= scale_x;
	step.y *= scale_y;

	if (type & Bounds::LEFT)
		pos.x -= step.x;
	if (type & Bounds::RIGHT)
		pos.x += step.x;
	if (type & Bounds::TOP)
		pos.y -= step.y;
	if (type & Bounds::BOTTOM)
		pos.y += step.y;

	return pos;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// RectField
////////////////////////////////////////////////////////////////////////////////////////////////////


RectField::RectField(sf::Vector2f position, sf::Vector2f size, sf::Color color)
{
	rect.setPosition(position);
	rect.setSize(size);
	rect.setFillColor(color);
}


void RectField::set_size(sf::Vector2f size)
{
	rect.setSize(size);
}

sf::Vector2f RectField::get_size()
{
	return rect.getSize();
}


void RectField::set_position_impl(sf::Vector2f position)
{
	rect.setPosition(position);
}

void RectField::set_color_impl(sf::Color color)
{
	rect.setFillColor(color);
}

void RectField::set_scale_impl(float scale_x, float scale_y)
{
	rect.setScale({ scale_x, scale_y });
}

sf::Vector2f RectField::get_position()
{
	return rect.getPosition();
}


sf::Vector2f RectField::get_bounds(unsigned int type, float scale_x, float scale_y)
{
	sf::Vector2f step = rect.getSize() / 2.f;
	sf::Vector2f pos = rect.getPosition() + step; // Half width right, half height down
	step.x *= scale_x;
	step.y *= scale_y;

	if (type & Bounds::LEFT)
		pos.x -= step.x;
	if (type & Bounds::RIGHT)
		pos.x += step.x;
	if (type & Bounds::TOP)
		pos.y -= step.y;
	if (type & Bounds::BOTTOM)
		pos.y += step.y;

	return pos;
}

void RectField::set_position_by_bounds(sf::Vector2f position, unsigned int type, float scale_x, float scale_y)
{
	sf::Vector2f bounds = get_bounds(type, scale_x, scale_y);	
	
	if (type & Bounds::LEFT)
		bounds.x -= padding.x;
	if (type & Bounds::RIGHT)
		bounds.x += padding.x;
	if (type & Bounds::TOP)
		bounds.y -= padding.y;
	if (type & Bounds::BOTTOM)
		bounds.y += padding.y;
	sf::Vector2f offset = (position - bounds);
	sf::Vector2f newpos = get_position() + offset;
	set_position(newpos);
}



void RectField::get_hovered()
{
	//sf::Vector2f pos = rect.getPosition();
	//sf::Vector2f size = rect.getSize();

	//// Should use current view? WARNING: Cursor still interacts with objects outside the view
	sf::Vector2f mouse_position = state->window->mapPixelToCoords(state->mouse_screen_position);

	//// This will not work if the size is negative

	//if (mouse_position.x > pos.x &&
	//	mouse_position.x < pos.x + size.x &&
	//	mouse_position.y > pos.y &&
	//	mouse_position.y < pos.y + size.y)
	//{
	//	hovered = 1;
	//}
	//else
	//	hovered = 0;

	if (rect.getGlobalBounds().contains(mouse_position))
		hovered = 1;
	else
		hovered = 0;
}


// Checks mouse collision, click and activation events
void RectField::update()
{
	if (hovered && state->mouse_clicked && !mouse_down_with_no_hover)
	{
		activated = 1;
		has_user_focus = 1;
		toggled = !toggled;
	}
	else
	{
		activated = 0;
	}

	if (state->mouse_down && !hovered)
	{
		mouse_down_with_no_hover = 1;
		
	}

	if (state->mouse_up)
	{
		mouse_down_with_no_hover = 0;
	}
}

void RectField::draw()
{
	state->window->draw(rect);
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// RectView
////////////////////////////////////////////////////////////////////////////////////////////////////

RectView::RectView(sf::Vector2f viewport_position, sf::Vector2f viewport_size, sf::Vector2f rectfield_size, sf::Color color)
{
	state->views.push_back(sf::View());

	// We can just take the address of the last thing in the vector, hopefully
	view_ptr = &state->views[state->views.size() - 1];

	// I'm thinking about whether to set the RectField's position at all since it only renders in the view. However, if in the future it interacts with other objects based on that position, it will overlap with other RectViews. For now I'll just set it to viewport_position and leave that problem for future me
	set_position(viewport_position);
	set_size(rectfield_size);
	//viewport_position -= get_size() / 2.f;

	sf::Vector2f wb = get_window_bounds(Bounds::BOTTOM_RIGHT);
	//sf::Vector2f wb = { 2560.f, 1540.f };
	sf::Vector2f wbs = { 1.f / wb.x, 1.f / wb.y};
	//std::cout << "(" << wbs.x << ", " << wbs.y << ")\n";
	sf::Vector2f pos = { viewport_position.x * wbs.x, viewport_position.y * wbs.y };
	sf::Vector2f size = { viewport_size.x * wbs.x, viewport_size.y * wbs.y };
	//std::cout << "(" << size.x << ", " << size.y << ")\n";
	view_ptr->setViewport({ pos, size });
	view_ptr->setCenter(get_bounds(Bounds::TOP_LEFT) + viewport_size / 2.f);
	view_ptr->setSize(viewport_size);

	//auto s = view_ptr->getSize();
	//auto p = view_ptr->getCenter();
	//std::cout << "(" << p.x << ", " << p.y << "), (" << s.x << ", " << s.y << ")\n";
	//std::cout << get_bounds(Bounds::TOP_LEFT).x << ", " << get_bounds(Bounds::TOP_LEFT).y << "\n";
}

void RectView::move_view(sf::Vector2f offset)
{
	sf::Vector2f new_pos = view_ptr->getCenter() + offset;
	sf::Vector2f size = view_ptr->getSize() / 2.f;

	if (new_pos.x - size.x < get_bounds(Bounds::LEFT).x)
		new_pos.x = get_bounds(Bounds::LEFT).x + size.x;
	if (new_pos.x + size.x > get_bounds(Bounds::RIGHT).x)
		new_pos.x = get_bounds(Bounds::RIGHT).x - size.x;
	if (new_pos.y - size.y < get_bounds(Bounds::TOP).y)
		new_pos.y = get_bounds(Bounds::TOP).y + size.y;
	if (new_pos.y + size.y > get_bounds(Bounds::BOTTOM).y)
		new_pos.y = get_bounds(Bounds::BOTTOM).y - size.y;
	view_ptr->setCenter(new_pos);
}

// Might remove
void RectView::draw()
{
	// Moved changing these views to the state's draw() function to accomodate objects that don't set their own views
	RectField::draw();
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// Button
////////////////////////////////////////////////////////////////////////////////////////////////////

Button::Button(sf::Vector2f position, sf::Vector2f size, sf::Color color)
{
	rect.setPosition(position);
	rect.setSize(size);
	rect.setFillColor(color);
}

// Checks if button is pressed basically
void Button::update()
{
	RectField::update();
	if (activated && toggled)
		on = 1;
	else if (activated && !toggled)
		on = 0;
}




////////////////////////////////////////////////////////////////////////////////////////////////////
// Text
////////////////////////////////////////////////////////////////////////////////////////////////////

Text::Text(sf::Vector2f position, unsigned int font_size, sf::Color color)
{
	text.setFont(state->fonts[0]);
	text.setPosition(position);
	text.setCharacterSize(font_size);
	text.setFillColor(color);
	text.setString("text");
}

void Text::set_size(unsigned int font_size)
{
	text.setCharacterSize(font_size);
	set_position_by_bounds(position_reference, type, scale_x, scale_y);
}

sf::Vector2f Text::get_size()
{
	sf::FloatRect text_rect = text.getGlobalBounds();
	return text_rect.getSize();
}

void Text::set_position_impl(sf::Vector2f position)
{
	if (!set_alignment)
		position_reference = recompute_position(position, type, scale_x, scale_y);
	text.setPosition(position);
}

sf::Vector2f Text::get_position()
{
	return text.getPosition();
}

void Text::set_string(sf::String string)
{
	text.setString(string);
	set_position_by_bounds(position_reference, type, scale_x, scale_y);
}

sf::Vector2f Text::get_bounds(unsigned int type, float scale_x, float scale_y)
{
	sf::FloatRect text_rect = text.getGlobalBounds();
	sf::Vector2f step = text_rect.getSize() / 2.f;
	sf::Vector2f pos = text.getPosition() + step;

	if (type & Bounds::LEFT)
		pos.x -= step.x;
	if (type & Bounds::RIGHT)
		pos.x += step.x;
	if (type & Bounds::TOP)
		pos.y -= step.y;
	if (type & Bounds::BOTTOM)
		pos.y += step.y;

	pos.x *= scale_x;
	pos.y *= scale_y;

	return pos;
}


// Set the given boundary to the position provided. The boundary is top left by default.
void Text::set_position_by_bounds(sf::Vector2f position, unsigned int type, float scale_x, float scale_y)
{
	this->position_reference = position;
	this->type = type;
	this->scale_x = scale_x;
	this->scale_y = scale_y;

	set_alignment = 1;

	sf::Vector2f newpos = recompute_position(position, type, scale_x, scale_y);
	set_position(newpos);

	set_alignment = 0;
}

void Text::set_color_impl(sf::Color color)
{
	text.setFillColor(color);
}

void Text::set_scale_impl(float scale_x, float scale_y)
{
	text.setScale({ scale_x, scale_y });
}

//void get_hovered();

sf::Vector2f Text::recompute_position(sf::Vector2f position, unsigned int type, float scale_x, float scale_y)
{
	sf::Vector2f bounds = get_bounds(type, scale_x, scale_y);

	if (type & Bounds::LEFT)
		bounds.x -= padding.x;
	if (type & Bounds::RIGHT)
		bounds.x += padding.x;
	if (type & Bounds::TOP)
		bounds.y -= padding.y;
	if (type & Bounds::BOTTOM)
		bounds.y += padding.y;

	sf::Vector2f offset = (position - bounds);
	sf::Vector2f pos = get_position();
	sf::Vector2f newpos = pos + offset;

	return newpos;
}

void Text::get_hovered()
{
	//sf::FloatRect rect = text.getLocalBounds();
	//float offset = text.getCharacterSize() / 2.f;
	//sf::Vector2f pos = rect.getPosition();
	//pos.x -= offset;
	//sf::Vector2f size = rect.getSize();
	//size.x += offset;

	// Should use current view? WARNING: Cursor still interacts with objects outside the view
	sf::Vector2f mouse_position = state->window->mapPixelToCoords(state->mouse_screen_position);

	// This will not work if the size is negative

	//if (mouse_position.x > pos.x &&
	//	mouse_position.x < pos.x + size.x &&
	//	mouse_position.y > pos.y &&
	//	mouse_position.y < pos.y + size.y)
	//{
	//	hovered = 1;
	//}
	//else
	//	hovered = 0;

	if (text.getGlobalBounds().contains(mouse_position))
		hovered = 1;
	else
		hovered = 0;
}

void Text::update()
{
}

void Text::draw()
{
	state->window->draw(text);
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// TextButton
////////////////////////////////////////////////////////////////////////////////////////////////////

TextButton::TextButton(sf::Vector2f position, sf::Vector2f size, sf::Color color)
{
	button.set_position(position);
	button.set_size(size);
	button.set_color(color);

	text.set_position_by_bounds(button.get_bounds(Bounds::CENTER), Bounds::CENTER);
	text.bind(button);
}



////////////////////////////////////////////////////////////////////////////////////////////////////
// Menu
////////////////////////////////////////////////////////////////////////////////////////////////////

Menu::Menu(unsigned int width, unsigned int height)
{
	if (width == 0)
		width = 1;
	if (height == 0)
		height = 1;

	this->width = width;
	this->height = height;

	buttons.resize(width * height);
	fit_rect();
	for (auto& obj : buttons)
	{
		obj.button.bind(*this);
	}
	rect.setFillColor({ 100, 100, 100, 100 });
	set_grid(width, height);
}


void Menu::fit_rect()
{
	sf::Vector2f size = buttons[0].button.rect.getSize();
	rect.setSize({ (width * (size.x + padding.x)) + padding.x, (height * (size.y + padding.y)) + padding.y });
}

void Menu::set_grid(unsigned int width, unsigned int height)
{
	buttons[0].button.set_position_by_bounds(get_bounds(Bounds::TOP_LEFT), Bounds::TOP_LEFT);
	
	for (int i = 1; i < buttons.size(); i++)
	{
		// Calculate the row and column indices for the current button
		int row = i / width;
		int col = i % width;

		// If the current button is in the same row as the previous button
		if (col != 0)
		{
			// Position the current button to the right of the previous button
			buttons[i].button.set_position_by_bounds(buttons[i - 1].button.get_bounds(Bounds::RIGHT), Bounds::LEFT);
		}
		// If the current button is in the next row
		else
		{
			// Position the current button below the first button in the previous row
			buttons[i].button.set_position_by_bounds(buttons[i - width].button.get_bounds(Bounds::BOTTOM), Bounds::TOP);
		}
	}
}

void Menu::set_padding(sf::Vector2f padding)
{
	this->padding = padding;
	for (auto& obj : buttons)
	{
		obj.button.padding = padding;
	}
	fit_rect();
	set_grid(width, height);
}



////////////////////////////////////////////////////////////////////////////////////////////////////
// TextInput
////////////////////////////////////////////////////////////////////////////////////////////////////

TextInput::TextInput(sf::Vector2f position, sf::Vector2f size)
{
	set_position(position);
	set_size(size);
	if (text_index == 0)
		text.resize(1);
	//text[0].set_padding({ 50.f, 0.f });
	text[0].set_string("");
	text[0].affected_by_bound = 0;
	text[0].ignore_focus = 1;
	
	text[0].bind(*this);
	set_alignment(Align::LEFT);

	//float whitespaceWidth = text[0].text.getFont()->getGlyph(U' ', text[0].text.getCharacterSize(), (text[0].text.getStyle() == sf::Text::Bold)).advance;

	
	
	//float whitespace_width = text[0].text.getGlobalBounds().width;
	//letter_limit = get_size().x / whitespace_width;
	//text[0].set_string("");
	//std::cout << "Size: " << get_size().x << "\nWhitespace: " << whitespace_width << "\nLetter limit: " << letter_limit << "\n";
}


//void TextInput::set_color_impl(sf::Color color)
//{
//	RectField::set_color_impl(color);
//}

void TextInput::set_alignment(unsigned int alignment)
{
	if (alignment == Align::LEFT)
	{
		from = Bounds::TOP_LEFT;
		to = Bounds::BOTTOM_LEFT;
	}
	if (alignment == Align::CENTER)
	{
		from = Bounds::TOP;
		to = Bounds::BOTTOM;
	}
	if (alignment == Align::RIGHT)
	{
		from = Bounds::TOP_RIGHT;
		to = Bounds::BOTTOM_RIGHT;
	}

	text[0].set_position_by_bounds(get_bounds(from), from);

	for (int i = 1; i < text.size(); i++)
	{
		text[i].set_position_by_bounds(text[i - 1].get_bounds(to), from);
	}
}

void TextInput::add_line()
{
	text.push_back(Text());
	text_index++;

	// Upon line removal, accesses dangling pointer (fixme)
	//text[text_index].bind(text[text_index - 1]);

	text[text_index].text = text[0].text;
	text[text_index].padding = text[0].padding;
	text[text_index].ignore_focus = 1;
	text[text_index].set_position_by_bounds(text[text_index - 1].get_bounds(to), from);
}

void TextInput::remove_line()
{
	if (text_index == 0)
	{
		text[text_index].set_string("");
		return;
	}
	text.pop_back();
	text_index--;
}

void TextInput::process_input_string(sf::String& input_string)
{
	if (!has_user_focus)
		return;

	// It's not a very good idea to modify user inputs directly, so:
	sf::String kstr = state->keyboard_input;

	// Basically event loop but for a string (when pressing many keys at once, more than one key may enter keyboard_input)
	for (int c = 0; c < kstr.getSize(); c++)
	{
		// Tell display_text() to work
		string_changed = 1;
		// If hit backspace, remove last character
		if (kstr[c] == Ctrl::H) // backspace
		{
			if (string.getSize() != 0)
			{
				if (cursor_position == 0)
					return;
				cursor_position--;
			}
			kstr.erase(c);
			string.erase(cursor_position);
			// Can only do this if not using 'c' anywhere below
			if (c != 0)
				c--;
			continue;
		}

		// Put letter where thingy is
		string.insert(cursor_position, kstr[c]);
		cursor_position++;
	}
}

void TextInput::display_text()
{
	if (!has_user_focus)
		return;

	// Reset string state
	string_changed = 0;

	// If text is changed at all, "refresh" the lines by removing everything and starting from the beginning.
	int space_pos = -1;
	sf::String next_str;
	sf::String str = string;

	// Remove all lines
	while (text_index != 0)
		remove_line();

	// Init
	text[0].set_string(string);

	// Everything below is for line wrapping
	if (!use_line_wrap)
		return;

	// Continue while the text line is bigger than the rectfield. The entire substring after what is wrapped will be passed to the next line.
	while (text[text_index].get_size().x > get_size().x - text[text_index].padding.x * 2)
	{
		// Default for when math doesn't work right
		int last = str.getSize();
		
		// Find the difference between first character of the line and character at i
		sf::Vector2f start = text[text_index].text.findCharacterPos(0);
		for (int i = 0; i < str.getSize(); i++)
		{
			sf::Vector2f pos = text[text_index].text.findCharacterPos(i);

			// Whichever character is causing the line wrap is gonna be the start for the space loop (last in line)
			if (abs(pos.x - start.x) > get_size().x - text[text_index].padding.x * 2)
			{
				last = i;
				break;
			}
		}
		
		//std::cout << "Last at " << last << "\n";
		for (int i = last; i >= 0; i--)
		{
			if (str[i] == ' ')
			{
				space_pos = i;
				//std::cout << "space_pos at " << space_pos << "\n";
				break;
			}
		}
		// If no spaces before
		if (space_pos == -1)
		{
			//std::cout << "No space before\n";
			for (int i = last; i < str.getSize(); i++)
			{
				if (str[i] == ' ')
				{
					space_pos = i;
					//std::cout << "space_pos at " << space_pos << "\n";
					break;
				}
			}
		}
		if (space_pos == -1)
		{
			//std::cout << "No space_pos\n";
			return;
		}

		// Cut current line's string off at where it should be wrapped
		next_str = str.substring(space_pos + 1, str.getSize() - (space_pos + 1));
		str.erase(space_pos + 1, next_str.getSize());
		text[text_index].set_string(str);

		// Stick what's cut off into next line
		add_line();
		text[text_index].set_string(next_str);

		// Make this string the next iteration's problem
		str = next_str;
		
		// Reset to default
		space_pos = -1;
	}
	
}

void TextInput::update_cursor()
{

	// Before anything else, find out where the cursor position is
	// Side note, if for whatever reason the bound texts are not updated first I will need to manually update them

	if (!state->mouse_clicked || !has_user_focus)
		return;

	sf::Vector2f mouse_position = state->window->mapPixelToCoords(state->mouse_screen_position);
	int cpos = 0, jay = -1;
	bool stop = 0;
	for (int i = 0; i < text.size(); i++)
	{
		switch (i)
		{
		case 0:
			std::cout << "First text:\n";
			break;
		case 1:
			std::cout << "Second text:\n";
			break;
		case 2:
			std::cout << "Third text:\n";
			break;
		}
		text[i].get_hovered();
		std::cout << i << "\n";

		
		if (i != 0)
		{
			
			cpos += text[i - 1].text.getString().getSize();
			//std::cout << "\t\tcpos: " << cpos << "\n";
			//std::cout << text[i - 1].text.getString().toAnsiString() << "\n";
		}		
		
		if (!text[i].hovered)
			continue;

		float smallest = INFINITY;
		float diff = smallest;
		for (int j = 0; j < text[i].text.getString().getSize(); j++)
		{
			std::cout << "\tCharacter " << j << "\n";

			sf::Vector2f pos = text[i].text.findCharacterPos(j);
			diff = abs(mouse_position.x - pos.x);

			if (diff <= smallest)
			{
				smallest = diff;
				jay = j;
			}
		}
	}
	if (jay == -1)
		cursor_position = string.getSize();
	else
		cursor_position = jay + cpos;
}


void TextInput::update()
{
	RectField::update();
	process_input_string(string);
	if (string_changed)
		display_text();
	update_cursor();
}

void TextInput::draw()
{
	RectField::draw();
	for (auto& t : text)
	{
		t.draw();
	}
}