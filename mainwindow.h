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
    void cleanQueue();
    bool fillReservationStationAdders(QString op, QString resReg, QString vj, QString vk);
    bool fillReservationStationLoads(QString resReg, QString imm, QString addrReg);
    bool fillReservationStationStores(QString vj, QString imm, QString addrReg);
    bool fillReservationStationMults(QString op, QString resReg, QString vj, QString vk);
    void checkReservationStations();
    void markRegisterBusy(QString reg, QString stationName, QString stationNumber);
    void markMemoryElementBusy(QString address);
    void unmarkMemoryElementBusy(QString address);
    void showRegValues();
    void showResStations();
    void showMemory();
    void resetProcessor();
    void writeOutputMessage(QString mess);

private slots:
    void on_compileButton_clicked();

    void on_clkButton_clicked();

    void on_startButton_clicked();

    void on_resetButton_clicked();

    void on_skipButton_clicked();

    void on_codeTextEdit_textChanged();
private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
