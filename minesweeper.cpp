// minesweeper.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

// Minesweeper
// A simple minesweeper game with a beautiful tui written in C++
// Author: CR24
// Version: 1.5.0

// THIS PROGRAM ONLY HAS SUPPORT FOR WINDOWS MACHINES, SORRY

#define VERSION "v1.5.0"

#include <iostream>
#include <time.h> // For time()
#include <chrono> // For clock
#include <Windows.h> // For getWindowSize() && updateScreen();
#include <conio.h> // For keyboard controls (windows only)
#include <iomanip> // For printing float values formatted

using namespace std;

struct cell
{
	bool revealed = false;
	bool isMine = false;
	int mine_count = 0;
	bool exploded = false;
	bool flagged = false;
	int flag_count = 0;
};

bool running = true;
bool gameOver = false;
bool won = false;
int tmp_x = 0, tmp_y = 0;
int revealed_cells = 0;
int flags = 0;
auto start = chrono::high_resolution_clock::now();
int window_x, window_y;
int old_window_x = 0, old_window_y = 0;

// Settings
bool setting_showKeybinds = 1;
int setting_flower_icon = 0;
int setting_cells_style = 1;
int time_played = 0;

void setCursor(short x, short y) {
	cout << "\033[" << y << ";" << x << "H";
}

void getWindowSize() {

	old_window_x = window_x;
	old_window_y = window_y;

	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	window_x = csbi.srWindow.Right - csbi.srWindow.Left + 1;
	window_y = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
	if (old_window_x != window_x || old_window_y != window_y)
		system("cls");
}

void updateScreen() {
	cout << "\033[H";
}

string toGlyph(const cell& key) {

	if (key.exploded && key.revealed)
		return u8"💥\033[0m";

	if (key.flagged && key.revealed && gameOver && !key.isMine)
		return u8"❌\033[0m";

	if (key.isMine && gameOver && won) {
		switch (setting_flower_icon) {
			case 0:
				return u8"🌸\033[0m";
				break;
			case 1:
				return u8"🪷\033[0m";
				break;
			case 2:
				return u8"🌹\033[0m";
				break;
			case 3:
				return u8"🌷\033[0m";
				break;
			case 4:
				return u8"🌼\033[0m";
				break;
			case 5:
				return u8"🌻\033[0m";
				break;
			default:
				return u8"🌸\033[0m";
		}
	}

	if (key.flagged || key.flagged && key.isMine)
		return u8"🚩\033[0m";

	if (key.isMine && gameOver && !won)
		return u8"💣\033[0m";

	if (key.revealed && won)
		return u8"　\033[0m";

	if (!key.revealed || key.mine_count == 0)
		return u8"　\033[0m";

	if (key.mine_count == 1)
		return u8"１\033[0m";

	if (key.mine_count == 2)
		return u8"２\033[0m";

	if (key.mine_count == 3)
		return u8"３\033[0m";

	if (key.mine_count == 4)
		return u8"４\033[0m";

	if (key.mine_count == 5)
		return u8"５\033[0m";

	if (key.mine_count == 6)
		return u8"６\033[0m";

	if (key.mine_count == 7)
		return u8"７\033[0m";

	if (key.mine_count == 8)
		return u8"８\033[0m";

	if (key.mine_count == 9)
		return u8"９\033[0m";

	if (!key.isMine && won)
		return u8"　\033[0m";
}

void buttonColor(int button, int screen) {
	if (tmp_y == button)
	{
		cout << "\033[41m";
	}
	if (tmp_x-1 == button && screen == 3)
	{
		cout << "\033[42m";
	}
}

void saveSettings() {

	FILE* settings;

	if (fopen_s(&settings, "settings.txt", "w") != 0) {
		cerr << "Error: An error occurred when writing to file 'settings.txt'\n";
		system("pause");
		return;
	}

	fprintf_s(settings, "ShowKeybinds=%d\nFlowerIcon=%d\nCellsCountStyle=%d\nTimePlayed=%d",
		setting_showKeybinds,
		setting_flower_icon,
		setting_cells_style,
		time_played
	);
	fclose(settings);
}


void loadSettings() {

	FILE* settings;

	// Default values when starting the program for the first time
	if (fopen_s(&settings, "settings.txt", "r") != NULL) {
		cout << "Settings file doesn't exist, setting everything to default.\n";
		system("pause");

		if ((fopen_s(&settings, "settings.txt", "w")) != NULL) {
			cerr << "Error: Cannot open file 'settings.txt'\n";
			return;
		}

		// Write default values to settings.txt
		fprintf_s(settings, "ShowKeybinds=%d\nFlowerIcon=%d\nCellsCountStyle=%d\nTimePlayed=%d",
			setting_showKeybinds,
			setting_flower_icon,
			setting_cells_style,
			time_played
		);
	}
	fclose(settings);

	// Read settings.txt
	if ((fopen_s(&settings, "settings.txt", "r")) != NULL) {
		cerr << "Error: Cannot open file 'settings.txt'";
		return;
	}

	int tmp_bool = 0;
	fscanf_s(settings, "ShowKeybinds=%d\nFlowerIcon=%d\nCellsCountStyle=%d\nTimePlayed=%d",
		&tmp_bool,
		&setting_flower_icon,
		&setting_cells_style,
		&time_played
	);
	setting_showKeybinds = (tmp_bool != 0);
	fclose(settings);
}

