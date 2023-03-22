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

int algorithmStep = 0;
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
    ui->resetButton->setEnabled(true);
}

void MainWindow::compileProgram()
{
    code = ui->textEdit->toPlainText();
    nextInstruction = 0;

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
        use registers[i].setBusy(true);
        ********************************************************/

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
    initAlgorithm();
    ui->startButton->setEnabled(false);
    ui->clkButton->setEnabled(true);
}


void MainWindow::on_resetButton_clicked()
{
    cleanFields();
    nextInstruction = 0;
    ui->startButton->setEnabled(true);
    ui->clkButton->setEnabled(false);

}

void MainWindow::cleanFields()
{
    QString label_name;
    QLabel* targetLabel;

    for (int i = 0; i < QUEUE_SIZE; i++)
    {
        label_name = "queue_" + QString::number(i);
        targetLabel = ui->addsOp_0->parentWidget()->findChild<QLabel*>(label_name);
        targetLabel->setText("");
    }

    for (int i = 0; i < NUM_OF_ADDSUB_STATIONS; i++)
    {
        label_name = "addsOp_" + QString::number(i);
        targetLabel = ui->addsOp_0->parentWidget()->findChild<QLabel*>(label_name);
        targetLabel->setText("");
        label_name = "addsVj_" + QString::number(i);
        targetLabel = ui->addsOp_0->parentWidget()->findChild<QLabel*>(label_name);
        targetLabel->setText("");
        label_name = "addsVk_" + QString::number(i);
        targetLabel = ui->addsOp_0->parentWidget()->findChild<QLabel*>(label_name);
        targetLabel->setText("");
    }

    for (int i = 0; i < NUM_OF_LOAD_STATIONS; i++)
    {
        label_name = "load_" + QString::number(i);
        targetLabel = ui->addsOp_0->parentWidget()->findChild<QLabel*>(label_name);
        targetLabel->setText("");
    }

    for (int i = 0; i < NUM_OF_MULDIV_STATIONS; i++)
    {
        label_name = "mulsOp_" + QString::number(i);
        targetLabel = ui->addsOp_0->parentWidget()->findChild<QLabel*>(label_name);
        targetLabel->setText("");
        label_name = "mulsVj_" + QString::number(i);
        targetLabel = ui->addsOp_0->parentWidget()->findChild<QLabel*>(label_name);
        targetLabel->setText("");
        label_name = "mulsVk_" + QString::number(i);
        targetLabel = ui->addsOp_0->parentWidget()->findChild<QLabel*>(label_name);
        targetLabel->setText("");
    }

    for (int i = 0; i < NUM_OF_STORE_STATIONS; i++)
    {
        label_name = "storeReg_" + QString::number(i);
        targetLabel = ui->addsOp_0->parentWidget()->findChild<QLabel*>(label_name);
        targetLabel->setText("");
        label_name = "storeAddr_" + QString::number(i);
        targetLabel = ui->addsOp_0->parentWidget()->findChild<QLabel*>(label_name);
        targetLabel->setText("");
    }

    for (int i = 0; i < NUM_OF_REGISTERS; i++)
    {
        label_name = "f_" + QString::number(i);
        targetLabel = ui->addsOp_0->parentWidget()->findChild<QLabel*>(label_name);
        targetLabel->setText(QString::number(registers[i].getValue()));
    }
}

/********************************************************
Following functions should fill labels in reservations stations
Pay attention on register values (which values are ready)!!
********************************************************/

