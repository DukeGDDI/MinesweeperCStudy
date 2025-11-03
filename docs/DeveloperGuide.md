


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
└── Makefile
```

---

## 🧠 Design Notes

- The **Board** class encapsulates all rules, tile states, and win/loss conditions.
- The **ncurses UI** (`tui/app.cpp`) acts as a front-end layer, calling only public methods like:
  - `getRows()`, `getColumns()`, `getMines()`
  - `revealTile(row, col)`
  - `toggleTile(row, col)`
  - `getTile(row, col)`
  - `save(path)` / `load(path)`
- This separation makes it easy to reuse `Board` in other environments (e.g., GUI, Unreal Engine, or web apps).

---

