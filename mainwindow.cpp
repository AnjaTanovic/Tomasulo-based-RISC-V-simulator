#include "mainwindow.h"
#include "instruction.h"
#include "reservationstationaddsub.h"
#include "reservationstationload.h"
#include "reservationstationmuldiv.h"
#include "reservationstationstore.h"
#include "register.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QPainter>
#include <QRegularExpression>

#define NUM_OF_ADDSUB_STATIONS 3
#define NUM_OF_LOAD_STATIONS 5
#define NUM_OF_STORE_STATIONS 5
#define NUM_OF_MULDIV_STATIONS 2

#define NUM_OF_REGISTERS 4

#define QUEUE_SIZE 6

#define ADD_CLK_LATENCY 2
#define SUB_CLK_LATENCY 2
#define LOAD_CLK_LATENCY 2
#define STORE_CLK_LATENCY 2
#define MUL_CLK_LATENCY 10
#define DIV_CLK_LATENCY 40

#define ADDS_STATIONS_NAME "adds_"
#define LOADS_STATIONS_NAME "loads_"
#define STORES_STATIONS_NAME "stores_"
#define MULTS_STATIONS_NAME "mults_"

int algorithmStep = 0;
QStringList instructionsString;
QString code;
QString memText;
int nextQueueInstruction = 0; //which instruction from code is next to be inserted in queue
int nextInstruction = 0;
int numberOfInstructions = 0;
QList<Instruction> instructions;
QList<ReservationStationAddSub> stationsAddSub;
QList<ReservationStationLoad> stationsLoad;
QList<ReservationStationMulDiv> stationsMulDiv;
QList<ReservationStationStore> stationsStore;
QList<Register> registers;

/********************************************************
compile is not real compiling, change word
********************************************************/

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

    //create registers
    for (int i = 0; i < NUM_OF_REGISTERS; i++)
    {
        Register reg;
        registers.append(reg);
    }

    cleanFields();
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
    code = ui->codeTextEdit->toPlainText();
    nextInstruction = 0;
    instructions.clear();

    /********************************************************
    Make compiling correct with 1 or 0 lines of code!!!
    ********************************************************/

    instructionsString = code.split(QLatin1Char('\n'), Qt::SkipEmptyParts);
    numberOfInstructions = instructionsString.size();
    qDebug() << "Number of instructions: " << numberOfInstructions;

    bool correctCode = true;
    //Check format!
    for (int i = 0; i < instructionsString.size(); i++)
    {
        QRegularExpression regularInstruction1("^\\s*(add|sub|mul|div)\\s+([fF][0-3]),\\s*([fF][0-3]),\\s*([fF][0-3])\\s*$");
        QRegularExpression regularInstruction2("^\\s*(sd|ld)\\s+([fF][0-3]),\\s*([0-9]+),\\s*([fF][0-3])\\s*$");
        QRegularExpression regularInstruction3("^\\s*(sd|ld)\\s+([fF][0-3]),\\s*([0-9]+)[(]\\s*([fF][0-3])\\s*[)]\\s*$");

        if (regularInstruction1.match(instructionsString[i]).hasMatch())
        {

            Instruction instr(regularInstruction1.match(instructionsString[i]).captured(1),
                              regularInstruction1.match(instructionsString[i]).captured(2),
                              regularInstruction1.match(instructionsString[i]).captured(3),
                              regularInstruction1.match(instructionsString[i]).captured(4));
            instructions.append(instr);

            qDebug() << "Instruction number " + QString::number(i) + " is correct.";
        }
        else if (regularInstruction2.match(instructionsString[i]).hasMatch())
        {
            Instruction instr(regularInstruction2.match(instructionsString[i]).captured(1),
                              regularInstruction2.match(instructionsString[i]).captured(2),
                              regularInstruction2.match(instructionsString[i]).captured(3),
                              regularInstruction2.match(instructionsString[i]).captured(4));
            instructions.append(instr);

            qDebug() << "Instruction number " + QString::number(i) + " is correct.";
        }
        else if (regularInstruction3.match(instructionsString[i]).hasMatch())
        {
            Instruction instr(regularInstruction3.match(instructionsString[i]).captured(1),
                              regularInstruction3.match(instructionsString[i]).captured(2),
                              regularInstruction3.match(instructionsString[i]).captured(3),
                              regularInstruction3.match(instructionsString[i]).captured(4));
            instructions.append(instr);

            qDebug() << "Instruction number " + QString::number(i) + " is correct.";
        }
        else
        {
            qDebug() << "Instruction number " + QString::number(i) + " is NOT correct!!!";
            correctCode = false;
        }


        /********************************************************
        If command line is not correct mark it red
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

        ui->codeTextEdit->setExtraSelections(extraSelections);
        ui->codeTextEdit->show();
        //ui->codeTextEdit->insertPlainText("Hello");
        //ui->codeTextEdit->append("Anja");
        //qDebug() << "Underlined";
        //ui->codeTextEdit->setTextColor(Qt::red);

        /********************************************************
        Code above is not working.
        Try with changing whole string and print it again
        with append
        ********************************************************/
    }
    if (correctCode)
    {
        ui->compile_label->setText("Code is compiled successfuly.");
        ui->startButton->setEnabled(true);
        ui->clkButton->setEnabled(false);
        ui->resetButton->setEnabled(true);
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
            stationNotBusy = fillReservationStationAdders(instr.getOp(), instr.getReg1(), instr.getReg2(), instr.getReg3());
        else if (instr.getOp() == "ld")
            stationNotBusy = fillReservationStationLoads(instr.getReg1(), instr.getReg2(), instr.getReg3());
        else if (instr.getOp() == "sd")
            stationNotBusy = fillReservationStationStores(instr.getReg1(), instr.getReg2(), instr.getReg3());
        else
            stationNotBusy = fillReservationStationMults(instr.getOp(), instr.getReg1(), instr.getReg2(), instr.getReg3());

        /********************************************************
        Mark registers which have to get results (this can be done inside the fill rez stat functions)
        markRegisterBusy() or markMemoryElementBusy() (show in memory how element is still not written)
        use registers[i].setBusy(true);
        ********************************************************/

        //markMemoryElementBusy(); in store function!!

        /********************************************************
        Calculate reservations stations and forward results to everyone (regs and stations)
        checkReservationStations();
        ********************************************************/

        if (stationNotBusy)
        {
            nextInstruction++;
            shiftQueue();
        }
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
    //Fill queue

    /********************************************************
    put this in for loop
    ********************************************************/

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
        nextQueueInstruction = 6;
    else
        nextQueueInstruction = numberOfInstructions;
}

