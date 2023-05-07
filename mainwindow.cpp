#include "mainwindow.h"
#include "instruction.h"
#include "reservationstationaddsub.h"
#include "cpufeatures.h"
#include "reservationstationload.h"
#include "reservationstationmuldiv.h"
#include "reservationstationstore.h"
#include "register.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QPainter>
#include <QRegularExpression>
#include <QMessageBox>
#include <QTextBlock>

int clkCycle = 0;
QStringList instructionsString;
QStringList instructionsStringEmpty;
QString code;
int nextQueueInstruction = 0; //shows which instruction from code is next to be inserted in queue
int nextInstruction = 0;
int numberOfInstructions = 0;
QList<Instruction> instructions;
QList<ReservationStationAddSub> stationsAddSub;
QList<ReservationStationLoad> stationsLoad;
QList<ReservationStationMulDiv> stationsMulDiv;
QList<ReservationStationStore> stationsStore;
QList<Register> registers;
int memory[MEMORY_SIZE];
bool memoryPortA;
bool memoryPortB;
bool error = false;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //create reservation stations
    for (int i = 0; i < NUM_OF_ADDSUB_STATIONS; i++)
    {
        ReservationStationAddSub stAddSub;
        stationsAddSub.append(stAddSub);
    }

    ui->add_0->setColumnCount(1);
    ui->add_0->setRowCount(1);
    ui->add_0->verticalHeader()->setVisible(false);
    ui->add_0->horizontalHeader()->setVisible(false);
    ui->add_0->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    ui->add_1->setColumnCount(1);
    ui->add_1->setRowCount(1);
    ui->add_1->verticalHeader()->setVisible(false);
    ui->add_1->horizontalHeader()->setVisible(false);
    ui->add_1->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    ui->add_2->setColumnCount(1);
    ui->add_2->setRowCount(1);
    ui->add_2->verticalHeader()->setVisible(false);
    ui->add_2->horizontalHeader()->setVisible(false);
    ui->add_2->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

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

    ui->mul_0->setColumnCount(1);
    ui->mul_0->setRowCount(1);
    ui->mul_0->verticalHeader()->setVisible(false);
    ui->mul_0->horizontalHeader()->setVisible(false);
    ui->mul_0->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    ui->mul_1->setColumnCount(1);
    ui->mul_1->setRowCount(1);
    ui->mul_1->verticalHeader()->setVisible(false);
    ui->mul_1->horizontalHeader()->setVisible(false);
    ui->mul_1->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

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
    //ui->memoryTable->horizontalHeader()->setVisible(false);
    QStringList memColumns = { "Address", "Value"};
    ui->memoryTable->setHorizontalHeaderLabels(memColumns);
    ui->memoryTable->setStyleSheet("QHeaderView::section { background-color:rgb(218, 232, 252);}");
    ui->memoryTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    for (int i = 0; i < MEMORY_SIZE; i++)
    {
        memory[i] = 0;
        QTableWidgetItem *addr = new QTableWidgetItem(QString::number(i) + ":");
        addr->setTextAlignment(Qt::AlignCenter);
        ui->memoryTable->setItem(i, 0, addr);
    }
    memoryPortA = false;
    memoryPortB = false;
    ui->portTable->setColumnCount(2);
    ui->portTable->setRowCount(1);
    ui->portTable->verticalHeader()->setVisible(false);
    ui->portTable->horizontalHeader()->setVisible(false);
    ui->portTable->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->portTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    QTableWidgetItem *portA = new QTableWidgetItem("Port A");
    portA->setTextAlignment(Qt::AlignCenter);
    ui->portTable->setItem(0, 0, portA);
    QTableWidgetItem *portB = new QTableWidgetItem("Port B");
    portB->setTextAlignment(Qt::AlignCenter);
    ui->portTable->setItem(0, 1, portB);

    cleanQueue();
    showRegValues();
    showResStations();
    showMemory();
    ui->clkLabel->setText("");



    QMessageBox::information(this, "Welcome to Tomasulo-based RISC-V simulator!",
                          "<p align=center><b>Welcome to the Tomasulo-based RISC-V Simulator!</b></p>"
                          "<p align=justify>\nThank you for choosing this RISC-V simulator application. Application shows RISC-V "
                          "architecture based on Tomasulo algorithm and to help you get started with using the app, "
                          "a brief guide will be presented.</p>"
                          "<p align=justify>\nThis app is designed to illustrate simple Tomasulo-based RISC V processor, "
                          "featuring various visual components such as reservation stations, instruction queues, "
                          "load/store buffers and memory. To begin, input your desired assembler commands in the "
                          "designated command window and compile it accordingly."
                          "\nThe app currently supports basic RISC-V format instructions, including "
                          "add, addi, sub, mul, div, lw, sw and nop. Once your commands are compiled, you can "
                          "either click the 'Clk' button to see how the simulator works in real-time or choose to "
                          "skip 10 clocks for a quicker view, using 'Skip 10 clocks' button. The diagram will provide "
                          "you with a detailed illustration of all activities happening "
                          "within the processor, such as instruction execution and data transfers.</p>"
                          "<p>Enjoy using RISC-V simulator!</p>");
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
    resetProcessor();
    error = false;

    ui->startButton->setEnabled(false);
    ui->clkButton->setEnabled(false);
    ui->skipButton->setEnabled(false);
    ui->resetButton->setEnabled(false);
    ui->clkLabel->setText("");

    instructionsStringEmpty = code.split(QLatin1Char('\n'));
    instructionsString = code.split(QLatin1Char('\n'), Qt::SkipEmptyParts);
    numberOfInstructions = instructionsString.size();

    bool incorrectInstruction;
    QList<QTextEdit::ExtraSelection> extraSelections;

    bool correctCode = true;
    //Checking format
    for (int i = 0; i < instructionsStringEmpty.size(); i++)
    {
        QRegularExpression regularInstruction1("^\\s*(add|sub|mul|div|ADD|SUB|MUL|DIV)\\s+([xX][0-5])\\s*,\\s*([xX][0-5])\\s*,\\s*([xX][0-5])\\s*$");
        QRegularExpression regularInstruction2("^\\s*(sw|lw|SW|LW)\\s+([xX][0-5])\\s*,\\s*([0-9]+)\\s*,\\s*([xX][0-5])\\s*$");
        QRegularExpression regularInstruction3("^\\s*(sw|lw|SW|LW)\\s+([xX][0-5])\\s*,\\s*([0-9]+)[(]\\s*([xX][0-5])\\s*[)]\\s*$");
        QRegularExpression regularInstruction4("^\\s*(addi|ADDI)\\s+([xX][0-5])\\s*,\\s*([xX][0-5])\\s*,\\s*(-?[0-9]+)\\s*$");
        QRegularExpression regularInstruction5("^\\s*(nop|NOP)\\s*$");

        if (regularInstruction1.match(instructionsStringEmpty[i]).hasMatch())
        {

            Instruction instr(regularInstruction1.match(instructionsStringEmpty[i]).captured(1),
                              regularInstruction1.match(instructionsStringEmpty[i]).captured(2),
                              regularInstruction1.match(instructionsStringEmpty[i]).captured(3),
                              regularInstruction1.match(instructionsStringEmpty[i]).captured(4));
            instructions.append(instr);
            incorrectInstruction = false;

            //qDebug() << "Instruction number " + QString::number(i) + " is correct.";
        }
        else if (regularInstruction2.match(instructionsStringEmpty[i]).hasMatch())
        {
            Instruction instr(regularInstruction2.match(instructionsStringEmpty[i]).captured(1),
                              regularInstruction2.match(instructionsStringEmpty[i]).captured(2),
                              regularInstruction2.match(instructionsStringEmpty[i]).captured(3),
                              regularInstruction2.match(instructionsStringEmpty[i]).captured(4));
            instructions.append(instr);
            incorrectInstruction = false;

            //qDebug() << "Instruction number " + QString::number(i) + " is correct.";
        }
        else if (regularInstruction3.match(instructionsStringEmpty[i]).hasMatch())
        {
            Instruction instr(regularInstruction3.match(instructionsStringEmpty[i]).captured(1),
                              regularInstruction3.match(instructionsStringEmpty[i]).captured(2),
                              regularInstruction3.match(instructionsStringEmpty[i]).captured(3),
                              regularInstruction3.match(instructionsStringEmpty[i]).captured(4));
            instructions.append(instr);
            incorrectInstruction = false;

            //qDebug() << "Instruction number " + QString::number(i) + " is correct.";
        }
        else if (regularInstruction4.match(instructionsStringEmpty[i]).hasMatch())
        {
            Instruction instr("addi",
                              regularInstruction4.match(instructionsStringEmpty[i]).captured(2),
                              regularInstruction4.match(instructionsStringEmpty[i]).captured(3),
                              regularInstruction4.match(instructionsStringEmpty[i]).captured(4));
            instructions.append(instr);
            incorrectInstruction = false;

            //qDebug() << "Instruction number " + QString::number(i) + " is correct.";
        }
        else if (regularInstruction5.match(instructionsStringEmpty[i]).hasMatch())
        {
            Instruction instr("nop",
                              "",
                              "",
                              "");
            instructions.append(instr);
            incorrectInstruction = false;

            //qDebug() << "Instruction number " + QString::number(i) + " is correct.";
        }
        else if (instructionsStringEmpty[i].isEmpty())
        {
            incorrectInstruction = false;
        }
        else
        {
            writeOutputMessage("<p style=color:red>ERROR: Instruction number " + QString::number(i) + " is not correct!</p>");
            incorrectInstruction = true;
            correctCode = false;
        }

        //If command line is not correct mark it red
        if (incorrectInstruction == true)
        {
            QTextCursor cursor = ui->codeTextEdit->textCursor();
            cursor.movePosition(QTextCursor::Start);
            cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, i); // select line i+1

            QTextEdit::ExtraSelection selection;
            QColor lineColor(QColor(Qt::red).lighter(160)); // set the background color to red
            selection.format.setBackground(lineColor);
            selection.cursor = cursor;
            selection.cursor.select(QTextCursor::LineUnderCursor);

            extraSelections.append(selection);
        }
    }
    // Apply the background color change to the QTextEdit widget
    ui->codeTextEdit->setExtraSelections(extraSelections);

    if (correctCode)
    {
        if (numberOfInstructions > 0)
        {
            writeOutputMessage("Code is compiled successfuly.");
            writeOutputMessage("Number of instructions: " + QString::number(numberOfInstructions));
            writeOutputMessage("---------------------------------");
            ui->startButton->setEnabled(true);
            ui->clkButton->setEnabled(false);
            ui->skipButton->setEnabled(false);
            ui->resetButton->setEnabled(true);
        }
        else
        {
            writeOutputMessage("WARNING: You have to enter the program code before compiling.");
        }
    }
    else
    {
        writeOutputMessage("<p style=color:red>ERROR: Code contains errors.</p>");
        writeOutputMessage("---------------------------------");

        instructions.clear();   //clear all created elements of instructions list
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
        else if (instr.getOp() == "lw")
            stationNotBusy = fillReservationStationLoads(instr.getReg1(), instr.getReg2(), instr.getReg3());
        else if (instr.getOp() == "sw")
            stationNotBusy = fillReservationStationStores(instr.getReg1(), instr.getReg2(), instr.getReg3());
        else if (instr.getOp() == "nop")
            stationNotBusy = true;
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
		else
		{
            writeOutputMessage("INFO: All reservation stations for next instruction in queue are busy -> stall.");
		}
    }
    else
    {
        //Continue executing, to finish all started instructions
        checkReservationStations();
        showRegValues();
        showResStations();
        showMemory();
    }

    //Clk cycle has passed

    for (int i = 0; i < NUM_OF_ADDSUB_STATIONS; i++)
    {
        if (stationsAddSub[i].getBusy() && stationsAddSub[i].getWorking())
            stationsAddSub[i].setAtCycle(stationsAddSub[i].getAtCycle() + 1);
    }

    for (int i = 0; i < NUM_OF_LOAD_STATIONS; i++)
    {
        if (stationsLoad[i].getBusy() && stationsLoad[i].getWorking())
            stationsLoad[i].setAtCycle(stationsLoad[i].getAtCycle() + 1);
    }

    for (int i = 0; i < NUM_OF_MULDIV_STATIONS; i++)
    {
        if (stationsMulDiv[i].getBusy() && stationsMulDiv[i].getWorking())
            stationsMulDiv[i].setAtCycle(stationsMulDiv[i].getAtCycle() + 1);
    }

    for (int i = 0; i < NUM_OF_STORE_STATIONS; i++)
    {
        if (stationsStore[i].getBusy() && stationsStore[i].getWorking())
            stationsStore[i].setAtCycle(stationsStore[i].getAtCycle() + 1);
    }

    clkCycle++;

    if (error)
    {
        on_resetButton_clicked();
        ui->resetButton->setEnabled(false);
        ui->startButton->setEnabled(false);
    }

}

