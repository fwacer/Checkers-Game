/*
 * File: main.cpp
 * Title: Checkers Game
 * Author: Bryce Dombrowski
 *
 * Date: January 13th, 2018
 *
 * Sites:   brycedombrowski.com
 *          github.com/fwacer
 *
 * Contact: brycetdombrowski@gmail.com
 *
 * Info: This project began as an exercise to keep in coding shape over the winter break.
 * After finishing a rudimentary version of checkers that took place in the console, I very
 * quickly became tired of manually typing every move. This is when I decided to add a clickable
 * and shiny front end. Over the course of this project I learned improved coding practices,
 * which I hope is reflected in this final product, even if it is not perfect. If you have
 * any questions or have any feedback, please contact me! I would love to hear your input.
 *
 * Things to improve:
 * Currently, the whole scene is discarded and redrawn every time a move is made. There is
 * probably a better way to do this, by somehow keeping track of all objects in the scene and
 * adding or removing them as necessary.
 *
 * Known bug:
 * If a game piece is dragged and dropped outside of the gameboard or window, the piece is
 * not redrawn on the board. It can be redrawn by making some sort of move (legal or not,
 * does not matter) or by pressing the "refresh" button. This behaviour is caused by the
 * mouseReleaseEvent not triggering over any of the objects of the scene. If you know a
 * solution, please let me know.
 *
 * Acknowledgements:
 * This project had help from many of the Qt examples, especially:
 * http://doc.qt.io/qt-5/qtwidgets-graphicsScene-dragdroprobot-example.html
 */

#include <QTimer>
#include <QPushButton>
#include <QComboBox>
#include <QMessageBox>
#include <QCheckBox>

#include "main.h"
#include "checkersgame.h"
#include "backdrop.h"
#include "boardsquare.h"
#include "gamepiece.h"
#include "ai.h"
#include <map>

namespace CV{
    std::map<std::pair<char, char>, char> gameBoard;
    std::map<std::pair<char, char>, char> userCreatedBoard;

    int playerTurn = White; //White goes first
    int boardLayout = Standard;
    int gameStatus = ValidMove; //Displays in top right hand corner

    QString movesListString = QString("White\tBlack\n"); //keeps a list of the moves taken
    QString movesListString2 = QString("White\tBlack\n"); //for the second column if the first fills up
}
namespace CF{
    bool editedCustomBoardFlag = false; //Has a custom board been created?
    bool resetFlag = false; //Reset the game
    bool refreshFlag = false; //Refresh the scene

    bool whiteAIFlag = false; //Is white AI-controlled?
    bool blackAIFlag = false; //Is black AI-controlled?
    bool playerMovingFlag = false; //Avoids interrupting
}

