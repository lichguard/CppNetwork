#ifndef _MENU_H
#define _MENU_H

#include <string>
#define MAX_SUB_MENUS 10

class Menu
{
private:
	std::string title;
	int command;

	Menu* parent;
	Menu* child[MAX_SUB_MENUS];
	int selected;

	Menu::Menu(const std::string& Title, Menu* parent, int command = -1);
	Menu(const Menu&) {}
	Menu& operator=(const Menu&) {}
public:
	int register_action(int action, Menu*& current_menu);

	friend void draw_menu(Menu* menu, Menu*& current_menu, int level);
	friend Menu* create_menu();

	~Menu();
};

Menu* create_menu();
void draw_menu(Menu* menu, Menu*& current_menu, int level);
void call_draw_menu(Menu* main_menu, Menu*& current_menu);

#endif // !_MENU_