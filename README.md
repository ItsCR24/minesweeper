# A simple minesweeper game with a beautiful tui! 💣

A terminal-based minesweeper game written in C++ featuring 3 difficulty levels, a custom difficulty option and lots of customization options.

## Usage
```bash
usage: minesweeper [option]

options:
        -h, --help               show this help message and exit

difficulty options:
        -b, --beginner           starts the game with the beginner difficulty
        -i, --intermediate       starts the game with the intermediate difficulty
        -e, --expert             starts the game with the expert difficulty
```
to show this message, use the `-h` or `--help` option.

## Screenshots
<img width="600" alt="main_menu" src="https://github.com/user-attachments/assets/9992f85e-a999-437f-8cb0-3ee62191f285"/>
<img width="600" alt="gameplay" src="https://github.com/user-attachments/assets/dad156eb-9a78-4157-a552-112f70503c65"/>
<img width="600" alt="settings" src="https://github.com/user-attachments/assets/270a2f69-ebdb-4ae1-a3f3-92e966f0c5d3"/>
<img width="600" alt="intermediate_win" src="https://github.com/user-attachments/assets/487ec412-b288-42ec-b35b-7cdb8cd4d828"/>
<img width="600" alt="custom_difficulty" src="https://github.com/user-attachments/assets/b81267dc-497f-4cf6-922d-350483666de4"/>


## Compilation (via MSVC)
```bash
git clone https://github.com/ItsCR24/minesweeper
cd minesweeper
cl minesweeper.cpp /EHsc
```

## Download
You can download the precompiled binary from the [releases](https://github.com/blurrycmd/minesweeper/releases).
