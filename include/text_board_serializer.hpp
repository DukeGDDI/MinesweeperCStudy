#include "minesweeper.hpp"

class TextBoardSerializer : public ISerializable {
public:
    int save(const Board& board, std::ostream& out) override;
    int load(Board& board, std::istream& in) override;
};
