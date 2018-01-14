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

#include "main.h"
#include "checkersgame.h"
#include "backdrop.h"
#include "boardsquare.h"
#include "gamepiece.h"
#include <map>

std::map<std::pair<char, char>, char> gameBoard;
std::map<std::pair<char, char>, char> userCreatedBoard;

int playerTurn = White; //White goes first
int boardLayout = Standard;
int gameStatusFlag = ValidMove;

bool editedCustomBoardFlag = false;
bool resetFlag = false;
bool refreshFlag = false;


QString movesListString = QString("White\tBlack\n"); //keeps a list of the moves taken
QString movesListString2 = QString("White\tBlack\n"); //for the second column if the first fills up

void drawSceneBoard( QGraphicsScene & scene){
    int xOffset = 10;
    int yOffset = 85;

    QGraphicsTextItem * titleText = scene.addText(QString("CHECKERS"));
    titleText->setFont(QFont("Arial", 55));
    titleText->setPos(160, 0);

    QGraphicsTextItem * playerTurnText = scene.addText( (playerTurn == White) ? QString("White to move") : QString("Black to move") );
    playerTurnText->setFont(QFont("Calibri", 14));
    playerTurnText->setPos(0, 30);

    //Displays if the move was valid or if a colour has won
    QGraphicsTextItem * displayBar = scene.addText(QString(gameStateVector.at(gameStatusFlag).c_str()));
    displayBar->setFont(QFont("Arial", 18));
    displayBar->setPos(620+75, 30);

    //Displays the moves that have been made this game
    QGraphicsTextItem * movesList = scene.addText(movesListString);
    movesList->setFont(QFont("Times", 12));
    movesList->setPos(620+75, 150);
    //movesList->setTextWidth(100);
    movesList->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);

    if(movesListString.size() >= 694){ //if the text gets too long, off the window, start a new column
        QGraphicsTextItem * movesList2 = scene.addText(movesListString2);
        movesList2->setFont(QFont("Times", 12));
        movesList2->setPos(800+75, 150);
        movesList2->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);
    }

    QPushButton *resetButton = new QPushButton;
    QObject::connect(resetButton, &QPushButton::clicked, [](){resetFlag = true;});
    resetButton->setFont(QFont("Arial", 14));
    resetButton->setGeometry(QRect(620 + 75, 75, 120, 30));
    resetButton->setText("Reset Game");
    scene.addWidget(resetButton);

    QPushButton *refreshButton = new QPushButton;
    QObject::connect(refreshButton, &QPushButton::clicked, [](){refreshFlag = true;});
    refreshButton->setFont(QFont("Arial", 14));
    refreshButton->setGeometry(QRect(620 + 75 + 120, 75, 120, 30));
    refreshButton->setText("Refresh \u21BA"); //unicode refresh symbol
    scene.addWidget(refreshButton);

    if(boardLayout == CustomBoardCreate){ //Only shows play button if in the "Create" tab
        QPushButton *playButton = new QPushButton;
        QObject::connect(playButton, &QPushButton::clicked, [](){boardLayout = CustomBoardPlay; resetFlag = true;});
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
    if(editedCustomBoardFlag) comboBox->addItem(QString("Play Custom Board"), CustomBoardPlay);
    QObject::connect(comboBox, QOverload<int>::of(&QComboBox::activated),
        [&](int index){ if (index != 0) boardLayout = index; else boardLayout = Standard; });
    scene.addWidget(comboBox);

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

    if(userCreatingBoardFlag){
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
    scene->clear();
    if(gameStatusFlag != WhiteWin && gameStatusFlag != BlackWin && gameStatusFlag != Draw){ //if the game is running
        gameStatusFlag = takeTurn(gameBoard, std::make_pair(from, to), playerTurn);
        if (gameStatusFlag != ValidMove)
            std::cout<< gameStateVector.at(gameStatusFlag)<<std::endl;

        if(gameStatusFlag != InvalidMove){
            std::stringstream ss {};
            ss << char(toupper(from.first)) << from.second << " -> " << char(toupper(to.first)) << to.second << " ";
            if(gameStatusFlag == WhiteWin || gameStatusFlag == BlackWin || gameStatusFlag == Draw)
                ss<<"#";
            else if (playerTurn == White) //If the next player's turn is white
                ss << "\n";
            else
                ss <<"| ";

            if(movesListString.size() < 700)
                movesListString += QString(ss.str().c_str());
            else
                movesListString2 += QString(ss.str().c_str());
        }
    }
    drawSceneBoard(*scene);
    drawScenePieces(*scene, gameBoard);
}
//For creating custom layouts
void customBoardAddPiece(std::pair<char, char> square, int pieceType){
    userCreatedBoard[square] = pieces[pieceType];
    editedCustomBoardFlag = true;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    int width = 700; //620
    int height = 620;
    QGraphicsScene scene(0,0, width, height);
    resetBoard(gameBoard);
    emptyBoard(userCreatedBoard); //for custom created boards
    drawSceneBoard(scene);
    drawScenePieces(scene, gameBoard);

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
        if(resetFlag){
            userCreatingBoardFlag = false;
            switch(boardLayout){
            case Standard:
                resetBoard(gameBoard);
                break;
            case Kings:
                customBoardAllKings(gameBoard);
                break;
            case Jumpalicious:
                customBoardBigJumper(gameBoard);
                break;
            case TwoRows:
                customBoardEightPiecesEach(gameBoard);
                break;
            case CustomBoardCreate:
                editedCustomBoardFlag = false;
                emptyBoard(gameBoard);
                emptyBoard(userCreatedBoard);
                userCreatingBoardFlag = true;
                break;
            case CustomBoardPlay:
                gameBoard = userCreatedBoard;
                break;
                /*Room for future layouts*/
            }
            checkPromote(gameBoard);
            gameStatusFlag = checkWinStatus(gameBoard, playerTurn); //Protection for custom boards
            playerTurn = White;
            scene.clear();
            movesListString = QString("White\tBlack\n");
            drawSceneBoard(scene);
            drawScenePieces(scene, gameBoard);
            resetFlag = false;
        }
        if (refreshFlag){
            if(!userCreatingBoardFlag){
                scene.clear();
                drawSceneBoard(scene);
                drawScenePieces(scene, gameBoard);
                refreshFlag = false;
            }
        }
    });
    timer->start(100);
    view.show();
    return a.exec();
}
