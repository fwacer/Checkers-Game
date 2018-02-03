/*
 * File: ai.cpp
 * Author: Bryce Dombrowski
 *
 * Date: January 22nd, 2018
 *
 * Sites:   brycedombrowski.com
 *          github.com/fwacer
 *
 * Contact: brycetdombrowski@gmail.com
 */

#include "ai.h"
#include "checkersgame.h"
#include <QtGlobal>

std::pair<int, std::pair<char, char>> findBestJumpMoveAI(const std::map<std::pair<char, char>, char> &gameBoard,
                                                         const std::pair<char, char> &from){
    //Code look familiar? Much of it was written in checkersgame.cpp
    //It needed to now be modified to suit the AI, and copying the base tree explorer was easier then re-writing.

    //TODO: extra points for jumping kings
    //Optimise, currently very very slow (for what it should be) (probably too many "finds")

    std::set<std::pair<char, char>> possibilities {}; //squares that need to be searched
    std::set<std::pair<char, char>> searchedPossibilities{}; //squares that were already searched through

    int playerPiece = gameBoard.at(from);

    //The node could also be implemented as its own class
    struct Node {
    public:
        std::pair<char, char> square; //Which square this is
        std::stack<std::pair<char, char>> jumpPoints; //Where this node can jump to
        std::pair<char, char> jumpedOver = { 0,0 }; //What enemy token this node has jumped over
        bool searched = false;
        std::shared_ptr<Node> fromPtr = nullptr;
        Node(std::pair<char, char> square, std::shared_ptr<Node> fromPtr, std::pair<char, char> jumpedOver) {
            this->square = square;
            this->fromPtr = fromPtr;
            this->jumpedOver = jumpedOver;
        }
        Node(std::pair<char, char> x) {
            square = x;
        }
        Node() {

        }
    };

    Node current (from);
    possibilities.insert(from);

    int jumpedPieceAmount = 0;
    std::map<int, std::vector<std::pair<char, char>>> valueOfSquare{}; //Stores squares based on how many enemies they have jumped over.
    valueOfSquare[0].push_back(from);

    //auto temp = findJumpSquares(player, current.square, gameBoard);
    while (possibilities != searchedPossibilities) {
        auto temp = findJumpSquares(playerPiece, current.square, gameBoard); //from checkersgame.h

        if (!current.searched) {
            //assert(temp.first.size() == temp.second.size());
            for (unsigned int i = 0; i < temp.first.size(); i++) {
                auto el = temp.first.at(i);
                current.jumpPoints.push(el);
                possibilities.insert(el);
            }
            current.searched = true;
        }
        if (!current.jumpPoints.empty()) {
            auto top = current.jumpPoints.top();

            //Cycles through 'tops' that have already been searched
            while (searchedPossibilities.find(top) != searchedPossibilities.end()) {
                if (!current.jumpPoints.empty()) //avoid crash
                    current.jumpPoints.pop();

                if (current.jumpPoints.empty()) //avoid crash
                    break;
                else
                    top = current.jumpPoints.top();
            }
        }
        if (current.jumpPoints.empty()) { //Check if end of the line
            if (current.fromPtr == nullptr) {
                break;
            }
            else {
                //std::cout << "Tried: " << current.square.first << current.square.second << std::endl;
                valueOfSquare[jumpedPieceAmount].push_back(current.square);
                current = *(current.fromPtr); //return, at end of the line
                jumpedPieceAmount--;
            }
        }
        else {
            auto top = current.jumpPoints.top();
            current.jumpPoints.pop();
            searchedPossibilities.insert(current.square);
            current = Node(top, std::make_shared<Node>(current), temp.second.at(current.jumpPoints.size()/*-1 todo */)); //search down the branch
            jumpedPieceAmount++;
        }

    }
    //returns:
    //  pair   -> vector of points -> squares that the piece jumped to on the way to the finish
    //         -> vector of points -> enemy pieces squares that were jumped over along the path
    //
    // Note: the first vector of points is returned for the future implementation of animating the jumping sequence.
    std::pair<char, char> to = from; //default
    int val = 0;
    for (auto it = valueOfSquare.begin(); it != valueOfSquare.end(); ++it){
        if (it->first > val){
            val = it->first;
            int randomIndex = qrand() % it->second.size();
            to = it->second.at(randomIndex);
        }
    }

    if(val == 0){
        return {0, to};
    }
    else
        return { val, to };
}