bool MainWindow::fillReservationStationAdders(QString op, QString vj, QString vk)
{
    bool notBusy = false; //true if at least one of stations is not busy, otherwise false (all busy)
    QString label_name;
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
            label_name = "addsOp_" + QString::number(i);
            targetLabel = ui->addsOp_0->parentWidget()->findChild<QLabel*>(label_name);
            targetLabel->setText(op);

            //OPERANDS

            int regNumberJ = 0;
            int regNumberK = 0;
            /********************************************************
            use regular expresions to take number of register
            //register labels: f_0, f_1, f_2, f_3
            regNumberJ = ...
            regNumberK = ...

            values in registers (setValue) is happening in check res station function)
            here is only label writing
            ********************************************************/
            regBusy = registers[regNumberJ].getBusy();
            label_name = "addsVj_" + QString::number(i);
            targetLabel = ui->addsOp_0->parentWidget()->findChild<QLabel*>(label_name);
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

            label_name = "addsVk_" + QString::number(i);
            targetLabel = ui->addsOp_0->parentWidget()->findChild<QLabel*>(label_name);
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
            break;
        }
    }
    return notBusy;
}

bool MainWindow::fillReservationStationLoads(QString addrReg, QString imm)
{
    bool notBusy = false; //true if at least one of stations is not busy, otherwise false (all busy)
    QString label_name;
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
            /********************************************************
            use regular expresions to take number of register
            //register labels: f_0, f_1, f_2, f_3
            regNumber = ...

            values in registers (setValue) is happening in check res station function)
            here is only label writing
            ********************************************************/
            regBusy = registers[regNumber].getBusy();
            label_name = "load_" + QString::number(i);
            targetLabel = ui->addsOp_0->parentWidget()->findChild<QLabel*>(label_name);
            if (regBusy)
            {
                //write reg name
                targetLabel->setText(imm + "(" + addrReg + ")");
            }
            else
            {
                //calculate address and write value
                targetLabel->setText(QString::number(stationsLoad[i].getAddr()));
                stationsLoad[i].setAddr(imm.toInt() + registers[regNumber].getValue());
            }

            notBusy = true;
            break;
        }
    }
    return notBusy;
}

bool MainWindow::fillReservationStationStores(QString vj, QString addrReg, QString imm)
{
    bool notBusy = false; //true if at least one of stations is not busy, otherwise false (all busy)
    QString label_name;
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
            /********************************************************
            use regular expresions to take number of register
            //register labels: f_0, f_1, f_2, f_3
            regNumber = ...

            values in registers (setValue) is happening in check res station function)
            here is only label writing
            ********************************************************/
            regBusy = registers[regNumber].getBusy();
            label_name = "storeAddr_" + QString::number(i);
            targetLabel = ui->addsOp_0->parentWidget()->findChild<QLabel*>(label_name);
            if (regBusy)
            {
                //write reg name
                targetLabel->setText(imm + "(" + addrReg + ")");
            }
            else
            {
                //calculate address and write value
                stationsStore[i].setAddr(imm.toInt() + registers[regNumber].getValue());
                targetLabel->setText(QString::number(stationsStore[i].getAddr()));
            }
            regBusy = registers[regNumberJ].getBusy();
            label_name = "storeReg_" + QString::number(i);
            targetLabel = ui->addsOp_0->parentWidget()->findChild<QLabel*>(label_name);
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

bool MainWindow::fillReservationStationMults(QString op, QString vj, QString vk)
{
    bool notBusy = false; //true if at least one of stations is not busy, otherwise false (all busy)
    QString label_name;
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
            label_name = "mulsOp_" + QString::number(i);
            targetLabel = ui->addsOp_0->parentWidget()->findChild<QLabel*>(label_name);
            targetLabel->setText(op);

            //OPERANDS

            int regNumberJ = 0;
            int regNumberK = 0;
            /********************************************************
            use regular expresions to take number of register
            //register labels: f_0, f_1, f_2, f_3
            regNumberJ = ...
            regNumberK = ...

            values in registers (setValue) is happening in check res station function)
            here is only label writing
            ********************************************************/
            regBusy = registers[regNumberJ].getBusy();
            label_name = "mulsVj_" + QString::number(i);
            targetLabel = ui->addsOp_0->parentWidget()->findChild<QLabel*>(label_name);
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

            label_name = "mulsVk_" + QString::number(i);
            targetLabel = ui->addsOp_0->parentWidget()->findChild<QLabel*>(label_name);
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
            break;
        }
    }
    return notBusy;
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