void MainWindow::on_skipButton_clicked()
{
    for (int i = 0; i < 10; i++)
    {
        on_clkButton_clicked();
        if (error)
                break;
    }
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
    resetProcessor();
    cleanQueue();
    showRegValues();
    showResStations();
    showMemory();
    nextInstruction = 0;
    initAlgorithm();
    ui->startButton->setEnabled(false);
    ui->clkButton->setEnabled(true);
    ui->skipButton->setEnabled(true);
    //ui->compileOutput->setText("");
    ui->clkLabel->setText("Clock cycle: " + QString::number(clkCycle));
}

void MainWindow::on_resetButton_clicked()
{
    resetProcessor();
    cleanQueue();
    showRegValues();
    showResStations();
    showMemory();
    nextInstruction = 0;
    code = "";
    //ui->compileOutput->setText("");
    ui->startButton->setEnabled(true);
    ui->clkButton->setEnabled(false);
    ui->skipButton->setEnabled(false);
    ui->clkLabel->setText("");
}

void MainWindow::cleanQueue()
{
    QString labelName;
    QLabel* targetLabel;

    for (int i = 0; i < QUEUE_SIZE; i++)
    {
        labelName = "queue_" + QString::number(i);
        targetLabel = ui->addsOp_0->parentWidget()->findChild<QLabel*>(labelName);
        targetLabel->setText("");
    }
}