void MainWindow::shiftQueue()
{
    QString instruction;

    /********************************************************
    put this in for loop
    ********************************************************/

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

    if(nextQueueInstruction < numberOfInstructions)
    {
        ui->queue_5->setText(instructionsString[nextQueueInstruction]);
        nextQueueInstruction++;
    }
    else
    {
        ui->queue_5->setText("");
    }
}

void MainWindow::on_startButton_clicked()
{
    resetStationsAndRegisters();
    cleanFields();
    memText = "";
    initAlgorithm();
    ui->startButton->setEnabled(false);
    ui->clkButton->setEnabled(true);
}


void MainWindow::on_resetButton_clicked()
{
    cleanFields();
    nextInstruction = 0;
    memText = "";
    code = "";
    ui->startButton->setEnabled(true);
    ui->clkButton->setEnabled(false);
}

void MainWindow::cleanFields()
{
    QString labelName;
    QLabel* targetLabel;

    for (int i = 0; i < QUEUE_SIZE; i++)
    {
        labelName = "queue_" + QString::number(i);
        targetLabel = ui->addsOp_0->parentWidget()->findChild<QLabel*>(labelName);
        targetLabel->setText("");
    }

    for (int i = 0; i < NUM_OF_ADDSUB_STATIONS; i++)
    {
        labelName = "addsOp_" + QString::number(i);
        targetLabel = ui->addsOp_0->parentWidget()->findChild<QLabel*>(labelName);
        targetLabel->setText("");
        labelName = "addsVj_" + QString::number(i);
        targetLabel = ui->addsOp_0->parentWidget()->findChild<QLabel*>(labelName);
        targetLabel->setText("");
        labelName = "addsVk_" + QString::number(i);
        targetLabel = ui->addsOp_0->parentWidget()->findChild<QLabel*>(labelName);
        targetLabel->setText("");
    }

    for (int i = 0; i < NUM_OF_LOAD_STATIONS; i++)
    {
        labelName = "load_" + QString::number(i);
        targetLabel = ui->addsOp_0->parentWidget()->findChild<QLabel*>(labelName);
        targetLabel->setText("");
    }

    for (int i = 0; i < NUM_OF_MULDIV_STATIONS; i++)
    {
        labelName = "mulsOp_" + QString::number(i);
        targetLabel = ui->addsOp_0->parentWidget()->findChild<QLabel*>(labelName);
        targetLabel->setText("");
        labelName = "mulsVj_" + QString::number(i);
        targetLabel = ui->addsOp_0->parentWidget()->findChild<QLabel*>(labelName);
        targetLabel->setText("");
        labelName = "mulsVk_" + QString::number(i);
        targetLabel = ui->addsOp_0->parentWidget()->findChild<QLabel*>(labelName);
        targetLabel->setText("");
    }

    for (int i = 0; i < NUM_OF_STORE_STATIONS; i++)
    {
        labelName = "storeReg_" + QString::number(i);
        targetLabel = ui->addsOp_0->parentWidget()->findChild<QLabel*>(labelName);
        targetLabel->setText("");
        labelName = "storeAddr_" + QString::number(i);
        targetLabel = ui->addsOp_0->parentWidget()->findChild<QLabel*>(labelName);
        targetLabel->setText("");
    }

    for (int i = 0; i < NUM_OF_REGISTERS; i++)
    {
        labelName = "f_" + QString::number(i);
        targetLabel = ui->addsOp_0->parentWidget()->findChild<QLabel*>(labelName);
        targetLabel->setText(QString::number(registers[i].getValue()));
    }

    ui->memTextEdit->setText("");
}

