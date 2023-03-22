#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void compileProgram();
    void initAlgorithm();
    void shiftQueue();
    void cleanFields();
    bool fillReservationStationAdders(QString op, QString vj, QString vk);
    bool fillReservationStationLoads(QString addrReg, QString imm);
    bool fillReservationStationStores(QString vj, QString addrReg, QString imm);
    bool fillReservationStationMults(QString op, QString vj, QString vk);
    void resetStationsAndRegisters();

private slots:
    void on_compileButton_clicked();

    void on_clkButton_clicked();

    void on_startButton_clicked();

    void on_resetButton_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
