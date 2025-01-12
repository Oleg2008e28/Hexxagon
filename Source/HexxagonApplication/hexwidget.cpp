/**
* @file hexwidget.cpp
* @author Oleg2008e28
* @date 28.12.2024 18:56:21
* @brief Файл реализации методов класса HexWidget.
* @note https://github.com/Oleg2008e28/Hexxagon
*/
#include "hexwidget.h"
#include "ui_hexwidget.h"
/**
 * @brief HexWidget::HexWidget Конструктор класса.
 * @param parent Указатель на родительский виджет.
 */
HexWidget::HexWidget(QWidget *parent) : QWidget(parent), ui(new Ui::HexWidget){
    fieldVec = clearField;                                  // Создание чистого поля.
    painter = new QPainter();                               // Создание объекта Painter, для отрисовки виджета.
    lightSlotBrush = new QBrush(QColor(209, 180, 0));       // Цвет светлого борта.
    normalSlotBrush = new QBrush(QColor(255, 215, 0));      // Цвет основного полигона.
    darkSlotBrush = new QBrush(QColor(153, 129, 0));        // Цвет темного борта.
    bottomSlotBrush = new QBrush(QColor(178, 150, 0));      // Цвет дна.
    addSlotBrush = new QBrush(QColor(0, 255, 0, 180));      // Кисть для выделения ячейки добавления фишки.
    stepSlotBrush = new QBrush(QColor(0, 255, 255, 180));   // Кисть для выделения ячейки перестановки фишки.
    updater = new QTimer(this);                             // Создание таймера обновления.
    updater->start(30);                                     // Запуск таймера обновления.
    connect(updater, SIGNAL(timeout()), SLOT(update()));    // Включение обновления виджета по таймеру.
    ui->setupUi(this);                                      // Создание окна.
}
/**
 * @brief HexWidget::~HexWidget Деструктор класса.
 */
HexWidget::~HexWidget(){
    delete painter;                                 // Удаление объекта Painter.
    delete lightSlotBrush;                          // Удаление кисти светлого борта.
    delete normalSlotBrush;                         // Удаление кисти основного полигона.
    delete darkSlotBrush;                           // Удаление кисти темного борта.
    delete bottomSlotBrush;                         // Удаление кисти цвет дна.
    delete addSlotBrush;                            // Удаление кисти для выделения ячейки добавления фишки.
    delete stepSlotBrush;                           // Удаление кисти для выделения ячейки перестановки фишки.
    delete updater;                                 // Удаление таймера обновления.
    delete ui;
}
/**
 * @brief HexWidget::paintEvent Метод перерисовки виджета.
 * @param event Системное событие.
 */
void HexWidget::paintEvent(QPaintEvent* event){
    event->ignore();
    int indexCounter = 0;                                   // Счетчик индексов отрисованных ячеек.
    painter->begin(this);                                   // Запуск объекта QPainter.
    painter->setRenderHint(QPainter::Antialiasing, true);
    cordsVec.clear();                                       // Очистка вектора координат центров ячеек.
    int wigetHeight = this->height();                       // Высота виджета.
    ds = double(this->height())*0.015;                      // Вычисление шага перемещения.
    int h_op = wigetHeight /18;                             // Расстояние от центра шестигранника до стороны.
    rDetect = h_op;                                         // Радиус детектирования указателя мыши на ячейке.
    R = 2*h_op/sqrt(3);                                     // Радиус шестигранника.
    int smesh_y = double(R)*sin(double(60)* (M_PI / 180));  // Смещение по y при отрисовке поля.
    int smesh_x = double(R)*cos(double(60)* (M_PI / 180));  // Смещение по x при отрисовке поля.
    int x = R;                                              // Начальная координата x при отрисовке.
    int y = 4 * h_op + R;                                   // Начальная координата y при отрисовке.
    int m = 5;                                              // Минимальное количество ячеек в столбце.
    int delta = 0;                                          // Смещение по y при отрисовке столбца.
    int y_tmp = y;                                          // Координата y очередной ячейки в столбце.
    for (int j = 0; j < m * 2 - 1; j++){                    // Цикл отрисовки столбцов.
        for (int i= 0; i < m; i++){                         // Цикл отрисовки ячеек в столбце.
            drawItemSlot(x, y_tmp, R, indexCounter);        // Отрисовка одной ячейки.
            cordsVec.push_back(QPoint(x, y_tmp));           // Запись координат центра ячейки в вектор.
            y_tmp += smesh_y * 2;                           // Смещение по y для отрисовки следующей ячейки.
            indexCounter++;
        }
        if (j < m / 2){                                     // Если поле не отрисованно до половины,
            m++;                                            // то высота столбцов растет.
            delta -= smesh_y;                               // Смещение по y увеличивается.
        }
        else {                                              // Если отрисованно больше половины поля,
            m--;                                            // высота столбцов уменьшается.
            delta += smesh_y;                               // Смещение по y уменьшается.
        }
        x += smesh_x * 3;                                   // Смещение по x для отрисовки следующего столбца.
        y_tmp = y + delta;                                  // Смещение по y для отрисовки следующего столбца.
    }
    if (drawMoveChip){                                      // Если отрисовка временного объекта разрешена.
        QRadialGradient grad(center-QPoint(Rc*0.15,Rc*0.15)
                             ,Rc*0.9);                      //Создается градиент.
        grad.setColorAt(0.75, color);
        grad.setColorAt(0, Qt::white);                      // У всех элементов градиент от белого.
        painter -> setBrush ( grad );                       // На основе градиента создается кисть и
        painter->drawEllipse(center, Rc, Rc);               // устанавливается в рисовальщик.
    }
    painter->end();                                         // Отключение объекта QPainter.
}
/**
 * @brief HexWidget::drawItemSlot Метод отрисовки одной ячейки игрового поля.
 * @param x Координата x центра ячейки.
 * @param y Координата y центра ячейки.
 * @param R Радиус описываемой окружности.
 * @param index Номер ячейки.
 */