/********************************************************
Following functions should fill labels in reservations stations
Pay attention on register values (which values are ready)!!
********************************************************/

bool MainWindow::fillReservationStationAdders(QString op, QString resReg, QString vj, QString vk)
{
    bool notBusy = false; //true if at least one of stations is not busy, otherwise false (all busy)
    QString labelName;
    bool regBusy;
    QLabel* targetLabel;

    for (int i = 0; i < NUM_OF_ADDSUB_STATIONS; i++)
    {
        if (stationsAddSub[i].getBusy() == false)
        {
            stationsAddSub[i].setBusy(true);
            stationsAddSub[i].setOp(op);
            stationsAddSub[i].setAtCycle(0); //cycle ++ when both operands are ready (in check res station func)

            //show instruction on gui
            //OPERATION
            labelName = "addsOp_" + QString::number(i);
            targetLabel = ui->addsOp_0->parentWidget()->findChild<QLabel*>(labelName);
            targetLabel->setText(op);

            //OPERANDS

            int regNumberJ = 0;
            int regNumberK = 0;
            QRegularExpression regularRegister("^[fF]([0-3])$");
            if (regularRegister.match(vj).hasMatch() && regularRegister.match(vk).hasMatch())
            {
                regNumberJ = (regularRegister.match(vj).captured(1)).toInt();
                regNumberK = (regularRegister.match(vk).captured(1)).toInt();
            }
            else
                qDebug() << "Problem with registers in instructions.";

            regBusy = registers[regNumberJ].getBusy();
            labelName = "addsVj_" + QString::number(i);
            targetLabel = ui->addsOp_0->parentWidget()->findChild<QLabel*>(labelName);
            if (regBusy)
            {
                //write only reg name (f1, f2 ...)
                targetLabel->setText(vj);
            }
            else
            {
                //write value
                targetLabel->setText(QString::number(registers[regNumberJ].getValue()));
                stationsAddSub[i].setVj(QString::number(registers[regNumberJ].getValue()));
            }

            labelName = "addsVk_" + QString::number(i);
            targetLabel = ui->addsOp_0->parentWidget()->findChild<QLabel*>(labelName);
            regBusy = registers[regNumberK].getBusy();
            if (regBusy)
            {
                //write only reg name (f1, f2 ...)
                targetLabel->setText(vk);
            }
            else
            {
                //write value
                targetLabel->setText(QString::number(registers[regNumberK].getValue()));
                stationsAddSub[i].setVk(QString::number(registers[regNumberK].getValue()));
            }

            /********************************************************
            regs can be replaced with immediate (do it later)
            ********************************************************/

            notBusy = true;

            QString stationName = ADDS_STATIONS_NAME + QString::number(i);
            markRegisterBusy(resReg, stationName);

            break;
        }
    }
    return notBusy;
}

