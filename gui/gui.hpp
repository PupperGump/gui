#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <sstream>
#include <vector>
#include <string>

namespace Bounds
{
	enum
	{
		CENTER = 1 << 0,
		LEFT = 1 << 1,
		RIGHT = 1 << 2,
		TOP = 1 << 3,
		BOTTOM = 1 << 4,
		TOP_LEFT = TOP | LEFT,
		TOP_RIGHT = TOP | RIGHT,
		BOTTOM_LEFT = BOTTOM | LEFT,
		BOTTOM_RIGHT = BOTTOM | RIGHT
	};
}; // namespace Bounds

namespace Align
{
	enum
	{
		LEFT,
		RIGHT,
		CENTER 
	};
}; // namespace Align

// https://en.wikipedia.org/wiki/List_of_Unicode_characters#Control_codes
namespace Ctrl
{
	enum
	{
		A = 1,
		B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z
	};
}; // namespace Ctrl

struct key_code
{
	bool up = 0, down = 0, left = 0, right = 0;

	void reset()
	{
		up = 0;
		down = 0;
		left = 0;
		right = 0;
	}
};

struct mouse_button
{
	bool wheel_y = 0, wheel_x = 0;

	void reset()
	{
		wheel_x = 0;
		wheel_y = 0;
	}
};

class Object;
typedef std::vector<Object*> ObjVec; // So it doesn't look so ugly


template<class... Obj>
void set_vector(ObjVec& object_vector, Obj&... obj)
{
	(obj.set_vector(object_vector), ...);
}

// Base class for gui objects, can't be used before setting up the WindowState.
class Object
{
public:
	bool hovered = 0, mouse_down_with_no_hover = 0, mouse_clicked_with_no_hover = 0, activated = 0, toggled = 0, has_user_focus = 0;
	bool ignore_focus = 0; // Only allow the user to interact with one object at a time. Objects will be drawn in the order they're created and updated in reverse, so the topmost object will "catch" the user and set WindowState::object_focused to 1.

	// Information used to reposition in case of size changes (such as text alignment)
	sf::Vector2f position_reference, new_position, padding; // No padding by default
	unsigned int type = Bounds::TOP_LEFT;
	float scale_x = 1.f;
	float scale_y = 1.f;
	
	// prevents the auto-alignment from overriding direct calls to set_position()
	bool realign = 0;

	// Messy implementation requires lost_focus to be 1 to prevent 'stalling' for 1 click after creating the object.
	bool focus_toggled = 0, lost_focus = 1;

	// When true, sets the bound object's focus to 1
	bool catch_focus = 0;

	// Determines whether calling a function on an object affects a given object that is bound to it
	bool affected_by_bound = 1, string_changed = 0;

	// At the cost of more expensive objects, I've added this for flexibility with bound objects
	sf::View* view_ptr;


	Object();
	~Object();

	// Moves the object to the given index of the given vector
	void move_vector(ObjVec& object_vector, size_t position = 0);
	// Inserts the object at next_to's position and offsets the index by "offset"
	void move_vector(ObjVec& object_vector, Object& next_to, int offset = 0);

	// Removes the object from its current vector and pushes it to the given vector. I recommend not using this on bound objects.
	void set_vector(ObjVec& object_vector);

	// If the current object has other objects bound to it, all of those objects up the tree will be moved to the given vector to prevent draw ordering issues. When unbind_current is 0, the object will not be unbound, leading to undefined behavior.
	void set_vector(ObjVec& object_vector, bool unbind_current);
	
	// Returns 1 if object is bound, otherwise 0
	bool bound();
	// Make this object be affected by all transformations of the object it's bound to. Many objects can be bound to a single object, but you can only bind something to one other object. This will also group them together in the "parent's" vector (the object at the bottom of the tree) and objects will be drawn in the order they are bound and updated in reverse.
	void bind(Object& object);	
	
	// This will put the object at the end of the default vector. Use move_vector() if you want to change this.
	void unbind();

	// Basically sfml wrapper functions that affect bound objects

	virtual void set_position(sf::Vector2f position, bool affect_bound = 1);
	virtual void set_scale(float scale_x, float scale_y = 0.f, bool affect_bound = 1);
	virtual void set_color(sf::Color color, bool affect_bound = 1);

	// Sets the x and y distance between an object's boundaries and its given position. Best used with set_position_by_bounds()
	virtual void set_padding(sf::Vector2f padding);

	virtual sf::Vector2f get_position() = 0;

	//virtual sf::Vector2f get_scale() = 0;


	// Set the given boundary to the position provided. The boundary is top left by default.
	virtual void set_position_by_bounds(sf::Vector2f position, unsigned int type = Bounds::TOP | Bounds::LEFT, float scale_x = 1.f, float scale_y = 1.f) = 0;

	// Get the position of the object relative to its borders or center. Use scale_x and scale_y to multiply the offset from the center. i.e. get_bounds(Bounds::RIGHT, 0.5f, 0.5f) will get the position halfway between the center and the right edge, since it ignores scale_y by starting at the center. However, Bounds::TOP_RIGHT will shift it halfway up and halfway right.
	virtual sf::Vector2f get_bounds(unsigned int type = Bounds::CENTER, float scale_x = 1.f, float scale_y = 1.f) = 0;


