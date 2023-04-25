#include <gui.hpp>

WindowState* state;
const std::string fontfolder = "C:\\Windows\\Fonts\\";


bool set_state(WindowState& state_input)
{
	if (&state_input == NULL)
		return 0;
	state = &state_input;

	// init default resources (todo: add more fonts and an enum)
	sf::Font font;
	if (!font.loadFromFile(fontfolder + "arial.ttf"))
		std::cout << "Missing default font: arial.ttf\n";
	state->fonts.push_back(font);

	return 1;
}




////////////////////////////////////////////////////////////////////////////////////////////////////
// Object
////////////////////////////////////////////////////////////////////////////////////////////////////



Object::Object() 
{
	//std::cout << "Object created\n";
	push_vector(state->objects);
	current_vector = &state->objects;
	view_ptr = &state->views[0];
}
Object::Object(const Object& other)
{
	*this = other;
	push_vector(state->objects);
	current_vector = &state->objects;
}
Object::~Object()
{
	//std::cout << "Object destroyed\n";
	unbind();
	remove_vector(*current_vector);
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

	if (pos + position > object_vector.size() - 1)
	{
		std::cout << "move_vector: position invalid\n";
	}
	object_vector.erase(object_vector.begin() + pos);
	object_vector.insert(object_vector.begin() + position, this);
}

void Object::move_vector(ObjVec& object_vector, Object& next_to, int position)
{
	//int pos1 = 0, pos2 = 0;
	//pos1 = find_vector(object_vector);

	//if (pos1 == -1) // Not found
	//{
	//	std::cout << "move_vector: object not found\n";
	//	return;
	//}

	int pos2 = 0;

	//object_vector.erase(object_vector.begin() + pos1);
	set_vector(object_vector);
	pos2 = next_to.find_vector(object_vector);	
	
	if (pos2 + position > object_vector.size() - 1)
	{
		std::cout << "move_vector: pos2 too large\n";
	}
	if (pos2 == -1)
	{
		std::cout << "move_vector: pos2 less than 0\n";
	}
	object_vector.insert(object_vector.begin() + pos2 + position, this);

	//std::cout << "Pos 1: " << pos1 << "\nPos 2: " << pos2 << "\n";
}


void Object::set_position(sf::Vector2f position, bool affect_bound)
{
	set_position(position, 1, affect_bound);
}

void Object::set_position(sf::Vector2f position, bool is_caller, bool affect_bound)
{
	if (!affect_bound)
	{
		set_position_impl(get_position() + state->offset);
		return;
	}
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
		{
			//std::cout << obj->find_vector(state->objects);
			obj->set_position(position, 0, 1); // position doesn't matter here since we already have the offset
		}
	}

	set_position_impl(get_position() + state->offset);
}

//void Object::set_size(sf::Vector2f size, bool affect_bound)
//{
//	set_size(size, 1, affect_bound);
//}
//
//void Object::set_size(sf::Vector2f size, bool is_caller, bool affect_bound)
//{
//	if (is_caller)
//	{
//		state->caller = this;
//	}
//	else
//	{
//		if (state->caller == this)
//			return;
//	}
//	for (auto& obj : bound_objects)
//	{
//		// If scale_y is 0, use scale_x for both
//		if (obj->affected_by_bound)
//		{
//			obj->set_size(size, 0, 1);
//		}
//	}
//
//	set_size_impl(size);
//}

void Object::set_scale(float scale_x, float scale_y, bool affect_bound)
{
	set_scale(scale_x, scale_y, 1, affect_bound);
}

void Object::set_scale(float scale_x, float scale_y, bool is_caller, bool affect_bound)
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
				obj->set_scale(scale_x, scale_x, 0, 1);
			else
				obj->set_scale(scale_x, scale_y, 0, 1);
		}
	}

	set_scale_impl(scale_x, scale_y);
}

void Object::set_color(sf::Color color, bool affect_bound)
{
	set_color(color, 1, affect_bound);
}

void Object::set_color(sf::Color color, bool is_caller, bool affect_bound)
{
	if (!affect_bound)
	{
		set_color_impl(color);
		return;
	}

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
			obj->set_color(color, 0, 1);
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
	this->padding = padding;
}

bool Object::gain_focus_condition()
{
	return (hovered && state->mouse_clicked);
}

