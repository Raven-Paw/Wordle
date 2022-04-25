#include <SFML/Graphics.hpp>
#include <iostream>
#include <map>
#include <vector>
#include <map>
#include <random>
#include <string>

#include "header/WordDataSet.hpp"

using std::cout;

const int WIDHT = 500, HEIGHT = 600;

struct keyVector{char letter; int state;};
std::vector<keyVector> keyboard = {{'Q',0},{'W',0},{'E',0},{'R',0},{'T',0},{'Y',0},{'U',0},{'I',0},{'O',0},{'P',0},
                                    {'A',0},{'S',0},{'D',0},{'F',0},{'G',0},{'H',0},{'J',0},{'K',0},{'L',0},
                                    {'Z',0},{'X',0},{'C',0},{'V',0},{'B',0},{'N',0},{'M',0},};
int cell[6][5];
std::map<char, int> secretWordMap;  // store how many character occur in the secret word
std::map<char, int> guessWordMap;   // store how many character occur in the input word
std::string secretWord;     // store the secret word that need to be guess
bool game = true;       // Trigger the game

int guessCount = 0;   // How many guesses left   
std::vector<char> word; // Store all the input character
std::vector<std::vector<char>> guessWord; // Store all the input word

// Store all the color and can be access using word
// this is Map, using key to access values
std::map<std::string, sf::Color> colorscheme = {
    {"background", sf::Color(18,18,19,255)},
    {"overlay", sf::Color(18,18,19,150)},
    {"border", sf::Color(86,87,88,255)},
    {"activeBorder", sf::Color(150,150,150,255)},
    {"correctSpot", sf::Color(83,141,78,255)},
    {"wrongSpot", sf::Color(181,159,59,255)},
    {"donotExist", sf::Color(58,58,60,255)},
    {"text", sf::Color(255,255,255,255)},
    {"keys", sf::Color(129,131,132,255)}
    };