void menu(int screen, int& cells_x, int& cells_y, int& mines) {

	// Function drawing all the menu tuis

#ifdef _DEBUG
	cout << "X: " << tmp_x << endl;
	cout << "Y: " << tmp_y << endl;
	cout << "window_x: " << window_x << endl;
	cout << "window_y: " << window_y << endl;
	cout << "screen: " << screen << endl;
#endif // DEBUG
	if (screen == 0)
	{
		// Main menu
		updateScreen();
		getWindowSize();

		setCursor(((window_x / 2) - (9)), ((window_y / 2) - 2));
		cout << u8"\033[0m🚩\033[0m" << "\033[1m\033[3m Minesweeper \033[0m" << u8"\033[0m🚩\033[0m\n\n";

		cout << "\033[" << ((window_x / 2) - 2) << "G";
		buttonColor(0, screen);
		cout << "PLAY" << "\033[0m\n";

		cout << "\033[" << ((window_x / 2) - 4) << "G";
		buttonColor(1, screen);
		cout << "SETTINGS" << "\033[0m\n";

		cout << "\033[" << ((window_x / 2) - 2) << "G";
		buttonColor(2, screen);
		cout << "EXIT" << "\033[0m\n";

		// Version Info and credits
		#ifdef _DEBUG
		#else
		setCursor(0, window_y);
		cout << VERSION;
		setCursor(window_x - 5, window_y);
		cout << u8"©️CR24";
		#endif
	}

	if (screen == 1)
	{
		// Play/Difficulty menu
		updateScreen();
		getWindowSize();

		setCursor(((window_x / 2) - (17 / 2)), ((window_y / 2) - 3));
		cout << u8"\033[0m🚩\033[0m" << "\033[1m\033[3m Difficulty \033[0m" << u8"\033[0m🚩\033[0m\n\n";

		cout << "\033[" << ((window_x / 2) - 4) << "G";
		buttonColor(0, screen);
		cout << "Beginner" << "\033[0m\n";

		cout << "\033[" << ((window_x / 2) - 6) << "G";
		buttonColor(1, screen);
		cout << "Intermediate" << "\033[0m\n";

		cout << "\033[" << ((window_x / 2) - 3) << "G";
		buttonColor(2, screen);
		cout << "Expert" << "\033[0m\n";

		cout << "\033[" << ((window_x / 2) - 3) << "G";
		buttonColor(3, screen);
		cout << "Custom" << "\033[0m\n\n";

		cout << "\033[" << ((window_x / 2) - 2) << "G";
		buttonColor(4, screen);
		cout << "Back" << "\033[0m\n";
	}

	if (screen == 2)
	{
		updateScreen();
		getWindowSize();

		setCursor(((window_x / 2) - (14 / 2)), ((window_y / 2) - 5));
		cout << u8"\033[0m🚩\033[0m" << "\033[1m\033[3m Settings \033[0m" << u8"\033[0m🚩\033[0m\n\n";

		cout << "\033[" << ((window_x / 2) - 8) << "G";
		buttonColor(0, screen);
		cout << "Show keybinds\033[0m ";
		if (setting_showKeybinds) cout << u8"✅";
		else cout << u8"❌";
		cout << "\n\n";

		cout << "\033[" << ((window_x / 2) - 14) << "G";
		buttonColor(1, screen);
		cout << "Flower icon style\033[0m ";
		if (setting_flower_icon == 0) cout << "\033[48;2;35;160;20m";
		cout << u8"🌸\033[0m";
		if (setting_flower_icon == 1) cout << "\033[48;2;35;160;20m";
		cout << u8"🪷\033[0m";
		if (setting_flower_icon == 2) cout << "\033[48;2;35;160;20m";
		cout << u8"🌹\033[0m";
		if (setting_flower_icon == 3) cout << "\033[48;2;35;160;20m";
		cout << u8"🌷\033[0m";
		if (setting_flower_icon == 4) cout << "\033[48;2;35;160;20m";
		cout << u8"🌼\033[0m";
		if (setting_flower_icon == 5) cout << "\033[48;2;35;160;20m";
		cout << u8"🌻\033[0m";
		cout << "\n\n";

		cout << "\033[" << ((window_x / 2) - 18) << "G";
		buttonColor(2, screen);
		cout << "Revealed cells display style" << "\033[0m ";
		buttonColor(2, screen);
		if (setting_cells_style == 0) cout << "\033[3mHidden";
		if (setting_cells_style == 1) cout << "\033[3mDecimals";
		if (setting_cells_style == 2) cout << "\033[3mPercentage";
		cout << "\033[0m\n\n";

		cout << "\033[" << ((window_x / 2) - 2) << "G";
		buttonColor(3, screen);
		cout << "Info" << "\033[0m\n\n";

		cout << "\033[" << ((window_x / 2) - 2) << "G";
		buttonColor(4, screen);
		cout << "Back" << "\033[0m\n";
	}

	if (screen == 3)
	{
		// Custom difficulty menu
		updateScreen();
		getWindowSize();

		setCursor(((window_x / 2) - (23 / 2)), ((window_y / 2) - 3));
		cout << u8"\033[0m🚩\033[0m" << "\033[1m\033[3m Custom difficulty \033[0m" << u8"\033[0m🚩\033[0m\n\n";

		cout << "\033[" << ((window_x / 2) - 8) << "G";
		buttonColor(0, screen);
		cout << "Board width:" << "\033[" << ((window_x / 2) + 8) << "G" << cells_x << "\033[0m\n";

		cout << "\033[" << ((window_x / 2) - 8) << "G";
		buttonColor(1, screen);
		cout << "Board height:" << "\033[" << ((window_x / 2) + 8) << "G" << cells_y << "\033[0m\n";

		cout << "\033[" << ((window_x / 2) - 8) << "G";
		buttonColor(2, screen);
		cout << "Mines:" << "\033[" << ((window_x / 2) + 8) << "G" << mines << "\033[0m\n";

		cout << "\033[" << ((window_x / 2) - 3) << "G";
		buttonColor(3, screen);
		cout << "Confirm" << "\033[0m\n\n";

		cout << "\033[" << ((window_x / 2) - 2) << "G";
		buttonColor(4, screen);
		cout << "Back" << "\033[0m\n";
	}

	if (screen == 4)
	{
		updateScreen();
		getWindowSize();

		setCursor(((window_x / 2) - 5), ((window_y / 2) - 6));
		cout << u8"\033[0m🚩\033[0m" << "\033[1m\033[3m Info \033[0m" << u8"\033[0m🚩\033[0m\n\n";

		cout << "\033[" << ((window_x / 2) - 23) << "G";
		cout << "A simple minesweeper game with a beautiful tui!";
		cout << "\n\n";

		cout << "\033[" << ((window_x / 2) - 10) << "G";
		cout << "Time played: " << fixed << setprecision(2) << static_cast<float>(time_played)/60 << " min";
		cout << "\n\n";

		cout << "\033[" << ((window_x / 2) - 26) << "G";
		cout << "You can check out the source code of this game here:";
		cout << "\n";
		cout << "\033[" << ((window_x / 2) - 20) << "G";
		cout << "https://github.com/blurrycmd/minesweeper";
		cout << "\n\n";

		cout << "\033[" << ((window_x / 2) - 9) << "G";
		cout << "Minesweeper " << VERSION;
		cout << "\n";
		cout << "\033[" << ((window_x / 2) - 6) << "G";
		cout << "Made by CR24";
		cout << "\n\n";

		cout << "\033[" << ((window_x / 2) - 2) << "G";
		buttonColor(0, screen);
		cout << "Back" << "\033[0m\n";
	}
}

