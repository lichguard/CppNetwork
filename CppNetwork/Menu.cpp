#include "Menu.h"
#include "PVL.h"
#include <iostream>
using std::cout;
using std::endl;
using std::string;

Menu::Menu(const string& Title, Menu* parent, int command)
{
	for (int i = 0; i < MAX_SUB_MENUS; i++)
		this->child[i] = NULL;

	this->selected = 0;
	this->title = Title;
	this->parent = parent;
	this->command = command;
}

void draw_menu(Menu* menu, Menu*& current_menu, int level)
{
	//recursion
	if (menu == current_menu)
	{
		//deepest level of the menu, highlights the selected also the stop condition
		for (int i = 0; i < MAX_SUB_MENUS && menu->child[i] != NULL; i++)
		{
			if (i == menu->selected)
				cout << pvl::padString("", 22 + level * 5, '-');

			cout << endl << pvl::padString("", level * 5, ' ') << "> " << menu->child[i]->title << endl;

			if (i == menu->selected)
				cout << pvl::padString("", 22 + level * 5, '-');

		}
	}
	else
	{
		for (int i = 0; i < MAX_SUB_MENUS && menu->child[i] != NULL; i++)
		{
			cout << endl << pvl::padString("", level * 5, ' ') << "> " << menu->child[i]->title << endl;
			if (menu->selected == i) break;
		}

		draw_menu(menu->child[menu->selected], current_menu, level + 1);

		for (int i = menu->selected + 1; i < MAX_SUB_MENUS && menu->child[i] != NULL; i++)
			cout << endl << pvl::padString("", level * 5, ' ') << "> " << menu->child[i]->title << endl;
	}

}

void call_draw_menu(Menu* main_menu, Menu*& current_menu)
{
	cout << "======= " << "Main Menu" << " =======" << endl;
	draw_menu(main_menu, current_menu, 0);
	cout << endl << "===========================";
}

int Menu::register_action(int action, Menu*& current_menu)
{
	//down
	if (action == 72)
	{
		if (this->selected > 0)
			this->selected--;
		else
		{
			while (this->selected < MAX_SUB_MENUS - 1 && this->child[(this->selected) + 1] != NULL)
				this->selected++;
		}
	}
	//up
	else if (action == 80)
	{

		if (this->selected < MAX_SUB_MENUS - 1 && this->child[(this->selected) + 1] != NULL)
			this->selected++;
		else
			this->selected = 0;

	}
	//enter
	else if (action == 13)
	{
		if (this->child[this->selected] != NULL)
		{
			if (this->child[this->selected]->command != commands::Passive) //if (this->child[this->selected]->function != NULL)
			{
				if (this->child[this->selected]->command == commands::Back_To_Chat)
					return 0;

				return (this->child[this->selected]->command);
			}
			else
				current_menu = this->child[this->selected];
		}

	}
	//backspace or escape
	else if (action == 8 || action == 27)
	{
		if (this->parent != NULL)
			current_menu = (this->parent);
		else
			return 0;

	}

	return commands::Passive;
}

Menu* create_menu()
{

	Menu* menu = new Menu("Main Menu", NULL);
	Menu& main_menu = *menu;

	main_menu.child[0] = new Menu("Network", &main_menu);
	main_menu.child[0]->child[0] = new Menu("Connect", main_menu.child[0], commands::Connect);
	main_menu.child[0]->child[1] = new Menu("Disconnect", main_menu.child[0], commands::Disconnect);
	main_menu.child[0]->child[2] = new Menu("IP", main_menu.child[0], commands::Modify_IP);
	main_menu.child[0]->child[3] = new Menu("Port", main_menu.child[0], commands::Modify_Port);

	main_menu.child[1] = new Menu("Options", &main_menu);
	main_menu.child[1]->child[0] = new Menu("Clear Chat", main_menu.child[0]->child[1], commands::Clear_UI);
	main_menu.child[1]->child[1] = new Menu("Change size of history", main_menu.child[0]->child[1], commands::SizeOfChatHistory);

	main_menu.child[2] = new Menu("Stats", &main_menu);
	main_menu.child[3] = new Menu("Back to Chat", &main_menu, commands::Back_To_Chat);
	main_menu.child[4] = new Menu("Exit", &main_menu, commands::Exit);

	return menu;
}

Menu::~Menu()
{
	for (int i = 0; i < MAX_SUB_MENUS; i++)
	{
		if (!this->child[i]) // == NULL
			break;

		delete this->child[i];
	}
}