void HexWidget::drawItemSlot(int x, int y, int R, int index){
    QPolygon polygon;
    int r1 = R-R*0.1;                                // Вычисление большего внутреннего радиуса шестигранника.
    int r2 = r1-R*0.12;                              // Вычисление меньшего внутреннего радиуса шестигранника.
    int dX = double(R)*cos(double(60)* (M_PI / 180));
    int dY = double(R)*sin(double(60)* (M_PI / 180));
    int dx1 = double(r1)*cos(double(60)* (M_PI / 180));
    int dy1 = double(r1)*sin(double(60)* (M_PI / 180));
    int dx2 = double(r2)*cos(double(60)* (M_PI / 180));
    int dy2 = double(r2)*sin(double(60)* (M_PI / 180));
    if (fieldVec[index]<3){
        painter->setPen(Qt::black);
        painter->setBrush(*normalSlotBrush);

        polygon << QPoint(x+R, y) << QPoint(x+dX, y-dY)         // Отрисовка большого полигона.
                << QPoint(x-dX, y-dY) << QPoint(x-R, y)
                << QPoint(x-dX, y+dY) << QPoint(x+dX, y+dY)
                <<QPoint(x+R, y);
        painter->drawPolygon(polygon);

        painter->setPen(QColor(Qt::black));
        polygon.clear();                                        // Отрисовка верхнего борта.
        polygon << QPoint(x+dx1, y-dy1) << QPoint(x+dx2, y-dy2)
                << QPoint(x-dx2, y-dy2) << QPoint(x-dx1, y-dy1)
                << QPoint(x+dx1, y-dy1);
        painter->setBrush(*darkSlotBrush);
        painter->drawPolygon(polygon);

        polygon.clear();                                        // Отрисовка правого верхнего борта.
        polygon << QPoint(x+dx1, y-dy1) << QPoint(x+dx2, y-dy2)
                << QPoint(x+r2, y) << QPoint(x+r1, y)
                << QPoint(x+dx1, y-dy1);
        painter->setBrush(*lightSlotBrush);
        painter->drawPolygon(polygon);

        polygon.clear();                                        // Отрисовка правого нижнего борта.
        polygon << QPoint(x+r2, y) << QPoint(x+r1, y)
                << QPoint(x+dx1, y+dy1) << QPoint(x+dx2, y+dy2)
                << QPoint(x+r2, y);
        painter->setBrush(*lightSlotBrush);
        painter->drawPolygon(polygon);

        polygon.clear();                                        // Отрисовка нижнего борта.
        polygon <<  QPoint(x+dx1, y+dy1) << QPoint(x+dx2, y+dy2)
                << QPoint(x-dx2, y+dy2) << QPoint(x-dx1, y+dy1)
                << QPoint(x+dx1, y+dy1);
        painter->setBrush(*lightSlotBrush);
        painter->drawPolygon(polygon);

        polygon.clear();                                        // Отрисовка левого нижнего борта.
        polygon <<  QPoint(x-dx2, y+dy2) << QPoint(x-dx1, y+dy1)
                << QPoint(x-r1, y) << QPoint(x-r2, y)
                <<  QPoint(x-dx2, y+dy2);
        painter->setBrush(*darkSlotBrush);
        painter->drawPolygon(polygon);

        polygon.clear();                                        // Отрисовка левого верхнего борта.
        polygon <<  QPoint(x-r1, y) << QPoint(x-r2, y)
                << QPoint(x-dx2, y-dy2) << QPoint(x-dx1, y-dy1)
                <<  QPoint(x-r1, y);
        painter->setBrush(*darkSlotBrush);
        painter->drawPolygon(polygon);

        polygon.clear();                                        // Отрисовка дна ячейки.
        polygon << QPoint(x+r2, y) << QPoint(x+dx2, y-dy2)
                << QPoint(x-dx2, y-dy2) << QPoint(x-r2, y)
                << QPoint(x-dx2, y+dy2) << QPoint(x+dx2, y+dy2)
                <<QPoint(x+r2, y);
        painter->setBrush(*bottomSlotBrush);
        painter->drawPolygon(polygon);
    }
    for(auto a : addVec){                                        // Отрисовка клеток добавления
       if (a == index){
            polygon.clear();
            polygon << QPoint(x+R, y) << QPoint(x+dX, y-dY)
                    << QPoint(x-dX, y-dY) << QPoint(x-R, y)
                    << QPoint(x-dX, y+dY) << QPoint(x+dX, y+dY)
                    <<QPoint(x+R, y);
            painter->setBrush(*addSlotBrush);
            painter->drawPolygon(polygon);
            break;
       }
    }
    for(auto a : stepVec){                                       // Отрисовка клеток перестановки
        if (a == index){
            polygon.clear();
            polygon << QPoint(x+R, y) << QPoint(x+dX, y-dY)
                    << QPoint(x-dX, y-dY) << QPoint(x-R, y)
                    << QPoint(x-dX, y+dY) << QPoint(x+dX, y+dY)
                    <<QPoint(x+R, y);
            painter->setBrush(*stepSlotBrush);
            painter->drawPolygon(polygon);
        }
    }
    if (fieldVec[index]==1 || fieldVec[index]==2){
        QRadialGradient grad(QPoint(x,y)-QPoint(R*0.15,R*0.15),R*0.6);  // Создается градиент.
        if(fieldVec[index] == 1){                                       // Устанавливается цвет градиента в
            grad.setColorAt(0.75, Qt::red);                             // соответствии с цветом игрока.
        }
        if(fieldVec[index] == 2){
            grad.setColorAt(0.75, Qt::blue);
        }
        grad.setColorAt(0, Qt::white);                                  // У всех элементов градиент от белого.
        painter -> setBrush ( grad );                                   // На основе градиента создается кисть
        painter->drawEllipse(x-R*1.2/2,y-R*1.2/2, R*1.2, R*1.2);        // и устанавливается в рисовальщик.
    }
}
/**
 * @brief HexWidget::mousePressEvent Обработчик событий мыши.
 * @param event Системное событие.
 */
