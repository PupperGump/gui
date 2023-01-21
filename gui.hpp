#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
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

class Object;
typedef std::vector<Object*> ObjVec; // So it doesn't look so ugly


// Base class for gui objects, must be declared after setting up the WindowState.
class Object
{
public:
	bool hovered = 0, mouse_down_with_no_hover = 0, mouse_clicked_with_no_hover = 0, activated = 0, toggled = 0, has_user_focus = 0;
	bool ignore_focus = 0; // Only allow the user to interact with one object at a time. Objects will be drawn in the order they're created and updated in reverse, so the topmost object will "catch" the user and set WindowState::object_focused to 1.
	Object* bound_to = NULL;
	ObjVec bound_objects; // The objects that are bound to the current object
	ObjVec* current_vector; // The vector the object is currently in
	bool hide_object = 0; // Prevents updating/drawing the object when set to true

	// Information used to reposition in case of size changes (such as text alignment)
	sf::Vector2f position_reference, new_position, padding; // No padding by default
	unsigned int type = Bounds::TOP_LEFT;
	float scale_x = 1.f;
	float scale_y = 1.f;
	bool set_alignment = 0; // prevents the auto-alignment from overriding direct calls to set_position()

	// Messy implementation requires lost_focus to be 1 to prevent 'stalling' for 1 click after creating the object.
	bool focus_toggled = 0, lost_focus = 1;

	Object();
	~Object();

	virtual void set_position(sf::Vector2f position);
	virtual void set_scale(float scale_x, float scale_y = 0.f);
	virtual void set_color(sf::Color color1);
	virtual void set_padding(sf::Vector2f padding);

	virtual sf::Vector2f get_position() = 0;

	//virtual sf::Vector2f get_scale() = 0;


	// Set the given boundary to the position provided. The boundary is top left by default.
	virtual void set_position_by_bounds(sf::Vector2f position, unsigned int type = Bounds::TOP | Bounds::LEFT, float scale_x = 1.f, float scale_y = 1.f) = 0;

	// Get the position of the object relative to its borders or center. Use scale_x and scale_y to multiply the offset from the center. i.e. get_bounds(Bounds::RIGHT, 0.5f, 0.5f) will get the position halfway between the center and the right edge, since it ignores scale_y by starting at the center. However, adding Bounds::TOP will shift it halfway up.
	virtual sf::Vector2f get_bounds(unsigned int type = Bounds::CENTER, float scale_x = 1.f, float scale_y = 1.f) = 0;


	//virtual sf::Color get_color() = 0;




	void move_vector(ObjVec& object_vector, int position = 0);
	void move_vector(ObjVec& object_vector, Object& next_to, int position = 0);

	//Removes the object from its current vectorand pushes it to the given vector.
	void set_vector(ObjVec& object_vector);

	// If the current object has other objects bound to it, all of those objects up the tree will be moved to the given vector to prevent draw ordering issues. When unbind_current is 0, the object will not be unbound, leading to undefined behavior.
	void set_vector(ObjVec& object_vector, bool unbind_current);
	
	bool bound();
	void bind(Object& object);

	// This will put the object at the end of the default vector. Use move_vector() if you want to change this.
	void unbind();

private:
	friend class WindowState;

	void push_vector(ObjVec& object_vector);
	int find_vector(ObjVec& object);
	bool remove_vector(ObjVec& object_vector);

	virtual void get_hovered() {}
	virtual void update();
	virtual void draw() {}

	virtual void set_position(sf::Vector2f position, bool is_caller);
	virtual void set_scale(float scale_x, float scale_y, bool is_caller);
	virtual void set_color(sf::Color color, bool affect_bound);
	virtual void set_padding(sf::Vector2f padding, bool affect_bound);

	virtual void set_position_impl(sf::Vector2f position) = 0;
	virtual void set_scale_impl(float scale_x, float scale_y) = 0;
	virtual void set_color_impl(sf::Color color) = 0;
	virtual void set_padding_impl(sf::Vector2f padding) {}
	virtual void process_input_string() {}
};


void show_all();
void hide_all();
void show(Object& object, bool is_caller = 1);
void hide(Object& object, bool is_caller = 1);
void show(ObjVec& object_vector);
void hide(ObjVec& object_vector);



// The manager for user events. You must create your own instance of this and supply it to set_state(). You must also supply the given sf::RenderWindow when creating the state.
class WindowState
{
public:
	// Mouse state
	bool mouse_down = 0, mouse_was_down = 0, mouse_up = 0, mouse_was_up = 0, mouse_clicked = 0;