void kbmenu(int menuScreen, int& difficulty, int& cells_x, int& cells_y, int& mines) {
	// Keyboard controls for the menus
	while (menuScreen != -1) {
		if (_kbhit())
		{
			switch (_getch())
			{
			case 'w':
			case 'k':
			case 72:
				if (tmp_x == 0 && menuScreen != 4)
					tmp_y--;
				if (tmp_y < 0 && menuScreen == 0)
					tmp_y = 2;
				if (tmp_y < 0 && menuScreen == 1 || menuScreen == 3 && tmp_y < 0 && tmp_x == 0 || tmp_y < 0 && menuScreen == 2)
					tmp_y = 4;
				if (menuScreen == 3) {
					if (tmp_x == 1)
					{
						cells_x++;
						if (cells_x > 30 || cells_x < 1)
							cells_x = 1;
						if (mines > (((cells_x - 1) * (cells_y - 1)) + 1))
							mines = (((cells_x - 1) * (cells_y - 1)) + 1);
						system("cls");
					}
					if (tmp_x == 2)
					{
						cells_y++;
						if (cells_y > 24 || cells_y < 1)
							cells_y = 1;
						if (mines > (((cells_x - 1) * (cells_y - 1)) + 1))
							mines = (((cells_x - 1) * (cells_y - 1)) + 1);
						system("cls");
					}
					if (tmp_x == 3)
					{
						mines++;
						if (mines > (((cells_x - 1) * (cells_y - 1)) + 1))
							mines = 0;
						if (mines > (((cells_x - 1) * (cells_y - 1)) + 1))
							mines = (((cells_x - 1) * (cells_y - 1)) + 1);
						system("cls");
					}
				}
				menu(menuScreen, cells_x, cells_y, mines);
				break;
			case 's':
			case 'j':
			case 80:
				if (tmp_x == 0 && menuScreen != 4)
					tmp_y++;
				if (tmp_y > 2 && menuScreen == 0)
					tmp_y = 0;
				if (tmp_y > 4 && menuScreen == 1 || tmp_y > 4 && menuScreen == 2 || menuScreen == 3 && tmp_y > 4 && tmp_x == 0)
					tmp_y = 0;
				if (menuScreen == 3) {
					if (tmp_x == 1)
					{
						cells_x--;
						if (cells_x < 1)
							cells_x = 30;
						if (mines > (((cells_x - 1) * (cells_y - 1)) + 1))
							mines = (((cells_x - 1) * (cells_y - 1)) + 1);
						system("cls");
					}
					if (tmp_x == 2)
					{
						cells_y--;
						if (cells_y < 1)
							cells_y = 24;
						if (mines > (((cells_x - 1) * (cells_y - 1)) + 1))
							mines = (((cells_x - 1) * (cells_y - 1)) + 1);
						system("cls");
					}
					if (tmp_x == 3)
					{
						mines--;
						if (mines < 0)
							mines = ((cells_x - 1) * (cells_y - 1)) + 1;
						system("cls");
					}
				}
				menu(menuScreen, cells_x, cells_y, mines);
				break;
			case 13:
			case ' ':
				// This code defines what should happen on a click of each menu button
				if (menuScreen == 0) // Main menu
				{
					if (tmp_y == 0) {
						menuScreen = 1;
						system("cls");
						menu(menuScreen, cells_x, cells_y, mines);
					}
					if (tmp_y == 1) {
						menuScreen = 2;
						system("cls");
						tmp_x = 0;
						tmp_y = 0;
						menu(menuScreen, cells_x, cells_y, mines);
					}
					if (tmp_y == 2)
					{
						difficulty = -1;
						menuScreen = -1;
						running = false;
					}
				}
				else if (menuScreen == 1) // Play/Difficulty screen
				{
					if (tmp_y == 0) {
						difficulty = 0;
						menuScreen = -1;
						system("cls");
					}
					if (tmp_y == 1) {
						difficulty = 1;
						menuScreen = -1;
						system("cls");
					}
					if (tmp_y == 2) {
						difficulty = 2;
						menuScreen = -1;
						system("cls");
					}
					if (tmp_y == 3) {
						tmp_x = 0;
						tmp_y = 0;
						difficulty = 3;
						menuScreen = 3;
						system("cls");
						menu(menuScreen, cells_x, cells_y, mines);
					}
					if (tmp_y == 4)
					{
						tmp_x = 0;
						tmp_y = 0;
						menuScreen = 0;
						system("cls");
						menu(menuScreen, cells_x, cells_y, mines);
					}
				}
				else if (menuScreen == 2) // Settings screen
				{
					if (tmp_y == 0) {
						setting_showKeybinds = !setting_showKeybinds;
						system("cls");
						menu(menuScreen, cells_x, cells_y, mines);
					}
					if (tmp_y == 1) {
						setting_flower_icon++;
						if (setting_flower_icon > 5)
							setting_flower_icon = 0;
						system("cls");
						menu(menuScreen, cells_x, cells_y, mines);
					}
					if (tmp_y == 2) {
						setting_cells_style++;
						if (setting_cells_style > 2)
							setting_cells_style = 0;
						system("cls");
						menu(menuScreen, cells_x, cells_y, mines);
					}
					if (tmp_y == 3) {
						menuScreen = 4;
						system("cls");
						tmp_x = 0;
						tmp_y = 0;
						menu(menuScreen, cells_x, cells_y, mines);
					}
					if (tmp_y == 4) {
						menuScreen = 0;
						saveSettings();
						system("cls");
						tmp_x = 0;
						tmp_y = 0;
						menu(menuScreen, cells_x, cells_y, mines);
					}
				}
				else if (menuScreen == 3) // Custom difficulty screen
				{
					if (tmp_x != 0)
					{
						tmp_x = 0;
					}
					else
					{
						if (tmp_y == 0) {
							tmp_x = 1;
						}
						if (tmp_y == 1) {
							tmp_x = 2;
						}
						if (tmp_y == 2) {
							tmp_x = 3;
						}
						if (tmp_y == 3) {
							menuScreen = -1;
						}
						if (tmp_y == 4)
						{
							tmp_x = 0;
							tmp_y = 0;
							menuScreen = 1;
						}
					}
					system("cls");
					menu(menuScreen, cells_x, cells_y, mines);
				}
				else if (menuScreen == 4) {
					menuScreen = 2;
					system("cls");
					tmp_x = 0;
					tmp_y = 0;
					menu(menuScreen, cells_x, cells_y, mines);
				}
				break;
			default:
				break;
			}
		}
	}
}

