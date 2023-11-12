#include <iostream>
#include <SFML/Graphics.hpp>
#include <chrono>
#include <thread>
#include <stdlib.h>
#include <Windows.h>

using namespace std;

#define SCR_WIDTH 480
#define SCR_HEIGHT 640

#define MAIN_MENU 0
#define GAME_PLAYING 1
#define GAME_OVER 2

sf::Text scoreText;

sf::Texture mainMenuBackgroundIMG;
sf::Texture gameBackgroundIMG;

sf::Texture newGameButtonTexture;
sf::Texture highScoresButtonTexture;
sf::Texture quitGameButtonTexture;
sf::Texture newGameButtonSelectedTexture;
sf::Texture highScoresButtonSelectedTexture;
sf::Texture quitGameButtonSelectedTexture;
sf::Texture returnToMenuTexture;
sf::Texture returnToMenuSelectedTexture;

sf::Sprite mainMenuBackgroundSprite;
sf::Sprite gameBackgroundSprite;
sf::Sprite newGameButtonSprite;
sf::Sprite highScoresButtonSprite;
sf::Sprite quitGameButtonSprite;
sf::Sprite returnToMenuSprite;

short choice = 0; //menu selection choice
int score = 0; //player score
short gameState = MAIN_MENU;

const int boardWidth = 10;
const int boardHeight = 20;

int board[boardHeight][boardWidth] = { 0 };

sf::RectangleShape square(sf::Vector2f(29, 29));

struct Piece
{
    int x;
    int y;
    int shape[4][4];
    sf::Color shapeColor;

}currentPiece, nextPiece;

int tetrisShapes[7][4][4] = 
{
    {{1, 1, 1, 1},
     {0, 0, 0, 0},
     {0, 0, 0, 0},
     {0, 0, 0, 0}},

    {{1, 1, 1, 0},
     {1, 0, 0, 0},
     {0, 0, 0, 0},
     {0, 0, 0, 0}},

    {{1, 1, 1, 0},
     {0, 0, 1, 0},
     {0, 0, 0, 0},
     {0, 0, 0, 0}},

    {{1, 1, 0, 0},
     {1, 1, 0, 0},
     {0, 0, 0, 0},
     {0, 0, 0, 0}},

    {{0, 1, 1, 0},
     {1, 1, 0, 0},
     {0, 0, 0, 0},
     {0, 0, 0, 0}},

    {{1, 1, 0, 0},
     {0, 1, 1, 0},
     {0, 0, 0, 0},
     {0, 0, 0, 0}},

    {{0, 1, 0, 0},
     {1, 1, 1, 0},
     {0, 0, 0, 0},
     {0, 0, 0, 0}}
};

sf::Color colors[] =        //List of colors that are going to be chosen randomly for shapes
{
    sf::Color::Red,
    sf::Color::Blue,
    sf::Color::Green,
    sf::Color::Magenta,
    sf::Color::Yellow,
    sf::Color::White,
    sf::Color::Cyan,
};

bool isCollision() 
{
    for (int i = 0; i < 4; i++) 
    {
        for (int j = 0; j < 4; j++) 
        {
            if (currentPiece.shape[i][j] != 0) 
            {
                int boardX = currentPiece.x + j;
                int boardY = currentPiece.y + i;

                // Check boundaries and collision with existing pieces
                if (boardX < 0 || boardX >= boardWidth || boardY >= boardHeight || board[boardY][boardX] != 0) {
                    return true;
                }
            }
        }
    }
    return false;
}

void mergePiece() 
{
    for (int i = 0; i < 4; i++) 
    {
        for (int j = 0; j < 4; j++) 
        {
            if (currentPiece.shape[i][j] != 0) 
            {
                int boardX = currentPiece.x + j;
                int boardY = currentPiece.y + i;
                board[boardY][boardX] = 1;
            }
        }
    }
}