	sf::Vector2f mouse_position;

	// Flag to prevent multiple objects being interacted with at once
	bool object_focused = 0; 
	sf::RenderWindow* window = NULL;
	
	// Vector containing pointers to the objects that will be drawn and updated by default.
	ObjVec objects; 
	std::vector<sf::Font> fonts;
	sf::String keyboard_input;
 
	// For set_position(), to move bound objects with the caller object
	sf::Vector2f offset;
	
	// For recursive methods, to catch infinite loops
	Object* caller = NULL; 

	WindowState();
	WindowState(sf::RenderWindow& window);

	void set_window(sf::RenderWindow& window);

	// Call inside event loop
	void get_events(sf::Event& event);

	// Call after event loop
	void get_state();

	// update default vector
	void update();

	// Only needs to be called if using set_vector()
	void update(ObjVec& object_vector);

	// Call when drawing, will draw all objects in default vector
	void draw_objects();

	// Call this after draw_objects() if using set_vector()
	void draw_objects(ObjVec& object_vector);

	bool add_font(sf::Font font);

};



// Must be called to use WindowState functions
bool set_state(WindowState& state_input);




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

	//void set_position(sf::Vector2f position);
	void set_position_impl(sf::Vector2f position);

	sf::Vector2f get_position();

	//void set_scale(float scale, float scale_y);

	//sf::Vector2f get_scale();

	sf::Vector2f get_bounds(unsigned int type = Bounds::CENTER, float scale_x = 1.f, float scale_y = 1.f);

	// Set the given boundary to the position provided. The boundary is top left by default.
	void set_position_by_bounds(sf::Vector2f position, unsigned int type = Bounds::TOP | Bounds::LEFT, float scale_x = 1.f, float scale_y = 1.f);

	void set_color_impl(sf::Color color);

	void set_scale_impl(float scale_x, float scale_y);

	void get_hovered();

	bool is_focused();

	void update();

	void draw();
};

// A rectfield that activates and deactivates when clicked. Doesn't work when rotated or if the size is negative.
class Button : public RectField
{
public:
	bool on = 0;
	Button(sf::Vector2f position = { 0, 0 }, sf::Vector2f size = { 100, 100 }, sf::Color color = sf::Color::Blue);

	void update();
};


// Text ouptut object. All this does is provide the convenience of bounds and perhaps highlighting in the future. You can supply the font to the class with sf::Font::loadFromFile(), but it will be Arial by default.
class Text : public Object
{
public:
	sf::Text text;


	//Text();

	Text(sf::Vector2f position = { 0, 0 }, unsigned int font_size = 30u, sf::Color color = sf::Color::White);

	void set_size(unsigned int font_size);

	sf::Vector2f get_size();

	//void set_position(sf::Vector2f position);
	void set_position_impl(sf::Vector2f position);

	sf::Vector2f get_position();

	// Use this to keep the text aligned when changing the string
	void set_string(sf::String string);

	sf::Vector2f get_bounds(unsigned int type = Bounds::CENTER, float scale_x = 1.f, float scale_y = 1.f);

	// Set the given boundary to the position provided. The boundary is top left by default.
	void set_position_by_bounds(sf::Vector2f position, unsigned int type = Bounds::TOP | Bounds::LEFT, float scale_x = 1.f, float scale_y = 1.f);

	void set_color_impl(sf::Color color);

	void set_scale_impl(float scale_x, float scale_y);

	void set_padding_impl(sf::Vector2f padding);

	//void get_hovered();

	void update();

	void draw();

private:
	sf::Vector2f recompute_position(sf::Vector2f position, unsigned int type, float scale_x, float scale_y);
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
	//Menu();
	Menu(unsigned int width = 1, unsigned int height = 3);

	void fit_rect();
	void set_grid(unsigned int width = 1, unsigned int height = 3);
	void set_padding(sf::Vector2f padding);
};


class TextInput : public RectField
{
public:
	std::vector<Text> text;
	size_t text_index = 0;
	//unsigned int alignment = Align::LEFT;
	unsigned int from = 0, to = 0;
	bool use_line_wrap = 1, limit_lines_to_rect = 0;


	TextInput(sf::Vector2f position = { 0.f, 0.f }, sf::Vector2f size = { 300.f, 100.f });

	//void set_color_impl(sf::Color color);

	void set_alignment(unsigned int alignment);

	void add_line();

	void remove_line();

	void process_input_string();

	void update();

	void draw();
};