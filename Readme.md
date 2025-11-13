```                                         
 _____ _                                       
|     |_|___ ___ ___ _ _ _ ___ ___ ___ ___ ___ 
| | | | |   | -_|_ -| | | | -_| -_| . | -_|  _|
|_|_|_|_|_|_|___|___|_____|___|___|  _|___|_|  
                                  |_|          
```                                                                            

# 🧨 Minesweeper (C++ & ncurses)

A classic **Minesweeper** implementation written in **modern C++**, featuring a fully playable ASCII-based UI built on **ncurses**.

This project cleanly separates the *game logic* from the *terminal interface* (`build/minesweeper`), allowing both easy testing (`build/minesweeper_tests`) and options to leverage the core gameplay in other graphical formats besides the default ASCII based experienced provided with this project.

> **RUBRIC COMPLIANCE** - This project was created as the Final Project for a programing course at Duke University for the Game Design Development and Innovation (GDDI) Master program.  The full compliance with the rubric of the assignment can be found in the [RubricReport.md](docs/RubricReport.md).


## 📖 Backstory

Deep beneath the rolling meadows of the Forgotten Fields lies a network of ancient mines left behind after a long-forgotten war. The villagers who once tended these lands vanished centuries ago, leaving behind a deadly puzzle. You, a daring explorer, have arrived to chart the fields and uncover what remains. Each step you take risks uncovering a hidden explosive relic. With patience, logic, and a bit of luck, you may clear the land and restore peace to the valley — or vanish into legend like those before you.

## 🧭 **Primary Goal(s)**

The **primary goal** of *Minesweeper* is to **clear a rectangular grid of hidden tiles without detonating any mines**.
Players must use **logic and deduction** to identify which tiles are safe and which contain mines, marking suspected mines with flags and revealing all safe spaces to win the game.

**Victory Condition:** - Reveal all non-mine tiles on the board.

**Loss Condition:** - Reveal a tile containing a mine.

## ⚙️ **Core Game Features**

1. **Grid-Based Playfield**

   * The board is a grid of tiles (e.g., 9×9, 16×16, or custom size).
   * Each tile is initially hidden.

2. **Mines Placement**

   * A fixed number of mines are randomly placed on the board at the start of each game.
   * The number of mines remains constant throughout the game.

3. **Tile States**

   * **Hidden:** Not yet revealed.
   * **Revealed:** Shows either:
     * A number (1–8) indicating how many adjacent tiles contain mines, or
     * A blank space (if no adjacent mines).
   * **Flagged:** Marked by the player as suspected to contain a mine.

4. **Revealing Behavior**

   * Revealing a mine ends the game.
   * Revealing a tile with `0` adjacent mines automatically reveals all contiguous safe tiles (a recursive reveal).

5. **Save / Load (optional feature)**

    * Player progress can be saved and restored.

6. **Configurable Layout and Difficulty**

    * Custom grid sizes (row x cols) and number of mines can be set by user 

## 🎮 **Instructions for Play**

1. **Starting the Game**

   * The player is presented with a hidden grid.
   * The first tile clicked is guaranteed to be safe.

2. **Revealing Tiles (spacebar)**

    * If it contains a mine → Game Over.
    * If it contains a number → Display adjacent mine count.
    * If it’s empty → Automatically reveal adjacent safe tiles recursively.

3. **Flagging Tiles**

   * **F Key:** Toggle a flag on or off to mark a suspected mine.
   * Flags are used to help track dangerous spots.
   * Once Flagged, tile cannot be accidentally revealed.  Must be unflagged to reval

4. **Winning the Game**

   * Continue revealing safe tiles until all non-mine tiles are uncovered.
   * The number of flags should ideally match the total number of mines.

5. **Losing the Game**

   * Revealing a tile with a mine immediately ends the game.
   * All mines are revealed to show where they were located.

6. **Optional Controls**

   * **Reset / New Game:** Start a new randomized board.
   * **Save / Load:** Preserve or restore game state.

## 🧩 Controls

| Key | Action |
|-----|---------|
| ⬆️ / ⬇️ / ⬅️ / ➡️ or **H/J/K/L** | Move cursor |
| **Space / Enter** | Reveal tile |
| **f** | Toggle flag / question mark / off |
| **r** | Restart current board |
| **s** | Save current game as `saved_game.txt` |
| **q** | Quit |

## ▶️ Usage

**IMPORTANT** - For instructions on how to build this game for your platform, see [BuildReadme.md](docs/BuildReadme.md)

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

## 🖋️ Author

**Rodney Aiglstorfer**  
Graduate Student – Duke University, Game Design, Development & Innovation  
GitHub: [@ra9r](https://github.com/ra9r)

---

## 🧾 License

MIT License © 2025  
Feel free to use, modify, and distribute this project for educational or personal purposes.