void clearLines() 
{
    for (int i = boardHeight - 1; i >= 0; i--) 
    {
        bool lineFull = true;
        for (int j = 0; j < boardWidth; ++j) 
        {
            if (board[i][j] == 0) 
            {
                lineFull = false;
                break;
            }
        }

        if (lineFull)
        {
            for (int k = i; k > 0; k--) 
            {
                for (int j = 0; j < boardWidth; j++) 
                {
                    board[k][j] = board[k - 1][j];
                }
            }
            i++; // Re-check the same line since it has been shifted down
            score += 10;
            scoreText.setString(to_string(score));
        }
    }
}

void initGame() 
{
    currentPiece.x = boardWidth / 2 - 1;
    currentPiece.y = 0;

    nextPiece.x = boardWidth / 2 - 1;
    nextPiece.y = 0;

    int randomCurrentShape = rand() % 7;
    int randomNextShape = rand() % 7;
    sf::Color color = colors[rand() % 7];
    currentPiece.shapeColor = color;

    color = colors[rand() % 7];
    nextPiece.shapeColor = color;

    for (int i = 0; i < 4; i++) 
    {
        for (int j = 0; j < 4; j++) 
        {
            currentPiece.shape[i][j] = tetrisShapes[randomCurrentShape][i][j];
            nextPiece.shape[i][j] = tetrisShapes[randomNextShape][i][j];
        }
    }

}

void generatePiece()
{
    currentPiece.x = nextPiece.x;
    currentPiece.y = nextPiece.y;
    currentPiece.shapeColor = nextPiece.shapeColor;
    
    int randomShape = rand() % 7;
    sf::Color color = colors[rand() % 7];
    nextPiece.shapeColor = color;

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            currentPiece.shape[i][j] = nextPiece.shape[i][j];
            nextPiece.shape[i][j] = tetrisShapes[randomShape][i][j];
        }
    }

}

void movePieceDown() 
{
    currentPiece.y ++;

    if (isCollision()) 
    {
        currentPiece.y--;
        mergePiece();
        clearLines();

        // Check if the merged piece causes game over
        if (currentPiece.y == 0) 
        {
            gameState = GAME_OVER;
            
            return;
        }

        generatePiece();
    }
    this_thread::sleep_for(chrono::milliseconds(150));
}

void rotateShape() 
{
    Piece rotatedShape = currentPiece;
    Piece originalShape = currentPiece;

    for (int i = 0; i < 4; i++) 
    {
        for (int j = 0; j < 4; j++) 
        {
            rotatedShape.shape[i][j] = currentPiece.shape[3 - j][i];
        }
    }

    if (isCollision()) 
        currentPiece = originalShape; // Restore the original piece if rotation causes a collision


    currentPiece = rotatedShape;
}

void checkIfKeyIsPressed()
{
    switch (gameState)
    {
    case MAIN_MENU:
    {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && choice > 0)
            choice--;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) && choice < 3)
            choice++;

        break;
    }
    case GAME_PLAYING:
    {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
            rotateShape();

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
            movePieceDown();

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
        {
            currentPiece.x--;
            if (isCollision())
                currentPiece.x++;

        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
        {
            currentPiece.x++;

            if (isCollision())
                currentPiece.x--;
        }

        break;
    }
    case GAME_OVER:
    {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter))
        {
            gameState = MAIN_MENU;
            score = 0;
        }

        this_thread::sleep_for(chrono::milliseconds(50));
        break;
    }

    default:
        break;
    }

    this_thread::sleep_for(chrono::milliseconds(50));
}

void loadTextures()
{
    if (!mainMenuBackgroundIMG.loadFromFile("mainMenuBackground.png"))
        cout << "Could not load mainMenuBackground.png image" << "\n";

    if (!gameBackgroundIMG.loadFromFile("gameBackground.jpg"))
        cout << "Could not load gameBackground.jpg image" << "\n";

    if (!newGameButtonTexture.loadFromFile("newGameButton.png"))
        cout << "Could not load newGameButton.png image" << "\n";

    if (!highScoresButtonTexture.loadFromFile("highScoresButton.png"))
        cout << "Could not load highScoresButton.png image" << "\n";

    if (!quitGameButtonTexture.loadFromFile("quitGameButton.png"))
        cout << "Could not load quitGameButton.png image" << "\n";

    if (!returnToMenuTexture.loadFromFile("returnToMenuButton.png"))
        cout << "Could not load returnToMenuButton.png image" << "\n";

    if (!returnToMenuSelectedTexture.loadFromFile("returnToMenuSelectedButton.png"))
        cout << "Could not load returnToMenuSelectedButton.png image" << "\n";

    if (!newGameButtonSelectedTexture.loadFromFile("newGameButtonSelected.png"))
        cout << "Could not load newGameButtonSelected.png image" << "\n";

    if (!highScoresButtonSelectedTexture.loadFromFile("highScoresButtonSelected.png"))
        cout << "Could not load highScoresButtonSelected.png image" << "\n";

    if (!quitGameButtonSelectedTexture.loadFromFile("quitGameButtonSelected.png"))
        cout << "Could not load quitGameButtonSelected.png image" << "\n";
}

