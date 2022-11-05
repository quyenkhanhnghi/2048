#ifndef MAINWINDOW_HH
#define MAINWINDOW_HH

#include <QMainWindow>
#include <QLabel>
#include <QTimer>
#include <QInputDialog>
#include <iostream>
#include <map>
#include <vector>

#include "numbertile.hh"
#include "gameboard.hh"

using namespace std;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    // Slots for the navigation buttons for the game (up/down/left/right)
    void on_upButton_released();

    void on_leftButton_released();

    void on_downButton_released();

    void on_rightButton_released();

    // Menu option for the game setting to change the size of the game board
    void on_action3x3_triggered();

    void on_action4x4_triggered();

    void on_action5x5_triggered();

    void on_action6x6_triggered();

    // Slot for the quit button to quit the game
    void on_quitButton_released();

    // Slot fot the restart button to restart the game
    void on_RestartButton_released();

    // Slot fot the pause button to restart the game
    void on_pauseButton_released();

    // function responsible to update the coutdown watch in game
    void updateWatch();

    // Menu option to set the goal for the game instance
    void on_actionSet_Goal_triggered();

    // Menu option to quit the game
    void on_actionQuit_triggered();

    // Menu option to set the game goal to unlimited
    void on_actionUnlimited_toggled(bool arg1);

    // Menu option to set the seed for the game
    void on_actionSet_seed_triggered();

    // Menu option to set the seed for the game to be random (in range 1 -> 1000)
    void on_actionRandom_Seed_2_toggled(bool arg1);

private:
    Ui::MainWindow *ui;
    
    // All the attributes describe the current game instance
    bool gameOver_ = true;
    bool pause_ = false;
    bool unlimitedMode = false;
    bool useRandomSeed = false;
    int goal_ = 2048, seed_ = 0;
    int board_size_ = 4;
    int highScore_ = 0;
    int tile_size_ = 0;

    // a pointer to a Gameboard object instance 
    // board == NULL => no active game
    GameBoard * board = NULL;

    // A pointer for the game board of the game
    // use this to set the icons for the game and calculate the score
    std::vector<std::vector<NumberTile*>> * vectorBoard = NULL;

    // A map that has integer as key and QImages pointer as value
    // use this to store the QImages to later use for setting the icon for the GUI 
    map<int, QImage*> imagesMap;
    
    // variable for the timer in the game
    QTimer * timer = new QTimer();
    int minNum = 0;
    int secNum = 0;

    // A Container of QLabels pointer that are displaying to the GUI
    // Contain a maximum of 6x6 QLabel
    vector<vector<QLabel*>> GUIboard;

    // update the GUI element based on the state of the game (using GameBoard * board)
    void updateGUI();

    // Method using the method from GameBoard::move(Coords dir, int goal) and connect to button action
    void GUI_move(Coords coord);

    // inittialize all the GUI element like QLabel and set all the game's state variable to the default
    void init_GUI();

    // method to update the GUI element of goal, seed according to it's value
    void update_goal_ui();
    void update_seed_ui();

    // Method set the random_ variable to a random int (in range 1-1000)
    void setRandomSeed();

    // Method change the text of a QTextBrowser to display message to user like losing, winning or pause
    void gameMessage(QString message, string color);


    // Method to start the timer in the game
    void startStopWatch();

    // Method set the state of the game gameOver_, true for winning and false for losing.
    void gameOver_GUI(bool won);
};
#endif // MAINWINDOW_HH
