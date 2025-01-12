/**
* @file mainwindow.cpp
* @author Oleg2008e28
* @date 03.01.2025 10:11:07
* @brief Файл реализации методов класса главного окна приложения MainWindow.
* @note https://github.com/Oleg2008e28/Hexxagon
*/
#include "mainwindow.h"
#include "ui_mainwindow.h"
/**
 * @brief MainWindow::MainWindow Конструктор класса главного окна.
 * @param parent Родительский виджет.
 */
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow){
    ui->setupUi(this);
    mainWindowResize();
    game = new Game(this);                                      //Создание класса игры.
    painter = new QPainter;                                     //Создание объекта Qpainter.
//.................... Объединенные сигналы и слоты класса игры и графического виджета .......................
    connect(ui->widget, SIGNAL(transmitIndex(int)),             // Передача индекса ячейки
            game, SLOT(makeGame(int)));                         // выбранной пользователем.
    connect(game, SIGNAL(transmitField(QVector<int8_t>&)),      // Передача состояния игрового поля
            ui->widget, SLOT(setField(QVector<int8_t>&)));      // виджету.
    connect(game, SIGNAL(transmitMoveCells(QVector<int8_t>&,    // Передача виджету индексов ячеек,
                                           QVector<int8_t>&)),  // доступных для выполнения хода.
            ui->widget, SLOT(setMoveCells(QVector<int8_t>&,
                                          QVector<int8_t>&)));
    connect(game, SIGNAL(clearAvailableMove()),                 // Передача сигнала очистки выделения
            ui->widget, SLOT(clearAvailableMove()));            // доступных для хода ячеек.
    connect(game, SIGNAL(makeMove(int,int,int8_t,bool)),
            ui->widget, SLOT(makeMove(int,int,int8_t,bool)));
//.................. Объединенные сигналы и слоты класса игры и главного окна приложения .....................
    connect(game, SIGNAL(transmitScore(int,int)),
            this, SLOT(updateScore(int,int)));
    connect(game, SIGNAL(transmitPlayer(int8_t)),
            this, SLOT(updatePlayer(int8_t)));
    connect(game, SIGNAL(gameOver()),
            this, SLOT(on_buttonStop_clicked()));
//............................................................................................................
    ui->labelIconPlayer->setFixedWidth(40);                     // Установка размеров иконки
    ui->labelIconPlayer->setFixedHeight(40);                    // отображения игрока.
    updater = new QTimer(this);                                 // Создание таймера обновления.
    updater->start(30);                                         // Таймаут обновления 30 mS.
    connect(updater, SIGNAL(timeout()), SLOT(update()));        // Связь таймера с методом обновления.
}
/**
 * @brief MainWindow::~MainWindow Деструктор класса главного окна.
 */
MainWindow::~MainWindow(){
    delete updater;
    delete painter;
    delete game;
    delete ui;
}
/**
 * @brief MainWindow::on_buttonStart_clicked Обработчик нажатия кнопки запуска игры.
 */
