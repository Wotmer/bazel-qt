#include "mainwindow.h"
#include "merchwindow.h"
#include "ticketwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPixmap>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QFileInfo>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), isPilotsView(true)
{
    // Основной виджет
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

            // Фоновое изображение
    backgroundLabel = new QLabel(centralWidget);
    backgroundPixmap = QPixmap(":/images/f1_background.png");
    backgroundLabel->setPixmap(backgroundPixmap);
    backgroundLabel->setScaledContents(true);
    backgroundLabel->lower();

            // Основной layout
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

            // Центральный layout
    QHBoxLayout *hLayout = new QHBoxLayout();

            // Левая часть: карта гран-при или мерч гонщика
    imageLabel = new QLabel(this);
    imageLabel->setAlignment(Qt::AlignCenter);
    imageLabel->setPixmap(QPixmap(":/images/f1_logo.png")); // Логотип F1 по умолчанию
    imageLabel->setStyleSheet("border: 2px solid gray;"); // Серая граница
    hLayout->addWidget(imageLabel, 3); // 3/4 ширины

            // Правая часть: список гонщиков или трасс
    QVBoxLayout *rightLayout = new QVBoxLayout();

            // Список гонщиков
    listWidget = new QListWidget(this);
    listWidget->setIconSize(QSize(200, 100)); // Увеличиваем размер иконок
    listWidget->setViewMode(QListWidget::IconMode); // Режим иконок
    setupPilots(); // Заполняем список пилотов
    connect(listWidget, &QListWidget::itemDoubleClicked, this, &MainWindow::handleDoubleClick);

            // Кнопка переключения
    switchButton = new QPushButton("Показать трассы", this);
    connect(switchButton, &QPushButton::clicked, this, &MainWindow::switchView);

            // Label для информации о трассе
    trackInfoLabel = new QLabel("Информация о трассе будет здесь", this);

            // Label для расписания
    scheduleLabel = new QLabel(this);
    setupSchedule(); // Заполняем расписание

            // Календарь в правом нижнем углу
    calendarWidget = new QCalendarWidget(this);

    rightLayout->addWidget(listWidget);
    rightLayout->addWidget(switchButton);
    rightLayout->addWidget(trackInfoLabel);
    rightLayout->addWidget(scheduleLabel);
    rightLayout->addWidget(calendarWidget);

    hLayout->addLayout(rightLayout, 1); // 1/4 ширины
    mainLayout->addLayout(hLayout);
}

MainWindow::~MainWindow() {}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    // Масштабируем фоновое изображение при изменении размера окна
    backgroundLabel->setPixmap(backgroundPixmap.scaled(event->size(), Qt::KeepAspectRatioByExpanding));
    backgroundLabel->setGeometry(0, 0, event->size().width(), event->size().height());
    QMainWindow::resizeEvent(event);
}

void MainWindow::updateLeftView(const QString &itemName)
{
    if (isPilotsView) {
        // Если выбран гонщик, показываем его мерч
        if (itemName == "Mercedes") {
            imageLabel->setPixmap(QPixmap(":/images/mercedes.jpg"));
        } else if (itemName == "Red Bull") {
            imageLabel->setPixmap(QPixmap(":/images/red_bull.jpg"));
        } else if (itemName == "Ferrari") {
            imageLabel->setPixmap(QPixmap(":/images/ferrari.jpg"));
        } else if (itemName == "McLaren") {
            imageLabel->setPixmap(QPixmap(":/images/mclaren.jpg"));
        } else if (itemName == "Alpine") {
            imageLabel->setPixmap(QPixmap(":/images/alpine.jpg"));
        } else if (itemName == "Aston Martin") {
            imageLabel->setPixmap(QPixmap(":/images/aston_martin.jpg"));
        } else if (itemName == "Haas") {
            imageLabel->setPixmap(QPixmap(":/images/haas.jpg"));
        } else if (itemName == "Racing Bulls") {
            imageLabel->setPixmap(QPixmap(":/images/racing_bulls.jpg"));
        } else if (itemName == "Williams") {
            imageLabel->setPixmap(QPixmap(":/images/willams.jpg"));
        } else if (itemName == "Sauber") {
            imageLabel->setPixmap(QPixmap(":/images/sauber.jpg"));
        }
    } else {
        // Если выбрана трасса, показываем её карту
        if (itemName == "Spa") {
            imageLabel->setPixmap(QPixmap(":/images/spa.jpeg"));
        } else if (itemName == "Monza") {
            imageLabel->setPixmap(QPixmap(":/images/monza.jpeg"));
        } else if (itemName == "Montreal") {
            imageLabel->setPixmap(QPixmap(":/images/montreal.jpeg"));
        } else if (itemName == "Melbourne") {
            imageLabel->setPixmap(QPixmap(":/images/melbourne.jpeg"));
        } else if (itemName == "Monaco") {
            imageLabel->setPixmap(QPixmap(":/images/monaco.jpeg"));
        } else {
            imageLabel->setPixmap(QPixmap(":/images/f1_logo.png")); // Логотип F1 по умолчанию
        }
    }
}