void drawSceneBoard( QGraphicsScene & scene){
    int xOffset = 10;
    int yOffset = 85;

    QGraphicsTextItem * titleText = scene.addText(QString("CHECKERS"));
    titleText->setFont(QFont("Arial", 55));
    titleText->setPos(160, 0);

    //Displays github link
    QGraphicsTextItem * brandingLink = scene.addText(QString("https://github.com/fwacer/Checkers-Game"));
    brandingLink->setFont(QFont("Arial", 10));
    brandingLink->setPos(574, 0);
    brandingLink->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);

    //Displays author name
    QGraphicsTextItem * authorName = scene.addText(QString("By Bryce"));
    authorName->setFont(QFont("Calibri", 16));
    authorName->setPos(575, 50);

    QGraphicsTextItem * playerTurnText = scene.addText( (CV::playerTurn == White) ? QString("White to move") : QString("Black to move") );
    playerTurnText->setFont(QFont("Arial", 16));
    playerTurnText->setPos(0, 30);

    //Displays if the move was valid or if a colour has won
    QGraphicsTextItem * displayBar = scene.addText(QString(CV::gameStateVector.at(CV::gameStatus).c_str()));
    displayBar->setFont(QFont("Arial", 22));
    displayBar->setPos(620+75, 30);

    //Displays the moves that have been made this game
    QGraphicsTextItem * movesList = scene.addText(CV::movesListString);
    movesList->setFont(QFont("Times", 12));
    movesList->setPos(620+75, 150);
    //movesList->setTextWidth(100);
    movesList->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);

    if(CV::movesListString.size() >= 694){ //if the text gets too long, off the window, start a new column
        QGraphicsTextItem * movesList2 = scene.addText(CV::movesListString2);
        movesList2->setFont(QFont("Times", 12));
        movesList2->setPos(800+75, 150);
        movesList2->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);
    }

    //reset button
    QPushButton *resetButton = new QPushButton;
    QObject::connect(resetButton, &QPushButton::clicked, [](){CF::resetFlag = true;});
    resetButton->setFont(QFont("Arial", 14));
    resetButton->setGeometry(QRect(620 + 75, 75, 120, 30));
    resetButton->setText("Reset Game");
    scene.addWidget(resetButton);

    //refresh button
    QPushButton *refreshButton = new QPushButton;
    QObject::connect(refreshButton, &QPushButton::clicked, [](){CF::refreshFlag = true;});
    refreshButton->setFont(QFont("Arial", 14));
    refreshButton->setGeometry(QRect(620 + 75 + 120, 75, 120, 30));
    refreshButton->setText("Refresh \u21BA"); //unicode refresh symbol
    scene.addWidget(refreshButton);

    if(CV::boardLayout == CustomBoardCreate){ //Only shows play button if in the "Create" tab
        QPushButton *playButton = new QPushButton;
        QObject::connect(playButton, &QPushButton::clicked,
                         [](){CV::boardLayout = CustomBoardPlay; CF::resetFlag = true;});
        playButton->setFont(QFont("Arial", 14));
        playButton->setGeometry(QRect(620 + 75, 30, 240, 45));
        playButton->setText("Play!");
        scene.addWidget(playButton);
    }

    QComboBox *comboBox = new QComboBox;
    comboBox->setFont(QFont("Arial", 14));
    comboBox->setGeometry(620+75, 105, 240/*120*/, 30);
    comboBox->setFrame(true);
    comboBox->addItem(QString("Select Board Layout  \u25BC"), 0); //unicode down pointer
    comboBox->addItem(QString("Standard"), Standard);
    comboBox->addItem(QString("Kings"), Kings);
    comboBox->addItem(QString("Jumpalicious"), Jumpalicious);
    comboBox->addItem(QString("TwoRows"), TwoRows);
    comboBox->addItem(QString("Create Custom Board"), CustomBoardCreate);
    if(CF::editedCustomBoardFlag) comboBox->addItem(QString("Play Custom Board"), CustomBoardPlay);
    QObject::connect(comboBox, QOverload<int>::of(&QComboBox::activated),
        [&](int index){ if (index != 0) CV::boardLayout = index; else CV::boardLayout = Standard; });
    scene.addWidget(comboBox);

    QCheckBox *checkboxWhiteAI = new QCheckBox;
    checkboxWhiteAI->setFont(QFont("Arial", 14));
    checkboxWhiteAI->setText("White AI");
    checkboxWhiteAI->setGeometry(QRect(620 + 75 + 240, 75, 120, 30));
    checkboxWhiteAI->setCheckState(CF::whiteAIFlag ? Qt::Checked : Qt::Unchecked);
    QObject::connect(checkboxWhiteAI, QOverload<int>::of(&QCheckBox::stateChanged),
        [&](){ CF::whiteAIFlag = !CF::whiteAIFlag; CF::refreshFlag = true; });
    scene.addWidget(checkboxWhiteAI);

    QCheckBox *checkboxBlackAI = new QCheckBox;
    checkboxBlackAI->setFont(QFont("Arial", 14));
    checkboxBlackAI->setText("Black AI");
    checkboxBlackAI->setGeometry(QRect(620 + 75 + 240, 100, 120, 30));
    checkboxBlackAI->setCheckState(CF::blackAIFlag ? Qt::Checked : Qt::Unchecked);
    QObject::connect(checkboxBlackAI, QOverload<int>::of(&QCheckBox::stateChanged),
        [&](){ CF::blackAIFlag = !CF::blackAIFlag; CF::refreshFlag = true;});
    scene.addWidget(checkboxBlackAI);


    QPushButton *authorInfoButton = new QPushButton;
    QObject::connect(authorInfoButton, &QPushButton::clicked, [](){
        //Pop up to show author info
        QMessageBox::information(0, QString("Author info"),
                                 QString("This Checkers GUI was created by Bryce Dombrowski, using C++ and Qt.\n"
                                         "Links:\n"
                                         "https://github.com/fwacer/Checkers-Game\n"
                                         "https://brycedombrowski.com/2018/01/13/winter-2017-18-checkers-game-gui/"), QMessageBox::Ok);
    });
    authorInfoButton->setFont(QFont("Arial", 14));
    authorInfoButton->setGeometry(QRect(620 + 75 + 130, 75 + 60, 110, 30));
    authorInfoButton->setText("Author Info");
    scene.addWidget(authorInfoButton);

    //std::map<std::pair<char, char>, QGraphicsItem> visualBoard;
    QGraphicsItem *BackdropItem = new Backdrop(); //can accept drops and return an error if the user misses dropping on a valid square
    scene.addItem(BackdropItem);

    //Prints the black squares of the board
    for (char y = '1'; y <= '8'; y++){
        for (char x = 'a'; x <= 'h'; x++){
            if ((((y - 49) % 2) == 0) == (((x - 97) % 2) == 0)){  //XNOR to help with the diagonalness of the board
                QGraphicsItem *boardSquareItem = new BoardSquare((x-97)*75 + 75, 525-(y-49)*75 + yOffset, std::make_pair(x, y));
                //std::cout<<"Placed at "<<x<<","<<y<<std::endl;
                //visualBoard[std::make_pair(x, y)] = boardSquareItem;
                scene.addItem(boardSquareItem);
            //else -> is a white square, so doesn't matter
            }
        }
    }

    //Border rectangle
    scene.addRect(75, yOffset, 600, 600);

    //Draws the row numbers
    for(int i = 0; i < 8; i++){
        QGraphicsSimpleTextItem * number = new QGraphicsSimpleTextItem();
        number->setFont(QFont("Times",55));
        number->setPos(0, (75 * i)+yOffset-10);
        std::string s{};
        s += '8' - i;
        number->setText(QString(s[0]));
        scene.addItem(number);
    }
    //Draws the column letters

    for(int i = 0; i < 8; i++){
        QGraphicsSimpleTextItem * letter = new QGraphicsSimpleTextItem();
        letter->setFont(QFont("Times",55));
        letter->setPos(xOffset + 75 + (75 * i), 600 + yOffset);
        std::string s{};
        s += 'A' + i;
        letter->setText(QString(s[0]));
        scene.addItem(letter);
    }

    if(CF::userCreatingBoardFlag){
        QMessageBox::information(0, QString("Information"),
                                 QString("Welcome to the board layout creator!\n"
                                         "Left click a square to cycle through the colours.\n"
                                         "Right click to promote/demote.\n"
                                         "Press Play when you are ready.\n"
                                         "This board will not be saved if you enter the board creator again."), QMessageBox::Ok);

    }
}
void drawScenePieces(QGraphicsScene & scene, std::map<std::pair<char, char>, char> & gameBoard){
    //int xOffset = 10;
    int yOffset = 85;
    QColor colour = Qt::white;
    for (char y = '1'; y <= '8'; y++) {
        for (char x = 'a'; x <= 'h'; x++) {
            if ((((y-49) % 2) == 0) == (((x - 97) % 2) == 0)) { //Helps with the diagonalness of the board
                char piece = gameBoard[std::make_pair(x,y)];
                bool king = false;
                //std::cout<<"Found ["<<piece<<"] at "<<x<<","<<y<<std::endl;
                if (piece == pieces[Black]){
                    colour = Qt::black;
                }else if(piece == pieces[BlackKing]){
                    colour = Qt::black;
                    king = true;
                }else if (piece == pieces[White]){
                    colour = Qt::white;
                }else if (piece == pieces[WhiteKing]){
                    colour = Qt::white;
                    king = true;
                }else{
                    continue;
                }
                //std::cout<<"Placed ["<<(((piece == pieces[2]) || (piece == pieces[1])) ? "x" : "o")<<"] at "<<x<<","<<y<<std::endl;
                QGraphicsItem *gamePieceItem = new GamePiece((x-97)*75 + 75, 525 - (y-49)*75+yOffset, colour, std::make_pair(x, y), king);
                scene.addItem(gamePieceItem);
            }
            //else -> not on board, so doesn't matter
        }
    }
}
void redrawBoard(std::pair<char, char> from, std::pair<char, char> to, QGraphicsScene * scene){
    CF::playerMovingFlag = true;
    std::cout<<"Move: "<<char(from.first)<<char(from.second)<<"->"<<char(to.first)<<char(to.second)<<std::endl;
    scene->clear();
    if(CV::gameStatus != WhiteWin && CV::gameStatus != BlackWin && CV::gameStatus != Draw){ //if the game is running

        CV::gameStatus = takeTurn(CV::gameBoard, std::make_pair(from, to), CV::playerTurn);

        if (CV::gameStatus != ValidMove)
            std::cout<< CV::gameStateVector.at(CV::gameStatus)<<std::endl;

        if(CV::gameStatus != InvalidMove){
            std::stringstream ss {};
            ss << char(toupper(from.first)) << from.second << " -> " << char(toupper(to.first)) << to.second << " ";
            if(CV::gameStatus == WhiteWin || CV::gameStatus == BlackWin || CV::gameStatus == Draw)
                ss<<"#";
            else if (CV::playerTurn == White) //If the next player's turn is white
                ss << "\n";
            else
                ss <<"| ";

            if(CV::movesListString.size() < 700)
                CV::movesListString += QString(ss.str().c_str());
            else
                CV::movesListString2 += QString(ss.str().c_str());
        }
    }
    drawSceneBoard(*scene);
    drawScenePieces(*scene, CV::gameBoard);
    CF::playerMovingFlag = false;
}
//For creating custom layouts
void customBoardAddPiece(std::pair<char, char> square, int pieceType){
    CV::userCreatedBoard[square] = pieces[pieceType];
    CF::editedCustomBoardFlag = true;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    int width = 700; //620
    int height = 620;
    QGraphicsScene scene(0,0, width, height);
    resetBoard(CV::gameBoard);
    emptyBoard(CV::userCreatedBoard); //for custom created boards
    drawSceneBoard(scene);
    drawScenePieces(scene, CV::gameBoard);

    GraphicsView view(&scene);
    QRectF bounds = scene.itemsBoundingRect();
    bounds.setWidth(bounds.width()*0.9);
    bounds.setHeight(bounds.height()*0.9);

    view.fitInView(bounds, Qt::KeepAspectRatio);

    view.setRenderHint(QPainter::Antialiasing);
    view.setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    view.setBackgroundBrush(QColor(255,255,255));
    view.setWindowTitle("Checkers");
    view.showMaximized();

    QTimer *timer = new QTimer;
    QObject::connect(timer, &QTimer::timeout, [&scene](){
        if(CF::resetFlag){
            CF::userCreatingBoardFlag = false;
            switch(CV::boardLayout){
            case Standard:
                resetBoard(CV::gameBoard);
                break;
            case Kings:
                customBoardAllKings(CV::gameBoard);
                break;
            case Jumpalicious:
                customBoardBigJumper(CV::gameBoard);
                break;
            case TwoRows:
                customBoardEightPiecesEach(CV::gameBoard);
                break;
            case CustomBoardCreate:
                CF::editedCustomBoardFlag = false;
                emptyBoard(CV::gameBoard);
                emptyBoard(CV::userCreatedBoard);
                CF::userCreatingBoardFlag = true;
                break;
            case CustomBoardPlay:
                CV::gameBoard = CV::userCreatedBoard;
                break;
                /*Room for future layouts*/
            }
            checkPromote(CV::gameBoard);
            CV::gameStatus = checkWinStatus(CV::gameBoard, CV::playerTurn); //Protection for custom boards
            CV::playerTurn = White;
            scene.clear();
            CV::movesListString = QString("White\tBlack\n");
            CV::movesListString2 = QString("White\tBlack\n");
            drawSceneBoard(scene);
            drawScenePieces(scene, CV::gameBoard);
            CF::resetFlag = false;
        }
        else if(!CF::playerMovingFlag && !CF::userCreatingBoardFlag
                && CV::gameStatus != WhiteWin && CV::gameStatus != BlackWin && CV::gameStatus != Draw){

            //AI turn, if enabled
            if(CF::whiteAIFlag && CV::playerTurn == White){ //If it's White's turn and an AI is controlling it
                auto move = getMoveAI(CV::gameBoard, CV::playerTurn);
                redrawBoard(move.first, move.second, &scene);
            }
            else if(CF::blackAIFlag && CV::playerTurn == Black){ //If it's Blacks's turn and an AI is controlling it
                auto move = getMoveAI(CV::gameBoard, CV::playerTurn);
                redrawBoard(move.first, move.second, &scene);
            }
        }
        if (CF::refreshFlag){
            if(!CF::userCreatingBoardFlag){
                scene.clear();
                drawSceneBoard(scene);
                drawScenePieces(scene, CV::gameBoard);
                CF::refreshFlag = false;
            }
        }

    });
    timer->start(100);
    view.show();
    return a.exec();
}
