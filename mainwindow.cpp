#include "mainwindow.h"
#include "instruction.h"
#include "reservationstationaddsub.h"
#include "reservationstationload.h"
#include "reservationstationmuldiv.h"
#include "reservationstationstore.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QPainter>

#define NUM_OF_ADDSUB_STATIONS 3
#define NUM_OF_LOAD_STATIONS 5
#define NUM_OF_STORE_STATIONS 5
#define NUM_OF_MULDIV_STATIONS 2

#define ADD_CLK_LATENCY 2
#define SUB_CLK_LATENCY 2
#define LOAD_CLK_LATENCY 2
#define STORE_CLK_LATENCY 2
#define MUL_CLK_LATENCY 10
#define DIV_CLK_LATENCY 40

int algorithmStep = 0;
QStringList instructionsString;
QString code;
int nextBufferInstruction = 0; //koja instrukcija iz grupe svih u kodu je sledeca na redu da se ubaci u bafer
int nextInstruction = 0;
int numberOfInstructions = 0;
QList<Instruction> instructions;
QList<ReservationStationAddSub> stationsAddSub;
QList<ReservationStationLoad> stationsLoad;
QList<ReservationStationMulDiv> stationsMulDiv;
QList<ReservationStationStore> stationsStore;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QPixmap pix(":/img/img/tomasulo.png");

    int w = ui->picture_label->width();
    int h = ui->picture_label->height();
    ui->picture_label->setPixmap(pix.scaled(w,h,Qt::KeepAspectRatio));
    //ui->picture_label->setPixmap(pix.scaled(200,200, Qt::KeepAspectRatio));

    emptyBuffer();

    //create reservation stations
    for (int i = 0; i < NUM_OF_ADDSUB_STATIONS; i++)
    {
        ReservationStationAddSub stAddSub;
        stationsAddSub.append(stAddSub);
    }
    for (int i = 0; i < NUM_OF_LOAD_STATIONS; i++)
    {
        ReservationStationLoad stLoad;
        stationsLoad.append(stLoad);
    }
    for (int i = 0; i < NUM_OF_MULDIV_STATIONS; i++)
    {
        ReservationStationMulDiv stMulDiv;
        stationsMulDiv.append(stMulDiv);
    }
    for (int i = 0; i < NUM_OF_STORE_STATIONS; i++)
    {
        ReservationStationStore stStore;
        stationsStore.append(stStore);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_compileButton_clicked()
{
    compileProgram();
}

void MainWindow::compileProgram()
{
    code = ui->textEdit->toPlainText();

    /********************************************************
    Make compiling correct with 1 or 0 lines of code!!!
    ********************************************************/

    instructionsString = code.split(QLatin1Char('\n'), Qt::SkipEmptyParts);
    numberOfInstructions = instructionsString.size();
    qDebug() << "Number of instructions: " << numberOfInstructions;

    QStringList instructionParts;
    bool correctCode = true;
    //Check format!
    for (int i = 0; i < instructionsString.size(); i++)
    {
        /********************************************************
        INSERT REGULAR EXPRESSIONS!!
        ********************************************************/
        instructionParts = instructionsString[i].split(QLatin1Char(','), Qt::SkipEmptyParts);

        /********************************************************
        Check if input command is in correct format!!!
        example: operations which are not supported, registers which does not exist..
        if command line is not correct mark it red
        set variable correctCode on false if at least one line is not ok
        ********************************************************/

        QList<QTextEdit::ExtraSelection> extraSelections;

        QTextEdit::ExtraSelection textEdit;

        QColor lineColor = QColor(Qt::red).lighter(160);

        textEdit.format.setBackground(lineColor);
        textEdit.format.setProperty(QTextFormat::FullWidthSelection, true);
        textEdit.cursor = QTextCursor();
        textEdit.cursor.clearSelection();
        textEdit.cursor.movePosition(QTextCursor::Start);
        textEdit.cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, 1);
        extraSelections.append(textEdit);

        ui->textEdit->setExtraSelections(extraSelections);
        ui->textEdit->show();
        //ui->textEdit->insertPlainText("Hello");
        //ui->textEdit->append("Anja");
        //qDebug() << "Underlined";
        //ui->textEdit->setTextColor(Qt::red);

        /********************************************************
        Code above is not working.
        Try with changing whole string and print it again
        with append
        ********************************************************/

        if (correctCode)
        {
            //Instruction instr("add", "f1", "f2", "f3");
            Instruction instr(instructionParts[0], instructionParts[1], instructionParts[2], instructionParts[3]);
            instructions.append(instr);
        }
    }
    if (correctCode)
    {
        ui->compile_label->setText("Code is compiled successfuly.");
        ui->startButton->setEnabled(true);
        ui->clkButton->setEnabled(false);
    }
    else
    {
        ui->compile_label->setText("Code contains errors.");
        //clear all created elements of instructions list
        instructions.clear();
    }

}

