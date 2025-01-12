/**
* @file hexxagongame.h
* @author Oleg2008e28
* @date 31.12.2024 17:30:25
* @brief Заголовочный файл класса Game, реализующего игру гексогон. Класс предоставляет три режима работы
* программы: режим игры человек против человека, человек против компьютера, компьютер против компьютера.
* @note https://github.com/Oleg2008e28/Hexxagon
*/    
#ifndef GAME_H
#define GAME_H
#include <QVector>
#include <QMessageBox>
#include <QDebug>
#include <QObject>
#include <QThread>
#include <QtConcurrent/QtConcurrent>
#include <QRadialGradient>
#include <climits>
#include <algorithm>
#include <ctime>
//............................................................................................................
class Game : public QObject{
    Q_OBJECT
public:
    /**
     * @brief The Mode enum Возможные режимы игры.
     */
    enum class Mode{                                    //!Нет никакого режима.
        none,                                           //!Игрок против игрока.
        humanVShuman,                                   //!Игрок против компьютера.
        humanVSpc,                                      //!Компьютер против компьютера.
        pcVSpc
    };
private:
    /**
     * @brief The State enum Возможние состояния игрового автомата.
     */
    enum class State{
        none,                                           //!Игровой автомат не активен.
        waitIndex,                                      //!Ожидание индекса для определения ходов.
        humanMove,                                      //!Игрок против игрока.
        pcMove,                                         //!Игрок против компьютера.
        witePcMove                                      //!Состояние ожидания хода компьютера.
    };
    /**
     * @brief The Move struct Объект для хранения хода.
     */
    struct Move {
        int8_t beginPostion;                            //!Текущая позиция фишки.
        int8_t newPostion;                              //!Новая позиция фишки.
        bool clearBeginPostion;                        //!Очистка начальной позиции true-очистить, false-нет.
    };
//............................................................................................................
public:
explicit Game(QObject *parent = nullptr);               // Конструктор класса Game.
    ~Game();                                            // Деструктор класса Game.
    void pauseOn();                                     // Включить паузу хода компьютера.
    void pauseOff();                                    // Отключить паузу хода компьютера.
    void gameReset();                                   // Метод остановки игры.
    int getActivePlayer();                              // Метод возвращает значение текущего игрока.
    int getDepth_1();                                   // Метод возвращает глубину рекурсии 1.
    int getDepth_2();                                   // Метод возвращает глубины рекурсии 2.
    QVector<int8_t> getField();                         // Метод возвращает текущее состояние игрового поля.
    Mode getModeGame();                                 // Метод возвращает режима игры.
    void start(int beginPlayer, Mode mode,
               int depth_1=0, int depth_2=0);           // Метод запуска игры.
//............................................................................................................
protected:
    QVector<int8_t>
    getAddMoveCells(QVector<int8_t> const& field,
                    int8_t n);                          // Метод определяет доступные ячейки для ближнего хода.
    QVector<int8_t> getFarCells(int8_t n);              // Метод определяет индексы ячеек для дальнего хода.
    QVector<int8_t> getNearCells(int8_t n);             // Метод определяет индексы ячеек для ближнего хода.
    QVector<int8_t>
    getTransferMoveCells(QVector<int8_t> const& field,
                         int8_t n);                     // Метод определяет доступные ячейки для дальнего хода.
//............................................................................................................
private:
    void changePlayer();                                // Метод смены игрока.
    bool checkMoves(int8_t player);                     // Метод проверяет наличие ходов игрока.
    int checkScore(QVector<int8_t> const& field,
                   int player);                         // Оценочная функция игры "Гексогон".
    int counterChips(int8_t player);                    // Метод выполняет подсчет фишек указанного игрока.
    void createAvailableMove(int index);                // Метод определения доступных ходов для человека.
    void doMove(QVector<int8_t>& field, Move &move);    // Выполнение хода по структуре Move.
    bool findWinner();                                  // Метод поиска победителя.
    bool generateMoves(QVector<int8_t> const& field,
                       QVector<Move>& moves,
                       int player);                     // Метод генерации ходов игрока.
    int8_t getOpponent(int8_t player);                  // Метод возвращает значение противника.
    int indexMaxItem(QVector<int> const& Vec);          // Метод поиска наибольшего элемента в массиве.
    void makeComputerMove();                            // Метод выполняющий найденный ход компьютера.
    int miniMax(QVector<int8_t>const& field, int player, // Метод оценки ходов методом миниМакса с
               int masterPlayer, int depth, int level=0, // альфа-бета отсечением.
               int alpha=INT_MIN, int beta = INT_MAX);
    Move selectComputerMove(QVector<int8_t>& field,
                            int player, int depth);     // Метод выполняет выбор хода для компьютера.
    void startComputerMove();                           // Метод запускает вычисление хода компьютера в
                                                        // параллельном потоке.
//..........................................................................................................
signals:
    void clearAvailableMove();                          // Сигнал для отмены отображения доступных ходов.
    void gameOver();                                    // Сигнал о завершении игры.
    void makeMove(int currentPosition, int newPosition,
                  int8_t player, bool clearCurrentPos); // Сигнал графическому виджету сделать ход.
    void transmitField(QVector<int8_t>& field);         // Сигнал о передаче состояния поля.
    void transmitMoveCells(QVector<int8_t>&addMoveCells,
                    QVector<int8_t> &transferMoveCells);// Сигнал передачи виджету доступных ходов.
    void transmitPlayer(int8_t activePlayer);           // Сигнал передачи текущего игрока.
    void transmitScore(int score_1, int score_2);       // Сигнал передачи игрового счета.
//..........................................................................................................
public slots:
    void makeGame(int index);                           // Метод игрового режима Человек-Человек,
                                                        // и человек компьютер.
private slots:
    void computerMove();                                // Метод выполняет ход компьютера
                                                        // (полный ход, от начала до конца).
//..........................................................................................................
public:
    static const int8_t humanPlayer_1 = 1;              //!Номер первого игрока в режиме человек-человек.
    static const int8_t humanPlayer_2 = 2;              //!Номер второго игрока в режиме человек-человек.
    static const int8_t humanPlayer = 1;                //!Номер игрока человека в режиме человек-компьютер.
    static const int8_t computerPlayer = 2;             //!Номер игрока компьютера в режиме человек-компьютер.
    static const int8_t computerPlayer_1 = 1;           //!Номер компьютера 1 в режиме компьютер-компьютер.
    static const int8_t computerPlayer_2 = 2;           //!Номер компьютера 2 в режиме компьютер-компьютер.
//..........................................................................................................
private:
    State state = State::none;                          // Состояние игрового автомата.
    QVector<int8_t>field;                               // Вектор для хранения состояния игрового поля.
    QVector<int8_t> addMoveCells;                       // Вектор для хранения ближних ходов.
    QVector<int8_t> transferMoveCells;                  // Вектор для хранения дальних ходов.
    Mode modeGame = Mode::none;                         // Режим игры.
    int8_t activePlayer = 0;                            // Текущий игрок.
    int currentIndex = 0;                               // Текущий индекс игровой позиции.
    int gameDepth_1 = 0;                                // Глубина рекурсии для ПК 1.
    int gameDepth_2 = 0;                                // Глубина рекурсии для ПК 2.
    QFuture<Move>* future;                              // Переменная передачи хода из второго потока.
    QTimer* timer;                                      // Таймер для задержки вычисления хода компьютера.
    const int timeOutComputerMove = 1000;               // Время для вычисления хода компьютера.
    const QVector<int8_t>startField = {                 // Состояние игрового поля при старте.
                               1,0,0,0,2,0,0,0,0,0,     // 0-9
                               0,0,0,0,0,0,0,0,0,0,     // 10-19
                               0,0,3,0,0,0,2,0,0,3,     // 20-29
                               0,0,0,0,1,0,0,0,0,3,     // 30-39
                               0,0,0,0,0,0,0,0,0,0,     // 40-49
                               0,0,0,0,0,0,1,0,0,0,     // 50-59
                               2                        // 60
                             };
    const QVector<int8_t>clearField = {                 // Состояние пустого игрового поля при старте.
                               0,0,0,0,0,0,0,0,0,0,     // 0-9
                               0,0,0,0,0,0,0,0,0,0,     // 10-19
                               0,0,3,0,0,0,0,0,0,3,     // 20-29
                               0,0,0,0,0,0,0,0,0,3,     // 30-39
                               0,0,0,0,0,0,0,0,0,0,     // 40-49
                               0,0,0,0,0,0,0,0,0,0,     // 50-59
                               0                        // 60
                             };
};
//..........................................................................................................
#endif // GAME_H
