#include <iostream>
#include "TcpClient.h"
#include <thread>
#include <Windows.h>
#include <string>
#include <vector>
#include <conio.h>
#include "Menu.h"
#include "PVL.h"

using namespace std;


//vector<string> messeges;
char messeges[10][256] = { 0 };
int last_messege_index = 0;
TcpClient client;

string message_To_send = "";
string last_messege = "";
string user_name = "default1";
string port = "27015";
string ip = "10.0.0.2";//"127.0.0.1";
string password = "";

bool menu_open = false;

void process_script();
void autoconnect();
void  print_UI();

void print_UI()
{
	if (menu_open)
		return;

	system("cls");
	cout << "------------------CHAT ROOM---------------                  ONLINE" << endl;// << endl;

	//if (client.isConnected())
	//	cout << pvl::padString("", 60) << "You" << endl;
	int interim_index = last_messege_index;
	for (int i = 0; i < 10; i++)
	{
		interim_index--;
		if (interim_index == -1)
			interim_index = 9;

		cout << messeges[interim_index];
		for (size_t j = strlen(messeges[i]); j < 60; j++)
			cout << " ";

		cout << endl;
	}

	cout << "Messege: " << message_To_send;
}

/*
void push_new_messege(const std::string& new_messege)
{
	last_messege_index--;
	if (last_messege_index == -1)
		last_messege_index = 9;

	memcpy(messeges[last_messege_index], new_messege.c_str(), new_messege.size());
	memset(messeges[last_messege_index] + new_messege.size(), 0, 256 - new_messege.size());
	print_UI();
}
*/
void push_new_messege(const char *new_messege)
{
	last_messege_index--;
	if (last_messege_index == -1)
		last_messege_index = 9;

	memcpy(messeges[last_messege_index], new_messege, strlen(new_messege)+1);
	print_UI();
}

void recieve_input(string& text, bool allow_menu = true, bool password = false)
{
	int input;
	text = "";
	input = _getch();
	//if escaped was pressed
	if (input == 27 && allow_menu)
	{
		last_messege = "/o";
		process_script();
	}
	while (input != 13)
	{
		// if backspace was pressed
		if (input != 8)
		{
			text += static_cast<char>(input);
			cout << (password ? '*' : static_cast<char>(input));
		}
		input = _getch();
		if (input == 8 && text.length() > 0)
		{
			_putch('\b');
			cout << " ";
			_putch('\b');
			text.erase(text.end() - 1);
		}
		if (input == 27 && allow_menu)
		{
			last_messege = "/o";
			process_script();
		}
	}

	last_messege = message_To_send;
	message_To_send = "";
	print_UI();
}

void autoconnect()
{
	if (client.isConnected())
	return;
	string data = "";

	print_UI();
	cout << endl << "Your Name: ";
	recieve_input(message_To_send,false);
	user_name = last_messege;
	cout << endl << "Your password: ";
	recieve_input(message_To_send,false,true);
	password = last_messege;
	try
	{
		client.Connect(user_name, password,ip,port);
	}
	catch (const char* msg)
	{
		push_new_messege(std::string("Error connecting : " + std::string(msg)).c_str());
	}
}

void execute_command(int cmd)
{
	switch (cmd)
	{
	case commands::Clear_UI:
		//messeges.clear();
		//return commands::Back_To_Chat;
		break;
	case commands::Connect:
		if (client.isConnected())
		{
			cout << endl << "Already connected!" << endl;
	
		}
		else
		{
			autoconnect();
		//	return commands::Back_To_Chat;
		}
		break;
	case commands::Disconnect:
		if (!client.isConnected())
		{
			cout << endl << "Already disconnected!" << endl;
		}
		else
		{
			client.CloseConnection();
			//return commands::Back_To_Chat;
		}
		break;
	case commands::Exit:
		client.CloseConnection();
		exit(0);
		break;
	case commands::SizeOfChatHistory:
		cout << endl << "OBSOLETE: New chat size (old " << 0 << " ):";
//			std::cin >> size_of_chat_history;
//			if (size_of_chat_history <= 0 || size_of_chat_history >= 50)
//				size_of_chat_history = 9;
		//	return commands::Back_To_Chat;
		break;

	case commands::Modify_IP:
		if (client.isConnected())
		{
			cout << endl <<  "Cannot modify while connected!" << endl;
			return;
		}
		else
		{
			cout << endl << "IP (" << ip << "): ";
			cin >> ip;
		}
		break;

	case commands::Modify_Port:
		if (client.isConnected())
		{
			cout << endl << "Cannot modify while connected!" << endl;
			return;
		}
		else
		{
			cout << endl << "Port (" << port << "): ";
			cin >> port;
		}
		break;
	default:
	
		break;
	}
//	return cmd;


}

void process_script()
{
	if (last_messege == "/o")
	{
		menu_open = true;
		Menu* main_menu = NULL;
		Menu* current_menu = NULL;

		main_menu = current_menu = create_menu();

		system("cls");
		call_draw_menu(main_menu, current_menu);
		cout << "===========================";
		cout << endl << endl;
		cout << "Controls: backspace/esc, enter, arrows up/down";

		//gui action returns 0 to exit the gui
		int cmd = commands::Passive;
		do
		{
			cmd = current_menu->register_action(_getch(), current_menu);
			system("cls");
			call_draw_menu(main_menu, current_menu);
			cout << "===========================";

			execute_command(cmd);

			if(cmd != commands::Passive)
			cout << endl << endl << "Operation completed";

		} while (cmd != commands::Back_To_Chat);
		delete main_menu;
		menu_open = false;
	}
	else if (last_messege == "/clear")
		execute_command(commands::Clear_UI);
	else if (last_messege == "/disconnect")
		execute_command(commands::Disconnect);
	else if (last_messege == "/connect")
		execute_command(commands::Connect);
	else if (last_messege == "/exit")
		execute_command(commands::Exit);

}

void start_network()
{
	autoconnect();

	while (last_messege != "/exit")
	{
		recieve_input(message_To_send);
		if (last_messege[0] == '/')
		{
			process_script();
			print_UI();
		}
		else
			if (last_messege != "" && last_messege != " ")
				client.Send(trassmision::normal, last_messege);

		//print_UI();
	}

	client.CloseConnection();
}

int main()
{
	start_network();
	return 0;
}