bool MainWindow::fillReservationStationAdders(QString op, QString resReg, QString vj, QString vk)
{
    bool notBusy = false; //true if at least one of stations is not busy, otherwise false (all busy)
    int allReady = 0;

    for (int i = 0; i < NUM_OF_ADDSUB_STATIONS; i++)
    {
        if (stationsAddSub[i].getBusy() == false)
        {
            stationsAddSub[i].setBusy(true);
            stationsAddSub[i].setOp(op);
            stationsAddSub[i].setAtCycle(0);

            //OPERANDS

            int regNumberJ = 0;
            int regNumberK = 0; //string vj can be register or immediate
            bool vkIsImm = false;
            QRegularExpression regularRegister("^[xX]([0-5])$");

            //Vj
            if (regularRegister.match(vj).hasMatch())
            {
                regNumberJ = (regularRegister.match(vj).captured(1)).toInt();
                if (registers[regNumberJ].getQ() == "")
                {
                    //write value
                    stationsAddSub[i].setVj(QString::number(registers[regNumberJ].getValue()));
                    //stationsAddSub[i].setQj("");
                    allReady++;
                }
                else
                {
                    //write only reg name (f1, f2 ...)
                    stationsAddSub[i].setVj(vj);
                    stationsAddSub[i].setQj(registers[regNumberJ].getQ());
                }
            }
            else
                qDebug() << "Problem with registers in instructions.";

            //Vk
            if (regularRegister.match(vk).hasMatch())
            {
                regNumberK = (regularRegister.match(vk).captured(1)).toInt();
                if (registers[regNumberK].getQ() == "")
                {
                    //write value
                    stationsAddSub[i].setVk(QString::number(registers[regNumberK].getValue()));
                    //stationsAddSub[i].setQk("");
                    allReady++;
                }
                else
                {
                    //write only reg name (f1, f2 ...)
                    stationsAddSub[i].setVk(vk);
                    stationsAddSub[i].setQk(registers[regNumberK].getQ());
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

            markRegisterBusy(resReg, ADDSUB_STATIONS_NAME, QString::number(i));

            break;
        }
        allReady = 0;
    }
    return notBusy;
}

bool MainWindow::fillReservationStationLoads(QString resReg, QString imm, QString addrReg)
{
    bool notBusy = false; //true if at least one of stations is not busy, otherwise false (all busy)
    bool sameAddr = false; //true if at least one of store buffers has the same address as load address

    int regNumber = 0;
    int address = 0;
    QRegularExpression regularRegister("^[xX]([0-5])$");
    if (regularRegister.match(addrReg).hasMatch())
    {
        regNumber = (regularRegister.match(addrReg).captured(1)).toInt();

        //check if address register value is correct
        if (registers[regNumber].getQ() == "")
        {
            //calculate address
            address = imm.toInt() + registers[regNumber].getValue();
            //check if store buffers contain the same address
            for (int i = 0; i < NUM_OF_STORE_STATIONS; i++)
            {
                if (stationsStore[i].getBusy() && address == stationsStore[i].getAddr())
                {
                    sameAddr = true;
                    break;
                }
            }
            if (sameAddr)
                writeOutputMessage("INFO: Load is waiting for stores with same addresses to finish (preventing hazards).");
            else
            {
                //everything with address is ok
                for (int i = 0; i < NUM_OF_LOAD_STATIONS; i++)
                {
                    if (stationsLoad[i].getBusy() == false)
                    {
                        stationsLoad[i].setBusy(true);
                        stationsLoad[i].setAtCycle(0);
                        stationsLoad[i].setAddr(address);

                        //load station is working only when memory is available
                        if (memoryPortA == false)
                        {
                            stationsLoad[i].setWorking(true);
                            stationsLoad[i].setPort("A");
                            memoryPortA = true;
                        }
                        else if (memoryPortB == false)
                        {
                            stationsLoad[i].setWorking(true);
                            stationsLoad[i].setPort("B");
                            memoryPortB = true;
                        }
                        else
                            stationsLoad[i].setWorking(false);

                        notBusy = true;

                        markRegisterBusy(resReg, LOAD_STATIONS_NAME, QString::number(i));

                        break;
                    }
                }
            }
        }
        else
            writeOutputMessage("INFO: Load is waiting for address register to become available.");
    }
    else
        qDebug() << "Problem with registers in instructions.";

    return notBusy;
}

bool MainWindow::fillReservationStationStores(QString vj, QString imm, QString addrReg)
{
    bool notBusy = false; //true if at least one of stations is not busy, otherwise false (all busy)
    bool sameAddr = false; //true if at least one of store buffers has the same address as load address

    int regNumber = 0;
    int regNumberJ = 0;
    int address = 0;
    QRegularExpression regularRegister("^[xX]([0-5])$");
    if (regularRegister.match(addrReg).hasMatch() && regularRegister.match(vj).hasMatch())
    {
        regNumber = (regularRegister.match(addrReg).captured(1)).toInt();
        regNumberJ = (regularRegister.match(vj).captured(1)).toInt();

        //check if address register value is correct
        if (registers[regNumber].getQ() == "")
        {
            //calculate address
            address = imm.toInt() + registers[regNumber].getValue();
            //check if store buffers contain the same address
            for (int i = 0; i < NUM_OF_STORE_STATIONS; i++)
            {
                if (stationsStore[i].getBusy() && address == stationsStore[i].getAddr())
                {
                    sameAddr = true;
                    break;
                }
            }
            //check if load buffers contain the same address
            for (int i = 0; i < NUM_OF_STORE_STATIONS; i++)
            {
                if (stationsLoad[i].getBusy() && address == stationsLoad[i].getAddr())
                {
                    sameAddr = true;
                    break;
                }
            }
            if (sameAddr)
                writeOutputMessage("INFO: Store is waiting for loads and stores with same addresses to finish (preventing hazards).");
            else
            {
                //everything with address is ok
                for (int i = 0; i < NUM_OF_STORE_STATIONS; i++)
                {
                    if (stationsStore[i].getBusy() == false)
                    {
                        stationsStore[i].setBusy(true);
                        stationsStore[i].setAtCycle(0);
                        stationsStore[i].setAddr(address);

                        if (registers[regNumberJ].getQ() == "")
                        {
                            //write value
                            stationsStore[i].setVj(QString::number(registers[regNumberJ].getValue()));
                            //stationsStore[i].setQj("");

                            //store station is working only when memory and vj value are available
                            if (memoryPortA == false)
                            {
                                stationsStore[i].setWorking(true);
                                stationsStore[i].setPort("A");
                                memoryPortA = true;
                            }
                            else if (memoryPortB == false)
                            {
                                stationsStore[i].setWorking(true);
                                stationsStore[i].setPort("B");
                                memoryPortB = true;
                            }
                            else
                                stationsStore[i].setWorking(false);
                        }
                        else
                        {
                            //write only reg name (f1, f2 ...)
                            stationsStore[i].setVj(vj);
                            stationsStore[i].setQj(registers[regNumberJ].getQ());

                            //vj value is not available, so store station is not working
                            stationsStore[i].setWorking(false);
                        }

                        notBusy = true;

                        break;
                    }
                }
            }
        }
        else
            writeOutputMessage("INFO: Store is waiting for address register to become available.");
    }
    else
        qDebug() << "Problem with registers in instructions.";

    return notBusy;
}

bool MainWindow::fillReservationStationMults(QString op, QString resReg, QString vj, QString vk)
{
    bool notBusy = false; //true if at least one of stations is not busy, otherwise false (all busy)
    int allReady = 0;

    for (int i = 0; i < NUM_OF_MULDIV_STATIONS; i++)
    {
        if (stationsMulDiv[i].getBusy() == false)
        {
            stationsMulDiv[i].setBusy(true);
            stationsMulDiv[i].setOp(op);
            stationsMulDiv[i].setAtCycle(0);

            //OPERANDS

            int regNumberJ = 0;
            int regNumberK = 0;
            QRegularExpression regularRegister("^[xX]([0-5])$");

            //Vj
            if (regularRegister.match(vj).hasMatch())
            {
                regNumberJ = (regularRegister.match(vj).captured(1)).toInt();
                if (registers[regNumberJ].getQ() == "")
                {
                    //write value
                    stationsMulDiv[i].setVj(QString::number(registers[regNumberJ].getValue()));
                    //stationsMulDiv[i].setQj("");
                    allReady++;
                }
                else
                {
                    //write only reg name (f1, f2 ...)
                    stationsMulDiv[i].setVj(vj);
                    stationsMulDiv[i].setQj(registers[regNumberJ].getQ());
                }
            }
            else
                qDebug() << "Problem with registers in instructions.";

            //Vk
            if (regularRegister.match(vk).hasMatch())
            {
                regNumberK = (regularRegister.match(vk).captured(1)).toInt();
                if (registers[regNumberK].getQ() == "")
                {
                    //write value
                    stationsMulDiv[i].setVk(QString::number(registers[regNumberK].getValue()));
                    //stationsMulDiv[i].setQk("");
                    allReady++;
                }
                else
                {
                    //write only reg name (f1, f2 ...)
                    stationsMulDiv[i].setVk(vk);
                    stationsMulDiv[i].setQk(registers[regNumberK].getQ());
                }
            }
            else
                qDebug() << "Problem with registers in instructions.";

            if (allReady == 2)
                stationsMulDiv[i].setWorking(true);

            notBusy = true;

            markRegisterBusy(resReg, MULDIV_STATIONS_NAME, QString::number(i));

            break;
        }
        allReady = 0;
    }
    return notBusy;
}

void MainWindow::checkReservationStations()
{
    //For all busy and working reservation stations -> check if they have finished their work
    //For all busy reservation stations which are waiting for operands -> check if operands are computed in this cycle

    QList<QPair<int, QString>> cdb; //common data bus: (result, station name)

    //Step 1: Check if working stations finished and send results on CDB

    for (int i = 0; i < NUM_OF_ADDSUB_STATIONS; i++)
    {
        if (stationsAddSub[i].getBusy() &&stationsAddSub[i].getWorking())
        {
            if (stationsAddSub[i].getOp() == "add" || stationsAddSub[i].getOp() == "addi")
            {
                if (stationsAddSub[i].getAtCycle() == ADD_CLK_LATENCY)
                {
                    int result = stationsAddSub[i].calculate();
                    cdb.append(QPair<int, QString>(result, ADDSUB_STATIONS_NAME + QString::number(i)));
                }

            }
            else
            {
                if (stationsAddSub[i].getAtCycle() == SUB_CLK_LATENCY)
                {
                    int result = stationsAddSub[i].calculate();
                    cdb.append(QPair<int, QString>(result, ADDSUB_STATIONS_NAME + QString::number(i)));
                }
            }
        }
    }

    for (int i = 0; i < NUM_OF_MULDIV_STATIONS; i++)
    {
        if (stationsMulDiv[i].getBusy() && stationsMulDiv[i].getWorking())
        {
            if (stationsMulDiv[i].getOp() == "mul")
            {
                if (stationsMulDiv[i].getAtCycle() == MUL_CLK_LATENCY)
                {
                    int result = stationsMulDiv[i].calculate();
                    cdb.append(QPair<int, QString>(result, MULDIV_STATIONS_NAME + QString::number(i)));
                }

            }
            else
            {
                if (stationsMulDiv[i].getAtCycle() == DIV_CLK_LATENCY)
                {
                    //If division by zero occurs -> reset simulator
                    if (stationsMulDiv[i].getVk().toInt() == 0)
                    {
                        QMessageBox::critical(this, "Division By Zero",
                                              "Division by zero is undefined. Application is not able to continue execution. "
                                              "Click OK to reset simulator.");
                        error = true;
                    }
                    int result = stationsMulDiv[i].calculate();
                    cdb.append(QPair<int, QString>(result, MULDIV_STATIONS_NAME + QString::number(i)));
                }
            }
        }
    }

    if (error)
        return;

    for (int i = 0; i < NUM_OF_LOAD_STATIONS; i++)
    {
        if (stationsLoad[i].getBusy() && stationsLoad[i].getWorking())
        {
            if (stationsLoad[i].getAtCycle() == LOAD_CLK_LATENCY)
            {
                if (stationsLoad[i].getPort() == "A")
                    memoryPortA = false;
                else if (stationsLoad[i].getPort() == "B")
                    memoryPortB = false;
                else
                    qDebug() << "Problem in memory port logic.";

                if (stationsLoad[i].getAddr() >= MEMORY_SIZE)
                {
                    QMessageBox::critical(this, "Address out of range",
                                          "Memory address is out of range. Application is not able to continue execution. "
                                          "Click OK to reset simulator.");
                    error = true;
                }
                else
                {
                    int result = memory[stationsLoad[i].getAddr()];
                    stationsLoad[i].read();
                    cdb.append(QPair<int, QString>(result, LOAD_STATIONS_NAME + QString::number(i)));
                }
            }
        }
    }

    if (error)
        return;

    for (int i = 0; i < NUM_OF_STORE_STATIONS; i++)
    {
        if (stationsStore[i].getBusy() && stationsStore[i].getWorking())
        {
            if (stationsStore[i].getAtCycle() == LOAD_CLK_LATENCY)
            {
                if (stationsStore[i].getPort() == "A")
                    memoryPortA = false;
                else if (stationsStore[i].getPort() == "B")
                    memoryPortB = false;
                else
                    qDebug() << "Problem in memory port logic.";

                if (stationsStore[i].getAddr() >= MEMORY_SIZE)
                {
                    QMessageBox::critical(this, "Address out of range",
                                          "Memory address is out of range. Application is not able to continue execution. "
                                          "Click OK to reset simulator.");
                    error = true;
                }
                else
                {
                    memory[stationsStore[i].getAddr()] = stationsStore[i].getVj().toInt();
                    stationsStore[i].write();
                    //store does not send anything on cdb
                }
            }
        }
    }

    if (error)
        return;

    //Step 2: Registers and reservation stations which are waiting, takes what is for them

    for (int i = 0; i < NUM_OF_ADDSUB_STATIONS; i++)
    {
        if (stationsAddSub[i].getBusy() && !stationsAddSub[i].getWorking())
        {
            for (int j = 0; j < cdb.size(); j++)
            {
                if (stationsAddSub[i].getQj() == cdb[j].second)
                {
                    stationsAddSub[i].setVj(QString::number(cdb[j].first));
                    stationsAddSub[i].setQj("");
                }
                if (stationsAddSub[i].getQk() == cdb[j].second)
                {
                    stationsAddSub[i].setVk(QString::number(cdb[j].first));
                    stationsAddSub[i].setQk("");
                }
            }
            //completed reservation station
            if (stationsAddSub[i].getQj() == "" && stationsAddSub[i].getQk() == "")
                stationsAddSub[i].setWorking(true);
        }
    }

    for (int i = 0; i < NUM_OF_MULDIV_STATIONS; i++)
    {
        if (stationsMulDiv[i].getBusy() && !stationsMulDiv[i].getWorking())
        {
            for (int j = 0; j < cdb.size(); j++)
            {
                if (stationsMulDiv[i].getQj() == cdb[j].second)
                {
                    stationsMulDiv[i].setVj(QString::number(cdb[j].first));
                    stationsMulDiv[i].setQj("");
                }
                if (stationsMulDiv[i].getQk() == cdb[j].second)
                {
                    stationsMulDiv[i].setVk(QString::number(cdb[j].first));
                    stationsMulDiv[i].setQk("");
                }
            }
            //completed reservation station
            if (stationsMulDiv[i].getQj() == "" && stationsMulDiv[i].getQk() == "")
                stationsMulDiv[i].setWorking(true);
        }
    }

    for (int i = 0; i < NUM_OF_LOAD_STATIONS; i++)
    {
        if (stationsLoad[i].getBusy() && !stationsLoad[i].getWorking())
        {
            //check if memory port is available
            if (memoryPortA == false)
            {
                stationsLoad[i].setWorking(true);
                stationsLoad[i].setPort("A");
                memoryPortA = true;
            }
            else if (memoryPortB == false)
            {
                stationsLoad[i].setWorking(true);
                stationsLoad[i].setPort("B");
                memoryPortB = true;
            }
        }
    }

    for (int i = 0; i < NUM_OF_STORE_STATIONS; i++)
    {
        if (stationsStore[i].getBusy() && !stationsStore[i].getWorking())
        {
            for (int j = 0; j < cdb.size(); j++)
            {
                if (stationsStore[i].getQj() == cdb[j].second)
                {
                    stationsStore[i].setVj(QString::number(cdb[j].first));
                    stationsStore[i].setQj("");
                }
            }

            //check if reservation station is completed and memory port is available
            if (stationsStore[i].getQj() == "")
            {
                if (memoryPortA == false)
                {
                    stationsStore[i].setWorking(true);
                    stationsStore[i].setPort("A");
                    memoryPortA = true;
                }
                else if (memoryPortB == false)
                {
                    stationsStore[i].setWorking(true);
                    stationsStore[i].setPort("B");
                    memoryPortB = true;
                }
            }
        }
    }

    for (int i = 0; i < NUM_OF_REGISTERS; i++)
    {
        for (int j = 0; j < cdb.size(); j++)
        {
            if (registers[i].getQ() == cdb[j].second)
            {
                //skip registers[0] because the value of register x0 cannot be changed
                if (i != 0)
                    registers[i].setValue(cdb[j].first);
                registers[i].setQ("");
            }
        }
    }
}

void MainWindow::markRegisterBusy(QString reg, QString stationName, QString stationNumber)
{
    int regNumber = 0;
    QRegularExpression regularRegister("^[xX]([0-5])$");
    if (regularRegister.match(reg).hasMatch())
    {
        regNumber = (regularRegister.match(reg).captured(1)).toInt();
    }
    else
        qDebug() << "Problem with registers in instructions.";

    registers[regNumber].setQ(stationName + stationNumber);
}

void MainWindow::showRegValues()
{
    QString labelName;
    QLabel* targetLabel;

    for (int i = 0; i < NUM_OF_REGISTERS; i++)
    {
        labelName = "x_" + QString::number(i);
        targetLabel = ui->addsOp_0->parentWidget()->findChild<QLabel*>(labelName);
        targetLabel->setText(QString::number(registers[i].getValue()));
        labelName = "xq_" + QString::number(i);
        targetLabel = ui->addsOp_0->parentWidget()->findChild<QLabel*>(labelName);
        targetLabel->setText(registers[i].getQ());
    }
}

void MainWindow::showResStations()
{
    QString labelName;
    QLabel* targetLabel;
    QString tableName;
    QTableWidget* targetTable;

    for (int i = 0; i < NUM_OF_ADDSUB_STATIONS; i++)
    {
        if (stationsAddSub[i].getBusy())
        {
            labelName = "addsOp_" + QString::number(i);
            targetLabel = ui->addsOp_0->parentWidget()->findChild<QLabel*>(labelName);
            if (stationsAddSub[i].getOp() == "add" || stationsAddSub[i].getOp() == "addi")
                targetLabel->setText("+");
            else
                targetLabel->setText("-");
            labelName = "addsVj_" + QString::number(i);
            targetLabel = ui->addsOp_0->parentWidget()->findChild<QLabel*>(labelName);
            if (stationsAddSub[i].getQj() == "")
                targetLabel->setText(stationsAddSub[i].getVj());
            else
                targetLabel->setText(stationsAddSub[i].getQj());
            labelName = "addsVk_" + QString::number(i);
            targetLabel = ui->addsOp_0->parentWidget()->findChild<QLabel*>(labelName);
            if (stationsAddSub[i].getQk() == "")
                targetLabel->setText(stationsAddSub[i].getVk());
            else
                targetLabel->setText(stationsAddSub[i].getQk());

            if (stationsAddSub[i].getWorking())
            {
                QTableWidgetItem *add;
                if (stationsAddSub[i].getOp() == "add" || stationsAddSub[i].getOp() == "addi")
                    add = new QTableWidgetItem("+");
                else
                    add = new QTableWidgetItem("-");
                add->setTextAlignment(Qt::AlignCenter);
                add->setBackground(QBrush(QColor(Qt::red).lighter(160)));

                tableName = "add_" + QString::number(i);
                targetTable = ui->addsOp_0->parentWidget()->findChild<QTableWidget*>(tableName);
                targetTable->setItem(0, 0, add);
            }
            else
            {
                QTableWidgetItem *add = new QTableWidgetItem("+ -");;
                add->setTextAlignment(Qt::AlignCenter);
                add->setBackground(QBrush(QColor(Qt::white).lighter(160)));

                tableName = "add_" + QString::number(i);
                targetTable = ui->addsOp_0->parentWidget()->findChild<QTableWidget*>(tableName);
                targetTable->setItem(0, 0, add);
            }
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

            QTableWidgetItem *add = new QTableWidgetItem("+ -");;
            add->setTextAlignment(Qt::AlignCenter);
            add->setBackground(QBrush(QColor(Qt::white).lighter(160)));

            tableName = "add_" + QString::number(i);
            targetTable = ui->addsOp_0->parentWidget()->findChild<QTableWidget*>(tableName);
            targetTable->setItem(0, 0, add);
        }
    }

    for (int i = 0; i < NUM_OF_LOAD_STATIONS; i++)
    {
        if (stationsLoad[i].getBusy())
        {
            labelName = "load_" + QString::number(i);
            targetLabel = ui->addsOp_0->parentWidget()->findChild<QLabel*>(labelName);
            targetLabel->setText(QString::number(stationsLoad[i].getAddr()));
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
            if (stationsMulDiv[i].getOp() == "mul")
                    targetLabel->setText("*");
                else
                    targetLabel->setText("/");
            labelName = "mulsVj_" + QString::number(i);
            targetLabel = ui->addsOp_0->parentWidget()->findChild<QLabel*>(labelName);
            if (stationsMulDiv[i].getQj() == "")
                targetLabel->setText(stationsMulDiv[i].getVj());
            else
                targetLabel->setText(stationsMulDiv[i].getQj());
            labelName = "mulsVk_" + QString::number(i);
            targetLabel = ui->addsOp_0->parentWidget()->findChild<QLabel*>(labelName);
            if (stationsMulDiv[i].getQk() == "")
                targetLabel->setText(stationsMulDiv[i].getVk());
            else
                targetLabel->setText(stationsMulDiv[i].getQk());

            if (stationsMulDiv[i].getWorking())
            {
                QTableWidgetItem *mul;
                if (stationsMulDiv[i].getOp() == "mul")
                    mul = new QTableWidgetItem("*");
                else
                    mul = new QTableWidgetItem("/");
                mul->setTextAlignment(Qt::AlignCenter);
                mul->setBackground(QBrush(QColor(Qt::red).lighter(160)));

                tableName = "mul_" + QString::number(i);
                targetTable = ui->addsOp_0->parentWidget()->findChild<QTableWidget*>(tableName);
                targetTable->setItem(0, 0, mul);
            }
            else
            {
                QTableWidgetItem *mul = new QTableWidgetItem("* /");;
                mul->setTextAlignment(Qt::AlignCenter);
                mul->setBackground(QBrush(QColor(Qt::white).lighter(160)));

                tableName = "mul_" + QString::number(i);
                targetTable = ui->addsOp_0->parentWidget()->findChild<QTableWidget*>(tableName);
                targetTable->setItem(0, 0, mul);
            }
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

            QTableWidgetItem *mul = new QTableWidgetItem("* /");;
            mul->setTextAlignment(Qt::AlignCenter);
            mul->setBackground(QBrush(QColor(Qt::white).lighter(160)));

            tableName = "mul_" + QString::number(i);
            targetTable = ui->addsOp_0->parentWidget()->findChild<QTableWidget*>(tableName);
            targetTable->setItem(0, 0, mul);
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
            targetLabel->setText(QString::number(stationsStore[i].getAddr()));
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
        data->setTextAlignment(Qt::AlignCenter);
        ui->memoryTable->setItem(i, 1, data);
    }
    QTableWidgetItem *portA = new QTableWidgetItem("Port A");
    portA->setTextAlignment(Qt::AlignCenter);
    if (memoryPortA)
        portA->setBackground(QBrush(QColor(Qt::red).lighter(160)));
    else
        portA->setBackground(QBrush(QColor(Qt::white)));
    ui->portTable->setItem(0, 0, portA);

    QTableWidgetItem *portB = new QTableWidgetItem("Port B");
    portB->setTextAlignment(Qt::AlignCenter);
    if (memoryPortB)
        portB->setBackground(QBrush(QColor(Qt::red).lighter(160)));
    else
        portB->setBackground(QBrush(QColor(Qt::white)));
    ui->portTable->setItem(0, 1, portB);
}

void MainWindow::resetProcessor()
{
    //reset stations
    for (int i = 0; i < NUM_OF_ADDSUB_STATIONS; i++)
    {
        stationsAddSub[i].setBusy(false);
        stationsAddSub[i].setOp("");
        stationsAddSub[i].setVj("");
        stationsAddSub[i].setQj("");
        stationsAddSub[i].setVk("");
        stationsAddSub[i].setQk("");
        stationsAddSub[i].setAtCycle(0);
        stationsAddSub[i].setWorking(false);
    }
    for (int i = 0; i < NUM_OF_LOAD_STATIONS; i++)
    {
        stationsLoad[i].setBusy(false);
        stationsLoad[i].setAddr(0);
        stationsLoad[i].setAtCycle(0);
        stationsLoad[i].setWorking(false);
        stationsLoad[i].setPort("");
    }
    for (int i = 0; i < NUM_OF_MULDIV_STATIONS; i++)
    {
        stationsMulDiv[i].setBusy(false);
        stationsMulDiv[i].setOp("");
        stationsMulDiv[i].setVj("");
        stationsMulDiv[i].setQj("");
        stationsMulDiv[i].setVk("");
        stationsMulDiv[i].setQk("");
        stationsMulDiv[i].setAtCycle(0);
        stationsMulDiv[i].setWorking(false);
    }
    for (int i = 0; i < NUM_OF_STORE_STATIONS; i++)
    {
        stationsStore[i].setBusy(false);
        stationsStore[i].setAddr(0);
        stationsStore[i].setVj("");
        stationsStore[i].setQj("");
        stationsStore[i].setAtCycle(0);
        stationsStore[i].setWorking(false);
        stationsStore[i].setPort("");
    }

    //reset registers
    for (int i = 0; i < NUM_OF_REGISTERS; i++)
    {
        registers[i].setQ("");
        registers[i].setValue(0);
    }

    //reset memory
    for (int i = 0; i < MEMORY_SIZE; i++)
        memory[i] = 0;

    memoryPortA = false;
    memoryPortB = false;
}

void MainWindow::writeOutputMessage(QString mess)
{
    //qDebug() << mess;
    ui->compileOutput->append(mess);
}

void MainWindow::on_codeTextEdit_textChanged()
{
    QTextCursor cursor = ui->codeTextEdit->textCursor();

    QList<QTextEdit::ExtraSelection> oldSelections = ui->codeTextEdit->extraSelections();
    QTextEdit::ExtraSelection selection;
    QColor lineColor(QColor(Qt::white).lighter(160));// set the background color to white
    selection.format.setBackground(lineColor);
    selection.cursor = cursor;
    selection.cursor.select(QTextCursor::LineUnderCursor);

    // Apply the background color change to the QTextEdit widget
    oldSelections.append(selection);
    ui->codeTextEdit->setExtraSelections(oldSelections);
}


/********************************************************
Numbers on code editor
********************************************************/

//QTextDocument* document;
//QTextDocument* document_new;
//int blockCount = 0;
/*
    QString text = ui->codeTextEdit->toPlainText();
    QStringList linesText = text.split(QLatin1Char('\n'));

    document = ui->codeTextEdit->document();
    int blockCount = document_new->blockCount();



        QRegularExpression regularInstruction1("^([0-9]+:[ ])(.*)\\s*$");
        QRegularExpression regularInstruction2("^([0-9]*:)(.*)\\s*$");

        for (int i = 0; i < blockCount; i++) {
            int lineNumber = i + 1;
            QString lineNumberText = QString::number(lineNumber) + ": ";

            QTextCursor cursor = ui->codeTextEdit->textCursor();
            cursor.movePosition(QTextCursor::Start);
            cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, i); // select line i+1

            if (regularInstruction1.match(linesText[i]).hasMatch())   //ok
            {
                qDebug() << "ok";
            }
            else if (regularInstruction1.match(linesText[i]).hasMatch()) //problem
            {
                cursor.select(QTextCursor::LineUnderCursor);
                cursor.removeSelectedText();
                cursor.insertText(lineNumberText + regularInstruction1.match(linesText[i]).captured(2));
                qDebug() << "ok k";
            }
            else    //problem
            {
                cursor.select(QTextCursor::LineUnderCursor);
                cursor.removeSelectedText();
                cursor.insertText(lineNumberText);
                qDebug() << "prob";
            }



    }

        document_new = ui->codeTextEdit->document();
        int blockCount_new = document_new->blockCount();
        //QTextCursor cursor = ui->codeTextEdit->textCursor();
        QString text = ui->codeTextEdit->toPlainText();


        QRegularExpression regularInstruction1("^([0-9]+:[ ])(.*)\\s*$"); //correct line
        QRegularExpression regularInstruction2("^([0-9]*:)(.*)\\s*$"); //line changed with backspace or delete
        QRegularExpression regularInstruction3("^([^ ]*)\\s*$"); //line without number


            blockCount = blockCount_new;
            document = document_new;

            QStringList linesText = text.split(QLatin1Char('\n'));

            for (int i = 0; i < blockCount; i++) {
                int lineNumber = i + 1;
                QString lineNumberText = QString::number(lineNumber) + ": ";

                QTextCursor cursor = ui->codeTextEdit->textCursor();
                cursor.movePosition(QTextCursor::Start);
                cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, i); // select line i+1

                if (regularInstruction1.match(linesText[i]).hasMatch())   //new line with old line number
                {
                    qDebug() << "ok";
                }
                else if (regularInstruction2.match(linesText[i]).hasMatch())
                {
                    cursor.select(QTextCursor::LineUnderCursor);
                    cursor.removeSelectedText();
                    cursor.insertText(lineNumberText + regularInstruction2.match(linesText[i]).captured(2));
                    qDebug() << "ok solved backspace";
                }
                else if (regularInstruction3.match(linesText[i]).hasMatch())
                {
                    cursor.select(QTextCursor::LineUnderCursor);
                    cursor.removeSelectedText();
                    cursor.insertText(lineNumberText + regularInstruction3.match(linesText[i]).captured(2));
                    qDebug() << "ok solved";
                }
                else    //new line without line number
                {

                    cursor.select(QTextCursor::LineUnderCursor);
                    cursor.removeSelectedText();
                    cursor.insertText(lineNumberText);
                    qDebug() << "prob";
                }

            }

        text = ui->codeTextEdit->toPlainText();

        cursor = ui->codeTextEdit->textCursor();

        int currentPostion = cursor.positionInBlock();

        // Find the position of the ":" character on the current line
        cursor.movePosition(QTextCursor::StartOfLine);
        int numPos = text.indexOf(':', cursor.positionInBlock());
        int numPosAll = text.indexOf(':', cursor.position());

        if (currentPostion <= numPos + 1)
        {
            //cursor.movePosition(QTextCursor::Start);
            //cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, numPos + 1);
            cursor.setPosition(numPosAll + 2, QTextCursor::MoveAnchor);
            ui->codeTextEdit->setTextCursor(cursor);
        }
*/