void draw(cell board[24][30], int cells_x, int cells_y) {
	getWindowSize();
	updateScreen();

#ifdef _DEBUG
	cout << "\nX: " << tmp_x << endl;
	cout << "Y: " << tmp_y << endl;
	cout << "cells_x: " << cells_x << endl;
	cout << "cells_y: " << cells_y << endl;
	cout << "flags: " << flags << "      " << endl;
	cout << "won: " << won << endl;
	cout << "gameOver: " << gameOver << endl;
	cout << "running: " << running << endl;
	cout << "window_x: " << window_x << endl;
	cout << "window_y: " << window_y << endl;
#endif // DEBUG

	setCursor(((window_x / 2) - ((cells_x * 2) / 2)), ((window_y / 2) - (cells_y / 2) - 1));

	cout << u8"🚩" << flags << "       ";

	for (int y = 0; y < cells_y; y++)
	{
		// Add columns
		setCursor(((window_x / 2) - ((cells_x * 2) / 2)), ((window_y / 2) - (cells_y / 2) + y));

		for (int x = 0; x < cells_x; x++)
		{
			// If won, color mine cells green
			if (won && board[y][x].isMine)
			{
				cout << "\033[48;2;35;160;20m";
			}
			// If won, color non-mine cells lightblue
			else if (won && !board[y][x].isMine)
			{
				cout << "\033[48;2;80;190;255m";
			}

			// Selected cell
			if (x == tmp_x && y == tmp_y && !gameOver && !won)
			{
				cout << "\033[48;2;60;60;60m";
			}
			// Default unrevealed cell
			else if (!board[y][x].revealed && !won)
			{
				cout << "\033[48;2;35;160;20m";
			}
			// Revealed cell
			else if (board[y][x].revealed && !won)
			{
				cout << "\033[48;2;125;125;125m";
			}
			cout << toGlyph(board[y][x]);
		}
		cout << "\n";
	}

	// Cells revealed count
	if (setting_cells_style == 1)
	{
		cout << revealed_cells << "/" << (cells_x * cells_y) << "              ";
	}
	if (setting_cells_style == 2)
	{
		cout << fixed << setprecision(2) << (1.0 * revealed_cells / (cells_x * cells_y)) * 100 << "%        ";
	}
	if (setting_showKeybinds)
	{
		cout << "\n\n[Q] Quit | [E]/[F] Place flag\n";
	}
}

