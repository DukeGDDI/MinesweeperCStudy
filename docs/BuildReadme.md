# Build Instructions


## 🧰 Requirements

- A C++17 (or newer) compiler  
- [ncurses](https://invisible-island.net/ncurses/) (installed by default on most Linux and macOS systems)
- [gtest](https://github.com/google/googletest) (see install instructions)

## Install ncurses

### Ubuntu/Debian Linux
```bash
sudo apt install libncurses5-dev libncursesw5-dev
```

### MacOS (Homebrew)
```bash
brew install ncurses
```


## Install gtest

### Ubuntu/Debian Linux
```bash
sudo apt-get update
sudo apt-get install -y libgtest-dev
cd /usr/src/googletest
sudo cmake -S . -B build && sudo cmake --build build --target install
```

### MacOS (Homebrew)
```bash
brew install googletest
```

## ⚙️ Build Instructions

### Using Makefile
```bash
make
```

A call to `make` produces the following build artifacts.

| Artifact                      | Description                                  |
|-------------------------------|----------------------------------------------|
| `build/bin/minesweeper`       | Text based UI for minesweeper game.          | 
| `build/bin/minesweeper_tests` | Unit test suite for the mindsweeper library. |
| `build/lib/minesweeperlib.a`  | Minesweeper core game libarary.              |

### Running Tests
```bash
make test
```