void HexWidget::mousePressEvent(QMouseEvent *event){
    if (event->button() == Qt::LeftButton){                 // Проверка нажатия левой кнопки мыши.
        QPoint mousPos = event -> pos();                    // Получение координат мыши.
        int index;                                          // Переменная для определения индекса ячейки поля.
        for (index = 0; index < cordsVec.size(); index++){  // Перебор координат центров всех ячеек.
          int dx = mousPos.x() - cordsVec[index].x();       // Вычисление расстояния от мыши
          int dy = mousPos.y() - cordsVec[index].y();       // до центра ячейки.
          int rast = sqrt(dx * dx + dy * dy);               // Если расстояние окажется меньше радиуса
          if (rast < rDetect) {                             // детектирования, то ячейка
              break;                                        // найдена и цикл прерывается.
          }
        }
        if (avalibleFlag && index < cordsVec.size()){       // Если при переборе ячейка была найдена
            emit transmitIndex(index);                      // выдается сигнал с полученным индексом.
        }
    }
}
/**
 * @brief HexWidget::setField Метод установки состояния игрового поля.
 * @param newField Новое состояние игрового поля.
 */
void HexWidget::setField(QVector<int8_t> & newField){
    fieldVec = newField;
    if (drawMoveChip){                                  // Если происходит отрисовка хода.
        disconnect(updater, SIGNAL(timeout()),
                   this, SLOT(updateMoveChip()));       // Отключение слота обновления параметров объекта.
        avalibleFlag = true;                            // Разрешение виджету посылать сигнал индекса ячейки.
        drawMoveChip = false;                           // Запрет отрисовки движущегося объекта.
    }
}
/**
 * @brief HexWidget::setMoveCells Метод устанавливает значения ячеек доступных для хода игрока.
 * @param newAdd Вектор индексов ячеек доступных для ближнего хода.
 * @param newTtransfer Вектор индексов ячеек доступных для дальнего хода.
 */