void placeFlag(cell board[24][30], int x, int y, int cells_x, int cells_y, bool ignoreFlag) {

	if (!board[tmp_y][tmp_x].flagged && !board[tmp_y][tmp_x].revealed && flags > 0 && !ignoreFlag)
	{
		board[tmp_y][tmp_x].flagged = true;
		flags--;

		if (x < (cells_x - 1))
		{
			board[y][x + 1].flag_count++;
		}
		if (y > 0 && x < (cells_x - 1))
		{
			board[y - 1][x + 1].flag_count++;
		}
		if (y > 0)
		{
			board[y - 1][x].flag_count++;
		}
		if (y > 0 && x > 0)
		{
			board[y - 1][x - 1].flag_count++;
		}
		if (x > 0)
		{
			board[y][x - 1].flag_count++;
		}
		if (y < (cells_y - 1) && x > 0)
		{
			board[y + 1][x - 1].flag_count++;
		}
		if (y < (cells_y - 1))
		{
			board[y + 1][x].flag_count++;
		}
		if (y < (cells_y - 1) && x < (cells_x - 1))
		{
			board[y + 1][x + 1].flag_count++;
		}
	}
	else if (board[tmp_y][tmp_x].flagged)
	{
		board[tmp_y][tmp_x].flagged = false;
		flags++;
		if (x < (cells_x - 1))
		{
			board[y][x + 1].flag_count--;
		}
		if (y > 0 && x < (cells_x - 1))
		{
			board[y - 1][x + 1].flag_count--;
		}
		if (y > 0)
		{
			board[y - 1][x].flag_count--;
		}
		if (y > 0 && x > 0)
		{
			board[y - 1][x - 1].flag_count--;
		}
		if (x > 0)
		{
			board[y][x - 1].flag_count--;
		}
		if (y < (cells_y - 1) && x > 0)
		{
			board[y + 1][x - 1].flag_count--;
		}
		if (y < (cells_y - 1))
		{
			board[y + 1][x].flag_count--;
		}
		if (y < (cells_y - 1) && x < (cells_x - 1))
		{
			board[y + 1][x + 1].flag_count--;
		}
	}	
}

void checkWin(cell board[24][30], int x, int y, int cells_x, int cells_y, int mines) {

	// Check if player lost
	if (board[y][x].isMine)
	{
		board[y][x].exploded = true;
		if (revealed_cells > 0)
			revealed_cells--;
		for (int y = 0; y < cells_y; y++)
		{
			for (int x = 0; x < cells_x; x++)
			{
				board[y][x].revealed = true;
			}
		}
		gameOver = true;
		cout << "\nYou lost! :/\n";
	}

	// Check if player won
	if (revealed_cells == cells_x * cells_y - mines)
	{
		gameOver = true;
		won = true;
		cout << "\nYou Won! :)\n";
	}
}