	//virtual sf::Color get_color() = 0;







protected:
	friend class WindowState;

	// What the current object is bound to
	Object* bound_to = NULL; 
	// The objects that are bound to the current object
	ObjVec bound_objects; 
	// The vector the object is currently in
	ObjVec* current_vector;
	// Prevents updating/drawing the object when set to true
	bool hide_object = 0;

	void push_vector(ObjVec& object_vector);
	int find_vector(ObjVec& object);
	bool remove_vector(ObjVec& object_vector);

	virtual void get_hovered() {}
	virtual void update() {}
	virtual void draw() {}

	virtual void set_position(sf::Vector2f position, bool is_caller, bool affect_bound);
	virtual void set_scale(float scale_x, float scale_y, bool is_caller, bool affect_bound);
	virtual void set_color(sf::Color color, bool is_caller, bool affect_bound);
	virtual void set_padding(sf::Vector2f padding, bool is_caller);

	virtual void set_position_impl(sf::Vector2f position) = 0;
	virtual void set_scale_impl(float scale_x, float scale_y) = 0;
	virtual void set_color_impl(sf::Color color) = 0;
	virtual void process_input_string() {}
	virtual bool lose_focus_condition();
};


// The manager for user events. You must create your own instance of this and supply it to set_state(). You must also supply the given sf::RenderWindow when creating the state.
class WindowState
{
public:
	// Mouse state
	bool mouse_down = 0, mouse_was_down = 0, mouse_up = 0, mouse_was_up = 0, mouse_clicked = 0;

	sf::Vector2i mouse_screen_position;

	key_code key;
	mouse_button mouse;
	sf::Event::MouseWheelScrollEvent mouse_wheel_scroll;

	// Flag to prevent multiple objects being interacted with at once
	bool object_focused = 0; 
	sf::RenderWindow* window = NULL;
	
	// Vector containing pointers to the objects that will be drawn and updated by default.
	ObjVec objects; 
	std::vector<sf::Font> fonts;
	std::vector<sf::View> views;
	sf::String keyboard_input;
 
	// For set_position(), to move bound objects with the caller object
	sf::Vector2f offset;
	
	// For recursive methods, to catch infinite loops
	Object* caller = NULL; 

	WindowState();
	WindowState(sf::RenderWindow& window);

	// Required when using default constructor
	void set_window(sf::RenderWindow& window);

	void show_all();
	void hide_all();
	void show(Object& object, bool is_caller = 1);
	void hide(Object& object, bool is_caller = 1);
	void show(ObjVec& object_vector);
	void hide(ObjVec& object_vector);


	// Call inside event loop
	void get_events(sf::Event& event);

	// Call after event loop
	void get_state();



	// Call when drawing, will draw all objects in default vector
	void draw_objects();

	// Call this if using set_vector()
	void draw_objects(ObjVec& object_vector);	

	// Do not use. Temporarily public because I can't hide it from users while still sharing it with everything that inherits from Object.
	void update(ObjVec& object_vector);
protected:
};



// Must be called to use WindowState functions
bool set_state(WindowState& state_instance);




// Gets the position of the window boundaries. Default is center.
sf::Vector2f get_window_bounds(unsigned int type = Bounds::CENTER, float scale_x = 1.f, float scale_y = 1.f);



// A rectangle field class that handles boundaries and collision
class RectField : public Object
{
public:
	sf::RectangleShape rect;

	RectField(sf::Vector2f position = { 0, 0 }, sf::Vector2f size = { 100, 100 }, sf::Color color = sf::Color::Blue);

	void set_size(sf::Vector2f size);

	sf::Vector2f get_size();
	
	sf::Vector2f get_position();

	//sf::Vector2f get_scale();

	sf::Vector2f get_bounds(unsigned int type = Bounds::CENTER, float scale_x = 1.f, float scale_y = 1.f);

	// Set the given boundary to the position provided. The boundary is top left by default.
	void set_position_by_bounds(sf::Vector2f position, unsigned int type = Bounds::TOP | Bounds::LEFT, float scale_x = 1.f, float scale_y = 1.f);



protected:
	void get_hovered();
	void update();
	void draw();

	void set_position_impl(sf::Vector2f position);
	void set_color_impl(sf::Color color);
	void set_scale_impl(float scale_x, float scale_y);
};

class RectView : public RectField
{
public:
	float vertical_scroll_speed = 15.f;
	float horizontal_scroll_speed = 15.f;

	RectView(sf::Vector2f viewport_position = { 0.f, 0.f }, sf::Vector2f viewport_size = { 100.f, 100.f }, sf::Vector2f rectfield_size = { 100.f, 100.f }, sf::Color color = sf::Color::Blue);

	void set_viewport(sf::Vector2f viewport_position, sf::Vector2f viewport_size);

	sf::Vector2f get_viewport_bounds(unsigned int type = Bounds::CENTER, float scale_x = 1.f, float scale_y = 1.f);

	sf::Vector2f get_viewport_size();

