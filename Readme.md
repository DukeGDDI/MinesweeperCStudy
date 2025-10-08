# 🧨 Terminal Minesweeper (C + ncurses)

A fully playable **Minesweeper** clone written in **C** using the **ncurses** library for a retro terminal UI.  
Move with your arrow keys, flag tiles, reveal spaces, and survive the mines — all in glorious ANSI color.

![screenshot](docs/ASCIIMinesweeper.gif)

---

## 🎮 Features

- Classic Minesweeper gameplay  
- Arrow-key movement with highlighted cursor  
- Color-coded numbers (matching the original Minesweeper)  
- Save and load capability (`savegame.bin`)  
- Restart instantly without quitting  
- UTF-8 “black square” cursor marker for revealed blanks  
- Fully keyboard-driven interface  
- Works on Linux, macOS, and Windows (via WSL or MSYS2)

---

## 🧰 Requirements

- **C compiler** (GCC or Clang)
- **ncurses** or **ncursesw** development package  
  - Ubuntu / Debian:  
    ```bash
    sudo apt install libncursesw5-dev
    ```
  - macOS:  
    ```bash
    brew install ncurses
    ```

---

## ⚙️ Build Instructions

Clone the repository and build:

```bash
git clone https://github.com/yourusername/minesweeper-c.git
cd minesweeper-c
make
````

This will produce the binary in:

```
bin/minesweeper
```

Run it with:

```bash
./bin/minesweeper
```

---

## 🧾 Command Line Options

You can optionally specify **board size** and **mine count**:

```bash
./bin/minesweeper <board-size> <mine-count>
```

Examples:

```bash
./bin/minesweeper 10 15     # 10x10 grid with 15 mines
./bin/minesweeper 20 40     # 20x20 grid with 40 mines
```

---

## 🎮 Controls

| Key          | Action                                              |
| ------------ | --------------------------------------------------- |
| **↑ ↓ ← →**  | Move cursor                                         |
| **Spacebar** | Reveal tile                                         |
| **F**        | Toggle flag → question → covered                    |
| **S**        | Save current game to `savegame.bin`                 |
| **R**        | Restart new random board (same size and mine count) |
| **Q**        | Quit the game immediately                           |

---

## 🎨 Colors

| Number | Color            |
| ------ | ---------------- |
| 1      | Blue             |
| 2      | Green            |
| 3      | Red              |
| 4      | Indigo / Magenta |
| 5      | Brownish-Red     |
| 6      | Cyan             |
| 7      | Black            |
| 8      | Gray             |

---

## 💾 Saving

Press **`S`** at any time to save your current board to:

```
savegame.bin
```

> ⚠️ The save format is a simple binary dump — portable between runs on the same system.

---

## 🔁 Restarting

Press **`R`** to instantly start a new game with the same board size and mine count.
If you hit a mine, you can restart immediately without quitting.

---

## 🧱 File Structure

```
.
├── src/
│   ├── main.c              # Game UI & input loop
│   └── minesweeper.c       # Game logic
├── minesweeper.h           # Data structures & function declarations
├── Makefile                # Build script
├── README.md               # This file
└── savegame.bin            # Generated when you save
```

---

## 🧠 Technical Notes

* Uses **ncurses** for terminal control, colors, and key handling.
* UTF-8 locale enabled for rendering the black square cursor (`■`).
* Uses modular board logic via `minesweeper.h` (separate from UI).
* Portable C99-compliant codebase.

---

## 🚀 Future Enhancements

* Load previously saved games automatically
* Add timer and mine counter display
* Difficulty presets (Beginner / Intermediate / Expert)
* Windows console compatibility without WSL

---

## 🪪 License

MIT License © 2025 Rodney Aiglstorfer
You’re free to use, modify, and distribute this code — just keep attribution.

---

## 🖼️ Screenshots

*(Optional — place screenshots in `/docs/`)*

```
docs/
├── screenshot.png
└── gameplay.gif
```

Example placeholder:
![Gameplay Example](docs/gameplay.gif)

---

## ❤️ Acknowledgments

Built with love for the command line.
Inspired by the classic Minesweeper that taught us patience, logic, and humility.

---

## 🧩 Usage

You can start the game with optional parameters for **board size** and **mine count**.
If omitted, defaults are used: `{board-size:10}` `{mine-count:4}`

```bash
./bin/minesweeper {board-size:10} {mine-count:4}
```

Examples:

```bash
./bin/minesweeper          # 10x10 board, 4 mines (default)
./bin/minesweeper 12 10    # 12x12 board, 10 mines
./bin/minesweeper 20 40    # 20x20 board, 40 mines
```

---

```

---