bool Object::lose_focus_condition()
{
	return (!hovered && state->mouse_up);
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
		// If the other object is already bound to the current object, escape
		//other.bound_to = NULL;
		std::cout << "bind: other object already bound to this\n";
		return;
	}
	if (bound())
		unbind();
	bound_to = &other;

	Object* last = NULL; 
	auto& vec = bound_to->bound_objects;
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

	// Move right after "last" (pos -1 means left of, 0 means right of)
	if (last != NULL)
	{
		//std::cout << "last is not NULL\n";
	}
	else
	{
		last = &other;
		//std::cout << "last is NULL\n";
	}
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
	view_ptr = &state->views[0];
}

sf::Vector2f Object::get_bounded_bounds(unsigned int type, float scale_x, float scale_y, bool is_caller)
{
	sf::Vector2f bound = get_bounds(type, scale_x, scale_y);
	std::cout << "(" << bound.x << ", " << bound.y << ")\n";
	if (is_caller)
	{
		state->caller = this;
	}
	else
	{
		if (state->caller == this)
			return bound;
	}
	for (auto& obj : bound_objects)
	{
		switch (type)
		{
		case Bounds::LEFT:
			bound.x = std::min(obj->get_bounded_bounds(type, scale_x, scale_y, 0).x, bound.x);
			break;
		case Bounds::RIGHT:
			bound.x = std::max(obj->get_bounded_bounds(type, scale_x, scale_y, 0).x, bound.x);
			break;
		case Bounds::TOP:
			bound.y = std::min(obj->get_bounded_bounds(type, scale_x, scale_y, 0).y, bound.y);
			break;
		case Bounds::BOTTOM:
			bound.y = std::max(obj->get_bounded_bounds(type, scale_x, scale_y, 0).y, bound.y);
			break;
		}
	}
	return bound;
}

sf::Vector2f Object::get_all_bounds(unsigned int type, float scale_x, float scale_y)
{
	return get_bounded_bounds(type, scale_x, scale_y, 1);
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
	views.push_back(window.getDefaultView());
}

