/**
* @file mainwindow.h
* @author Oleg2008e28
* @date 03.01.2025 09:01:23
* @brief Заголовочный файл класса MainWindow, реализующего главное окно приложения.
* @note https://github.com/Oleg2008e28/Hexxagon
*/
#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <hexwidget.h>
#include <hexxagongame.h>
#include <QPainter>
#include <algorithm>
namespace Ui {
class MainWindow;
}
/**
 * @brief The MainWindow class Класс главного окна приложения.
 */
class MainWindow : public QMainWindow{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);                   // Конструктор класса главного окна.
    ~MainWindow();                                              // Деструктор класса главного окна.
private slots:
    void on_buttonStart_clicked();                              // Обработчик нажатия кнопки запуска игры.
    void on_buttonStop_clicked();                               // Обработчик нажатия кнопки останова игры.
    void mainWindowResize();                                    // Метод установки размера окна.
    void updateScore(int score_1, int score_2);                 // Метод установки счета в окошки отображения.
    void updatePlayer(int8_t player);                           // Метод отображения активного игрока.
    void on_radioHumanVsHuman_toggled(bool checked);            // Обработчик радиокнопки "Человек-Человек".
    void on_radioHumanVsComputer_toggled(bool checked);         // Обработчик радиокнопки "Человек-Компьютер".
    void on_radioComputerVsComputer_toggled(bool checked);      // Обработчик радиокнопки "Компьютер-Компьютер".
    void paintEvent(QPaintEvent* event);
private:
    Ui::MainWindow *ui;                                         // указатель на главное окно.
    Game* game;                                                 // Указатель на класс игры.
    QPainter* painter;                                          // Указатель на класс рисовальщика.
    QTimer* updater;                                            // Указатель на таймер обновления окна.
};
#endif // MAINWINDOW_H
