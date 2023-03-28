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

#define MEMORY_SIZE 1000

#define QUEUE_SIZE 6

#define ADD_CLK_LATENCY 2
#define SUB_CLK_LATENCY 2
#define LOAD_CLK_LATENCY 2
#define STORE_CLK_LATENCY 2
#define MUL_CLK_LATENCY 10
#define DIV_CLK_LATENCY 40

int clkCycle = 0;
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
int memory[MEMORY_SIZE];
bool memoryBusy[MEMORY_SIZE];

/********************************************************
compile is not real compiling, change word
********************************************************/

/********************************************************
Choose whether instructions use integer or floating point
values (names of registers and instructions are different)
Draw a diagram according to the decision.
********************************************************/

/********************************************************
Decide what app should do if divison by zero occurs
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

    //create memory
    ui->memoryTable->setColumnCount(2);
    ui->memoryTable->setRowCount(MEMORY_SIZE);
    ui->memoryTable->verticalHeader()->setVisible(false);
    ui->memoryTable->horizontalHeader()->setVisible(false);
    ui->memoryTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    for (int i = 0; i < MEMORY_SIZE; i++)
    {
        memory[i] = 0;
        memoryBusy[i] = false;
        QTableWidgetItem *addr= new QTableWidgetItem(QString::number(i) + " :");
        QTableWidgetItem *data= new QTableWidgetItem("0");
        ui->memoryTable->setItem(i, 0, addr);
        ui->memoryTable->setItem(i, 1, data);
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
    ui->startButton->setEnabled(false);
    ui->clkButton->setEnabled(false);
    ui->skipButton->setEnabled(false);
    ui->resetButton->setEnabled(false);
    ui->clkLabel->setText("");

    instructionsString = code.split(QLatin1Char('\n'), Qt::SkipEmptyParts);
    numberOfInstructions = instructionsString.size();
    qDebug() << "Number of instructions: " << numberOfInstructions;

    bool correctCode = true;
    //Checking format
    for (int i = 0; i < instructionsString.size(); i++)
    {
        QRegularExpression regularInstruction1("^\\s*(add|sub|mul|div|ADD|SUB|MUL|DIV)\\s+([fF][0-3])\\s*,\\s*([fF][0-3])\\s*,\\s*([fF][0-3])\\s*$");
        QRegularExpression regularInstruction2("^\\s*(sd|ld|SD|LD)\\s+([fF][0-3])\\s*,\\s*([0-9]+)\\s*,\\s*([fF][0-3])\\s*$");
        QRegularExpression regularInstruction3("^\\s*(sd|ld|SD|LD)\\s+([fF][0-3])\\s*,\\s*([0-9]+)[(]\\s*([fF][0-3])\\s*[)]\\s*$");
        QRegularExpression regularInstruction4("^\\s*(addi|ADDI)\\s+([fF][0-3])\\s*,\\s*([fF][0-3])\\s*,\\s*([0-9]+)\\s*$");

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
        else if (regularInstruction4.match(instructionsString[i]).hasMatch())
        {
            Instruction instr("addi",
                              regularInstruction4.match(instructionsString[i]).captured(2),
                              regularInstruction4.match(instructionsString[i]).captured(3),
                              regularInstruction4.match(instructionsString[i]).captured(4));
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
        if (numberOfInstructions > 0)
        {
            ui->compile_label->setText("Code is compiled successfuly.");
            ui->startButton->setEnabled(true);
            ui->clkButton->setEnabled(false);
            ui->skipButton->setEnabled(false);
            ui->resetButton->setEnabled(true);
        }
        else
        {
            ui->compile_label->setText("You have to enter the program code before compiling.");
        }
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
    ui->clkLabel->setText("Clock cycle: " + QString::number(clkCycle + 1));

    if(nextInstruction < numberOfInstructions) {
        Instruction instr = instructions.at(nextInstruction);

        bool stationNotBusy;
        if (instr.getOp() == "add" || instr.getOp() == "sub" || instr.getOp() == "addi")
            stationNotBusy = fillReservationStationAdders(instr.getOp(), instr.getReg1(), instr.getReg2(), instr.getReg3());
        else if (instr.getOp() == "ld")
            stationNotBusy = fillReservationStationLoads(instr.getReg1(), instr.getReg2(), instr.getReg3());
        else if (instr.getOp() == "sd")
            stationNotBusy = fillReservationStationStores(instr.getReg1(), instr.getReg2(), instr.getReg3());
        else
            stationNotBusy = fillReservationStationMults(instr.getOp(), instr.getReg1(), instr.getReg2(), instr.getReg3());

        checkReservationStations();
        showRegValues();
        showResStations();
        showMemory();

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
        checkReservationStations();
        showRegValues();
        showResStations();
        showMemory();
    }

    //Clk cycle has passed

    for (int i = 0; i < NUM_OF_ADDSUB_STATIONS; i++)
    {
        if (stationsAddSub[i].getBusy())
            stationsAddSub[i].setAtCycle(stationsAddSub[i].getAtCycle() + 1);
    }

    for (int i = 0; i < NUM_OF_LOAD_STATIONS; i++)
    {
        if (stationsLoad[i].getBusy())
            stationsLoad[i].setAtCycle(stationsLoad[i].getAtCycle() + 1);
    }

    for (int i = 0; i < NUM_OF_MULDIV_STATIONS; i++)
    {
        if (stationsMulDiv[i].getBusy())
            stationsMulDiv[i].setAtCycle(stationsMulDiv[i].getAtCycle() + 1);
    }

    for (int i = 0; i < NUM_OF_STORE_STATIONS; i++)
    {
        if (stationsStore[i].getBusy())
            stationsStore[i].setAtCycle(stationsStore[i].getAtCycle() + 1);
    }

    clkCycle++;

}

void MainWindow::on_skipButton_clicked()
{
    for (int i = 0; i < 10; i++)
        on_clkButton_clicked();
}

void MainWindow::initAlgorithm()
{
    clkCycle = 0;

    //Fill queue

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
    nextInstruction = 0;
    memText = "";
    initAlgorithm();
    ui->startButton->setEnabled(false);
    ui->clkButton->setEnabled(true);
    ui->skipButton->setEnabled(true);
    ui->compile_label->setText("");
    ui->clkLabel->setText("Clock cycle: " + QString::number(clkCycle));
}

void MainWindow::on_resetButton_clicked()
{
    resetStationsAndRegisters();
    cleanFields();
    nextInstruction = 0;
    memText = "";
    code = "";
    ui->compile_label->setText("");
    ui->startButton->setEnabled(true);
    ui->clkButton->setEnabled(false);
    ui->skipButton->setEnabled(false);
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
    ui->clkLabel->setText("");
}

bool MainWindow::fillReservationStationAdders(QString op, QString resReg, QString vj, QString vk)
{
    bool notBusy = false; //true if at least one of stations is not busy, otherwise false (all busy)
    bool regBusy;
    int allReady = 0;

    for (int i = 0; i < NUM_OF_ADDSUB_STATIONS; i++)
    {
        if (stationsAddSub[i].getBusy() == false)
        {
            stationsAddSub[i].setBusy(true);
            stationsAddSub[i].setOp(op);
            stationsAddSub[i].setResReg(resReg);
            stationsAddSub[i].setAtCycle(0);

            //OPERANDS

            int regNumberJ = 0;
            int regNumberK = 0; //string vj can be register or immediate
            bool vkIsImm = false;
            QRegularExpression regularRegister("^[fF]([0-3])$");
            if (regularRegister.match(vj).hasMatch())
            {
                regNumberJ = (regularRegister.match(vj).captured(1)).toInt();
                regBusy = registers[regNumberJ].getBusy();
                if (regBusy)
                {
                    //write only reg name (f1, f2 ...)
                    stationsAddSub[i].setVj(vj);
                }
                else
                {
                    //write value
                    stationsAddSub[i].setVj(QString::number(registers[regNumberJ].getValue()));
                    allReady++;
                }
            }
            else
                qDebug() << "Problem with registers in instructions.";

            if (regularRegister.match(vk).hasMatch())
            {
                regNumberK = (regularRegister.match(vk).captured(1)).toInt();
                regBusy = registers[regNumberK].getBusy();
                if (regBusy)
                {
                    //write only reg name (f1, f2 ...)
                    stationsAddSub[i].setVk(vk);
                }
                else
                {
                    //write value
                    stationsAddSub[i].setVk(QString::number(registers[regNumberK].getValue()));
                    allReady++;
                }
            }
            else
            {
                //check Vk
                vk.toInt(&vkIsImm);
                if (vkIsImm)
                {
                    stationsAddSub[i].setVk(vk);
                    allReady++;
                }
                else
                    qDebug() << "Problem with registers in instructions.";
            }

            if (allReady == 2)
                stationsAddSub[i].setWorking(true);

            notBusy = true;

            markRegisterBusy(resReg);

            break;
        }
        allReady = 0;
    }
    return notBusy;
}

bool MainWindow::fillReservationStationLoads(QString resReg, QString imm, QString addrReg)
{
    bool notBusy = false; //true if at least one of stations is not busy, otherwise false (all busy)
    bool regBusy;

    for (int i = 0; i < NUM_OF_LOAD_STATIONS; i++)
    {
        if (stationsLoad[i].getBusy() == false)
        {
            stationsLoad[i].setBusy(true);
            stationsLoad[i].setImm(imm);
            stationsLoad[i].setResReg(resReg);
            stationsLoad[i].setAtCycle(0);

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
            if (regBusy)
            {
                //write reg name
                stationsLoad[i].setAddrReg(addrReg);
                stationsLoad[i].setAddr(-1);
            }
            else
            {
                //calculate address and write value
                stationsLoad[i].setAddrReg(QString::number(registers[regNumber].getValue()));
                stationsLoad[i].setAddr(imm.toInt() + registers[regNumber].getValue());
                stationsLoad[i].setWorking(true);
            }

            notBusy = true;

            markRegisterBusy(resReg);

            break;
        }
    }
    return notBusy;
}

bool MainWindow::fillReservationStationStores(QString vj, QString imm, QString addrReg)
{
    bool notBusy = false; //true if at least one of stations is not busy, otherwise false (all busy)
    bool regBusy;
    int allReady = 0;

    for (int i = 0; i < NUM_OF_STORE_STATIONS; i++)
    {
        if (stationsStore[i].getBusy() == false)
        {
            stationsStore[i].setBusy(true);
            stationsStore[i].setImm(imm);
            stationsStore[i].setAtCycle(0);

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
            if (regBusy)
            {
                //write reg name
                stationsStore[i].setAddrReg(addrReg);
                stationsStore[i].setAddr(-1);
                markMemoryElementBusy(imm + "(" + addrReg + ")");
            }
            else
            {
                //calculate address and write value
                stationsStore[i].setAddrReg(QString::number(registers[regNumber].getValue()));
                stationsStore[i].setAddr(imm.toInt() + registers[regNumber].getValue());
                markMemoryElementBusy(QString::number(stationsStore[i].getAddr()));
                allReady++;
            }
            regBusy = registers[regNumberJ].getBusy();
            if (regBusy)
            {
                //write only reg name (f1, f2 ...)
                stationsStore[i].setVj(vj);
            }
            else
            {
                //write value
                stationsStore[i].setVj(QString::number(registers[regNumberJ].getValue()));
                allReady++;
            }

            if (allReady == 2)
                stationsStore[i].setWorking(true);

            notBusy = true;

            break;
        }
        allReady = 0;
    }
    return notBusy;
}

bool MainWindow::fillReservationStationMults(QString op, QString resReg, QString vj, QString vk)
{
    bool notBusy = false; //true if at least one of stations is not busy, otherwise false (all busy)
    bool regBusy;
    int allReady = 0;

    for (int i = 0; i < NUM_OF_MULDIV_STATIONS; i++)
    {
        if (stationsMulDiv[i].getBusy() == false)
        {
            stationsMulDiv[i].setBusy(true);
            stationsMulDiv[i].setOp(op);
            stationsMulDiv[i].setResReg(resReg);
            stationsMulDiv[i].setAtCycle(0);

            //OPERANDS

            int regNumberJ = 0;
            int regNumberK = 0;
            QRegularExpression regularRegister("^[fF]([0-3])$");
            if (regularRegister.match(vj).hasMatch())
            {
                regNumberJ = (regularRegister.match(vj).captured(1)).toInt();
                regBusy = registers[regNumberJ].getBusy();
                if (regBusy)
                {
                    //write only reg name (f1, f2 ...)
                    stationsMulDiv[i].setVj(vj);
                }
                else
                {
                    //write value
                    stationsMulDiv[i].setVj(QString::number(registers[regNumberJ].getValue()));
                    allReady++;
                }
            }
            else
                qDebug() << "Problem with registers in instructions.";

            if (regularRegister.match(vk).hasMatch())
            {
                regNumberK = (regularRegister.match(vk).captured(1)).toInt();
                regBusy = registers[regNumberK].getBusy();
                if (regBusy)
                {
                    //write only reg name (f1, f2 ...)
                    stationsMulDiv[i].setVk(vk);
                }
                else
                {
                    //write value
                    stationsMulDiv[i].setVk(QString::number(registers[regNumberK].getValue()));
                    allReady++;
                }
            }
            else
                qDebug() << "Problem with registers in instructions.";

            if (allReady == 2)
                stationsMulDiv[i].setWorking(true);

            notBusy = true;

            markRegisterBusy(resReg);

            break;
        }
        allReady = 0;
    }
    return notBusy;
}

void MainWindow::checkReservationStations()
{
    //For all busy and working reservation stations check if they have finished their work
    //For all busy reservation stations which are waiting for operands check if they are ready now

    /********************************************************
    Solve problem with instructions with patterns: op fi,fi,x or op fi,x,fi
    ********************************************************/

    /********************************************************
    Analyze war waw rar and raw hazards
    ********************************************************/

    bool regBusy;
    QList<QString> registersToFree;

    for (int i = 0; i < NUM_OF_ADDSUB_STATIONS; i++)
    {
        if (stationsAddSub[i].getBusy())
        {
            if (stationsAddSub[i].getWorking())
            {
                if (stationsAddSub[i].getOp() == "add" || stationsAddSub[i].getOp() == "addi")
                {
                    if (stationsAddSub[i].getAtCycle() == ADD_CLK_LATENCY)
                    {
                        registersToFree.append(stationsAddSub[i].getResReg());

                        int regNumber = 0;
                        QRegularExpression regularRegister("^[fF]([0-3])$");
                        if (regularRegister.match(stationsAddSub[i].getResReg()).hasMatch())
                        {
                            regNumber = (regularRegister.match(stationsAddSub[i].getResReg()).captured(1)).toInt();
                        }
                        else
                            qDebug() << "Problem with registers in instructions.";

                        int result = stationsAddSub[i].calculate();
                        registers[regNumber].setValue(result);
                    }

                }
                else
                {
                    if (stationsAddSub[i].getAtCycle() == SUB_CLK_LATENCY)
                    {
                        registersToFree.append(stationsAddSub[i].getResReg());

                        int regNumber = 0;
                        QRegularExpression regularRegister("^[fF]([0-3])$");
                        if (regularRegister.match(stationsAddSub[i].getResReg()).hasMatch())
                        {
                            regNumber = (regularRegister.match(stationsAddSub[i].getResReg()).captured(1)).toInt();
                        }
                        else
                            qDebug() << "Problem with registers in instructions.";

                        int result = stationsAddSub[i].calculate();
                        registers[regNumber].setValue(result);
                    }
                }
            }
            else
            {
                int allReady = 0;
                bool numOk;

                //check Vj
                stationsAddSub[i].getVj().toInt(&numOk);
                if (!numOk)
                {
                    int regNumberJ = 0;
                    QRegularExpression regularRegister("^[fF]([0-3])$");
                    if (regularRegister.match(stationsAddSub[i].getVj()).hasMatch())
                    {
                        regNumberJ = (regularRegister.match(stationsAddSub[i].getVj()).captured(1)).toInt();
                    }
                    else
                        qDebug() << "Problem with registers in instructions.";

                    regBusy = registers[regNumberJ].getBusy();
                    if (!regBusy)
                    {
                        stationsAddSub[i].setVj(QString::number(registers[regNumberJ].getValue()));
                        allReady++;
                    }
                }
                else
                    allReady++;

                //check Vk
                stationsAddSub[i].getVk().toInt(&numOk);
                if (!numOk)
                {
                    int regNumberK = 0;
                    QRegularExpression regularRegister("^[fF]([0-3])$");
                    if (regularRegister.match(stationsAddSub[i].getVk()).hasMatch())
                    {
                        regNumberK = (regularRegister.match(stationsAddSub[i].getVk()).captured(1)).toInt();
                    }
                    else
                        qDebug() << "Problem with registers in instructions. value of vk = " + stationsAddSub[i].getVk();

                    regBusy = registers[regNumberK].getBusy();
                    if (!regBusy)
                    {
                        stationsAddSub[i].setVk(QString::number(registers[regNumberK].getValue()));
                        allReady++;
                    }
                }
                else
                    allReady++;

                if (allReady == 2)
                    stationsAddSub[i].setWorking(true);
            }

        }
    }

    for (int i = 0; i < NUM_OF_MULDIV_STATIONS; i++)
    {
        if (stationsMulDiv[i].getBusy())
        {
            if (stationsMulDiv[i].getWorking())
            {
                if (stationsMulDiv[i].getOp() == "mul")
                {
                    if (stationsMulDiv[i].getAtCycle() == MUL_CLK_LATENCY)
                    {
                        registersToFree.append(stationsMulDiv[i].getResReg());

                        int regNumber = 0;
                        QRegularExpression regularRegister("^[fF]([0-3])$");
                        if (regularRegister.match(stationsMulDiv[i].getResReg()).hasMatch())
                        {
                            regNumber = (regularRegister.match(stationsMulDiv[i].getResReg()).captured(1)).toInt();
                        }
                        else
                            qDebug() << "Problem with registers in instructions.";

                        int result = stationsMulDiv[i].calculate();
                        registers[regNumber].setValue(result);
                    }

                }
                else
                {
                    if (stationsMulDiv[i].getAtCycle() == DIV_CLK_LATENCY)
                    {
                        registersToFree.append(stationsMulDiv[i].getResReg());

                        int regNumber = 0;
                        QRegularExpression regularRegister("^[fF]([0-3])$");
                        if (regularRegister.match(stationsMulDiv[i].getResReg()).hasMatch())
                        {
                            regNumber = (regularRegister.match(stationsMulDiv[i].getResReg()).captured(1)).toInt();
                        }
                        else
                            qDebug() << "Problem with registers in instructions.";

                        int result = stationsMulDiv[i].calculate();
                        registers[regNumber].setValue(result);
                    }
                }
            }
            else
            {
                int allReady = 0;
                bool numOk;

                //check Vj
                stationsMulDiv[i].getVj().toInt(&numOk);
                if (!numOk)
                {
                    int regNumberJ = 0;
                    QRegularExpression regularRegister("^[fF]([0-3])$");
                    if (regularRegister.match(stationsMulDiv[i].getVj()).hasMatch())
                    {
                        regNumberJ = (regularRegister.match(stationsMulDiv[i].getVj()).captured(1)).toInt();
                    }
                    else
                        qDebug() << "Problem with registers in instructions.";

                    regBusy = registers[regNumberJ].getBusy();
                    if (!regBusy)
                    {
                        stationsMulDiv[i].setVj(QString::number(registers[regNumberJ].getValue()));
                        allReady++;
                    }
                }
                else
                    allReady++;

                //check Vk
                stationsMulDiv[i].getVk().toInt(&numOk);
                if (!numOk)
                {
                    int regNumberK = 0;
                    QRegularExpression regularRegister("^[fF]([0-3])$");
                    if (regularRegister.match(stationsMulDiv[i].getVk()).hasMatch())
                    {
                        regNumberK = (regularRegister.match(stationsMulDiv[i].getVk()).captured(1)).toInt();
                    }
                    else
                        qDebug() << "Problem with registers in instructions.";

                    regBusy = registers[regNumberK].getBusy();
                    if (!regBusy)
                    {
                        stationsMulDiv[i].setVk(QString::number(registers[regNumberK].getValue()));
                        allReady++;
                    }
                }
                else
                    allReady++;

                if (allReady == 2)
                    stationsMulDiv[i].setWorking(true);
            }
        }
    }

    for (int i = 0; i < NUM_OF_LOAD_STATIONS; i++)
    {
        if (stationsLoad[i].getBusy())
        {
            if (stationsLoad[i].getWorking())
            {
                if (stationsLoad[i].getAtCycle() >= LOAD_CLK_LATENCY)
                {
                    //Check if memory element is busy
                    if (!memoryBusy[stationsLoad[i].getAddr()])
                    {
                        registersToFree.append(stationsLoad[i].getResReg());

                        int regNumber = 0;
                        QRegularExpression regularRegister("^[fF]([0-3])$");
                        if (regularRegister.match(stationsLoad[i].getResReg()).hasMatch())
                        {
                            regNumber = (regularRegister.match(stationsLoad[i].getResReg()).captured(1)).toInt();
                        }
                        else
                            qDebug() << "Problem with registers in instructions.";

                        int result = memory[stationsLoad[i].getAddr()];
                        stationsLoad[i].read();

                        registers[regNumber].setValue(result);
                    }
                }
            }
            else
            {
                bool numOk;

                //check addrReg
                stationsLoad[i].getAddrReg().toInt(&numOk);
                if (!numOk)
                {
                    int regNumber = 0;
                    QRegularExpression regularRegister("^[fF]([0-3])$");
                    if (regularRegister.match(stationsLoad[i].getAddrReg()).hasMatch())
                    {
                        regNumber = (regularRegister.match(stationsLoad[i].getAddrReg()).captured(1)).toInt();
                    }
                    else
                        qDebug() << "Problem with registers in instructions.";

                    regBusy = registers[regNumber].getBusy();
                    if (!regBusy)
                    {
                        stationsLoad[i].setAddrReg(QString::number(registers[regNumber].getValue()));
                        stationsLoad[i].setAddr(stationsLoad[i].getImm().toInt() + registers[regNumber].getValue());
                        stationsLoad[i].setWorking(true);
                    }

                }
            }
        }
    }

    for (int i = 0; i < NUM_OF_STORE_STATIONS; i++)
    {
        if (stationsStore[i].getBusy())
        {
            if (stationsStore[i].getWorking())
            {
                if (stationsStore[i].getAtCycle() == LOAD_CLK_LATENCY)
                {
                    unmarkMemoryElementBusy(QString::number(stationsStore[i].getAddr()));
                    memory[stationsStore[i].getAddr()] = stationsStore[i].getVj().toInt();
                    stationsStore[i].write();
                }
            }
            else
            {
                int allReady = 0;
                bool numOk;

                //check Vj
                stationsStore[i].getVj().toInt(&numOk);
                if (!numOk)
                {
                    int regNumberJ = 0;
                    QRegularExpression regularRegister("^[fF]([0-3])$");
                    if (regularRegister.match(stationsStore[i].getVj()).hasMatch())
                    {
                        regNumberJ = (regularRegister.match(stationsStore[i].getVj()).captured(1)).toInt();
                    }
                    else
                        qDebug() << "Problem with registers in instructions.";

                    regBusy = registers[regNumberJ].getBusy();
                    if (!regBusy)
                    {
                        stationsStore[i].setVj(QString::number(registers[regNumberJ].getValue()));
                        allReady++;
                    }
                }
                else
                    allReady++;

                //check addrReg
                stationsStore[i].getAddrReg().toInt(&numOk);
                if (!numOk)
                {
                    int regNumber = 0;
                    QRegularExpression regularRegister("^[fF]([0-3])$");
                    if (regularRegister.match(stationsStore[i].getAddrReg()).hasMatch())
                    {
                        regNumber = (regularRegister.match(stationsStore[i].getAddrReg()).captured(1)).toInt();
                    }
                    else
                        qDebug() << "Problem with registers in instructions.";

                    regBusy = registers[regNumber].getBusy();
                    if (!regBusy)
                    {
                        stationsStore[i].setAddrReg(QString::number(registers[regNumber].getValue()));
                        stationsStore[i].setAddr(stationsStore[i].getImm().toInt() + registers[regNumber].getValue());
                        markMemoryElementBusy(QString::number(stationsStore[i].getAddr()));
                        allReady++;
                    }
                }
                else
                    allReady++;

                if (allReady == 2)
                    stationsStore[i].setWorking(true);
            }
        }
    }

    //Free registers in the end of all loops, so no one can take them in this cycle
    for (int i = 0; i < registersToFree.size(); i++)
    {
        int regNumber = 0;
        QRegularExpression regularRegister("^[fF]([0-3])$");
        if (regularRegister.match(registersToFree[i]).hasMatch())
        {
            regNumber = (regularRegister.match(registersToFree[i]).captured(1)).toInt();
        }
        else
            qDebug() << "Problem with registers in instructions.";

        registers[regNumber].setBusy(false);
    }
}