std::pair<char, char> findSingleSquareMoveAI(const std::map<std::pair<char, char>, char> & gameBoard,
                                             const std::pair<char, char> & from) {

    std::vector<std::pair<char, char>> possibleMoves {};
    char playerPiece = gameBoard.find(from)->second;
    auto it = gameBoard.find(std::make_pair(from.first + 1, from.second + 1));

    if (playerPiece == pieces[Black] || playerPiece == pieces[BlackKing] || playerPiece == pieces[WhiteKing]) { //Pieces that can move up
        //top right square
        it = gameBoard.find(std::make_pair(from.first + 1, from.second + 1));
        if(it != gameBoard.end()){
            if(it->second == pieces[Empty]){ //if the square is empty
                possibleMoves.push_back(it->first);
            }
        }

        //top left square
        it = gameBoard.find(std::make_pair(from.first - 1, from.second + 1));
        if(it != gameBoard.end()){
            if(it->second == pieces[Empty]){ //if the square is empty
                possibleMoves.push_back(it->first);
            }
        }

    }
    if (playerPiece == pieces[White] || playerPiece == pieces[BlackKing] || playerPiece == pieces[WhiteKing]) { // Pieces that can move up
        //bottom left square
        it = gameBoard.find(std::make_pair(from.first - 1, from.second - 1));
        if(it != gameBoard.end()){
            if(it->second == pieces[Empty]){ //if the square is empty
                possibleMoves.push_back(it->first);
            }
        }
        //bottom right square
        it = gameBoard.find(std::make_pair(from.first + 1, from.second - 1));
        if(it != gameBoard.end()){
            if(it->second == pieces[Empty]){ //if the square is empty
                possibleMoves.push_back(it->first);
            }
        }
    }
    if(possibleMoves.size() == 0){
        return {'z', 'z'};
    }else{
        int randomIndex = qrand() % possibleMoves.size();
        return possibleMoves.at(randomIndex);
    }
}

std::pair<std::pair<char, char>, std::pair<char, char>> getMoveAI(std::map<std::pair<char, char>, char> & gameBoard,
               int & playerTurn)
{
    //find all the pieces available to move
    std::vector<std::pair<char, char>> pieceVec;
    for (auto it = gameBoard.begin(); it != gameBoard.end(); ++it){
        if (it->second == pieces[playerTurn] || it->second==pieces[playerTurn+1]){
            pieceVec.push_back(it->first);
        }
    }
    if (pieceVec.size() == 0){ // sanity check
        std::cout<<"Programmer error: AI has no pieces to move."<<std::endl;
        throw "Programmer error: AI has no pieces to move.";
    }

    std::pair<std::pair<char, char>, std::pair<char, char>> bestMove = std::make_pair(std::make_pair('z', 'z'), std::make_pair('z', 'z'));
    int val = 0;
    std::vector<std::pair<std::pair<char, char>, std::pair<char, char>>> possibleMoves {};
    for(auto it = pieceVec.begin(); it != pieceVec.end(); ++it){
        auto temp = findBestJumpMoveAI(gameBoard, *it);
        if(temp.first > val){
            std::vector<std::pair<std::pair<char, char>, std::pair<char, char>>> newPossibleMoves;
            newPossibleMoves.push_back(std::make_pair(*it, temp.second));
            val = temp.first;
            possibleMoves = newPossibleMoves;
        }else if (temp.first == val){
            possibleMoves.push_back(std::make_pair(*it, temp.second));
        }
    }

    if(val>0){
        /*for (auto it = possibleMoves.begin(); it != possibleMoves.end(); ++it)
            std::cout << char(it->first.first)<<char(it->first.second) << "->" <<char(it->second.first)<<char(it->second.second) << std::endl;
        */
        int randomIndex = qrand() % possibleMoves.size();
        bestMove = possibleMoves.at(randomIndex);
    }

    else { //Don't bother looking for a single square if a jump square was found
        std::vector<std::pair<std::pair<char, char>, std::pair<char, char>>> possibleSingleMoves {};
        for(auto it = pieceVec.begin(); it != pieceVec.end(); ++it){
            std::pair<char, char> temp = findSingleSquareMoveAI(gameBoard, *it);
            if (temp != std::make_pair('z', 'z')){
                possibleSingleMoves.push_back(std::make_pair(*it, temp));
            }
        }

        int randomIndex = qrand() % possibleSingleMoves.size();
        bestMove = possibleSingleMoves.at(randomIndex);

    }
    if (bestMove.second == std::make_pair('z', 'z')){ // Ensure a move has been found
        std::cout<<"Programmer error: AI has no valid moves."<<std::endl;
        throw "Programmer error: AI has no valid moves.";
    }

    return bestMove;
}
