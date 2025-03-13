#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QComboBox>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMainWindow>
#include <QObject>
#include <QProgressBar>
#include <QPushButton>
#include <QSpinBox>
#include <QString>
#include <QVBoxLayout>


QT_BEGIN_NAMESPACE

namespace Ui {
class MainWindow;
}  // namespace Ui

QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

   public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

   private slots:
    void onSpinBoxValueChanged(int value);
    void onListItemClicked(QListWidgetItem *item);
    void onNameEditEnterPressed();
    void onStatusChanged(int index);
    void onListItemDoubleClicked(QListWidgetItem* item);
   private:
    Ui::MainWindow* ui;

    QSpinBox* spinBox;
    QListWidget* listWidget;
    QLabel* nameLabel;
    QLineEdit* nameEdit;
    QListWidgetItem* selectedItem = nullptr;
    QComboBox* comboBox;
    QMap<QListWidgetItem*, int> ticketStatuses;

    void updateItemColor(QListWidgetItem* item);
};
#endif  // MAINWINDOW_H