void MainWindow::on_buttonStart_clicked(){
    int gameDepth_1 = 0;                                        // Глубина рекурсии 1.
    int gameDepth_2 = 0;                                        // Глубина рекурсии 2
    int8_t player = 0;                                          // Номер игрока, выполняющего первый ход.
    Game::Mode gameMode;                                        // Режим игры.
    if (ui->radioDepth_1_0->isChecked()){                       // Определение рекурсии для первого ПК.
        gameDepth_1 = 0;
    }else
    if(ui->radioDepth_1_1->isChecked()){
        gameDepth_1 = 1;
    }else
    if(ui->radioDepth_1_2->isChecked()){
        gameDepth_1 = 2;
    }

    if (ui->radioDepth_2_0->isChecked()){                       // Определение рекурсии для второго ПК.
        gameDepth_2 = 0;
    }else
    if(ui->radioDepth_2_1->isChecked()){
        gameDepth_2 = 1;
    }else
    if(ui->radioDepth_2_2->isChecked()){
        gameDepth_2 = 2;
    }
    if(ui->radioHumanVsHuman->isChecked()){                     // Определение режима игры.
        gameMode = Game::Mode::humanVShuman;                    // Режим игры.
        player = Game::humanPlayer_1;                           // Номер игрока делающего первый ход.
    }
    if(ui->radioHumanVsComputer->isChecked()){
        gameMode = Game::Mode::humanVSpc;
        player = Game::humanPlayer;
    }
    if(ui->radioComputerVsComputer->isChecked()){
        gameMode = Game::Mode::pcVSpc;
        player = Game::computerPlayer_1;
    }
    game->start(player, gameMode, gameDepth_1, gameDepth_2);    // Запуск игры с вычисленными параметрами.
    ui->groupMode->setDisabled(true);                           // перевод в неактивное состояние элементов
    ui->groupDepth_1->setDisabled(true);                        // управления главного окна программы на
    ui->groupDepth_2->setDisabled(true);                        // время игры.
    ui->buttonStart->setDisabled(true);
    ui->buttonStop->setEnabled(true);
}
/**
 * @brief MainWindow::on_buttonStop_clicked Обработчик нажатия кнопки останова игры.
 */
void MainWindow::on_buttonStop_clicked(){
    if (ui->radioHumanVsHuman->isChecked()){                    // Если игра человек-человек,
        ui->groupMode->setEnabled(true);                        // доступна только панель выбора режима.
    }else
    if (ui->radioHumanVsComputer->isChecked()){                 // Если ига человек-компьютер,
        ui->groupMode->setEnabled(true);                        // доступна панель выбора режима,
        ui->groupDepth_1->setEnabled(true);                     // и установка глубины рекурсии.
    }else
    if (ui->radioComputerVsComputer->isChecked()){              // Если игра компьютер-компьютер,
        ui->groupMode->setEnabled(true);                        // доступна панель выбора режима,
        ui->groupDepth_1->setEnabled(true);                     // и панели установки глубин рекурсии для
        ui->groupDepth_2->setEnabled(true);                     // обоих компьютеров.
    }
    ui->buttonStart->setEnabled(true);                          // Доступна кнопка старт.
    ui->buttonStop->setDisabled(true);                          // Неактивна кнопка стоп.
    ui->lineEditPlayer->clear();                                // Очистка полей игрока,
    ui->lineEditScore_1->clear();                               // и счета.
    ui->lineEditScore_2->clear();
    game->gameReset();                                          // Остановка игры.
}
/**
 * @brief MainWindow::on_radioHumanVsHuman_toggled Обработчик переключения радиокнопки "Человек-Человек".
 * @param checked Если радиокнопка установлена, параметр = true.
 */
void MainWindow::on_radioHumanVsHuman_toggled(bool checked){
    if (checked==true){
        ui->groupDepth_1->setDisabled(true);
        ui->groupDepth_2->setDisabled(true);
        ui->labelScore_1->setText("Игрок 1:");
        ui->labelScore_2->setText("Игрок 2:");
    }
}
/**
 * @brief MainWindow::on_radioHumanVsComputer_toggled Обработчик переключения радиокнопки "Человек-Компьютер".
 * @param checked Если радиокнопка установлена, параметр = true.
 */
void MainWindow::on_radioHumanVsComputer_toggled(bool checked){
    if (checked==true){
        ui->groupDepth_1->setEnabled(true);
        ui->groupDepth_2->setDisabled(true);
        ui->labelScore_1->setText("Игрок:");
        ui->labelScore_2->setText("Компьютер:");
    }
}
/**
 * @brief MainWindow::on_radioComputerVsComputer_toggled Обработчик радиокнопки "Компьютер-Компьютер".
 * @param checked Если радиокнопка установлена, параметр = true.
 */
void MainWindow::on_radioComputerVsComputer_toggled(bool checked){
    if (checked==true){
        ui->groupDepth_1->setEnabled(true);
        ui->groupDepth_2->setEnabled(true);
        ui->labelScore_1->setText("Компьютер 1:");
        ui->labelScore_2->setText("Компьютер 2:");
    }
}
/**
 * @brief MainWindow::updateScore Метод установки счета в окошки отображения.
 * @param score_1 Счет первого игрока.
 * @param score_2 Счет второго игрока.
 */
