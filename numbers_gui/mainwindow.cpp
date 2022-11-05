#include "mainwindow.hh"
#include "ui_mainwindow.h"

const Coords LEFT = {0, -1};
const Coords UP = {-1, 0};
const Coords RIGHT = {0, 1};
const Coords DOWN = {1, 0};


// Const value to better display the number tiles to the board
const int X_OFFSET = 18;
const int Y_OFFSET = 40;
const int MAX_SIZE = 6;
const int GAP = 3;
const int PIXEL_SIZE = 540;


// Path to the resources number tile icons
vector<QString> ICON_PATHS = {
    "../resources/0.png",
    "../resources/2.png",
    "../resources/4.png",
    "../resources/8.png",
    "../resources/16.png",
    "../resources/32.png",
    "../resources/64.png",
    "../resources/128.png",
    "../resources/256.png",
    "../resources/512.png",
    "../resources/1024.png",
    "../resources/2048.png"
};


// Style for QT element in different game senerio
QString WHITE_BACKGROUND = "background-color: rgb(255, 255, 255)";
QString WIN_BACKGROUND = "background-color:rgb(91,167,100)";
QString LOSE_BACKGROUND = "background-color:rgb(67,63,64)";


using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Initialize all the QLabel that act like number tile for the game
    for (int i = 0; i < MAX_SIZE; i++)
    {
        vector<QLabel*> rowQlabel;
        for (int j = 0; j < MAX_SIZE; j++) {
            QLabel *label = new QLabel(this);
            rowQlabel.push_back(label);
            label->hide();
        }
        GUIboard.push_back(rowQlabel);
    }

    // Read all icon to QImage
    int number = 0;
    for (const auto & path : ICON_PATHS) {
        imagesMap[number] = new QImage(path);
        number = number == 0 ? 2 : number * 2;
    }

    connect(timer,SIGNAL(timeout()), this, SLOT(updateWatch()));

    init_GUI();
}

// Free all memory that have been allocated
MainWindow::~MainWindow()
{
    for (auto & row : GUIboard) {
        for (auto & tile: row) {
            delete tile;
        }
    }

    for (auto & pair : imagesMap) {
        delete pair.second;
    }

    if (board != NULL) {
        delete board;
    }

    delete ui;
}

// Start the stop watch
void MainWindow::startStopWatch()
{
    minNum = 0;
    secNum = 0;
    ui->MinuteLcdNumber->display(0);
    ui->SecondLcdNumber->display(0);
    timer->start(1000);
}



void MainWindow::init_GUI()
{
    gameOver_ = false;

    // delete game if there is one still existing
    if (board != NULL)
        delete board;

    // clear the game message
    gameMessage("", "reset");

    // update goal and seed text gui
    update_goal_ui();
    update_seed_ui();

    startStopWatch();

    // init new GameBoard instance
    board = new GameBoard();
    board->init_empty(board_size_);

    // set seed_ to a random int if the game mode useRandomSeed == true
    if (useRandomSeed) {
        setRandomSeed();
    }
    board->fill(seed_);

    // Calculate the tile size based on the desired board size
    tile_size_ = (PIXEL_SIZE - (board_size_-1) * GAP) / board_size_;

    // Hide all QLabel
    for (int i = 0; i < MAX_SIZE; i++) {
        for (int j = 0; j < MAX_SIZE; j++) {
            GUIboard[i][j]->hide();
        }
    }

    // Show all the QLabel that in range board_size_ and set appropriate size for them
    for (int i = 0; i < board_size_; i++) {
        for (int j = 0; j < board_size_; j++) {
            GUIboard[i][j]->show();
            GUIboard[i][j]->setGeometry(X_OFFSET + j * (tile_size_ + GAP),Y_OFFSET + i*(tile_size_ + GAP),tile_size_, tile_size_);
        }
    }

    updateGUI();
}

void MainWindow::updateGUI()
{
    vectorBoard = board->get_board();


    // calculating and set the score for the gui
    // the score are calculated by sum all the numberTile on the game board
    int score = board->score();
    ui->ScoreText->setText(QString::number(score));
    ui->ScoreText->setAlignment(Qt::AlignCenter);
    highScore_ = max(highScore_, score);
    ui->HighScoreText->setText(QString::number(highScore_));
    ui->HighScoreText->setAlignment(Qt::AlignCenter);

    for (int i = 0; i < board_size_; i++) {
        for (int j = 0; j < board_size_; j++)
        {
            // if the game is paused then show all tile as empty
            if (pause_) {
                GUIboard[i][j]->setPixmap(QPixmap::fromImage(*imagesMap[0]).scaled(tile_size_, tile_size_));
                continue;
            }
            int value = vectorBoard->at(i).at(j)->get_value();
            GUIboard[i][j]->setPixmap(QPixmap::fromImage(*imagesMap[value]).scaled(tile_size_, tile_size_));
        }
    }
}


