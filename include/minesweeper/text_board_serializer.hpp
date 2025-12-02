/*                                       
 *   _____ _                                       
 *  |     |_|___ ___ ___ _ _ _ ___ ___ ___ ___ ___ 
 *  | | | | |   | -_|_ -| | | | -_| -_| . | -_|  _|
 *  |_|_|_|_|_|_|___|___|_____|___|___|  _|___|_|  
 *                                  |_|          
 */
#include <iostream>
#include "board.hpp"

#ifndef TEXTBOARD_SERIALIZER
#define TEXTBOARD_SERIALIZER
class TextBoardSerializer : public ISerializable {
public:
    TextBoardSerializer() = default;
    ~TextBoardSerializer() override = default;
    
    int save(Board& board, std::ostream& out) override;
    int load(Board& board, std::istream& in) override;
private:
    TileState intToTileState(int value);
};
#endif