void showDisplay(sf::RenderWindow& window){
    const int size = 50;    // size of the Square
    
    // Title WORDLE
    sf::Font font;
    font.loadFromFile("font/KarnakPro-CondensedBlack.ttf");
    sf::Font font2;
    font2.loadFromFile("font/KarnakPro-Light.ttf");

    sf::Text wordle;
    wordle.setFont(font);
    wordle.setCharacterSize(50);
    wordle.setString("WORDLE");
    wordle.setFillColor(colorscheme["text"]);
    wordle.setPosition(sf::Vector2f((WIDHT/2)-(wordle.getLocalBounds().width/2), 25));
    window.draw(wordle);

    // Separation Line
    sf::Vertex line[] =
    {
        sf::Vertex(sf::Vector2f(0, 85)),
        sf::Vertex(sf::Vector2f(500, 85))
    };
    window.draw(line, 2, sf::Lines);

    // show the all the Word input
    sf::RectangleShape rect;
    int pady = 95;
    for (int y = 0; y < 6; y++) // There are 6 Row
    {   
        int padx = 120;
        for (int x = 0; x < 5; x++) // There are 5 character each Row
        {   
            if(cell[y][x] == 1){rect.setFillColor(colorscheme["correctSpot"]);}
            else if(cell[y][x] == 2){rect.setFillColor(colorscheme["wrongSpot"]);}
            else if(cell[y][x] == 3){rect.setFillColor(colorscheme["donotExist"]);}
            else {
                rect.setFillColor(colorscheme["background"]);
                rect.setOutlineColor(colorscheme["border"]);
                rect.setOutlineThickness(-2);
            }
            
            rect.setSize(sf::Vector2f(size,size));
            rect.setPosition(sf::Vector2f((x*size)+padx, (y*size)+pady));   // Postion is J * size_of_square + the pad... to make the square appear slightly center
            padx += 5;

            // Show All the guess word attempt
            sf::Text guesses;
            guesses.setFont(font2);
            guesses.setString(guessWord[y][x]);
            guesses.setStyle(sf::Text::Bold);
            guesses.setCharacterSize(20);
            guesses.setFillColor(sf::Color::White);
            int x2 = (size/2) + rect.getPosition().x - (guesses.getLocalBounds().width/2);
            int y2 = (size/2) + rect.getPosition().y - guesses.getLocalBounds().height;
            guesses.setPosition(sf::Vector2f(x2, y2));

            
            // Show the input word
            sf::Text input;
            input.setFont(font2);
            if(x < word.size()) // the word is only 5 character
            {
                if(y == guessCount) // only show the current row
                {
                input.setString(word[x]);
                input.setStyle(sf::Text::Bold);
                input.setCharacterSize(20);
                input.setFillColor(sf::Color::White);
                int x3 = ((size/2) + rect.getPosition().x) - (input.getLocalBounds().width/2);
                int y3;
                y3 = (size/2)+rect.getPosition().y - input.getLocalBounds().height;
                input.setPosition(sf::Vector2f(x3, y3));
                rect.setOutlineColor(colorscheme["activeBorder"]);
                }
            }
            window.draw(rect);     // draw all the square
            window.draw(guesses);
            window.draw(input);
        }
        pady += 5;
    }

    // Show the keyboard
    int key_w = 30, key_h = 50;
    rect.setOutlineColor(colorscheme["background"]);
    rect.setOutlineThickness(-2);
    rect.setSize(sf::Vector2f(key_w,key_h));

    sf::Text keys;
    keys.setFont(font2);
    keys.setCharacterSize(15);
    keys.setFillColor(colorscheme["text"]);
    keys.setStyle(sf::Text::Bold);

    int y = 0;
    for (int i = 0; i < keyboard.size(); i++)
    {   
        int j = i;
        if(j>18)
        {
            j-=19;
            y = 2;
            // Change color
            if(keyboard[i].state == 1){rect.setFillColor(colorscheme["correctSpot"]);}  // Correct Spot
            else if(keyboard[i].state == 2){rect.setFillColor(colorscheme["wrongSpot"]);}   // Wrongs Spot
            else if(keyboard[i].state == 3){rect.setFillColor(colorscheme["donotExist"]);}  // Doesn't Exist
            else{rect.setFillColor(colorscheme["keys"]);}   // Default

            rect.setPosition(sf::Vector2f((j*key_w)+145, y*key_h+436));
            window.draw(rect);

            keys.setString(keyboard[i].letter);
            keys.setPosition(sf::Vector2f((((key_w/2)+rect.getPosition().x)-keys.getLocalBounds().width/2), (((key_h/2)+rect.getPosition().y)-keys.getLocalBounds().height) ));
            window.draw(keys); 
        }
        else if(j>9)
        {   
            j-=10;
            y = 1;
            // Change color
            if(keyboard[i].state == 1){rect.setFillColor(colorscheme["correctSpot"]);}  // Correct Spot
            else if(keyboard[i].state == 2){rect.setFillColor(colorscheme["wrongSpot"]);}   // Wrongs Spot
            else if(keyboard[i].state == 3){rect.setFillColor(colorscheme["donotExist"]);}  // Doesn't Exist
            else{rect.setFillColor(colorscheme["keys"]);}   // Default

            rect.setPosition(sf::Vector2f((j*key_w)+115, y*key_h+433));
            window.draw(rect);

            keys.setString(keyboard[i].letter);
            keys.setPosition(sf::Vector2f((((key_w/2)+rect.getPosition().x)-keys.getLocalBounds().width/2), (((key_h/2)+rect.getPosition().y)-keys.getLocalBounds().height)));
            window.draw(keys); 
        }
        else if(j>-1)
        {
            // Change color
            if(keyboard[i].state == 1){rect.setFillColor(colorscheme["correctSpot"]);}  // Correct Spot
            else if(keyboard[i].state == 2){rect.setFillColor(colorscheme["wrongSpot"]);}   // Wrongs Spot
            else if(keyboard[i].state == 3){rect.setFillColor(colorscheme["donotExist"]);}  // Doesn't Exist
            else{rect.setFillColor(colorscheme["keys"]);}   // Default

            rect.setPosition(sf::Vector2f((j*key_w)+105, y*key_h+430));
            window.draw(rect);

            keys.setString(keyboard[i].letter);
            keys.setPosition(sf::Vector2f((((key_w/2)+rect.getPosition().x)-(keys.getLocalBounds().width/2)), (((key_h/2)+rect.getPosition().y)-(keys.getLocalBounds().height))));
            window.draw(keys); 
        }
    }
}

