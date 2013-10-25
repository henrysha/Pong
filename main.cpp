#include <SFML\Graphics.hpp>
#include <SFML\Audio.hpp>
#include <ctime>
#include <cmath>
#include <cstdlib>
#include <SFML\Window.hpp>

//define some constants
const float pi = 3.14159f;
int gameWidth = sf::VideoMode::getDesktopMode().width;
int gameHeight = sf::VideoMode::getDesktopMode().height;
sf::Vector2f paddleSize(gameWidth/50, gameHeight/5);
const float ballRadius = gameHeight/60.f;
const int maxScore = 7;

//define basic values
float difficulty = 2.f;
float ballSpeed = gameWidth/difficulty;
float ballAngle = 0.f;// will be changed
sf::Color bgColor = sf::Color::Black;
sf::Color textColor = sf::Color::White;
sf::Color scoreColor = sf::Color::Magenta;
int leftScoreValue;
int rightScoreValue;
bool ballSpeedDisplay = false;
bool playing = false;

//paddles and ball
sf::RectangleShape leftPaddle;
sf::RectangleShape rightPaddle;
sf::CircleShape ball;

void init(sf::RectangleShape &leftPaddle, sf::RectangleShape &rightPaddle, sf::CircleShape &ball, float &ballAngle){
	leftPaddle.setPosition(paddleSize.x, gameHeight/2);
	rightPaddle.setPosition(gameWidth - paddleSize.x, gameHeight/2);
	ball.setPosition (gameWidth/2, gameHeight/2);
	//make sure ballAngle is not too vertical
	do
	{
		ballAngle = (std::rand() % 360) * 2 * pi / 360; //changes later;
	}
	while (std::abs(std::cos(ballAngle)) < 0.7f);
}

void init(sf::CircleShape &ball, float &ballAngle)
{
	ball.setPosition (gameWidth/2, gameHeight/2);
	//make sure ballAngle is not too vertical
	do
	{
		ballAngle = (std::rand() % 360) * 2 * pi / 360; //changes later;
	}
	while (std::abs(std::cos(ballAngle)) < 0.7f);
}

void init(int &leftScoreValue, int &rightScoreValue, sf::Text &leftScore, sf::Text &rightScore)
{
	leftScoreValue = 0;
	rightScoreValue = 0;
	leftScore.setString(std::to_string(leftScoreValue));
	rightScore.setString(std::to_string(rightScoreValue));
}

//create Paddle
sf::RectangleShape createPaddle(sf::RectangleShape &paddle, float outlineThickness, sf::Color outlineColor, sf::Color fillColor){
	paddle.setSize (paddleSize - sf::Vector2f(outlineThickness, outlineThickness));
	paddle.setOutlineThickness (outlineThickness);
	paddle.setOutlineColor (outlineColor);
	paddle.setFillColor (fillColor);
	paddle.setOrigin (paddleSize / 2.f);
	return paddle;
}

//create Ball
sf::CircleShape createBall (sf::CircleShape &ball, float outlineThickness, sf::Color outlineColor, sf::Color fillColor){
	ball.setRadius (ballRadius - outlineThickness);
	ball.setOutlineThickness (outlineThickness);
	ball.setOutlineColor (outlineColor);
	ball.setFillColor (fillColor);
	ball.setOrigin (ballRadius / 2, ballRadius / 2);
	return ball;
}