// method change background and message
// if the param color == "green" => change background to green
// if the param color == "reset" => change background to default
// else => change bachground to grey (losing)
void MainWindow::gameMessage(QString message, string color) {
    if (color == "reset") {
        ui->centralwidget->setStyleSheet(WHITE_BACKGROUND);
        ui->GameStatus->setText("");
        return;
    } else if (color == "green") {
        ui->centralwidget->setStyleSheet(WIN_BACKGROUND);
    } else {
        ui->centralwidget->setStyleSheet(LOSE_BACKGROUND);
    }

    ui->GameStatus->setFont(QFont("Cantarell", 40, 40));
    ui->GameStatus->setText(message);
    ui->GameStatus->setAlignment(Qt::AlignCenter);
}

void MainWindow::gameOver_GUI(bool won)
{
    gameOver_ = true;
    if (won) {
        gameMessage("You Won!", "green");
    }
    else {
        gameMessage("You Lose!", "black");
    }
}

void MainWindow::GUI_move(Coords coord)
{
    if ( board == NULL ) {
        return;
    }

    if ( gameOver_ || pause_ ) {
        return;
    }

    if( board->move(coord, goal_) && !unlimitedMode )
    {
        timer->stop();
        gameOver_GUI(true);
    }
    else if( board->is_full() )
    {
        timer->stop();
        gameOver_GUI(false);
        return;
    }
    board->new_value(false);
    updateGUI();
}


// actions call GUI_move() with appropriate Coords
void MainWindow::on_upButton_released()
{
    GUI_move(UP);
}

void MainWindow::on_leftButton_released()
{
    GUI_move(LEFT);
}

void MainWindow::on_downButton_released()
{
    GUI_move(DOWN);
}

void MainWindow::on_rightButton_released()
{
    GUI_move(RIGHT);
}


// update the board_size_ based on which button get clicked
void MainWindow::on_action3x3_triggered()
{
    board_size_ = 3;
    init_GUI();
}

void MainWindow::on_action4x4_triggered()
{
    board_size_ = 4;
    init_GUI();
}

void MainWindow::on_action5x5_triggered()
{
    board_size_ = 5;
    init_GUI();
}

void MainWindow::on_action6x6_triggered()
{
    board_size_ = 6;
    init_GUI();
}


// change the goalText gui based on goal_ variable
void MainWindow::update_goal_ui() {
    ui->goalText->setText(QString::number(goal_));
}

// change the seedText gui based on seed_ variable
void MainWindow::update_seed_ui() {
    ui->seedText->setText(QString::number(seed_));
}

// set the seed_ variable to a random int (range 1-1000)
void MainWindow::setRandomSeed() {
    srand (time(NULL));
    seed_ = rand() % 1000;

    update_seed_ui();
}

// method get call every second, update the value for the stop watch
void MainWindow::updateWatch() {
    secNum += 1;
    if (secNum == 60) {
        secNum = 0;
        minNum += 1;
    }

    ui->MinuteLcdNumber->display(minNum);
    ui->SecondLcdNumber->display(secNum);
}


// action for the set goal menu, query the user for a goal
// repeat until user enter a valid one ( a power of 2 )
void MainWindow::on_actionSet_Goal_triggered()
{
    int n = 3;
    while ((n & (n-1)) != 0)
    {
        n = QInputDialog::getInt(this, "Goal", "Enter your goal (a power of 2)",
                                     2, 32);
    }

    goal_ = n;
    ui->actionUnlimited->setChecked(false);
    unlimitedMode = false;
    update_goal_ui();
}


// action of the quit button
void MainWindow::on_actionQuit_triggered()
{
    this->close();
}

void MainWindow::on_quitButton_released()
{
    this->close();
}


// action for the pause button
void MainWindow::on_pauseButton_released()
{
    if (gameOver_) {
        return;
    }
    if (pause_) {
        timer->start(1000);
        pause_ = false;
        gameMessage("", "reset");
    }
    else {
        timer->stop();
        pause_ = true;
        gameMessage("Paused", "black");
    }
    updateGUI();
}

// action for the restart button
void MainWindow::on_RestartButton_released()
{
    if (board == NULL) {
        return;
    }

    if (useRandomSeed) {
        setRandomSeed();
    }

    board->fill(seed_);

    gameOver_ = false;
    pause_ = false;

    gameMessage("", "reset");
    startStopWatch();

    updateGUI();
}

// action for setting the game goal to unlimited
void MainWindow::on_actionUnlimited_toggled(bool arg1)
{
    unlimitedMode = arg1;
    if (unlimitedMode) {
        ui->goalText->setText("Unlimited");
    } else {
        update_goal_ui();
    }
}

// action for setting the game seed
void MainWindow::on_actionSet_seed_triggered()
{
    int input = QInputDialog::getInt(this, "Seed", "Enter a seed:");
    seed_ = input;
    ui->actionRandom_Seed_2->setChecked(false);
    useRandomSeed = false;
    update_seed_ui();
    init_GUI();
}

// toggle random seed action
void MainWindow::on_actionRandom_Seed_2_toggled(bool arg1)
{
    useRandomSeed = arg1;
}