bool MainWindow::fillReservationStationLoads(QString resReg, QString imm, QString addrReg)
{
    bool notBusy = false; //true if at least one of stations is not busy, otherwise false (all busy)
    QString labelName;
    bool regBusy;
    QLabel* targetLabel;

    for (int i = 0; i < NUM_OF_LOAD_STATIONS; i++)
    {
        if (stationsLoad[i].getBusy() == false)
        {
            stationsLoad[i].setBusy(true);
            stationsLoad[i].setAddrReg(addrReg);
            stationsLoad[i].setImm(imm);
            stationsLoad[i].setAtCycle(0); //cycle ++ when address is ready (in check res station func)

            //show instruction on gui

            //ADDRESS REGISTER

            int regNumber = 0;
            QRegularExpression regularRegister("^[fF]([0-3])$");
            if (regularRegister.match(addrReg).hasMatch())
            {
                regNumber = (regularRegister.match(addrReg).captured(1)).toInt();
            }
            else
                qDebug() << "Problem with registers in instructions.";

            regBusy = registers[regNumber].getBusy();
            labelName = "load_" + QString::number(i);
            targetLabel = ui->addsOp_0->parentWidget()->findChild<QLabel*>(labelName);
            if (regBusy)
            {
                //write reg name
                targetLabel->setText(imm + "(" + addrReg + ")");
            }
            else
            {
                //calculate address and write value
                stationsLoad[i].setAddr(imm.toInt() + registers[regNumber].getValue());
                targetLabel->setText(QString::number(stationsLoad[i].getAddr()));
            }

            notBusy = true;

            QString stationName = LOADS_STATIONS_NAME + QString::number(i);
            markRegisterBusy(resReg, stationName);

            break;
        }
    }
    return notBusy;
}

bool MainWindow::fillReservationStationStores(QString vj, QString imm, QString addrReg)
{
    bool notBusy = false; //true if at least one of stations is not busy, otherwise false (all busy)
    QString labelName;
    bool regBusy;
    QLabel* targetLabel;

    for (int i = 0; i < NUM_OF_STORE_STATIONS; i++)
    {
        if (stationsStore[i].getBusy() == false)
        {
            stationsStore[i].setBusy(true);
            stationsStore[i].setAddrReg(addrReg);
            stationsStore[i].setImm(imm);
            stationsStore[i].setAtCycle(0); //cycle ++ when address and register are ready (in check res station func)

            //show instruction on gui

            //ADDRESS REGISTER AND REGISTER FOR STORE

            int regNumber = 0;
            int regNumberJ = 0;
            QRegularExpression regularRegister("^[fF]([0-3])$");
            if (regularRegister.match(addrReg).hasMatch() && regularRegister.match(vj).hasMatch())
            {
                regNumber = (regularRegister.match(addrReg).captured(1)).toInt();
                regNumberJ = (regularRegister.match(vj).captured(1)).toInt();
            }
            else
                qDebug() << "Problem with registers in instructions.";

            regBusy = registers[regNumber].getBusy();
            labelName = "storeAddr_" + QString::number(i);
            targetLabel = ui->addsOp_0->parentWidget()->findChild<QLabel*>(labelName);
            if (regBusy)
            {
                //write reg name
                targetLabel->setText(imm + "(" + addrReg + ")");
                markMemoryElementBusy(imm + "(" + addrReg + ")");
            }
            else
            {
                //calculate address and write value
                stationsStore[i].setAddr(imm.toInt() + registers[regNumber].getValue());
                targetLabel->setText(QString::number(stationsStore[i].getAddr()));
                markMemoryElementBusy(QString::number(stationsStore[i].getAddr()));
            }
            regBusy = registers[regNumberJ].getBusy();
            labelName = "storeReg_" + QString::number(i);
            targetLabel = ui->addsOp_0->parentWidget()->findChild<QLabel*>(labelName);
            if (regBusy)
            {
                //write only reg name (f1, f2 ...)
                targetLabel->setText(vj);
            }
            else
            {
                //write value
                targetLabel->setText(QString::number(registers[regNumberJ].getValue()));
                stationsStore[i].setVj(QString::number(registers[regNumberJ].getValue()));
            }

            notBusy = true;

            break;
        }
    }
    return notBusy;
}