void MainWindow::markRegisterBusy(QString reg)
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
}

void MainWindow::markMemoryElementBusy(QString address)
{
    //only load needs to check ih memory element is busy (called before read method of class ReservationStationLoad)
    //if it is busy, wait untill element is stable, and then load it in register
    memoryBusy[address.toInt()] = true;
    memText.append("Location " + address + " is busy.\n");
    ui->memTextEdit->setText(memText);
}

void MainWindow::unmarkMemoryElementBusy(QString address)
{
    memoryBusy[address.toInt()] = false;
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

void MainWindow::showRegValues()
{
    QString labelName;
    QLabel* targetLabel;

    for (int i = 0; i < NUM_OF_REGISTERS; i++)
    {
        labelName = "f_" + QString::number(i);
        targetLabel = ui->addsOp_0->parentWidget()->findChild<QLabel*>(labelName);
        targetLabel->setText(QString::number(registers[i].getValue()));
    }
}

void MainWindow::showResStations()
{
    QString labelName;
    QLabel* targetLabel;

    for (int i = 0; i < NUM_OF_ADDSUB_STATIONS; i++)
    {
        if (stationsAddSub[i].getBusy())
        {
            labelName = "addsOp_" + QString::number(i);
            targetLabel = ui->addsOp_0->parentWidget()->findChild<QLabel*>(labelName);
            targetLabel->setText(stationsAddSub[i].getOp());
            labelName = "addsVj_" + QString::number(i);
            targetLabel = ui->addsOp_0->parentWidget()->findChild<QLabel*>(labelName);
            targetLabel->setText(stationsAddSub[i].getVj());
            labelName = "addsVk_" + QString::number(i);
            targetLabel = ui->addsOp_0->parentWidget()->findChild<QLabel*>(labelName);
            targetLabel->setText(stationsAddSub[i].getVk());
        }
        else
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
    }

    for (int i = 0; i < NUM_OF_LOAD_STATIONS; i++)
    {
        if (stationsLoad[i].getBusy())
        {
            labelName = "load_" + QString::number(i);
            targetLabel = ui->addsOp_0->parentWidget()->findChild<QLabel*>(labelName);
            if (stationsLoad[i].getAddr() != -1)
                targetLabel->setText(QString::number(stationsLoad[i].getAddr()));
            else
                targetLabel->setText(stationsLoad[i].getImm() + "(" + stationsLoad[i].getAddrReg() + ")");
        }
        else
        {
            labelName = "load_" + QString::number(i);
            targetLabel = ui->addsOp_0->parentWidget()->findChild<QLabel*>(labelName);
            targetLabel->setText("");
        }
    }

    for (int i = 0; i < NUM_OF_MULDIV_STATIONS; i++)
    {
        if (stationsMulDiv[i].getBusy())
        {
            labelName = "mulsOp_" + QString::number(i);
            targetLabel = ui->addsOp_0->parentWidget()->findChild<QLabel*>(labelName);
            targetLabel->setText(stationsMulDiv[i].getOp());
            labelName = "mulsVj_" + QString::number(i);
            targetLabel = ui->addsOp_0->parentWidget()->findChild<QLabel*>(labelName);
            targetLabel->setText(stationsMulDiv[i].getVj());
            labelName = "mulsVk_" + QString::number(i);
            targetLabel = ui->addsOp_0->parentWidget()->findChild<QLabel*>(labelName);
            targetLabel->setText(stationsMulDiv[i].getVk());
        }
        else
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
    }

    for (int i = 0; i < NUM_OF_STORE_STATIONS; i++)
    {
        if (stationsStore[i].getBusy())
        {
            labelName = "storeReg_" + QString::number(i);
            targetLabel = ui->addsOp_0->parentWidget()->findChild<QLabel*>(labelName);
            targetLabel->setText(stationsStore[i].getVj());
            labelName = "storeAddr_" + QString::number(i);
            targetLabel = ui->addsOp_0->parentWidget()->findChild<QLabel*>(labelName);
            if (stationsStore[i].getAddr() != -1)
                targetLabel->setText(QString::number(stationsStore[i].getAddr()));
            else
                targetLabel->setText(stationsStore[i].getImm() + "(" + stationsStore[i].getAddrReg() + ")");
        }
        else
        {
            labelName = "storeReg_" + QString::number(i);
            targetLabel = ui->addsOp_0->parentWidget()->findChild<QLabel*>(labelName);
            targetLabel->setText("");
            labelName = "storeAddr_" + QString::number(i);
            targetLabel = ui->addsOp_0->parentWidget()->findChild<QLabel*>(labelName);
            targetLabel->setText("");
        }
    }
}

void MainWindow::showMemory()
{
    for (int i = 0; i < MEMORY_SIZE; i++)
    {
        QTableWidgetItem *data= new QTableWidgetItem(QString::number(memory[i]));
        if (memoryBusy[i])
            data->setBackground(QBrush(QColor(Qt::red).lighter(160)));
        ui->memoryTable->setItem(i, 1, data);
    }
}

void MainWindow::resetStationsAndRegisters()
{
    //reset stations
    for (int i = 0; i < NUM_OF_ADDSUB_STATIONS; i++)
    {
        stationsAddSub[i].setBusy(false);
        stationsAddSub[i].setOp("");
        stationsAddSub[i].setVj("");
        stationsAddSub[i].setVk("");
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
        stationsMulDiv[i].setVj("");
        stationsMulDiv[i].setVk("");
        stationsMulDiv[i].setAtCycle(0);
    }
    for (int i = 0; i < NUM_OF_STORE_STATIONS; i++)
    {
        stationsStore[i].setBusy(false);
        stationsStore[i].setAddr(0);
        stationsStore[i].setVj("");
        stationsStore[i].setAtCycle(0);
    }

    //reset registers
    for (int i = 0; i < NUM_OF_REGISTERS; i++)
    {
        registers[i].setBusy(false);
        registers[i].setValue(0);
    }
}

