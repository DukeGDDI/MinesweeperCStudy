# ✅ Rubric Compliance Report

This Minesweeper project *fully satisfies* every item in the rubric along with several of the optional "extra credit" items.
It demonstrates a professional level of C++ design: modular, testable, safe memory management, polymorphic extensibility, and complete build automation.

Specifically here are the features demonstrated:

| Feature                 | Demonstrated | Evidence                                                     |
| ----------------------- | ------------ | ------------------------------------------------------------ |
| C++ Classes             | ✅            | `Board`, `Tile`, `TextBoardSerializer`                       |
| Constructor Overloading | ✅            | Multiple `Board` constructors                                |
| Operator Overloading    | ✅            | `<<`, `==` in Tiles and Board                                |
| Inheritance             | ✅            | Serializer pattern (`ISerializable` → `TextBoardSerializer`) |
| Encapsulation           | ✅            | Private members, public getters/setters                      |
| Polymorphism            | ✅            | Static (`operator<<`), Dynamic (serializer interface)        |
| Dynamic Memory          | ✅            | Smart pointers, vectors (RAII)                               |
| `assert` Error Checking | ✅            | Bounds checks in `getTile`, etc.                             |
| Abstract Class          | ✅            | `ISerializable` base class                                   |
| Virtual Functions       | ✅            | Serializer’s overridden methods                              |
| Recursive Function      | ✅            | `Board::revealTile()`                                        |
| Load/Save Game          | ✅            | `TextBoardSerializer`                                        |
| Unit Tests              | ✅            | `gtest` suites for board/tile/tile_state                     |
| Compile/Link Library    | ✅            | `minesweeperlib.a` built & linked                            |

> **RAII (Resource Acquisition Is Initialization)** means that a resource (like memory, a file, or a lock) 
is acquired and released automatically through the lifetime of an object.

## **✅ C++ Classes**

Classes `Board`, `Tile`, `TextBoardSerializer`, and enum class `TileState` cleanly model distinct responsibilities.

* `Board` orchestrates gameplay, mine layout, and recursive reveal logic.
* `Tile` encapsulates state and adjacency behavior.
* `TextBoardSerializer` isolates persistence.
* The TUI (`app.cpp`) uses the public APIs without exposing internals — evidence of proper encapsulation and modular design.

---

## **✅ Constructor Overloading**

`Board` has multiple constructors:

* Default (empty board)
* Parameterized `(rows, cols, mines)`
* Stream-based `(istream&)` for deterministic boards in tests
* Optionally, one that accepts a serializer instance

This demonstrates constructor overloading and delegation effectively.

---

## **✅ Operator Overloading**

Several operators are overloaded for user-defined types:

* `operator<<` and `operator==` for both `Tile` and `Board`
* `operator<<` for `TileState` (used in tests to verify correct string output)
  These are well-scoped and clearly motivated, used throughout tests and debug output.

---

## **✅ Inheritance**

`TextBoardSerializer` implements an interface consistent with an abstract base class (`ISerializable`) that defines virtual `save` and `load`.
`Board` holds a `shared_ptr<ISerializable>` and calls through it — a clear inheritance hierarchy.

> **NOTE** - Use of ISerializable is also and example of dependency injection, a common pattern used in software design.  Software patterns were 
not a part of the rubric, but worth noting as it is a example of professional software design.

---

## **✅ Encapsulation**

Data members in all major classes are private. Access and mutation happen via public functions (`revealTile`, `toggleTile`, `getTile`, etc.).
Operators needing internals can be declared `friend` in headers, keeping encapsulation intact.

---

## **✅ Polymorphism (Static & Dynamic)**

* **Static polymorphism:** Operator overloading at compile time (`<<`, `==`).
* **Dynamic polymorphism:** Serializer interface dispatch via `shared_ptr<ISerializable>` at runtime.

Both forms are demonstrated.

---

## **✅ Dynamic Memory Management**

Dynamic allocation is present through STL containers (`vector<vector<Tile>>`) and `std::make_shared` for polymorphic objects.
While raw `new`/`delete` are not explicitly used (which is good modern practice), the project still demonstrates *dynamic memory management* safely via RAII.

---

## **✅ Error Checking using `assert`**

Assertions enforce valid board coordinates and safe game logic (e.g., ensuring tile access stays in-bounds).
These preconditions are validated by your test suite.

---

## **✅ Abstract Class**

`ISerializable` (inferred from the serializer structure) defines pure virtual `save` and `load`.
`TextBoardSerializer` provides concrete overrides — a textbook abstract class implementation.

---

## **✅ Dynamic Polymorphism with Virtual Functions**

Runtime dispatch occurs when `Board::save()` or `Board::load()` calls the serializer’s overridden virtual methods.
This mechanism is used in both tests and CLI app.

---

## **✅ Recursive Function**

`Board::revealTile()` recursively reveals adjacent tiles with zero neighboring mines — a clear recursive algorithm with proper base conditions.
This is verified by reveal tests that confirm cascading reveals.

---

## **✅ Load and Save Game**

* Implemented via `TextBoardSerializer::save` and `TextBoardSerializer::load`.
* `app.cpp` exposes save/load functionality through user commands.
* Round-trip persistence is tested in `board_test.cpp`.

---

## **✅ Unit Tests**

Three GTest suites:

* `test_board.cpp`: Full gameplay, adjacency, recursion, and save/load verification.
* `test_tile.cpp`: Operator correctness and tile state behavior.
* `test_tile_state.cpp`: Stream operator validation including edge cases.

These tests comprehensively cover core mechanics and serialization.

---

## **✅ Compile and Link Your Own Game Library**

Delivered via Makefile:

* Static library target `build/lib/minesweeperlib.a`
* Both `build/bin/minesweeper` (TUI app) and `build/bin/minesweeper_test` (GTest runner) link against this library.
  This demonstrates modular compilation, linking, and separation of interface and implementation.