void WindowState::set_window(sf::RenderWindow& window)
{
	this->window = &window;
	views.push_back(window.getDefaultView());
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

	for (auto& objvec : object.obj_vecs)
	{
		show(*objvec);
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

	for (auto& objvec : object.obj_vecs)
	{
		hide(*objvec);
	}

	object.hide_object = 1;
}

void WindowState::show(ObjVec& object_vector)
{
	for (auto& obj : object_vector)
	{
		show(*obj);
	}
}
void WindowState::hide(ObjVec& object_vector)
{
	for (auto& obj : object_vector)
	{
		hide(*obj);
	}
}

// Call inside event loop
void WindowState::get_events(sf::Event& event)
{
	switch (event.type)
	{
	case sf::Event::MouseWheelScrolled:
		state->mouse_wheel_scroll = event.mouseWheelScroll;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
		{
			mouse.wheel_x = 1;
			break;
		}
		mouse.wheel_y = 1;
		break;
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
	case sf::Event::KeyPressed:
		// Due to the lack of any conversions between key codes and the characters they represent, I need a way to mark that the key was pressed. One way I can do this is to stuff the key codes into a vector or use booleans. However, this means that I must reset or remove them when using them, meaning I can only use a key action for one object. I could also track the presses in state and instead use those to modify a struct in the object itself. The disadvantage is the size of the objects will be larger as the keys I need to check increase. I'd also rather not use vectors to keep the event checking as quick as possible. 
		switch (event.key.code)
		{
		case sf::Keyboard::Delete:
			keyboard_input += (unsigned int)127;
			break;
		case sf::Keyboard::Left:
			key.left = 1;
			break;
		case sf::Keyboard::Up:
			key.up = 1;
			break;
		case sf::Keyboard::Right:
			key.right = 1;
			break;
		case sf::Keyboard::Down:
			key.down = 1;
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
	// Reset everything
	keyboard_input.clear();
	key.reset();
	mouse.reset();

	object_focused = 0;
	found_default = 0;

	mouse_screen_position = sf::Mouse::getPosition(*state->window);
	mouse_coord_position = state->window->mapPixelToCoords(mouse_screen_position);

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

void WindowState::update(ObjVec& object_vector)
{
	
	if (object_vector.size() == 0)
		return;

	// Loop backwards through the object vector to "depth-check" object_vector if object.ignore_focus is 0
	for (int i = object_vector.size() - 1; i >= 0; i--)
	{
		//std::cout << object_vector[i]->name << "\n";
		// Don't update objects that are hidden or nonexistent
		if (object_vector[i] == NULL)
			continue;
		if (object_vector[i]->hide_object)
		{
			// If object is hidden it shouldn't keep focus
			object_vector[i]->has_user_focus = 0;
			continue;
		}

		//object_vector[i]->locked_focus = 0;		

		// Figure out if the mouse is hovering over the object and whether to set bound focus
		object_vector[i]->get_hovered();

		// Catch and pass focus, used for things like the knob on the slider
		if (object_vector[i]->hovered && object_vector[i]->catch_focus)
		{
			if (object_vector[i]->bound())
			{
				object_vector[i]->bound_to->has_user_focus = 1;
			}
		}
		
		// Check user focus
		if (object_vector[i]->gain_focus_condition())
			object_vector[i]->has_user_focus = 1;
		if (object_vector[i]->lose_focus_condition())
			object_vector[i]->has_user_focus = 0;

		// Update the object every frame if you don't care about focus
		if (object_vector[i]->ignore_focus)
		{
			object_vector[i]->update();
			continue;
		}

		// If there is no object currently in focus, update. Lock focus if the current object doesn't "pass" it over to another object
		if (!object_focused)
		{
			if (object_vector[i]->hovered || object_vector[i]->has_user_focus)
			{
				object_vector[i]->update();
				if (!object_vector[i]->catch_focus)
				{
					object_focused = 1;
					//object_vector[i]->locked_focus = 1;
				}
			}
		}
	}
}

void WindowState::draw_objects()
{
	// I changed it again. Bound objects will be positioned directly after the last object in bound_to->bound_objects. In other words, bound objects will appear above what they're bound to, and those that are bound later will appear above the rest.

	draw_objects(objects);
}

void WindowState::draw_objects_impl(std::vector<ObjVec*> vec)
{
	//std::cout << "template\n";
	for (int i = 0; i < vec.size(); i++)
	{
		if (typeid(ObjVec) != typeid(*vec[i]))
		{
			std::cout << "ERROR: draw_objects(): item " << i << " not of type ObjVec\n";
			vec.erase(vec.begin() + i);
			i--;
			continue;
		}
		if (vec[i] == &state->objects)
			found_default = 1;
	}

	for (int i = vec.size() - 1; i >= 0; i--)
	{
		for (auto& obj : *vec[i])
		{
			for (auto& v : obj->obj_vecs)
			{
				update(*v);
			}
		}
		update(*vec[i]);
	}
	if (!found_default)
	{
		draw_objects();
	}
	for (int i = 0; i < vec.size(); i++)
	{

		draw_objects_impl(*vec[i]);
		
		for (auto& obj : *vec[i])
		{
			for (auto& v : obj->obj_vecs)
			{
				draw_objects_impl(*v);
			}
		}
	}
}

void WindowState::draw_objects_impl(ObjVec& object_vector)
{
	// Setting the view is slightly expensive (73 floats are created I think, at least 37), so maybe a comparison would be better since most of these will probably have the default view. todo: profile both
	for (auto& obj : object_vector)
	{
		if (obj->hide_object)
			continue;
		draw_objects_impl(*obj);
		//if (obj->view_ptr != &views[0])
		//{
		//	//std::cout << "\tSpecial view:\n\n";
		//	state->window->setView(*obj->view_ptr);
		//}
		//else
		//{
		//	//std::cout << "\tDefault view:\n\n";
		//	state->window->setView(state->views[0]);
		//}
		//obj->draw();
	}
	//std::cout << "\tEnd default view:\n\n";
	state->window->setView(state->window->getDefaultView());
}

void WindowState::draw_objects_impl(Object& object)
{
	if (object.view_ptr != &views[0])
	{
		//std::cout << "\tSpecial view:\n\n";
		window->setView(*object.view_ptr);
	}
	else
	{
		//std::cout << "\tDefault view:\n\n";
		window->setView(views[0]);
	}
	object.draw();
}

sf::Vector2f WindowState::get_window_bounds(unsigned int type, float scale_x, float scale_y)
{
	// I could use sf::VideoMode::getDesktopMode(), however, I'm not sure if it will reference the correct "current" mode, especially if multiple windows are open. Since a new window is supplied for each state for different windows, I'd rather just use the window's information.
	// After testing, I find that although the view stretches and shrinks weirdly the boundaries remain consistent.
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
	name = "RectField";
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
	// Should use current view? WARNING: Cursor still interacts with objects outside the view
	sf::Vector2f mouse_position = state->window->mapPixelToCoords(state->mouse_screen_position, *view_ptr);


	if (rect.getGlobalBounds().contains(mouse_position))
		hovered = 1;
	else
		hovered = 0;
}


// Checks mouse collision, click and activation events
void RectField::update()
{
	//std::cout << "updating ";
	if (hovered && state->mouse_clicked && !mouse_down_with_no_hover)
	{
		activated = 1;
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
	//std::cout << "RectField:\n";
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
	set_size(viewport_size);
	//viewport_position -= get_size() / 2.f;

	set_viewport(viewport_position, viewport_size);

	view_ptr->setCenter(get_bounds(Bounds::TOP_LEFT) + viewport_size / 2.f);
}

void RectView::set_viewport(sf::Vector2f viewport_position, sf::Vector2f viewport_size)
{
	sf::Vector2f wb = state->get_window_bounds(Bounds::BOTTOM_RIGHT);
	//sf::Vector2f wb = { 2560.f, 1540.f };
	sf::Vector2f wbs = { 1.f / wb.x, 1.f / wb.y };
	//std::cout << "(" << wbs.x << ", " << wbs.y << ")\n";
	sf::Vector2f pos = { viewport_position.x * wbs.x, viewport_position.y * wbs.y };
	sf::Vector2f size = { viewport_size.x * wbs.x, viewport_size.y * wbs.y };
	//std::cout << "(" << size.x << ", " << size.y << ")\n";
	view_ptr->setViewport({ pos, size });
	view_ptr->setSize(viewport_size);

	// Refresh the stupid boundary checker
	move_view({ 0.f, 0.f });
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


sf::Vector2f RectView::get_viewport_bounds(unsigned int type, float scale_x, float scale_y)
{
	sf::Vector2f step = view_ptr->getViewport().getSize() / 2.f;
	sf::Vector2f pos = view_ptr->getViewport().getPosition() + step; // Half width right, half height down
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

	sf::Vector2f size = state->window->getDefaultView().getSize();
	pos.x *= size.x;
	pos.y *= size.y;

	return pos;
}

sf::Vector2f RectView::get_viewport_size()
{
	sf::Vector2f size = state->window->getDefaultView().getSize();
	sf::Vector2f ret = view_ptr->getViewport().getSize();

	ret.x *= size.x;
	ret.y *= size.y;

	return ret;
}

// update needed for scrolling n stuff

void RectView::update()
{
	RectField::update();
	if (!hovered)
		return;
	if (state->mouse.wheel_x)
	{
		//std::cout << state->mouse_wheel_scroll.delta << "\n";
		move_view({ -state->mouse_wheel_scroll.delta * horizontal_scroll_speed, 0.f });
	}
	if (state->mouse.wheel_y)
	{
		//std::cout << state->mouse_wheel_scroll.delta << "\n";
		move_view({ 0.f, -state->mouse_wheel_scroll.delta * vertical_scroll_speed });
	}
}
// Might remove
void RectView::draw()
{
	//std::cout << "RectView:\n";
	// Moved changing these views to the state's draw() function to accomodate objects that don't set their own views
	RectField::draw();
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// Button
////////////////////////////////////////////////////////////////////////////////////////////////////

Button::Button(sf::Vector2f position, sf::Vector2f size, sf::Color color)
{
	name = "Button";
	rect.setPosition(position);
	rect.setSize(size);
	rect.setFillColor(color);
}

// Checks if button is pressed basically
void Button::update()
{
	RectField::update();
	//if (activated && toggled)
	//	on = 1;
	//else if (activated && !toggled)
	//	on = 0;
}



////////////////////////////////////////////////////////////////////////////////////////////////////
// CircleField
////////////////////////////////////////////////////////////////////////////////////////////////////


CircleField::CircleField(sf::Vector2f position, float size, sf::Color color)
{
	name = "CircleField";
	circle.setPosition(position);
	circle.setRadius(size);
	circle.setFillColor(color);
}


void CircleField::set_size(float size)
{
	circle.setRadius(size);
}

float CircleField::get_size()
{
	return circle.getRadius();
}


void CircleField::set_position_impl(sf::Vector2f position)
{
	circle.setPosition(position);
}

void CircleField::set_color_impl(sf::Color color)
{
	circle.setFillColor(color);
}

void CircleField::set_scale_impl(float scale_x, float scale_y)
{
	circle.setScale({ scale_x, scale_y });
}

sf::Vector2f CircleField::get_position()
{
	return circle.getPosition();
}


sf::Vector2f CircleField::get_bounds(unsigned int type, float scale_x, float scale_y)
{
	sf::Vector2f step = sf::Vector2f(get_size(), get_size());
	sf::Vector2f pos = circle.getPosition() + step; // Half width right, half height down
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

void CircleField::set_position_by_bounds(sf::Vector2f position, unsigned int type, float scale_x, float scale_y)
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



void CircleField::get_hovered()
{
	// sfml contains function for circle is stupid since it just checks position and position + diameter
	sf::Vector2f mouse_position = state->window->mapPixelToCoords(state->mouse_screen_position);

	sf::Vector2f diff = mouse_position - get_bounds(Bounds::CENTER);

	diff.x *= diff.x;
	diff.y *= diff.y;

	float dist = sqrtf(diff.x + diff.y);
	
	if (dist <= get_size())
		hovered = 1;
	else
		hovered = 0;
}


// Checks mouse collision, click and activation events
void CircleField::update()
{
	if (hovered && state->mouse_clicked && !mouse_down_with_no_hover)
	{
		activated = 1;
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

void CircleField::draw()
{
	state->window->draw(circle);
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// Text
////////////////////////////////////////////////////////////////////////////////////////////////////

Text::Text(sf::Vector2f position, unsigned int font_size, sf::Color color)
{
	name = "Text";

	ignore_focus = 1;
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
	text.setPosition(position);

	// If not called by set_position_by_bounds(), set position_reference to current position?
	if (!realign)
	{
		position_reference = uncompute_position(position, type, scale_x, scale_y);
	}
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
	sf::FloatRect text_rect = text.getLocalBounds();
	sf::Vector2f size = text_rect.getSize();
	size.x += text_rect.left;
	size.y += text_rect.top;
	sf::Vector2f step = size / 2.f;
	//std::cout << text_rect.left << ", " << text_rect.top << "\n";
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

	realign = 1;

	sf::Vector2f newpos = recompute_position(position, type, scale_x, scale_y);
	set_position(newpos);

	realign = 0;
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

sf::Vector2f Text::uncompute_position(sf::Vector2f position, unsigned int type, float scale_x, float scale_y)
{
	// This function is undboutedly a work of genius. In one fell swoop, I not only perfectly implemented the correct solution, but did so without even knowing the specifics of what was happening.
	sf::Vector2f newpos = position;
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
	newpos -= offset;

	return newpos;
}

void Text::get_hovered()
{
	// Should use current view? WARNING: Cursor still interacts with objects outside the view
	sf::Vector2f mouse_position = state->window->mapPixelToCoords(state->mouse_screen_position);


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
	if (use_ss)
	{
		set_string(ss.str());
		if (!keep_text)
		{
			ss.str(L"");
			ss.clear();
		}
		use_ss = 0;
	}
	state->window->draw(text);
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// TextButton
////////////////////////////////////////////////////////////////////////////////////////////////////

TextButton::TextButton(sf::Vector2f position, sf::Vector2f size, sf::Color color)
{
	set_position(position);
	set_size(size);
	set_color(color);

	text.set_position_by_bounds(get_bounds(Bounds::CENTER), Bounds::CENTER);
	text.bind(*this);

	::set_vector(vec, text);
	obj_vecs.push_back(&vec);
}

void TextButton::set_size_impl(sf::Vector2f size)
{
	Button::set_size(size);
	text.set_position_by_bounds(get_bounds(Bounds::CENTER), Bounds::CENTER);
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
		obj.bind(*this);
	}
	rect.setFillColor({ 100, 100, 100, 100 });
	set_grid(width, height);
}

void Menu::fit_rect()
{
	sf::Vector2f size = buttons[0].rect.getSize();
	rect.setSize({ (width * (size.x + padding.x)) + padding.x, (height * (size.y + padding.y)) + padding.y });
}

void Menu::set_grid(unsigned int width, unsigned int height)
{
	buttons[0].set_position_by_bounds(get_bounds(Bounds::TOP_LEFT), Bounds::TOP_LEFT);
	
	for (int i = 1; i < buttons.size(); i++)
	{
		// Calculate the row and column indices for the current button
		int row = i / width;
		int col = i % width;

		// If the current button is in the same row as the previous button
		if (col != 0)
		{
			// Position the current button to the right of the previous button
			buttons[i].set_position_by_bounds(buttons[i - 1].get_bounds(Bounds::RIGHT), Bounds::LEFT);
		}
		// If the current button is in the next row
		else
		{
			// Position the current button below the first button in the previous row
			buttons[i].set_position_by_bounds(buttons[i - width].get_bounds(Bounds::BOTTOM), Bounds::TOP);
		}
	}
}

void Menu::set_size(sf::Vector2f size)
{
	for (auto& b : buttons)
	{
		b.set_size(size);
	}
	set_grid(width, height);
	fit_rect();
}

void Menu::set_padding(sf::Vector2f padding)
{
	this->padding = padding;
	for (auto& obj : buttons)
	{
		obj.padding = padding;
	}
	set_grid(width, height);
	fit_rect();
}



////////////////////////////////////////////////////////////////////////////////////////////////////
// TextInput
////////////////////////////////////////////////////////////////////////////////////////////////////

TextInput::TextInput(sf::Vector2f position, sf::Vector2f size)
{
	//std::cout << "construct\n";
	name = "TextInput";

	set_position(position);
	set_size(size);
	set_viewport(position, size);
	if (text_index == 0)
		text.resize(1);
	//text[0].set_padding({ 50.f, 0.f });
	text[0].set_string("");
	text[0].affected_by_bound = 0;
	text[0].ignore_focus = 1;
	
	text[0].bind(*this);
	set_alignment(Align::LEFT);

	keyboard_cursor.setSize({ 3.f, (float)text[0].text.getCharacterSize() });
	keyboard_cursor.setFillColor(sf::Color::Yellow);
}


//void TextInput::set_color_impl(sf::Color color)
//{
//	RectField::set_color_impl(color);
//}

void TextInput::set_alignment(unsigned int alignment)
{
	//std::cout << "set alignment\n";
	this->alignment = alignment;
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

bool TextInput::lose_focus_condition()
{
	return (!hovered && state->mouse_clicked);
}

void TextInput::add_line()
{
	//std::cout << "add line\n";
	text.push_back(Text());
	text_index++;

	// Upon line removal, accesses dangling pointer (fixme)
	//text[text_index].bind(text[text_index - 1]);

	text[text_index].text = text[0].text;
	text[text_index].affected_by_bound = 0;
	text[text_index].padding = text[0].padding;
	text[text_index].ignore_focus = 1;
	text[text_index].set_position_by_bounds(text[text_index - 1].get_bounds(to), from);
}

void TextInput::remove_line()
{
	//std::cout << "remove line\n";
	if (text_index == 0)
	{
		text[text_index].set_string("");
		return;
	}
	text[text_index].unbind();
	text.pop_back();
	text_index--;
}

void TextInput::process_input_string(sf::String& input_string)
{
	//std::cout << "process input string\n";
	if (!has_user_focus)
		return;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
		return;
	//std::cout << key.up << key.down << key.left << key.right << "\n";
	// It's not a very good idea to modify user inputs directly, so:
	sf::String kstr = state->keyboard_input;

	// Basically event loop but for a string (when pressing many keys at once, more than one key may enter keyboard_input)
	for (int c = 0; c < kstr.getSize(); c++)
	{
		//std::cout << kstr[c] << "\n";
		// Tell display_text() to work
		string_changed = 1;
		// If hit backspace, remove last character
		if (kstr[c] == Ctrl::H) // backspace
		{
			if (string.getSize() != 0)
			{
				if (cursor_position == 0)
					continue;
				cursor_position--;
			}
			kstr.erase(c);
			string.erase(cursor_position);
			// Can only do this if not using 'c' anywhere below
			if (c != 0)
				c--;
			continue;
		}

		// If hit delete, remove cursor_position
		if (kstr[c] == 127)
		{
			if (string.getSize() == 0 || cursor_position > string.getSize() - 1)
				return;
			kstr.erase(c);
			string.erase(cursor_position);
			// Can only do this if not using 'c' anywhere below
			if (c != 0)
				c--;
			continue;
		}

		
		// Put letter where thingy is
		//if (!box_full)
		string.insert(cursor_position, kstr[c]);
		cursor_position++;
	}

	if (state->key.left)
	{
		//std::cout << "left\n";
		if (cursor_position != 0)
			cursor_position--;
	}

	if (state->key.right)
	{
		//std::cout << "right\n";
		if (cursor_position != string.getSize())
			cursor_position++;
	}

	int c = 0, next = 0, line = 0;
	for (int i = 0; i < text.size(); i++)
	{
		next = text[i].text.getString().getSize();
		if (c + next < cursor_position)
			c += next;
		else
		{
			line = i;
			break;
		}
	}

	if (line > 0 && state->key.up)
	{
		float cur = keyboard_cursor.getPosition().x;
		int up_letter = 0;
		for (int i = 0; i < text[line - 1].text.getString().getSize(); i++)
		{
			float right = text[line - 1].text.findCharacterPos(i).x;
			if (right >= cur)
			{
				if (i == 0)
				{
					break;
				}
				float left = text[line - 1].text.findCharacterPos(i - 1).x;
				float left_diff = fabsf(cur - left);
				float right_diff = fabsf(right - cur);
				if (left_diff <= right_diff)
					i--;
				up_letter = i;
				break;
			}
		}
		int size = text[line - 1].text.getString().getSize();
		cursor_position = (c - size) + up_letter;
	}
	//else
	//{
	//	if (key.up)
	//		cursor_position = 0;
	//}

	if (line < text_index && state->key.down)
	{
		//c += text[line].text.getString().getSize();
		float cur = keyboard_cursor.getPosition().x;
		int down_letter = 0;
		for (int i = 0; i < text[line + 1].text.getString().getSize(); i++)
		{
			float right = text[line + 1].text.findCharacterPos(i).x;
			if (right >= cur)
			{
				if (i == 0)
				{
					break;
				}
				float left = text[line + 1].text.findCharacterPos(i - 1).x;
				float left_diff = fabsf(cur - left);
				float right_diff = fabsf(right - cur);
				if (left_diff <= right_diff)
					i--;
				down_letter = i;
				break;
			}
		}
		int size = text[line].text.getString().getSize();
		cursor_position = (c + size) + down_letter;
	}
	//else
	//{
	//	if (key.down)
	//		cursor_position = string.getSize();
	//}
}

void TextInput::display_text()
{
	//std::cout << "display text\n";
	if (!has_user_focus)
		return;

	// Reset string state
	string_changed = 0;

	// If text is changed at all, "refresh" the lines by removing everything and starting from the beginning.
	int space_pos = -1;
	sf::String next_str;
	sf::String str = string;

	// Remove all lines
	for (int i = 0; i <= text_index; i++)
	{
		text[i].set_string("");
	}

	// Init
	text[0].set_string(string);

	// Everything below is for line wrapping
	if (!use_line_wrap)
		return;

	// Continue while the text line is bigger than the rectfield. The entire substring after what is wrapped will be passed to the next line.

	int index = 0, count = 0;
	while (count++ != line_limit && text[index].get_size().x > get_size().x - text[index].padding.x * 2.f)
	{
		//if (index != 0)
		//{
		//	text[index].set_position_by_bounds(text[index - 1].get_bounds(to), from);
		//}
		// Experimental and not very good preventing text from going outside box
		//if (text[index].get_bounds(Bounds::BOTTOM).y > get_bounds(Bounds::BOTTOM).y - text[index].get_size().y)
		//{
		//	// Append to last line and stop adding stuff
		//	text[index].set_string(str);
		//	box_full = 1;
		//	break;
		//}
		//else
		//	box_full = 0;

		// Default for when math doesn't work right
		int last = str.getSize();
		
		// Find the difference between first character of the line and character at i
		sf::Vector2f start = text[index].text.findCharacterPos(0);
		for (int i = 0; i < str.getSize(); i++)
		{
			sf::Vector2f pos = text[index].text.findCharacterPos(i);

			// Whichever character is causing the line wrap is gonna be the start for the space loop (last in line)
			if (abs(pos.x - start.x) > get_size().x - text[index].padding.x * 2)
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
			space_pos = last - 2;
		}
		if (space_pos == -1)
		{
			//std::cout << "No space_pos\n";
			return;
		}

		// Cut current line's string off at where it should be wrapped
		next_str = str.substring(space_pos + 1, str.getSize() - (space_pos + 1));
		str.erase(space_pos + 1, next_str.getSize());
		text[index].set_string(str);

		// Stick what's cut off into next line
		if (index >= text_index)
			add_line();
		index++;
		text[index].set_string(next_str);

		// Make this string the next iteration's problem
		str = next_str;
		
		// Reset to default
		space_pos = -1;
	}

	// Fixes an issue where, after removing text, the most recently added text will be shown over another line. No performance impact found.
	set_alignment(alignment);
	
}

void TextInput::update_cursor()
{
	//std::cout << "update cursor\n";
	// Before anything else, find out where the cursor position is
	// Side note, if for whatever reason the bound texts are not updated first I will need to manually update them

	if (!state->mouse_clicked || !has_user_focus)
		return;

	sf::Vector2f mouse_position = state->window->mapPixelToCoords(state->mouse_screen_position, *view_ptr);
	sf::Vector2f pos;
	int cpos = 0, jay = -1;
	//bool stop = 0;
	for (int i = 0; i < text.size(); i++)
	{
		if (i != 0)
		{
			cpos += text[i - 1].text.getString().getSize();
		}

		text[i].get_hovered();

		if (!text[i].hovered)
			continue;

		float smallest = INFINITY;
		float diff = smallest;
		for (int j = 0; j < text[i].text.getString().getSize(); j++)
		{
			pos = text[i].text.findCharacterPos(j);
			diff = abs(mouse_position.x - pos.x);

			if (diff <= smallest)
			{
				smallest = diff;
				jay = j;
			}
		}
		break;

	}
	if (jay == -1)
		cursor_position = string.getSize();
	else
	{
		cursor_position = jay + cpos;
	}
}


void TextInput::update()
{
	//std::cout << "update\n";
	RectView::update();
	process_input_string(string);
	if (string_changed)
		display_text();
	update_cursor();
}

void TextInput::draw()
{
	//std::cout << "TextInput:\n";
	//std::cout << "draw\n";
	RectField::draw();
	//RectView::draw();

	// Needed since although they are aligned perfectly in the objects vector, there's no easy way to just set the conditions and let it run. Might fix
	for (auto& t : text)
	{
		//std::cout << "TextInput text:\n";
		t.draw();
	}

	int c = 0, next = 0;
	for (int i = 0; i < text.size(); i++)
	{
		next = text[i].text.getString().getSize();
		if (c + next < cursor_position)
			c += next;
		else
		{
			line_in_focus = i;
			break;
		}
	}

	//std::cout << "c: " << c << "\n";

	keyboard_cursor.setPosition(text[line_in_focus].text.findCharacterPos(cursor_position - c));
	if (has_user_focus)
		state->window->draw(keyboard_cursor);
}




////////////////////////////////////////////////////////////////////////////////////////////////////
// Slider
////////////////////////////////////////////////////////////////////////////////////////////////////


Slider::Slider(sf::Vector2f position, sf::Vector2f size, float min, float max)
{
	// Note: max will be (min - max) + min if it's less than min
	name = "Slider";

	set_position(position);
	set_size(size);

	knob.set_size(get_size().y * 2);
	knob.bind(*this);
	knob.catch_focus = 1;
	//knob.ignore_focus = 1;

	knob.set_position_by_bounds(get_bounds(Bounds::CENTER), Bounds::CENTER);

	this->min = min;
	this->max = max;

	tmin.bind(*this);
	tmax.bind(*this);
	tval.bind(*this);

	tmin.set_padding({ knob.get_size(), knob.get_size() });
	tmax.set_padding({ knob.get_size(), knob.get_size() });
	tval.set_padding({ knob.get_size(), knob.get_size() });

	tmin << min;
	tmax << max;
	tval << val;

	::set_vector(vec, tmin, tmax, tval, knob);

	// Access violation reading location
	//tmin.move_vector(vec, *this, 1);
	//tmax.move_vector(vec, *this, 1);
	//tval.move_vector(vec, *this, 1);
	//knob.move_vector(vec, *this, 1);
	obj_vecs.push_back(&vec);
}

void Slider::set_size(sf::Vector2f size)
{
	RectField::set_size(size);

	tmin.set_size(get_size().y * 2);
	tmax.set_size(get_size().y * 2);
	tval.set_size(get_size().y * 2);

	tmin.set_position_by_bounds(get_bounds(Bounds::LEFT), Bounds::RIGHT);
	tmax.set_position_by_bounds(get_bounds(Bounds::RIGHT), Bounds::LEFT);
	tval.set_position_by_bounds(get_bounds(Bounds::TOP), Bounds::BOTTOM);
}

void Slider::set_range(float min, float max, float val)
{
	this->min = min;
	this->max = max;
	
	if (val > 0)
		this->val = max / 2.f;
	else
		this->val = val;

	tmin << min;
	tmax << max;
	tval << std::fixed << std::setprecision(precision) << val;
}


void Slider::update()
{
	// Can reverse the order of these as a potential fix
	RectField::update();
	//state->update(vec);

	if (!state->mouse_down)
		return;

	sf::Vector2f mouse_position = state->window->mapPixelToCoords(state->mouse_screen_position);

	// A problem I'm encountering is being unable to use objects that are part of the class as a way to determine focus since it first requires the object to be updated, which requires some sort of focus. Basically, I need a way to make this CircleField update with the slider. One thing I can do is create a new vector as part of the class and set everything to it, then update these manually. This will basically mean that when slider is updated, everything else gets updated.
	// Oops, this still doesn't solve the issue because it's all hinged on whether the Slider can update. 
	// catch_focus fixes this issue but it might cause other problems with updating overlayed objects
	if (state->mouse_down)
		has_user_focus = 1;
	if (state->mouse_up)
		has_user_focus = 0;

	float size = get_size().x;

	float offset = mouse_position.x - get_position().x;
	if (offset >= get_size().x)
		offset = get_size().x;
	else if (offset <= 0)
		offset = 0;
	float diff = abs(max - min);
	val = offset / get_size().x * diff + min;

	knob.set_position_by_bounds(get_bounds(Bounds::LEFT) + sf::Vector2f(offset, 0.f), Bounds::CENTER);

	tval << std::fixed << std::setprecision(precision) << val;
}

void Slider::draw()
{
	//state->update(vec);
	RectField::draw();
	state->draw_objects_impl(vec);
}