void HexWidget::setMoveCells(QVector<int8_t> &newAdd, QVector<int8_t> &newTtransfer){
    addVec = newAdd;
    stepVec = newTtransfer;
}
/**
 * @brief HexWidget::clearAvailableMove Метод отмены выделения ячеек доступных ходов.
 */
void HexWidget::clearAvailableMove(){
    addVec.clear();
    stepVec.clear();
}
/**
 * @brief HexWidget::makeMove  Метод выполнения хода на виджете.
 * @param beginIndex Индекс ячейки фишки, выполняющей ход.
 * @param NewIndex  Индекс ячейки в которую выполняется ход.
 * @param Player Игрок, выполняющий ход.
 * @param clearPosition Флаг, определяющий необходимость очистки ячейки.
 */
void HexWidget::makeMove(int beginIndex, int NewIndex, int8_t Player, bool clearPosition){
    if (!drawMoveChip){                                     // Если отрисовка временного объекта не начата.
        newIndex = NewIndex;                                // установка индекса ячеек конца и начала
        player = Player;                                    // движения фишки.
        enemy = (player == 1) ? 2 : (player == 2) ? 1 : 0;  // Вычисление значения противника.
        color = (player == 1) ? Qt::red : Qt::blue;         // Вычисление значения противника.
        if (clearPosition){                                 // Если ход с очисткой текущей позиции,
            fieldVec[beginIndex] = 0;                       // очищаем ее.
        }
        point_begin = cordsVec[beginIndex];                 // Получение координаты X точки начала движения.
        point_end = cordsVec[NewIndex];                     // Получение координаты Y точки начала движения.
        int dX = point_end.x() - point_begin.x();           // Изменение координаты X.
        int dY = point_end.y() - point_begin.y();           // Изменение координаты Y.
        S = sqrt(dX*dX + dY*dY);                            // Длина отрезка перемещения.
        nS = S/ds;                                          // Количество шагов передвижения.
        dx = dX/nS;                                         // Изменение координаты X на один шаг перемещения.
        dy = dY/nS;                                         // Изменение координаты Y на один шаг перемещения.
        center = cordsVec[beginIndex];                      // Координаты центра движущегося объекта.
        counter = 0;                                        // Обнуление счетчика позиций отрисовки объекта.
        updateMoveChip();
    }
}
/**
  * @brief HexWidget::updateMoveChip Метод изменения переменных движущегося объекта.
  */
 void HexWidget::updateMoveChip(){
        if (!drawMoveChip){                                 // Если отрисовка временного объекта не начата.
         connect(updater, SIGNAL(timeout()),
                 this, SLOT(updateMoveChip()));             // Подключение таймера к слоту обновления объекта.
         avalibleFlag = false;                              // Запрет сигнала передачи индекса.
         drawMoveChip = true;                               // Разрешение на отрисовку временного объекта.
        }
        if (counter <= nS){                                 // Если не все шаги движения сделаны,
           Rc = 0.6 * R *
              (1.0f + sin(3.14 / S * counter * ds) * 0.5);  // Вычисляется новый радиус объекта и
           center.setX(point_begin.x() + dx * counter);     // новая точка положения объекта.
           center.setY(point_begin.y() + dy * counter);
           counter++;                                       // Увеличение счетчика позиций перемещения.
        }
        else {                                              // Если все позиции перемещения отрисованы,
            disconnect(updater, SIGNAL(timeout()),
                       this, SLOT(updateMoveChip()));       // отключение слота обновления параметров объекта.
            fieldVec[newIndex] = player;                                // Изменение значения в новой позиции.
            QVector<int8_t> tmpArountCells = getNearCells(newIndex);    // Вектор ячеек вокруг новой позиции.
            for (auto a : tmpArountCells) {                             // Если вокруг фишки есть противник,
                if (fieldVec[a] == enemy) fieldVec[a] = player;         // его фишки перекрашиваются.
            }
            avalibleFlag = true;                            // Разрешение сигнала передачи индекса.
            drawMoveChip = false;                           // Запрет на отрисовку временного объекта.
        }
}
 /**
 * @brief HexWidget::getNearCells Метод получения индексов ячеек, соседних с заданной.
 * @param n Заданная ячейка.
 * @return Векотор индексов соседних ячеек.
 */