void map(std::string str, std::map<char, int>& mp){
    for (int i = 0; i < 5; i++)
    {
        mp[str[i]]++;
    }
}

void checkWord(){
    std::string guessword="";

    map(secretWord, secretWordMap); // Count the character occurences of secret Word

    for (int i = 0; i < 5; i++){ guessword = guessword + guessWord[guessCount][i]; }
    
    map(guessword, guessWordMap);

    int correctSpot[5];

    // Correct Spot
    for (int x = 0; x < 5; x++)
    {
        if(guessWord[guessCount][x] == secretWord[x])
        {
            cell[guessCount][x] = 1;
            for (int i = 0; i < keyboard.size(); i++)
            {
                if(keyboard[i].letter == toupper(guessWord[guessCount][x])){
                    keyboard[i].state = 1;
                }
            }
            
        }
        else
        {      
            if(secretWordMap[guessWord[guessCount][x]] != 0)    // Check if the character exist in the secretWord
            {   
                if(guessWordMap[guessWord[guessCount][x]] <= secretWordMap[guessWord[guessCount][x]])    
                {
                    cell[guessCount][x] = 2;
                    for (int i = 0; i < keyboard.size(); i++)
                    {
                        if(keyboard[i].letter == toupper(guessWord[guessCount][x])){
                            if(keyboard[i].state != 1){keyboard[i].state = 2;}
                        }
                    }
                }
            }
            else
            {
                cell[guessCount][x] = 3;
                for (int i = 0; i < keyboard.size(); i++)
                {
                    if(keyboard[i].letter == toupper(guessWord[guessCount][x])){
                        if(keyboard[i].state != 1){keyboard[i].state = 3;}
                    }
                }
            }
        }

    }
    
    secretWordMap.clear();
    guessWordMap.clear();
}

bool correctGuess(){
    int correctplacement=0;
    for (int x = 0; x < 5; x++)
    {
        if(guessWord[guessCount][x] == secretWord[x]){correctplacement++;}
    }
    if(correctplacement == 5){
        return true;
    }
    
    return false;
}