void MainWindow::updateScore(int score_1, int score_2){
    ui->lineEditScore_1->setText(QString::number(score_1));
    ui->lineEditScore_2->setText(QString::number(score_2));
}
/**
 * @brief MainWindow::updatePlayer Метод отображения активного игрока.
 * @param player Игрок, чей ход выполняется.
 */
void MainWindow::updatePlayer(int8_t player){
    QString mess;
    if (ui->radioHumanVsHuman->isChecked()){                    // Если выбран режим Человек-человек.
        if (player == Game::humanPlayer_1){
            mess = "Игрок 1";
        }
        else{
            mess = "Игрок 2";
        }
    }
    if (ui->radioHumanVsComputer->isChecked()){                 // Если выбран режим человек-компьютер.
        if (player == Game::humanPlayer){
            mess = "Игрок";
        }
        else{
            mess = "Компьютер";
        }
    }
    if (ui->radioComputerVsComputer->isChecked()){              // Если выбран режим компьютер компьютер.
        if (player == Game::computerPlayer_1){
            mess = "Компьютер 1";
        }
        else{
            mess = "Компьютер 2";
        }
    }
    ui->lineEditPlayer->setText(mess);
}
/**
 * @brief MainWindow::mainWindowResize Метод установки размера окна.
 */
void MainWindow::mainWindowResize(){
    int size[6];
    size[0]=ui->groupMode->minimumSizeHint().width();           // Находятся минимальные рекомендуемые размеры
    size[1]=ui->groupDepth_1->minimumSizeHint().width();        // панелей управления, и по ним определяется
    size[2]=ui->groupDepth_2->minimumSizeHint().width();        // ширина окна.
    size[3]=ui->groupPlayer->minimumSizeHint().width() +
            ui->lineEditPlayer->minimumSizeHint().width();
    size[4]=ui->groupScore->minimumSizeHint().width();
    size[5]=ui->buttonStart->minimumSizeHint().width()+
            ui->buttonStop->minimumSizeHint().width();
    int width = size[0];
    for (int i =0; i<6; i++){
        width = size[i]>width ? size[i] : width;
    }
    this->setMinimumWidth(ui->widget->width()+width+20);
    this->setMaximumWidth(ui->widget->width()+width+20);        // Высота окна определяется размером виджета
    this->setMaximumHeight(ui->widget->height());               //или панелью управления.
}
/**
 * @brief MainWindow::paintEvent Метод перерисовки главного окна.
 * @param event Системное событие.
 */
void MainWindow::paintEvent(QPaintEvent* event){
    event->ignore();
    painter->begin(this);
    QPoint posIconPlayer = ui->labelIconPlayer->pos();    // в координатах окна.
    QRect rect(posIconPlayer.x(), posIconPlayer.y(),      // Создание прямоугольной области по размерам метки.
               ui->labelIconPlayer->width(),
               ui->labelIconPlayer->height());
    QRadialGradient grad(rect.center()-QPoint(3,3),20);   // Создается градиент с центром в центре ячейки.
    grad.setColorAt(0, Qt::white);                        // У всех элементов градиент от белого.
    if(game->getActivePlayer() == 1){         // Устанавливается цвет градиента в соответствии с цветом игрока.
         grad.setColorAt(0.75, Qt::red);      // Установка основного цвета градиента.
         painter -> setBrush ( grad );        // На основе градиента создается кисть.
         painter->drawEllipse(rect);          // Отрисовка иконки игрока в области метки.
     } else
     if(game->getActivePlayer() == 2){        // Устанавливается цвет градиента в соответствии с цветом игрока.
         grad.setColorAt(0.75, Qt::blue);     // Установка основного цвета градиента.
         painter -> setBrush ( grad );        // На основе градиента создается кисть.
         painter->drawEllipse(rect);          // Отрисовка иконки игрока в области метки.
     }
     painter->end();
}