void MainWindow::handleDoubleClick(QListWidgetItem *item)
{
    QString itemName = item->text();
    updateLeftView(itemName); // Обновляем левую часть

    if (isPilotsView) {
        // Открываем окно заказа мерча для гонщика
        openMerchWindow(item);
    } else {
        // Открываем окно покупки билетов для трассы
        openTicketWindow(item);
    }
}

void MainWindow::openTicketWindow(QListWidgetItem *item)
{
    QString trackName = item->text();
    QDate eventDate;

            // Устанавливаем даты гран-при для каждой трассы
    if (trackName == "Spa") {
        eventDate = QDate(2024, 7, 28);
    } else if (trackName == "Monza") {
        eventDate = QDate(2024, 9, 1);
    } else if (trackName == "Montreal") {
        eventDate = QDate(2024, 6, 9);
    } else if (trackName == "Melbourne") {
        eventDate = QDate(2024, 3, 24);
    } else if (trackName == "Monaco") {
        eventDate = QDate(2024, 5, 26);
    } else {
        eventDate = QDate::currentDate().addDays(30); // Пример для других трасс
    }

            // Открываем окно покупки билетов
    TicketWindow *ticketWindow = new TicketWindow(trackName, eventDate, this);
    ticketWindow->exec();
}

void MainWindow::setupPilots()
{
    listWidget->clear();
    QListWidgetItem *item1 = new QListWidgetItem(QIcon(":/images/mercedes.jpg"), "");
    QListWidgetItem *item2 = new QListWidgetItem(QIcon(":/images/red_bull.jpg"), "");
    QListWidgetItem *item3 = new QListWidgetItem(QIcon(":/images/ferrari.jpg"), "");
    QListWidgetItem *item4 = new QListWidgetItem(QIcon(":/images/mclaren.jpg"), "");
    QListWidgetItem *item5 = new QListWidgetItem(QIcon(":/images/alpine.jpg"), "");
    QListWidgetItem *item6 = new QListWidgetItem(QIcon(":/images/aston_martin.jpg"), "");
    QListWidgetItem *item7 = new QListWidgetItem(QIcon(":/images/haas.jpg"), "");
    QListWidgetItem *item8 = new QListWidgetItem(QIcon(":/images/racing_bulls.jpg"), "");
    QListWidgetItem *item9 = new QListWidgetItem(QIcon(":/images/willams.jpg"), "");
    QListWidgetItem *item10 = new QListWidgetItem(QIcon(":/images/sauber.jpg"), "");

    listWidget->addItem(item1);
    listWidget->addItem(item2);
    listWidget->addItem(item3);
    listWidget->addItem(item4);
    listWidget->addItem(item5);
    listWidget->addItem(item6);
    listWidget->addItem(item7);
    listWidget->addItem(item8);
    listWidget->addItem(item9);
    listWidget->addItem(item10);
}

void MainWindow::setupTracks()
{
    listWidget->clear();
    QListWidgetItem *item1 = new QListWidgetItem(QIcon(":/images/spa.jpeg"), "");
    QListWidgetItem *item2 = new QListWidgetItem(QIcon(":/images/monza.jpeg"), "");
    QListWidgetItem *item3 = new QListWidgetItem(QIcon(":/images/montreal.jpeg"), "");
    QListWidgetItem *item4 = new QListWidgetItem(QIcon(":/images/melbourne.jpeg"), "");
    QListWidgetItem *item5 = new QListWidgetItem(QIcon(":/images/monaco.jpeg"), "");

    listWidget->addItem(item1);
    listWidget->addItem(item2);
    listWidget->addItem(item3);
    listWidget->addItem(item4);
    listWidget->addItem(item5);
}

void MainWindow::setupSchedule()
{
    scheduleLabel->setText("Расписание заездов:\n"
        "Melbourne: 24 марта 2024\n"
        "Monaco: 26 мая 2024\n"
        "Montreal: 9 июня 2024\n"
        "Spa: 28 июля 2024\n"
        "Monza: 1 сентября 2024\n");
}

void MainWindow::switchView()
{
    if (isPilotsView) {
        setupTracks(); // Заполняем список трасс
        switchButton->setText("Показать пилотов");
        imageLabel->setPixmap(QPixmap(":/images/f1_logo.png")); // Логотип F1
    } else {
        // Переключаемся на пилотов
        setupPilots(); // Заполняем список пилотов
        switchButton->setText("Показать трассы");
    }
    isPilotsView = !isPilotsView;
}

void MainWindow::showDriverImage(QListWidgetItem *item)
{
    QString driverName = item->text();
    if (driverName == "Льюис Хэмилтон") {
        imageLabel->setPixmap(QPixmap(":/images/pilot1.png"));
    } else if (driverName == "Макс Ферстаппен") {
        imageLabel->setPixmap(QPixmap(":/images/pilot2.png"));
    }
}

void MainWindow::updateTrackInfo(int index)
{
    QString trackName = comboBox->itemText(index);
    trackInfoLabel->setText("Информация о трассе: " + trackName);
    updateLeftView(trackName); // Обновляем левую часть
}

void MainWindow::openMerchWindow(QListWidgetItem *item)
{
    MerchWindow *merchWindow = new MerchWindow(item->text(), this);
    merchWindow->exec(); // Открываем окно модально
}