int main()
{
    std::srand(std::time(0));

    std::string gueassWordArray[6];
    sf::RenderWindow window(sf::VideoMode(WIDHT, HEIGHT), "Wordle");

    sf::Image icon;
    icon.loadFromFile("Wordle.png"); // File/Image/Pixel
    window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

    guessWord.push_back({' ',' ',' ',' ',' '});
    guessWord.push_back({' ',' ',' ',' ',' '});
    guessWord.push_back({' ',' ',' ',' ',' '});
    guessWord.push_back({' ',' ',' ',' ',' '});
    guessWord.push_back({' ',' ',' ',' ',' '});
    guessWord.push_back({' ',' ',' ',' ',' '});

    secretWord = wordDataSet[rand() % 12972];
    transform(secretWord.begin(), secretWord.end(), secretWord.begin(), ::toupper);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            if(guessCount < 6 && game)
            {
                switch (event.type)
                {
                    case sf::Event::KeyReleased:

                        // Key Pressed
                        switch (event.key.code)
                        {
                            case sf::Keyboard::A :
                                if(word.size()<5){word.push_back('A');}
                                break;

                            case sf::Keyboard::B :
                                if(word.size()<5){word.push_back('B');}
                                break;
                            case sf::Keyboard::C :
                                if(word.size()<5){word.push_back('C');}
                                break;
                            case sf::Keyboard::D :
                                if(word.size()<5){word.push_back('D');}
                                break;
                            case sf::Keyboard::E :
                                if(word.size()<5){word.push_back('E');}
                                break;
                            case sf::Keyboard::F :
                                if(word.size()<5){word.push_back('F');}
                                break;
                            case sf::Keyboard::G :
                                if(word.size()<5){word.push_back('G');}
                                break;
                            case sf::Keyboard::H :
                                if(word.size()<5){word.push_back('H');}
                                break;
                            case sf::Keyboard::I :
                                if(word.size()<5){word.push_back('I');}
                                break;
                            case sf::Keyboard::J :
                                if(word.size()<5){word.push_back('J');}
                                break;
                            case sf::Keyboard::K :
                                if(word.size()<5){word.push_back('K');}
                                break;
                            case sf::Keyboard::L :
                                if(word.size()<5){word.push_back('L');}
                                break;
                            case sf::Keyboard::M :
                                if(word.size()<5){word.push_back('M');}
                                break;
                            case sf::Keyboard::N :
                                if(word.size()<5){word.push_back('N');}
                                break;
                            case sf::Keyboard::O :
                                if(word.size()<5){word.push_back('O');}
                                break;
                            case sf::Keyboard::P :
                                if(word.size()<5){word.push_back('P');}
                                break;
                            case sf::Keyboard::Q :
                                if(word.size()<5){word.push_back('Q');}
                                break;
                            case sf::Keyboard::R :
                                if(word.size()<5){word.push_back('R');}
                                break;
                            case sf::Keyboard::S :
                                if(word.size()<5){word.push_back('S');}
                                break;
                            case sf::Keyboard::T :
                                if(word.size()<5){word.push_back('T');}
                                break;
                            case sf::Keyboard::U :
                                if(word.size()<5){word.push_back('U');}
                                break;
                            case sf::Keyboard::V :
                                if(word.size()<5){word.push_back('V');}
                                break;
                            case sf::Keyboard::W :
                                if(word.size()<5){word.push_back('W');}
                                break;
                            case sf::Keyboard::X :
                                if(word.size()<5){word.push_back('X');}
                                break;
                            case sf::Keyboard::Y :
                                if(word.size()<5){word.push_back('Y');}
                                break;
                            case sf::Keyboard::Z :
                                if(word.size()<5){word.push_back('Z');}
                                break;
                            
                            case sf::Keyboard::BackSpace:
                                if(word.size() != 0){word.pop_back();}
                                break;

                            case sf::Keyboard::Enter:
                                if(word.size()==5)
                                {
                                    guessWord[guessCount] = word;
                                    checkWord();
                                    if(correctGuess()){game=false;}
                                    
                                    guessCount++;
                                    word.clear();
                                }
                                break;
                        }
                    break;
                }
            }
        }


        window.clear(colorscheme["background"]);    // Clear the windows then paint the bg black
        showDisplay(window);                        // Show the Display
        if(guessCount > 5){
                sf::RectangleShape rect;
                rect.setPosition(0,0);
                rect.setFillColor(colorscheme["overlay"]);
                rect.setSize(sf::Vector2f(WIDHT, HEIGHT));
                window.draw(rect);

                sf::Font font;
                font.loadFromFile("font/KarnakPro-CondensedBlack.ttf");

                sf::Text text;
                text.setFont(font);
                text.setString("You ran out of Guesses");
                text.setCharacterSize(50);
                text.setFillColor(colorscheme["wrongSpot"]);
                text.setPosition(WIDHT/2-text.getGlobalBounds().width/2, HEIGHT/2-text.getGlobalBounds().height);
                window.draw(text);
            }
        else if(!game){
                sf::RectangleShape rect;
                rect.setPosition(0,0);
                rect.setFillColor(colorscheme["overlay"]);
                rect.setSize(sf::Vector2f(WIDHT, HEIGHT));
                window.draw(rect);

                sf::Font font;
                font.loadFromFile("font/KarnakPro-CondensedBlack.ttf");

                sf::Text text;
                text.setFont(font);
                text.setString("Congratulations");
                text.setCharacterSize(50);
                text.setFillColor(colorscheme["wrongSpot"]);
                text.setPosition(WIDHT/2-text.getGlobalBounds().width/2, HEIGHT/2-text.getGlobalBounds().height);
                window.draw(text);
            }
        window.display();                           // Make the window display everything
    }
    return 0;
}