int main()
{
	std::srand(static_cast<unsigned int> (std::time(NULL)));

	//create the window
	sf::RenderWindow window(sf::VideoMode(gameWidth, gameHeight),"Pong", sf::Style::Fullscreen);
	window.setVerticalSyncEnabled(true);
	window.setMouseCursorVisible(false);

	//create the left paddle
	createPaddle (leftPaddle, 2.f, sf::Color::Cyan, sf::Color::Blue);

	//create the right paddle
	createPaddle (rightPaddle, 2.f, sf::Color::Cyan, sf::Color::Blue);

	//create the ball
	createBall (ball, 1.5f, sf::Color::Magenta, sf::Color::Yellow);	

	//load the ball_bounce soundeffect
	sf::SoundBuffer ballSoundBuffer;
	if (!ballSoundBuffer.loadFromFile("resources/ball.wav"))
		EXIT_FAILURE;	
	sf::Sound soundEffect(ballSoundBuffer);
	soundEffect.setVolume(20);

	//load the font
	sf::Font textFont;
	if (!textFont.loadFromFile("resources/BAUHS93.TTF"))
		EXIT_FAILURE;
	
	//create pauseMessage
	sf::Text pauseMessage;
	pauseMessage.setString("Press Enter to begin\nPress ESC to Quit");
	pauseMessage.setColor(textColor);
	pauseMessage.setFont(textFont);
	pauseMessage.setCharacterSize(gameHeight/10);
	pauseMessage.setPosition (gameWidth/2 - pauseMessage.getLocalBounds().width, gameHeight/2 - pauseMessage.getLocalBounds().height);

	//create backgroundText
	sf::Text backgroundText;
	backgroundText.setString("Press ESC to end the Game\nPress + to Increase Ball Speed\nPress - to Decrease Ball Speed\nPress F3 to Display Ball Speed");
	backgroundText.setColor(textColor);
	backgroundText.setFont(textFont);
	backgroundText.setPosition(gameWidth/4, gameHeight - backgroundText.getLocalBounds().height * 2);
	backgroundText.setCharacterSize(gameHeight/21);

	sf::Text ballSpeedText;
	ballSpeedText.setString("Current Ball Speed : " + std::to_string(ballSpeed));
	ballSpeedText.setFont(textFont);
	ballSpeedText.setColor(textColor);
	ballSpeedText.setCharacterSize(gameHeight/20);
	ballSpeedText.setPosition(gameWidth/2 - ballSpeedText.getLocalBounds().width/2, gameHeight/10);

	sf::Text leftScore;
	leftScore.setCharacterSize(gameHeight/5);
	leftScore.setFont(textFont);
	leftScore.setColor(scoreColor);
	leftScore.setString(std::to_string(leftScoreValue));
	leftScore.setStyle(sf::Text::Bold);
	leftScore.setPosition(gameWidth/2 - gameWidth/5 - leftScore.getLocalBounds().width, gameHeight/2 - leftScore.getCharacterSize()/2);


	sf::Text rightScore;
	rightScore.setCharacterSize(gameHeight/5);
	rightScore.setFont(textFont);
	rightScore.setColor(scoreColor);
	rightScore.setString(std::to_string(rightScoreValue));
	rightScore.setStyle(sf::Text::Bold);
	rightScore.setPosition(gameWidth/2 + gameWidth/5, gameHeight/2 - rightScore.getCharacterSize()/2);


	sf::Clock clock;
	
	//initialize paddles and ball
	init(leftPaddle, rightPaddle, ball, ballAngle);

	//initialize score
	init(leftScoreValue, rightScoreValue, leftScore, rightScore);

	//run the program as long as the window is open
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			//"close requested" event || esc pressed: close the window
			if((event.type == sf::Event::Closed)
				|| ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Escape)))
			{
				window.close();
				break;
			}

			if (!playing)
				if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Return)
				{
					playing = true;
					clock.restart();
					init(leftPaddle, rightPaddle, ball, ballAngle);
					init(leftScoreValue, rightScoreValue, leftScore, rightScore);
					difficulty = 2.f;
					ballSpeed = gameWidth/difficulty;
					ballSpeedText.setString("Current Ball Speed : " + std::to_string(ballSpeed));
				}
			
			//increase ball speed
			if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Add)
				if (difficulty > .1f)
				{
					difficulty = difficulty - .1f;
					ballSpeed = gameWidth/difficulty;
					ballSpeedText.setString("Current Ball Speed : " + std::to_string(ballSpeed));
				}

			//decrease ball speed
			if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Subtract)
			{
				difficulty = difficulty + .1f;
				ballSpeed = gameWidth/difficulty;
				ballSpeedText.setString("Current Ball Speed : " + std::to_string(ballSpeed));
			}

			//toggle ball speed text display
			if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::F3)
			{
				if (!ballSpeedDisplay)
					ballSpeedDisplay = true;
				else
					ballSpeedDisplay = false;
			}
		}

		//clear the window to Black bgcolor
		window.clear(bgColor);

		if (playing)
		{
			float deltaTime = clock.restart().asSeconds();

			//get the player's mouse location
			float mouseLocation = sf::Mouse::getPosition().y;
			float windowLocation = window.getPosition().y;

			//move the paddle to the mouseLocation's height
			if (mouseLocation > windowLocation + paddleSize.y/2 && mouseLocation < windowLocation + gameHeight - paddleSize.y/2)
			{
				leftPaddle.setPosition (leftPaddle.getPosition().x, mouseLocation - windowLocation);
				rightPaddle.setPosition (rightPaddle.getPosition().x, mouseLocation - windowLocation);
			}
			else if (mouseLocation < windowLocation + paddleSize.y/2)
			{
				leftPaddle.setPosition (leftPaddle.getPosition().x, paddleSize.y/2);
				rightPaddle.setPosition (rightPaddle.getPosition().x , paddleSize.y/2);
			}
			else if (mouseLocation > windowLocation + gameHeight - paddleSize.y/2)
			{
				leftPaddle.setPosition (leftPaddle.getPosition().x, gameHeight - paddleSize.y/2);
				rightPaddle.setPosition (rightPaddle.getPosition().x, gameHeight - paddleSize.y/2);
			}

			//move the ball
				ball.move( std::cos(ballAngle) * ballSpeed * deltaTime, std::sin(ballAngle) * ballSpeed * deltaTime);

			//check for the collision of ball and paddle
				//left paddle
			if (ball.getPosition().x - ballRadius < leftPaddle.getPosition().x + paddleSize.x/2 &&
				ball.getPosition().x > leftPaddle.getPosition().x &&
				ball.getPosition().y >= leftPaddle.getPosition().y - paddleSize.y/2 &&
				ball.getPosition().y <= leftPaddle.getPosition().y + paddleSize.y/2 )
			{
				if(ball.getPosition().y > leftPaddle.getPosition().y)
					ballAngle = pi - ballAngle + (std::rand() % 20) * pi / 180;
				else
					ballAngle = pi - ballAngle - (std::rand() % 20) * pi / 180;
				soundEffect.play();
				ball.setPosition(ball.getPosition().x + std::cos(ballAngle) * ballSpeed * deltaTime / 2, ball.getPosition().y);
			}
			
				//right paddle
			if (ball.getPosition().x + ballRadius > rightPaddle.getPosition().x - paddleSize.x/2 &&
				ball.getPosition().x < rightPaddle.getPosition().x &&
				ball.getPosition().y >= rightPaddle.getPosition().y - paddleSize.y/2 &&
				ball.getPosition().y <= rightPaddle.getPosition().y + paddleSize.y/2 )
				{
				if(ball.getPosition().y > rightPaddle.getPosition().y)
					ballAngle = pi - ballAngle + (std::rand() % 20) * pi / 180;
				else
					ballAngle = pi - ballAngle - (std::rand() % 20) * pi / 180;
				soundEffect.play();
				ball.setPosition(ball.getPosition().x + std::cos(ballAngle) * ballSpeed * deltaTime / 2, ball.getPosition().y);
			}

			//check for the collision of ball and window
			if (ball.getPosition().y - ballRadius < 0.f)
			{
				ballAngle = -ballAngle;
				ball.setPosition (ball.getPosition().x , ball.getPosition().y + std::sin(ballAngle) * ballSpeed * deltaTime / 2);
				soundEffect.play();
			}
		
			if (ball.getPosition().y + ballRadius > gameHeight)
			{
				ballAngle = -ballAngle;
				ball.setPosition (ball.getPosition().x, ball.getPosition().y + std::sin(ballAngle) * ballSpeed * deltaTime / 2);
				soundEffect.play();
			}

			if (ball.getPosition().x - ballRadius < 0.f)
			{
				init (ball, ballAngle);
				rightScoreValue++;
				rightScore.setString(std::to_string(rightScoreValue));
			}

			if (ball.getPosition().x + ballRadius > gameWidth)
			{
				init (ball, ballAngle);
				leftScoreValue++;
				leftScore.setString(std::to_string(leftScoreValue));
			}
			if (leftScoreValue == maxScore)
			{
				playing = false;
				pauseMessage.setString("Left Side Won!!\nIf you want to play again press Enter\nIf you want to Quit press ESC");
			}
			if (rightScoreValue == maxScore)
			{
				playing = false;
				pauseMessage.setString("Right Side Won!!\nIf you want to play again press Enter\nIf you want to Quit press ESC");
			}
		
			//draw backgroundText instructing the way to end the game
			window.draw(backgroundText);
		
			//draw ball speed
			if (ballSpeedDisplay)
				window.draw(ballSpeedText);

			//draw scores
			window.draw(leftScore);
			window.draw(rightScore);

			//draw the paddles and ball
			window.draw(leftPaddle);
			window.draw(rightPaddle);
			window.draw(ball);
		}
		else
		{
			window.draw(pauseMessage);
		}
		//display on screen
		window.display();
	}
	return 0;
}