void autoopen(cell board[24][30], int x, int y, int cells_x, int cells_y, int mines) {

	board[y][x].revealed = true;

	if (board[y][x].mine_count == 0 && !board[y][x].isMine && board[y][x].revealed)
	{
		if (x < (cells_x - 1))
		{
			if (!board[y][x + 1].revealed)
			{
				revealed_cells++;
				board[y][x + 1].revealed = true;
				if (board[y][x + 1].mine_count == 0 && !board[y][x + 1].isMine)
				{
					autoopen(board, x + 1, y, cells_x, cells_y, mines);
				}
			}
		}
		if (y > 0 && x < (cells_x - 1))
		{
			if (!board[y - 1][x + 1].revealed)
			{
				revealed_cells++;
				board[y - 1][x + 1].revealed = true;
				if (board[y - 1][x + 1].mine_count == 0 && board[y - 1][x + 1].revealed && !board[y - 1][x + 1].isMine)
				{
					autoopen(board, x + 1, y - 1, cells_x, cells_y, mines);
				}
			}
		}
		if (y > 0)
		{
			if (!board[y - 1][x].revealed)
			{
				revealed_cells++;
				board[y - 1][x].revealed = true;
				if (board[y - 1][x].mine_count == 0 && board[y - 1][x].revealed && !board[y - 1][x].isMine)
				{
					autoopen(board, x, y - 1, cells_x, cells_y, mines);
				}
			}
		}
		if (y > 0 && x > 0)
		{
			if (!board[y - 1][x - 1].revealed)
			{
				revealed_cells++;
				board[y - 1][x - 1].revealed = true;
				if (board[y - 1][x - 1].mine_count == 0 && board[y - 1][x - 1].revealed && !board[y - 1][x - 1].isMine)
				{
					autoopen(board, x - 1, y - 1, cells_x, cells_y, mines);
				}
			}
		}
		if (x > 0)
		{
			if (!board[y][x - 1].revealed)
			{
				revealed_cells++;
				board[y][x - 1].revealed = true;
				if (board[y][x - 1].mine_count == 0 && board[y][x - 1].revealed && !board[y][x - 1].isMine)
				{
					autoopen(board, x - 1, y, cells_x, cells_y, mines);
				}
			}
		}
		if (y < (cells_y - 1) && x > 0)
		{

			if (!board[y + 1][x - 1].revealed)
			{
				revealed_cells++;
				board[y + 1][x - 1].revealed = true;
				if (board[y + 1][x - 1].mine_count == 0 && board[y + 1][x - 1].revealed && !board[y + 1][x - 1].isMine)
				{
					autoopen(board, x - 1, y + 1, cells_x, cells_y, mines);
				}
			}
		}
		if (y < (cells_y - 1))
		{
			if (!board[y + 1][x].revealed)
			{
				revealed_cells++;
				board[y + 1][x].revealed = true;
				if (board[y + 1][x].mine_count == 0 && board[y + 1][x].revealed && !board[y + 1][x].isMine)
				{
					autoopen(board, x, y + 1, cells_x, cells_y, mines);
				}
			}
		}
		if (y < (cells_y - 1) && x < (cells_x - 1))
		{
			if (!board[y + 1][x + 1].revealed)
			{
				revealed_cells++;
				board[y + 1][x + 1].revealed = true;
				if (board[y + 1][x + 1].mine_count == 0 && board[y + 1][x + 1].revealed && !board[y + 1][x + 1].isMine)
				{
					autoopen(board, x + 1, y + 1, cells_x, cells_y, mines);
				}
			}
		}
	}

	if (board[tmp_y][tmp_x].mine_count > 0 && board[tmp_y][tmp_x].revealed && board[tmp_y][tmp_x].mine_count == board[tmp_y][tmp_x].flag_count) {
		if (x < (cells_x - 1))
		{
			if (!board[y][x + 1].revealed && !board[y][x + 1].flagged)
			{
				revealed_cells++;
				board[y][x + 1].revealed = true;
				checkWin(board, x + 1, y, cells_x, cells_y, mines);
				if (board[y][x + 1].mine_count == 0 && !board[y][x + 1].isMine)
				{
					autoopen(board, x + 1, y, cells_x, cells_y, mines);
				}
			}
		}
		if (y > 0 && x < (cells_x - 1))
		{
			if (!board[y - 1][x + 1].revealed && !board[y - 1][x + 1].flagged)
			{
				revealed_cells++;
				board[y - 1][x + 1].revealed = true;
				checkWin(board, x + 1, y - 1, cells_x, cells_y, mines);
				if (board[y - 1][x + 1].mine_count == 0 && board[y - 1][x + 1].revealed && !board[y - 1][x + 1].isMine)
				{
					autoopen(board, x + 1, y - 1, cells_x, cells_y, mines);
				}
			}
		}
		if (y > 0)
		{
			if (!board[y - 1][x].revealed && !board[y - 1][x].flagged)
			{
				revealed_cells++;
				board[y - 1][x].revealed = true;
				checkWin(board, x, y - 1, cells_x, cells_y, mines);
				if (board[y - 1][x].mine_count == 0 && board[y - 1][x].revealed && !board[y - 1][x].isMine)
				{
					autoopen(board, x, y - 1, cells_x, cells_y, mines);
				}
			}
		}
		if (y > 0 && x > 0)
		{
			if (!board[y - 1][x - 1].revealed && !board[y - 1][x - 1].flagged)
			{
				revealed_cells++;
				board[y - 1][x - 1].revealed = true;
				checkWin(board, x - 1, y - 1, cells_x, cells_y, mines);
				if (board[y - 1][x - 1].mine_count == 0 && board[y - 1][x - 1].revealed && !board[y - 1][x - 1].isMine)
				{
					autoopen(board, x - 1, y - 1, cells_x, cells_y, mines);
				}
			}
		}
		if (x > 0)
		{
			if (!board[y][x - 1].revealed && !board[y][x - 1].flagged)
			{
				revealed_cells++;
				board[y][x - 1].revealed = true;
				checkWin(board, x - 1, y, cells_x, cells_y, mines);
				if (board[y][x - 1].mine_count == 0 && board[y][x - 1].revealed && !board[y][x - 1].isMine)
				{
					autoopen(board, x - 1, y, cells_x, cells_y, mines);
				}
			}
		}
		if (y < (cells_y - 1) && x > 0)
		{

			if (!board[y + 1][x - 1].revealed && !board[y + 1][x - 1].flagged)
			{
				revealed_cells++;
				board[y + 1][x - 1].revealed = true;
				checkWin(board, x - 1, y + 1, cells_x, cells_y, mines);
				if (board[y + 1][x - 1].mine_count == 0 && board[y + 1][x - 1].revealed && !board[y + 1][x - 1].isMine)
				{
					autoopen(board, x - 1, y + 1, cells_x, cells_y, mines);
				}
			}
		}
		if (y < (cells_y - 1))
		{
			if (!board[y + 1][x].revealed && !board[y + 1][x].flagged)
			{
				revealed_cells++;
				board[y + 1][x].revealed = true;
				checkWin(board, x, y + 1, cells_x, cells_y, mines);
				if (board[y + 1][x].mine_count == 0 && board[y + 1][x].revealed && !board[y + 1][x].isMine)
				{
					autoopen(board, x, y + 1, cells_x, cells_y, mines);
				}
			}
		}
		if (y < (cells_y - 1) && x < (cells_x - 1))
		{
			if (!board[y + 1][x + 1].revealed && !board[y + 1][x + 1].flagged)
			{
				revealed_cells++;
				board[y + 1][x + 1].revealed = true;
				checkWin(board, x + 1, y + 1, cells_x, cells_y, mines);
				if (board[y + 1][x + 1].mine_count == 0 && board[y + 1][x + 1].revealed && !board[y + 1][x + 1].isMine)
				{
					autoopen(board, x + 1, y + 1, cells_x, cells_y, mines);
				}
			}
		}
	}
}