bool MainWindow::fillReservationStationMults(QString op, QString resReg, QString vj, QString vk)
{
    bool notBusy = false; //true if at least one of stations is not busy, otherwise false (all busy)
    QString labelName;
    bool regBusy;
    QLabel* targetLabel;

    for (int i = 0; i < NUM_OF_MULDIV_STATIONS; i++)
    {
        if (stationsMulDiv[i].getBusy() == false)
        {
            stationsMulDiv[i].setBusy(true);
            stationsMulDiv[i].setOp(op);
            stationsMulDiv[i].setAtCycle(0); //cycle ++ when both operands are ready (in check res station func)

            //show instruction on gui
            //OPERATION
            labelName = "mulsOp_" + QString::number(i);
            targetLabel = ui->addsOp_0->parentWidget()->findChild<QLabel*>(labelName);
            targetLabel->setText(op);

            //OPERANDS

            int regNumberJ = 0;
            int regNumberK = 0;
            QRegularExpression regularRegister("^[fF]([0-3])$");
            if (regularRegister.match(vj).hasMatch() && regularRegister.match(vk).hasMatch())
            {
                regNumberJ = (regularRegister.match(vj).captured(1)).toInt();
                regNumberK = (regularRegister.match(vk).captured(1)).toInt();
            }
            else
                qDebug() << "Problem with registers in instructions.";

            regBusy = registers[regNumberJ].getBusy();
            labelName = "mulsVj_" + QString::number(i);
            targetLabel = ui->addsOp_0->parentWidget()->findChild<QLabel*>(labelName);
            if (regBusy)
            {
                //write only reg name (f1, f2 ...)
                targetLabel->setText(vj);
            }
            else
            {
                //write value
                targetLabel->setText(QString::number(registers[regNumberJ].getValue()));
                stationsMulDiv[i].setVj(QString::number(registers[regNumberJ].getValue()));
            }

            labelName = "mulsVk_" + QString::number(i);
            targetLabel = ui->addsOp_0->parentWidget()->findChild<QLabel*>(labelName);
            regBusy = registers[regNumberK].getBusy();
            if (regBusy)
            {
                //write only reg name (f1, f2 ...)
                targetLabel->setText(vk);
            }
            else
            {
                //write value
                targetLabel->setText(QString::number(registers[regNumberK].getValue()));
                stationsMulDiv[i].setVk(QString::number(registers[regNumberK].getValue()));
            }


            /********************************************************
            regs can be replaced with immediate (do it later)
            ********************************************************/

            notBusy = true;

            QString stationName = MULTS_STATIONS_NAME + QString::number(i);
            markRegisterBusy(resReg, stationName);

            break;
        }
    }
    return notBusy;
}

void MainWindow::markRegisterBusy(QString reg, QString station)
{
    int regNumber = 0;
    QRegularExpression regularRegister("^[fF]([0-3])$");
    if (regularRegister.match(reg).hasMatch())
    {
        regNumber = (regularRegister.match(reg).captured(1)).toInt();
    }
    else
        qDebug() << "Problem with registers in instructions.";

    registers[regNumber].setBusy(true);
    registers[regNumber].setStation(station);
}

void MainWindow::unmarkRegisterBusy(QString reg)
{
    int regNumber = 0;
    QRegularExpression regularRegister("^[fF]([0-3])$");
    if (regularRegister.match(reg).hasMatch())
    {
        regNumber = (regularRegister.match(reg).captured(1)).toInt();
    }
    else
        qDebug() << "Problem with registers in instructions.";

    registers[regNumber].setBusy(false);
    registers[regNumber].setStation("");
}

void MainWindow::markMemoryElementBusy(QString address)
{
    //only load needs to check ih memory element is busy (called before read method of class ReservationStationLoad)
    //if it is busy, wait untill element is stable, and then load it in register
    memText.append("Location " + address + " is busy.\n");
    ui->memTextEdit->setText(memText);
}

void MainWindow::unmarkMemoryElementBusy(QString address)
{
    memText = ui->memTextEdit->toPlainText();

    QStringList memString = memText.split(QLatin1Char('\n'), Qt::SkipEmptyParts);
    QString newMemText;
    for (int i = 0; i < memString.size(); i++)
    {
        if (memString[i].compare("Location " + address + " is busy.") == 0)
        {
            newMemText.append(memString[i]);
        }
    }
    memText = newMemText;
    ui->memTextEdit->setText(memText);
}

void MainWindow::resetStationsAndRegisters()
{
    //reset stations
    for (int i = 0; i < NUM_OF_ADDSUB_STATIONS; i++)
    {
        stationsAddSub[i].setBusy(false);
        stationsAddSub[i].setOp("");
        stationsAddSub[i].setVj(0);
        stationsAddSub[i].setVk(0);
        stationsAddSub[i].setAtCycle(0);
    }
    for (int i = 0; i < NUM_OF_LOAD_STATIONS; i++)
    {
        stationsLoad[i].setBusy(false);
        stationsLoad[i].setAddr(0);
        stationsLoad[i].setAtCycle(0);
    }
    for (int i = 0; i < NUM_OF_MULDIV_STATIONS; i++)
    {
        stationsMulDiv[i].setBusy(false);
        stationsMulDiv[i].setOp("");
        stationsMulDiv[i].setVj(0);
        stationsMulDiv[i].setVk(0);
        stationsMulDiv[i].setAtCycle(0);
    }
    for (int i = 0; i < NUM_OF_STORE_STATIONS; i++)
    {
        stationsStore[i].setBusy(false);
        stationsStore[i].setAddr(0);
        stationsStore[i].setVj(0);
        stationsStore[i].setAtCycle(0);
    }

    //reset registers
    for (int i = 0; i < NUM_OF_REGISTERS; i++)
    {
        registers[i].setBusy(false);
        registers[i].setValue(0);
    }
}
