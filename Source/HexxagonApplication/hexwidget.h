/**
* @file hexwidget.h
* @author Oleg2008e28
* @date 28.12.2024 18:23:47
* @brief Заголовочный файл класса HexWidget, реализующего игровое поле для игры гексогон.
* @note https://github.com/Oleg2008e28/Hexxagon
*/
#ifndef HEXWIDGET_H
#define HEXWIDGET_H
#include <QWidget>
#include <QPainter>
#include <QBrush>
#include <QDebug>
#include <QObject>
#include <QVector>
#include <QPoint>
#include <QGradient>
#include <QMouseEvent>
#include <QTimer>
#include <QDebug>
#include "hexxagongame.h"

namespace Ui {
class HexWidget;
}
/**
 * @brief The HexWidget class Класс игрового поля для игры гексогон.
 */
class HexWidget : public QWidget
{
    Q_OBJECT
public:
    explicit HexWidget(QWidget *parent = nullptr);      // Конструктор класса.
    ~HexWidget();                                       // Деструктор класса.
signals:
    void transmitIndex(int index);
public slots:
    void setField(QVector<int8_t>& newField);           // Слот получения вектора состояния поля.
    void setMoveCells(QVector<int8_t> &addVec,
                      QVector<int8_t> &transferVec);    // Передача свободных ходов.
    void clearAvailableMove();                          // Очистить вектора подсказок ходов на виджете.
    void makeMove(int beginIndex, int newIndex,
                  int8_t player, bool clearPosition);   // Выполнить ход на виджете.
private slots:
    void updateMoveChip();
private:
    void paintEvent(QPaintEvent* event);                // Метод отрисовки всего виджета.
    void drawItemSlot(int x, int y, int R, int index);  // Метод отрисовки одной ячейки игрового поля.
    void mousePressEvent(QMouseEvent *event);           // Обработчик нажатия мыши.
    QVector<int8_t> getNearCells(int8_t n);             // Метод вычисления индексов соседних ячеек.
private:
    const QVector<int8_t>clearField = {                 // Состояние пустого игрового поля при старте.
                               0,0,0,0,0,0,0,0,0,0,     // 0-9
                               0,0,0,0,0,0,0,0,0,0,     // 10-19
                               0,0,3,0,0,0,0,0,0,3,     // 20-29
                               0,0,0,0,0,0,0,0,0,3,     // 30-39
                               0,0,0,0,0,0,0,0,0,0,     // 40-49
                               0,0,0,0,0,0,0,0,0,0,     // 50-59
                               0                        // 60
                             };
    Ui::HexWidget *ui;                                  // Указатель на объект виджет.
    QBrush* lightSlotBrush;                             // Указатель на кисть для отрисовки светлого борта.
    QBrush* normalSlotBrush;                            // Указатель на кисть для основного полигона
    QBrush* darkSlotBrush;                              // Указатель на кисть для темного борта
    QBrush* bottomSlotBrush;                            // Указатель на кисть для дна
    QBrush* addSlotBrush;                               // Указатель на кисть выделения ячейки добавления.
    QBrush* stepSlotBrush;                              // Указатель на кисть выделения ячейки перестановки.
    QPainter* painter;                                  // Указатель на объект Painter, для отрисовки виджета.
    QTimer* updater;                                    // Указатель на таймер обновления виджета.
    QVector<int8_t>fieldVec;                            // Вектор состояния игрового поля.
    QVector<int8_t>addVec;                              // Вектор индексов ходов с добавлением фишки.
    QVector<int8_t>stepVec;                             // Вектор индексов ходов с перестановкой фишки.
    QVector<QPoint>cordsVec;                            // Вектор координат центров ячеек поля.
    int rDetect;                                        // Радиус для определения ячейки указателем мыши.
    bool drawMoveChip = false;                          // Разрешение отрисовки ходящей фишки.
    QColor color;                                       // Переменная для цвета движущегося объекта.
    QPoint center;                                      // Точка центра движущегося объекта.;
    int ds;                                             // Шаг перемещения при движении.
    QPoint point_begin;                                 // Точка начала движения объекта.
    QPoint point_end;                                   // Точка окончания движения объекта.
    int Rc;                                             // Вычисляемый радиус движущегося объекта.
    int R;                                              // Радиус фишки на поле.
    int nS;                                             // Количество точек перемещения.
    int dx;                                             // Приращение координаты x на один ход при движении.
    int dy;                                             // Приращение координаты y на один ход при движении.
    int S;                                              // Длина отрезка перемещения.
    int8_t enemy;                                       // Значение противника.
    int8_t player;                                      // Значение игрока.
    int newIndex;                                       // Индекс ячейки хода.
    int counter = 0;                                    // Счетчик шагов перемещения.
    bool avalibleFlag = true;                           // Флаг разрешения посылки сигналов выбора ячейки.
};
#endif // HEXWIDGET_H
