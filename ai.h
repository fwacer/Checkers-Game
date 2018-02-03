/*
 * File: ai.h
 * Author: Bryce Dombrowski
 *
 * Date: January 22nd, 2018
 *
 * Sites:   brycedombrowski.com
 *          github.com/fwacer
 *
 * Contact: brycetdombrowski@gmail.com
 */

#ifndef AI_H
#define AI_H

#include <stdio.h>
#include <stdlib.h>

#include <vector>
#include <map>
#include <stack>

std::pair<int, std::pair<char, char>> findBestJumpMoveAI(const std::map<std::pair<char, char>, char> & gameBoard,
                                                         const std::pair<char, char> & from);

std::pair<char, char> findSingleSquareMoveAI(const std::pair<char, char> & from,
                      const std::map<std::pair<char, char>, char> & gameBoard);

std::pair<std::pair<char, char>, std::pair<char, char>> getMoveAI(std::map<std::pair<char, char>, char> & gameBoard,
               int & playerTurn);

#endif