	void move_view(sf::Vector2f offset);
protected:
	void update();
	void draw();
};


// A rectfield that activates and deactivates when clicked. Doesn't work when rotated or if the size is negative.
class Button : public RectField
{
public:

	Button(sf::Vector2f position = { 0, 0 }, sf::Vector2f size = { 100, 100 }, sf::Color color = sf::Color::Blue);

protected:
	void update();
};


// A circle field class that handles boundaries and collision
class CircleField : public Object
{
public:
	sf::CircleShape circle;

	CircleField(sf::Vector2f position = { 0, 0 }, float size = 100, sf::Color color = sf::Color::Blue);

	void set_size(float size);

	float get_size();

	sf::Vector2f get_position();

	//sf::Vector2f get_scale();

	sf::Vector2f get_bounds(unsigned int type = Bounds::CENTER, float scale_x = 1.f, float scale_y = 1.f);

	// Set the given boundary to the position provided. The boundary is top left by default.
	void set_position_by_bounds(sf::Vector2f position, unsigned int type = Bounds::TOP | Bounds::LEFT, float scale_x = 1.f, float scale_y = 1.f);



protected:
	//friend class Slider;

	void get_hovered();
	void update();
	void draw();

	void set_position_impl(sf::Vector2f position);
	void set_color_impl(sf::Color color);
	void set_scale_impl(float scale_x, float scale_y);
};


// Text ouptut object. All this does is provide the convenience of bounds and perhaps highlighting in the future. You can supply the font to the class with sf::Font::loadFromFile(), but it will be Arial by default.
class Text : public Object
{
public:
	sf::Text text;
	std::stringstream ss;
	bool use_ss = 0;

	Text(sf::Vector2f position = { 0, 0 }, unsigned int font_size = 30u, sf::Color color = sf::Color::White);

	void set_size(unsigned int font_size);

	sf::Vector2f get_size();

	sf::Vector2f get_position();

	// Sets the string and realigns the text
	void set_string(sf::String string);

	sf::Vector2f get_bounds(unsigned int type = Bounds::CENTER, float scale_x = 1.f, float scale_y = 1.f);

	void set_position_by_bounds(sf::Vector2f position, unsigned int type = Bounds::TOP | Bounds::LEFT, float scale_x = 1.f, float scale_y = 1.f);

	// No clue why I need to return ss but I'm not arguing logic with a computer
	template <typename T>
	std::stringstream& operator << (T param)
	{
		use_ss = 1;
		ss << param;
		return ss;
	}


protected:	
	friend class TextInput;
	void update();
	void draw();
	void set_position_impl(sf::Vector2f position);
	void set_color_impl(sf::Color color);
	void set_scale_impl(float scale_x, float scale_y);
	sf::Vector2f recompute_position(sf::Vector2f position, unsigned int type, float scale_x, float scale_y);
	sf::Vector2f uncompute_position(sf::Vector2f position, unsigned int type, float scale_x, float scale_y);
	void get_hovered();
};


// A convenience class that binds a text to a button and centers the text
class TextButton
{
public:
	Button button;
	Text text;

	TextButton(sf::Vector2f position = { 0.f, 0.f }, sf::Vector2f size = { 100.f, 100.f }, sf::Color color = sf::Color::Blue);
};


// A grid of TextButtons within a bounding box
class Menu : public RectField
{
public:
	std::vector<TextButton> buttons;
	unsigned int width = 1, height = 1;
	bool called = 0;
	sf::Vector2f start;

	// Width and height refers to how many buttons in either direction
	Menu(unsigned int width = 1, unsigned int height = 3);

	void fit_rect();
	void set_grid(unsigned int width = 1, unsigned int height = 3);
	void set_padding(sf::Vector2f padding);
};


class TextInput : public RectView
{
public:
	bool use_line_wrap = 1, box_full = 0;
	int line_limit = -1;
	unsigned int alignment = Align::LEFT;

	size_t cursor_position = 0, line_in_focus = 0;
	sf::RectangleShape keyboard_cursor;
	//int letter_limit = 0;
	sf::String string;
	//key_code key;



	TextInput(sf::Vector2f position = { 0.f, 0.f }, sf::Vector2f size = { 300.f, 100.f });

	//void set_color_impl(sf::Color color);

	void set_alignment(unsigned int alignment);

	bool lose_focus_condition();

protected:
	void add_line();

	void remove_line();

	void process_input_string(sf::String& input_string);

	//void line_wrap(sf::String& str);

	void display_text();

	void update_cursor();

	std::vector<Text> text;
	size_t text_index = 0;
	unsigned int from = 0, to = 0;
	void update();
	void draw();
};



class Slider : public RectField
{
public:
	std::ostringstream ss;
	ObjVec vec;
	Text tmin, tmax, tval;
	CircleField knob;
	float min = 0.f, max = 0.f, val = max / 2.f;
	int precision = 2;
	//bool fix_the_stupid_bound_bug = 1;
	Slider(sf::Vector2f position = { 0.f, 0.f }, sf::Vector2f size = { 100.f, 20.f }, float min = 0, float max = 100);

	void update();

	void draw();
};