void MainWindow::on_clkButton_clicked()
{
    //one button click = one clock cycle

    if(nextInstruction < numberOfInstructions) {
        Instruction instr = instructions.at(nextInstruction);

        bool stationNotBusy;
        if (instr.getOp() == "add" || instr.getOp() == "sub")
            stationNotBusy = fillReservationStationAdders(instr.getOp(), instr.getReg2(), instr.getReg3());
        else if (instr.getOp() == "ld")
            stationNotBusy = fillReservationStationLoads(instr.getReg2(), instr.getReg3());
        else if (instr.getOp() == "sd")
            stationNotBusy = fillReservationStationStores(instr.getReg1(), instr.getReg2(), instr.getReg3());
        else
            stationNotBusy = fillReservationStationMults(instr.getOp(), instr.getReg2(), instr.getReg3());

        /********************************************************
        Mark registers which have to get results (this can be done inside the fill rez stat functions)
        markRegisterBusy() or markMemoryElementBusy() (show in memory how element is still not written)
        ********************************************************/

        /********************************************************
        Calcute reservations stations and forward results to everyone (regs and stations)
        checkReservationStations();
        ********************************************************/

        shiftQueue();

        if (stationNotBusy)
            nextInstruction++;
    }
    else
    {
        /********************************************************
        Continue executing, to finish all started instructions
        ********************************************************/
    }

}

void MainWindow::initAlgorithm()
{
    //Fill buffer

    if(instructionsString.size() > 0)
        ui->queue_0->setText(instructionsString[0]);

    if(instructionsString.size() > 1)
        ui->queue_1->setText(instructionsString[1]);

    if(instructionsString.size() > 2)
        ui->queue_2->setText(instructionsString[2]);

    if(instructionsString.size() > 3)
        ui->queue_3->setText(instructionsString[3]);

    if(instructionsString.size() > 4)
        ui->queue_4->setText(instructionsString[4]);

    if(instructionsString.size() > 5)
        ui->queue_5->setText(instructionsString[5]);


    if(numberOfInstructions > 6)
        nextBufferInstruction = 6;
    else
        nextBufferInstruction = numberOfInstructions;
}

void MainWindow::shiftQueue()
{
    QString instruction;

    instruction = ui->queue_1->text();
    ui->queue_0->setText(instruction);

    instruction = ui->queue_2->text();
    ui->queue_1->setText(instruction);

    instruction = ui->queue_3->text();
    ui->queue_2->setText(instruction);

    instruction = ui->queue_4->text();
    ui->queue_3->setText(instruction);

    instruction = ui->queue_5->text();
    ui->queue_4->setText(instruction);

    if(nextBufferInstruction < numberOfInstructions)
    {
        ui->queue_5->setText(instructionsString[nextBufferInstruction]);
        nextBufferInstruction++;
    }
    else
    {
        ui->queue_5->setText("");
    }
}

void MainWindow::on_startButton_clicked()
{
    initAlgorithm();
    ui->startButton->setEnabled(false);
    ui->clkButton->setEnabled(true);
}


void MainWindow::on_resetButton_clicked()
{
    emptyBuffer();
    ui->startButton->setEnabled(true);
    ui->clkButton->setEnabled(false);
    nextInstruction = 0;
}

void MainWindow::emptyBuffer()
{
    ui->queue_5->setText("");
    ui->queue_4->setText("");
    ui->queue_3->setText("");
    ui->queue_2->setText("");
    ui->queue_1->setText("");
    ui->queue_0->setText("");
}

/********************************************************
Following functions should fill labels in reservations stations
It returns true if station is not busy, otherwise false
Pay attention on register values (which values are ready)!!
********************************************************/

bool MainWindow::fillReservationStationAdders(QString op, QString reg2, QString reg3)
{

}

bool MainWindow::fillReservationStationLoads(QString reg2, QString reg3)
{

}

bool MainWindow::fillReservationStationStores(QString reg1, QString reg2, QString reg3)
{

}

bool MainWindow::fillReservationStationMults(QString op, QString reg2, QString reg3)
{

}
