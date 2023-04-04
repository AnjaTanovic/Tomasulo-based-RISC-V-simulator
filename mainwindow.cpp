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

int clkCycle = 0;
QStringList instructionsString;
QString code;
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
bool memoryPortA;
bool memoryPortB;
bool divisionByZero = false;

/********************************************************
compile is not real compiling, change word
********************************************************/

/********************************************************
Draw a diagram or use tables for showing stations
********************************************************/

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
    //ui->memoryTable->horizontalHeader()->setVisible(false);
    QStringList memColumns = { "Address", "Value"};
    ui->memoryTable->setHorizontalHeaderLabels(memColumns);
    ui->memoryTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    for (int i = 0; i < MEMORY_SIZE; i++)
    {
        memory[i] = 0;
        QTableWidgetItem *addr = new QTableWidgetItem(QString::number(i) + " :");
        QTableWidgetItem *data = new QTableWidgetItem("0");
        ui->memoryTable->setItem(i, 0, addr);
        ui->memoryTable->setItem(i, 1, data);
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
    divisionByZero = false;

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
        QRegularExpression regularInstruction1("^\\s*(add|sub|mul|div|ADD|SUB|MUL|DIV)\\s+([xX][0-5])\\s*,\\s*([xX][0-5])\\s*,\\s*([xX][0-5])\\s*$");
        QRegularExpression regularInstruction2("^\\s*(sd|ld|SD|LD)\\s+([xX][0-5])\\s*,\\s*([0-9]+)\\s*,\\s*([xX][0-5])\\s*$");
        QRegularExpression regularInstruction3("^\\s*(sd|ld|SD|LD)\\s+([xX][0-5])\\s*,\\s*([0-9]+)[(]\\s*([xX][0-5])\\s*[)]\\s*$");
        QRegularExpression regularInstruction4("^\\s*(addi|ADDI)\\s+([xX][0-5])\\s*,\\s*([xX][0-5])\\s*,\\s*([0-9]+)\\s*$");

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
		else
		{
			qDebug() << "All reservation stations for next instruction in queue are busy -> stall.";
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

    if (divisionByZero)
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
        if (divisionByZero)
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
    ui->compile_label->setText("");
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
    ui->compile_label->setText("");
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
                qDebug() << "Load is waiting for stores with same addresses to finish (preventing hazards).";
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
            qDebug() << "Load is waiting for address register to become available.";
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
                qDebug() << "Store is waiting for loads and stores with same addresses to finish (preventing hazards).";
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
            qDebug() << "Load is waiting for address register to become available.";
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

    /********************************************************
    Analyze war waw rar and raw hazards
    ********************************************************/

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
                    if (stationsMulDiv[i].getQk().toInt() == 0)
                    {
                        QMessageBox::critical(this, "Division By Zero",
                                              "Division by zero is undefined. Application is not able to continue execution. "
                                              "Click OK to reset simulator.");
                        divisionByZero = true;
                    }
                    int result = stationsMulDiv[i].calculate();
                    cdb.append(QPair<int, QString>(result, MULDIV_STATIONS_NAME + QString::number(i)));
                }
            }
        }
    }

    if (divisionByZero)
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

                int result = memory[stationsLoad[i].getAddr()];
                stationsLoad[i].read();
                cdb.append(QPair<int, QString>(result, LOAD_STATIONS_NAME + QString::number(i)));
            }
        }
    }

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

                memory[stationsStore[i].getAddr()] = stationsStore[i].getVj().toInt();
                stationsStore[i].write();
                //store does not send anything on cdb
            }
        }
    }

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

    /********************************************************
    This logic will prioritize loads for memory (make these accesses fair)
    ********************************************************/

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

    for (int i = 0; i < NUM_OF_ADDSUB_STATIONS; i++)
    {
        if (stationsAddSub[i].getBusy())
        {
            labelName = "addsOp_" + QString::number(i);
            targetLabel = ui->addsOp_0->parentWidget()->findChild<QLabel*>(labelName);
            if (stationsAddSub[i].getOp() == "add")
                targetLabel->setText("+");
            else
                targetLabel->setText("-");
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

