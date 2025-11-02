```                                         
 _____ _                                       
|     |_|___ ___ ___ _ _ _ ___ ___ ___ ___ ___ 
| | | | |   | -_|_ -| | | | -_| -_| . | -_|  _|
|_|_|_|_|_|_|___|___|_____|___|___|  _|___|_|  
                                  |_|          
```                                                                            

# 🧨 Minesweeper (C++ & ncurses)

A classic **Minesweeper** implementation written in **modern C++**, featuring a fully playable ASCII-based UI built on **ncurses**.

This project cleanly separates the *game logic* (`Board` class) from the *terminal interface* (`tui/app.cpp`), allowing both easy testing and a nostalgic command-line gameplay experience.

---

## 🎮 Features

- **Pure C++ game logic** (in `minesweeper.cpp` / `minesweeper.hpp`)
- **Interactive ncurses TUI**
  - Arrow keys or H/J/K/L to move
  - Space or Enter to reveal
  - `f` to toggle flags
  - `r` to restart
  - `s` to save game
  - `q` to quit
- **Left-aligned board layout**
- **Highlight on all tiles** (even revealed ones)
- **Save and restore** game state via `Board::save()` and `Board::load()`
- **Configurable board size and mine count** via command line

---

## 🧰 Requirements

- A C++17 (or newer) compiler  
- [ncurses](https://invisible-island.net/ncurses/) (installed by default on most Linux and macOS systems)

On macOS:
```bash
brew install ncurses
```

On Debian/Ubuntu:
```bash
sudo apt install libncurses5-dev libncursesw5-dev
```

---

## ⚙️ Build Instructions

### Using Makefile
```bash
make
```

---

## ▶️ Usage

### Start a new default game (16×30, 99 mines)
```bash
./build/bin/minesweeper
```

### Start a custom game
```bash
./build/bin/minesweeper <rows> <columns> <mines>
# Example:
./build/bin/minesweeper 10 20 40
```

### Resume from a saved game
```bash
./build/bin/minesweeper path/to/savefile.txt
```

If you omit the extension, it defaults to `save_game.txt`.

---

## 🧩 Controls

| Key | Action |
|-----|---------|
| ⬆️ / ⬇️ / ⬅️ / ➡️ or **H/J/K/L** | Move cursor |
| **Space / Enter** | Reveal tile |
| **f** | Toggle flag / question mark |
| **r** | Restart current board |
| **s** | Save current game |
| **q** | Quit |

---

## 💾 Save and Load

- Games are saved using the `Board::save(path)` method, which stores all tile states and metadata.  
- Use `Board::load(path)` to restore a saved game.
- When loading via command line (`./build/bin/minesweeper savefile.txt`), the game infers board dimensions and mine count automatically.

---

## 🧱 Project Structure

```
minesweeper/
├── include/
│   └── board.hpp             
|   └── text_board_serializer.hpp
|   └── tile_state.hpp
|   └── tile.hpp
├── src/
│   └── minesweeper.cpp        
|   └── text_board_serializer.cpp
|   └── tile_state.cpp
|   └── tile.cpp
├── tui/
│   └── app.cpp                # ncurses interface (this app)
├── tests/
│   └── test_board.cpp         # Unit Tests
├── build/
│   ├── bin/                   # Compiled binaries "minesweeper" and "minesweeper_tests"
│   └── lib/                   # Optional static libs and minesweeper.o
└── README.md
```

---

## 🧠 Design Notes

- The **Board** class encapsulates all rules, tile states, and win/loss conditions.
- The **ncurses UI** (`tui/app.cpp`) acts as a front-end layer, calling only public methods like:
  - `getRows()`, `getColumns()`
  - `reveal(row, col)`
  - `toggleTile(row, col)`
  - `getTile(row, col)`
  - `save(path)` / `load(path)`
- This separation makes it easy to reuse `Board` in other environments (e.g., GUI, Unreal Engine, or web apps).

---

## 🧪 Testing

The `tests/main.cpp` file (if present) can be used as a standalone tester for the `Board` class without ncurses dependencies.

Example:
```bash
g++ -std=c++17 -Iinclude tests/main.cpp src/minesweeper.cpp -o build/bin/ms_test
./build/bin/ms_test
```

---

## 🖋️ Author

**Rodney Aiglstorfer**  
Graduate Student – Duke University, Game Design, Development & Innovation  
GitHub: [@RodneyAiglstorfer](https://github.com/RodneyAiglstorfer)

---

## 🧾 License

MIT License © 2025  
Feel free to use, modify, and distribute this project for educational or personal purposes.

## Dependencies

### GTest

```
git submodule add https://github.com/google/googletest.git third_party/googletest
git submodule update --init --recursive
```

### Ubuntu/Debian Linux
```
sudo apt-get update
sudo apt-get install -y libgtest-dev
# libgtest-dev ships sources; build locally:
cd /usr/src/googletest
sudo cmake -S . -B build && sudo cmake --build build --target install
```
### MacOS (Homebrew)
```
brew install googletest
```