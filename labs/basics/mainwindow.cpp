#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    //количество билетов
    spinBox = new QSpinBox(this);
    spinBox->setRange(0, 100);
    spinBox->setValue(0);
    connect(spinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::onSpinBoxValueChanged);

    //поля с билетами
    listWidget = new QListWidget(this);
    connect(listWidget, &QListWidget::itemClicked, this, &MainWindow::onListItemClicked);
    connect(listWidget, &QListWidget::itemDoubleClicked, this, &MainWindow::onListItemDoubleClicked);

    //названия полей
    nameLabel = new QLabel("Имя билета: ", this);
    nameEdit = new QLineEdit(this);
    connect(nameEdit, &QLineEdit::returnPressed, this, &MainWindow::onNameEditEnterPressed);

    //статус изучения
    comboBox = new QComboBox(this);
    comboBox->addItems({"Дефолтный", "Желтый", "Зеленый"});
    connect(comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onStatusChanged);

    QWidget* window = new QWidget(this);
    setCentralWidget(window);
    QVBoxLayout* layout = new QVBoxLayout(window);
    layout->addWidget(spinBox);
    layout->addWidget(listWidget);
    layout->addWidget(nameLabel);
    layout->addWidget(nameEdit);
    layout->addWidget(comboBox);



}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::onSpinBoxValueChanged(int value) {
    listWidget->clear();
    ticketStatuses.clear();
    for (int i = 1; i <= value; ++i) {
        QListWidgetItem* item = new QListWidgetItem("Билет " + QString::number(i), listWidget);
        ticketStatuses[item] = 0;
        updateItemColor(item);
    }
}

void MainWindow::onNameEditEnterPressed() {
    if (selectedItem) {
        QString newName = nameEdit->text().trimmed();
        if (!newName.isEmpty()) {
            selectedItem->setText(newName);
            nameLabel->setText("Имя билета: " + newName);
        }
    }
}

void MainWindow::onStatusChanged(int index) {
    if(selectedItem){
        ticketStatuses[selectedItem] = index;
        updateItemColor(selectedItem);
    }
}

void MainWindow::onListItemDoubleClicked(QListWidgetItem* item) {
    if (item) {
        int currentStatus = ticketStatuses[item];
        if (currentStatus == 0 || currentStatus == 1) {
            ticketStatuses[item] = 2;
        } else {
            ticketStatuses[item] = 1;
        }
        updateItemColor(item);
        comboBox->setCurrentIndex(ticketStatuses[item]);
    }
}

void MainWindow::onListItemClicked(QListWidgetItem* item) {
    if (item) {
        selectedItem = item;
        nameLabel->setText("Имя билета: " + item->text());
        nameEdit->setText(item->text());
    }
}


void MainWindow::updateItemColor(QListWidgetItem* item) {
    if (!item) return;
    int status = ticketStatuses[item];
    switch (status) {
        case 0: item->setBackground(QBrush(Qt::white)); break;
        case 1: item->setBackground(QBrush(Qt::yellow)); break;
        case 2: item->setBackground(QBrush(Qt::green)); break;
    }

    listWidget->update();
}