void loadSprites()
{
    mainMenuBackgroundSprite.setTexture(mainMenuBackgroundIMG);
    mainMenuBackgroundSprite.setScale(sf::Vector2f(0.315, 0.24));

    gameBackgroundSprite.setTexture(gameBackgroundIMG);
    gameBackgroundSprite.setScale(sf::Vector2f(0.9, 0.96));

    newGameButtonSprite.setTexture(newGameButtonTexture);
    newGameButtonSprite.setPosition(sf::Vector2f(SCR_WIDTH / 3.4, SCR_HEIGHT / 2.8));

    highScoresButtonSprite.setTexture(highScoresButtonTexture);
    highScoresButtonSprite.setPosition(sf::Vector2f(SCR_WIDTH / 3.4, SCR_HEIGHT / 2.05));

    quitGameButtonSprite.setTexture(quitGameButtonTexture);
    quitGameButtonSprite.setPosition(sf::Vector2f(SCR_WIDTH / 3.4, SCR_HEIGHT / 1.6));

    returnToMenuSprite.setTexture(returnToMenuTexture);
    returnToMenuSprite.setPosition(sf::Vector2f(SCR_WIDTH / 3.4, SCR_HEIGHT / 2.2));
}

int main()
{
    short x = rand() % 2;
    square.setFillColor(sf::Color::Black);  

    sf::Font font;
    sf::Text gameOverText;
    sf::Color color;

    sf::RenderWindow window(sf::VideoMode(SCR_WIDTH, SCR_HEIGHT), "Game"); //Function that creates a window with "Tetris" title,
                                                                           //width and height values are taken from SCR_WIDTH and SCR_HEIGHT

    loadTextures();
    loadSprites();
    initGame();
   

    sf::RectangleShape gameOverBackground(sf::Vector2f(480, 400));
    gameOverBackground.setPosition(0, 100);
    gameOverBackground.setFillColor(sf::Color(132, 132, 132, 175));  
 
    square.setPosition(sf::Vector2f(200, 10));
    square.setFillColor(sf::Color::Red);

    if (!font.loadFromFile("Chewy.ttf"))        //Checks if font can be loaded
    {
        cout << "Font could not be loaded";
    }  
    
    //Text initialization here

    gameOverText.setFont(font);
    gameOverText.setString("Game Over");
    gameOverText.setFillColor(sf::Color::White);
    gameOverText.setPosition(SCR_WIDTH/3, SCR_HEIGHT/4);
    gameOverText.setCharacterSize(48);

    scoreText.setFont(font);
    scoreText.setString(to_string(score));
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(SCR_WIDTH - 130, SCR_HEIGHT/2.3);
    scoreText.setCharacterSize(20);

    vector <vector <sf::RectangleShape>> shapeInstances(boardHeight, vector<sf::RectangleShape>(boardWidth, square)); //Vector that stores all generated shapes 
    vector <vector <sf::RectangleShape>> vectorStat(2, vector<sf::RectangleShape>(4,square));
    

    for (int i = 0; i < boardHeight; i++)
    {
        for (int j = 0; j < boardWidth; j++)
            shapeInstances[i][j].setPosition(sf::Vector2f(j * 30 + 15, i * 30 + 23));
    }

    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 4; j++)
            vectorStat[i][j].setPosition(sf::Vector2f((SCR_WIDTH - 150) + j * 30, 140 + i * 30));
    }

    while (window.isOpen()) //Main loop
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        checkIfKeyIsPressed();

        switch (gameState)
        {
            case MAIN_MENU:
            {
                window.draw(mainMenuBackgroundSprite);
                window.draw(newGameButtonSprite);
                window.draw(highScoresButtonSprite);
                window.draw(quitGameButtonSprite);  

                switch (choice)
                {
                    case 0:
                    {
                        newGameButtonSprite.setTexture(newGameButtonSelectedTexture);
                        highScoresButtonSprite.setTexture(highScoresButtonTexture);
                        quitGameButtonSprite.setTexture(quitGameButtonTexture);

                        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter))
                        {
                            scoreText.setString(to_string(score));
                            scoreText.setPosition(SCR_WIDTH - 130, SCR_HEIGHT / 2.3);
                            gameState = GAME_PLAYING;
                            memset(board, 0, sizeof(board));
                            
                            for (int i = 0; i < 2; i++)
                                for (int j = 0; j < 4; j++)
                                    vectorStat[i][j].setFillColor(sf::Color::Black);

                        }
                        break;
                    }

                    case 1:
                    {
                        newGameButtonSprite.setTexture(newGameButtonTexture);
                        highScoresButtonSprite.setTexture(highScoresButtonSelectedTexture);
                        quitGameButtonSprite.setTexture(quitGameButtonTexture);

                        break;
                    }

                    case 2:
                    {
                        newGameButtonSprite.setTexture(newGameButtonTexture);
                        highScoresButtonSprite.setTexture(highScoresButtonTexture);
                        quitGameButtonSprite.setTexture(quitGameButtonSelectedTexture);

                        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter))
                        {
                            window.close();
                            exit(1);
                        }
                        
                        break;
                    }

                    default:
                        break;
                }

                    break;
            }
            case GAME_PLAYING:
            {
                window.draw(gameBackgroundSprite);
                window.draw(scoreText);

                movePieceDown();

                for (int i = 0; i < boardHeight; i++)
                {
                    for (int j = 0; j < boardWidth; j++)
                    {
                        if (board[i][j] != 0)
                            shapeInstances[i][j].setFillColor(shapeInstances[i][j].getFillColor());

                        else if (currentPiece.y <= i && i < currentPiece.y + 4 && currentPiece.x <= j && j < currentPiece.x + 4 &&
                            currentPiece.shape[i - currentPiece.y][j - currentPiece.x] != 0)
                            shapeInstances [i][j].setFillColor(currentPiece.shapeColor);

                        else
                            shapeInstances[i][j].setFillColor(sf::Color::Black);
                    }
                }

                for (int i = 0; i < 2; i++)
                {
                    for (int j = 0; j < 4; j++)
                        if (nextPiece.shape[i][j] != 0)
                            vectorStat[i][j].setFillColor(nextPiece.shapeColor);
                        else
                            vectorStat[i][j].setFillColor(sf::Color::Black);

                }

                for (auto i : shapeInstances) //Draw all shape instances on the screen
                    for(auto j : i)
                        window.draw(j);

                for (int i = 0; i < 2; i++) //Draw the next shape in the stats menu
                    for (int j = 0; j < 4; j++)
                        window.draw(vectorStat[i][j]);

                break;
            }
            case GAME_OVER:
            {
                scoreText.setPosition(SCR_WIDTH / 2.4, SCR_HEIGHT / 2.9);
                scoreText.setString("Score : " + to_string(score));

                window.draw(gameBackgroundSprite);

                for (auto i : shapeInstances) //Draw all shape instances on the screen
                    for(auto j : i)
                        window.draw(j);

                for (int i = 0; i < 2; i++) //Draw the next shape in the stats menu
                    for (int j = 0; j < 4; j++)
                        window.draw(vectorStat[i][j]);


                window.draw(gameOverBackground);
                window.draw(gameOverText);
                window.draw(returnToMenuSprite);
                window.draw(scoreText);

                break;
            }
            default:
                break;
        }        
          
        window.display();
    }


    return 0;

}