QVector<int8_t> HexWidget::getNearCells(int8_t n) {
    QVector<int8_t> pV;
    int8_t begin_1 = 0;                     // Начальный индекс левого столбца.
    int8_t end_1 = 0;                       // Конечный индекс левого столбца.
    int8_t a_1 = 0;                         // Смещение 1 для левого столбца.
    int8_t b_1 = 0;                         // Смещение 2 для левого столбца.

    int8_t begin_2 = 0;                     // Начальный индекс текущего столбца.
    int8_t end_2 = 0;                       // Конечный индекс текущего столбца.

    int8_t begin_3 = 0;                     // Начальный индекс правого столбца.
    int8_t end_3 = 0;                       // Конечный индекс правого столбца.
    int8_t a_3 = 0;                         // Смещение 1 для правого столбца.
    int8_t b_3 = 0;                         // Смещение 2 для правого столбца.


    if (n >= 0 && n <= 4) {                 // Если заданная ячейка находится в первом столбце поля.
        begin_2 = 0;                        // Начальный индекс текущего столбца.
        end_2 = 4;                          // Конечный индекс текущего столбца.
        begin_3 = 5;                        // Начальный индекс правого столбца.
        end_3 = 10;                         // Конечный индекс правого столбца.
        a_3 = 5;                            // Смещение 1 для правого столбца.
        b_3 = 6;                            // Смещение 2 для правого столбца.
    }
    if (n >= 5 && n <= 10) {                // Если заданная ячейка находится во втором столбце поля.
        begin_1 = 0;                        // Начальный индекс левого столбца.
        end_1 = 4;                          // Конечный индекс левого столбца.
        a_1 = -6;                           // Смещение 1 для левого столбца.
        b_1 = -5;                           // Смещение 2 для левого столбца.
        begin_2 = 5;                        // Начальный индекс текущего столбца.
        end_2 = 10;                         // Конечный индекс текущего столбца.
        begin_3 = 11;                       // Начальный индекс правого столбца.
        end_3 = 17;                         // Конечный индекс правого столбца.
        a_3 = 6;                            // Смещение 1 для правого столбца.
        b_3 = 7;                            // Смещение 2 для правого столбца.
    }
    if (n >= 11 && n <= 17) {               // Если заданная ячейка находится в третьем столбце поля.
        begin_1 = 5;                        // Начальный индекс левого столбца.
        end_1 = 10;                         // Конечный индекс левого столбца.
        a_1 = -7;                           // Смещение 1 для левого столбца.
        b_1 = -6;                           // Смещение 2 для левого столбца.
        begin_2 = 11;                       // Начальный индекс текущего столбца.
        end_2 = 17;                         // Конечный индекс текущего столбца.
        begin_3 = 18;                       // Начальный индекс правого столбца.
        end_3 = 25;                         // Конечный индекс правого столбца.
        a_3 = 7;                            // Смещение 1 для правого столбца.
        b_3 = 8;                            // Смещение 2 для правого столбца.
    }
    if (n >= 18 && n <= 25) {               // Если заданная ячейка находится в четвертом столбце поля.
        begin_1 = 11;                       // Начальный индекс левого столбца.
        end_1 = 17;                         // Конечный индекс левого столбца.
        a_1 = -8;                           // Смещение 1 для левого столбца.
        b_1 = -7;                           // Смещение 2 для левого столбца.
        begin_2 = 18;                       // Начальный индекс текущего столбца.
        end_2 = 25;                         // Конечный индекс текущего столбца.
        begin_3 = 26;                       // Начальный индекс правого столбца.
        end_3 = 34;                         // Конечный индекс правого столбца.
        a_3 = 8;                            // Смещение 1 для правого столбца.
        b_3 = 9;                            // Смещение 2 для правого столбца.
    }

    if (n >= 26 && n <= 34) {               // Если заданная ячейка находится в пятом столбце поля.
        begin_1 = 18;                       // Начальный индекс левого столбца.
        end_1 = 25;                         // Конечный индекс левого столбца.
        a_1 = -9;                           // Смещение 1 для левого столбца.
        b_1 = -8;                           // Смещение 2 для левого столбца.
        begin_2 = 26;                       // Начальный индекс текущего столбца.
        end_2 = 34;                         // Конечный индекс текущего столбца.
        begin_3 = 35;                       // Начальный индекс правого столбца.
        end_3 = 42;                         // Конечный индекс правого столбца.
        a_3 = 8;                            // Смещение 1 для правого столбца.
        b_3 = 9;                            // Смещение 2 для правого столбца.
    }
    if (n >= 35 && n <= 42) {               // Если заданная ячейка находится в шестом столбце поля.
        begin_1 = 26;                       // Начальный индекс левого столбца.
        end_1 = 34;                         // Конечный индекс левого столбца.
        a_1 = -9;                           // Смещение 1 для левого столбца.
        b_1 = -8;                           // Смещение 2 для левого столбца.
        begin_2 = 35;                       // Начальный индекс текущего столбца.
        end_2 = 42;                         // Конечный индекс текущего столбца.
        begin_3 = 43;                       // Начальный индекс правого столбца.
        end_3 = 49;                         // Конечный индекс правого столбца.
        a_3 = 7;                            // Смещение 1 для правого столбца.
        b_3 = 8;                            // Смещение 2 для правого столбца.
    }
    if (n >= 43 && n <= 49) {               // Если заданная ячейка находится в седьмом столбце поля.
        begin_1 = 35;                       // Начальный индекс левого столбца.
        end_1 = 42;                         // Конечный индекс левого столбца.
        a_1 = -8;                           // Смещение 1 для левого столбца.
        b_1 = -7;                           // Смещение 2 для левого столбца.
        begin_2 = 43;                       // Начальный индекс текущего столбца.
        end_2 = 49;                         // Конечный индекс текущего столбца.
        begin_3 = 50;                       // Начальный индекс правого столбца.
        end_3 = 55;                         // Конечный индекс правого столбца.
        a_3 = 6;                            // Смещение 1 для правого столбца.
        b_3 = 7;                            // Смещение 2 для правого столбца.
    }
    if (n >= 50 && n <= 55) {               // Если заданная ячейка находится в восьмом столбце поля.
        begin_1 = 43;                       // Начальный индекс левого столбца.
        end_1 = 49;                         // Конечный индекс левого столбца.
        a_1 = -7;                           // Смещение 1 для левого столбца.
        b_1 = -6;                           // Смещение 2 для левого столбца.
        begin_2 = 50;                       // Начальный индекс текущего столбца.
        end_2 = 55;                         // Конечный индекс текущего столбца.
        begin_3 = 56;                       // Начальный индекс правого столбца.
        end_3 = 60;                         // Конечный индекс правого столбца.
        a_3 = 5;                            // Смещение 1 для правого столбца.
        b_3 = 6;                            // Смещение 2 для правого столбца.
    }
    if (n >= 56 && n <= 60) {               // Если заданная ячейка находится в девятом столбце поля.
        begin_1 = 50;                       // Начальный индекс левого столбца.
        end_1 = 55;                         // Конечный индекс левого столбца.
        a_1 = -6;                           // Смещение 1 для левого столбца.
        b_1 = -5;                           // Смещение 2 для левого столбца.
        begin_2 = 56;                       // Начальный индекс текущего столбца.
        end_2 = 60;                         // Конечный индекс текущего столбца.
    }

    if (begin_1 != end_1) {                                                     //Вычисление ячеек в левом
        if (n + a_1 >= begin_1 && n + a_1 <= end_1)  pV.push_back(n + a_1);     // столбце (если он есть).
        if (n + b_1 >= begin_1 && n + b_1 <= end_1)  pV.push_back(n + b_1);
    }
    if (begin_2 != end_2) {                                                     // Вычисление ячеек в текущем
        if (n - 1 >= begin_2) pV.push_back(n - 1);                              // столбце.
        if (n + 1 <= end_2) pV.push_back(n + 1);
    }
    if (begin_3 != end_3) {                                                     // Вычисление ячеек в правом
        if (n + a_3 >= begin_3 && n + a_3 <= end_3)  pV.push_back(n + a_3);     // столбце (если он есть).
        if (n + b_3 >= begin_3 && n + b_3 <= end_3)  pV.push_back(n + b_3);
    }
    return pV;
}