void addNumbers(cell board[24][30], int cells_x, int cells_y) {
	for (int y = 0; y < cells_y; y++)
	{
		for (int x = 0; x < cells_x; x++)
		{
			if (board[y][x].isMine == false)
			{
				int mine_count = 0;
				if (x < (cells_x - 1) && board[y][x + 1].isMine == true)
				{
					mine_count++;
				}
				if (y > 0 && x < (cells_x - 1) && board[y - 1][x + 1].isMine == true)
				{
					mine_count++;
				}
				if (y > 0 && board[y - 1][x].isMine == true)
				{
					mine_count++;
				}
				if (y > 0 && x > 0 && board[y - 1][x - 1].isMine == true)
				{
					mine_count++;
				}
				if (x > 0 && board[y][x - 1].isMine == true)
				{
					mine_count++;
				}
				if (y < (cells_y - 1) && x > 0 && board[y + 1][x - 1].isMine == true)
				{
					mine_count++;
				}
				if (y < (cells_y - 1) && board[y + 1][x].isMine == true)
				{
					mine_count++;
				}
				if (y < (cells_y - 1) && x < (cells_x - 1) && board[y + 1][x + 1].isMine == true)
				{
					mine_count++;
				}
				board[y][x].mine_count = mine_count;
			}
		}
	}
}

void placeMines(cell board[24][30], int mines, int cells_x, int cells_y) {

	srand(time(0));

	for (int i = 0; i < mines; i++)
	{
		int x = rand() % cells_x;
		int y = rand() % cells_y;

		if (board[y][x].isMine == true)
		{
			i--;
		}
		else
		{
			board[y][x].isMine = true;
		}
	}
	flags = mines;
}

void kbgame(cell board[24][30], int cells_x, int cells_y, int mines) {

	while (!gameOver) {
		if (_kbhit())
		{
			switch (_getch())
			{
			case 'w':
			case 'k':
			case 72:
				tmp_y--;
				if (tmp_y < 0)
					tmp_y = 0;
				draw(board, cells_x, cells_y);
				break;
			case 'a':
			case 'h':
			case 75:
				tmp_x--;
				if (tmp_x < 0)
					tmp_x = 0;
				draw(board, cells_x, cells_y);
				break;
			case 's':
			case 'j':
			case 80:
				tmp_y++;
				if (tmp_y > (cells_y - 1))
					tmp_y = (cells_y - 1);
				draw(board, cells_x, cells_y);
				break;
			case 'd':
			case 'l':
			case 77:
				tmp_x++;
				if (tmp_x > (cells_x - 1))
					tmp_x = (cells_x - 1);
				draw(board, cells_x, cells_y);
				break;
			case 'e':
			case 'f':
				placeFlag(board, tmp_x, tmp_y, cells_x, cells_y, false);
				draw(board, cells_x, cells_y);
				break;
			case 13:
			case ' ':
				// Place Flag
				if (board[tmp_y][tmp_x].flagged)
				{
					placeFlag(board, tmp_x, tmp_y, cells_x, cells_y, true);
					draw(board, cells_x, cells_y);
				}
				else
				{
					// Open cell
					if (!board[tmp_y][tmp_x].revealed)
						revealed_cells++;
					autoopen(board, tmp_x, tmp_y, cells_x, cells_y, mines);
					checkWin(board, tmp_x, tmp_y, cells_x, cells_y, mines);
					draw(board, cells_x, cells_y);
				}
				// Open cells around number if enough flags placed
				if (board[tmp_y][tmp_x].revealed && board[tmp_y][tmp_x].mine_count == board[tmp_y][tmp_x].flag_count) {
					autoopen(board, tmp_x, tmp_y, cells_x, cells_y, mines);
					checkWin(board, tmp_x, tmp_y, cells_x, cells_y, mines);
				}
				break;
			case 'q':
				gameOver = true;
				running = false;
			default:
				break;
			}
		}
	}
}

int main(int argc, char* argv[])
{
	// Initialize settings etc.
	system("chcp 65001 >nul");
	loadSettings();

	while (running) {

		cout << "\033[?25l";

		cell board[24][30];

		gameOver = false;
		won = false;
		tmp_x = 0, tmp_y = 0;
		revealed_cells = 0;
		char option;
		int difficulty;
		int cells_x = 1;
		int cells_y = 1;
		int mines;

		tmp_x = 0;
		tmp_y = 0;
		
		if (argc > 1)
		{
			if (argv[1] == string("--beginner") || argv[1] == string("-b"))
			{
				system("cls");
				difficulty = 0;
			}
			if (argv[1] == string("--intermediate") || argv[1] == string("-i"))
			{
				system("cls");
				difficulty = 1;
			}
			if (argv[1] == string("--expert") || argv[1] == string("-e"))
			{
				system("cls");
				difficulty = 2;
			}
			if (argv[1] == string("--help") || argv[1] == string("-h"))
			{
				cout << "\nusage: minesweeper [option]";

				cout << "\n\noptions:";
				cout << "\n\t-h, --help \t\t show this help message and exit";

				cout << "\n\ndifficulty options:";
				cout << "\n\t-b, --beginner \t\t starts the game with the beginner difficulty";
				cout << "\n\t-i, --intermediate \t starts the game with the intermediate difficulty";
				cout << "\n\t-e, --expert \t\t starts the game with the expert difficulty";
				cout << "\n";
				cout << "\033[?25h";
				return 0;
			}

			if (argv[1] != string("--beginner") && argv[1] != string("-b") &&
				argv[1] != string("--intermediate") && argv[1] != string("-i") && 
				argv[1] != string("--expert") && argv[1] != string("-e") &&
				argv[1] != string("--help") && argv[1] != string("-h") && argc > 1)
			{
				cout << "\n\033[91mError: no such argument/option. Try again and check spelling mistakes.\033[0m";
				cout << "\nusage: minesweeper [option]";

				cout << "\n\noptions:";
				cout << "\n\t-h, --help \t\t show this help message and exit";

				cout << "\n\ndifficulty options:";
				cout << "\n\t-b, --beginner \t\t starts the game with the beginner difficulty";
				cout << "\n\t-i, --intermediate \t starts the game with the intermediate difficulty";
				cout << "\n\t-e, --expert \t\t starts the game with the expert difficulty";
				cout << "\n";
				cout << "\033[?25h";
				return 1;
			}
		}
		else
		{
			system("cls");
			menu(0, cells_x, cells_y, mines);
			kbmenu(0, difficulty, cells_x, cells_y, mines);
		}

		// Exit program
		if (difficulty == -1) {
			break;
		}

		switch (difficulty)
		{
		case 0:
			// Beginner
			cells_x = 8;
			cells_y = 8;
			mines = 10;
			break;
		case 1:
			// Intermediate
			cells_x = 16;
			cells_y = 16;
			mines = 40;
			break;
		case 2:
			// Expert
			cells_x = 30;
			cells_y = 16;
			mines = 99;
			break;
		case 3:
			// Custom
			break;
		default:
			cells_x = 8;
			cells_y = 8;
			mines = 10;
			break;
		}

		for (int y = 0; y < cells_y; y++)
		{
			for (int x = 0; x < cells_x; x++)
			{
				board[y][x].revealed = false;
				board[y][x].isMine = false;
				board[y][x].mine_count = 0;
				board[y][x].exploded = false;
				board[y][x].flagged = false;
			}
		}

		tmp_x = 0;
		tmp_y = 0;

		placeMines(board, mines, cells_x, cells_y);
		addNumbers(board, cells_x, cells_y);
		draw(board, cells_x, cells_y);

		start = chrono::high_resolution_clock::now();
		kbgame(board, cells_x, cells_y, mines);

		auto end = chrono::high_resolution_clock::now();
		time_played += chrono::duration_cast<chrono::seconds>(end - start).count();
		cout << u8"\n⏱️ " << chrono::duration_cast<chrono::seconds>(end - start).count() << "s\n";

		cout << "\nDifficulty: ";
		switch (difficulty)
		{
		case 0:
			cout << u8"🥉";
			break;
		case 1:
			cout << u8"🥈";
			break;
		case 2:
			cout << u8"🥇";
			break;
		case 3:
			cout << u8"🏅";
			break;
		default:
			cout << u8"🏅";
			break;
		}

		cout << "\033[?25h";
		cout << "\nReturn to menu? (y/n): ";
		cin >> option;
		if (option == 'n' || option == 'N')
			running = false;
		else
		{
			running = true;
			if (argc > 1) {
				tmp_x = 0;
				tmp_y = 0;
				cout << "\033[?25l";
				system("cls");
				menu(0, cells_x, cells_y, mines);
				kbmenu(0, difficulty, cells_x, cells_y, mines);
			}
		}
	}
	saveSettings();
	cout << u8"\n\nBye..! 👋\n\n";
	
	cout << "\033[?25h";
}

// Beginner: 8x8 10 Mines
// Intermediate: 16x16 40 Mines
// Expert: 30x16 99 Mines
// Custom: Max 30x24 668 Mines
