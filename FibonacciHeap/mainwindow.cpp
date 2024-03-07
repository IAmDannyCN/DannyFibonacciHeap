#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPainter>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::mainwindow)
{
    ui->setupUi(this);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    /*
    // 创建 QPainter 对象
    QPainter painter(this);

    // 设置画笔和画刷
    painter.setPen(Qt::NoPen); // 无边框
    painter.setBrush(Qt::green); // 绿色画刷

    // 计算圆的中心位置
    int centerX = width() / 2;
    int centerY = height() / 2;

    // 画实心圆
    int radius = 10;
    painter.drawEllipse(centerX - radius, centerY - radius, 2 * radius, 2 * radius);
    */

    double xscale = (double)ui->widget->width()/(20 + (double)nodes.maxx);
    double yscale = (double)ui->widget->height()/(20 + (double)nodes.maxy);
    double scale = min(xscale, yscale);

    double xmiddle = (nodes.maxx + 20) / 2.0;
    double ymiddle = (nodes.maxy + 10) / 2.0;

    double xbase=ui->widget->width() / 2.0;
    double ybase=ui->widget->height() / 2.0 + 50;

    // cout << "ui: " << ui->widget->width() <<","<< ui->widget->height() << endl;
    // cout << "my: " << nodes.maxx <<","<< nodes.maxy << endl;
    // cout << xscale <<" "<< yscale <<" "<< scale << endl;
    // cout << xbase <<" "<< ybase << endl;
    // cout << xmiddle <<" "<< ymiddle << endl;

    QPainter painter(this);

    painter.setPen(QPen(QColor(Qt::black), 3));
    for(LINE line: lines.lineList)
    {
        int x1 = (line.x1 - xmiddle) * scale + xbase;
        int x2 = (line.x2 - xmiddle) * scale + xbase;
        int y1 = (line.y1 - ymiddle) * scale + ybase;
        int y2 = (line.y2 - ymiddle) * scale + ybase;
        painter.drawLine(x1, y1, x2, y2);
    }

    for(NODE node: nodes.nodeList)
    {
        int x = (node.x - xmiddle) * scale + xbase;
        int y = (node.y - ymiddle) * scale + ybase;
        painter.setPen(QPen(QColor(node.isMinRoot?(Qt::red):(Qt::black)), 3));
        painter.setBrush(QColor(node.marked?(Qt::black):(Qt::white)));
        painter.drawEllipse(x - 15, y - 15, 30, 30);

        QString text = QString::number(node.val);
        int bias = (text.length() - 1) * 4;
        painter.setPen(QPen(node.marked?(Qt::white):(Qt::black)));
        painter.setFont(QFont("Consolas", 12));
        painter.drawText(x - 5 - bias, y + 5, text);
    }
}

void MainWindow::GetParameter()
{
    string heapid_s = ui->heapid_content->text().toStdString();
    string para1_s = ui->para1_content->text().toStdString();
    string para2_s = ui->para2_content->text().toStdString();

    try {
        heapid = stoi(heapid_s);
    } catch (const invalid_argument&) {
        heapid = 0;
    } catch (const out_of_range&) {
        heapid = 0;
    }

    try {
        para1 = stoi(para1_s);
    } catch (const invalid_argument&) {
        para1 = 0;
    } catch (const out_of_range&) {
        para1 = 0;
    }

    try {
        para2 = stoi(para2_s);
    } catch (const invalid_argument&) {
        para2 = 0;
    } catch (const out_of_range&) {
        para2 = 0;
    }
}

void MainWindow::UpdateWidget()
{   
    nodes.clear();
    lines.clear();
    srand((unsigned int)time(NULL));
    string potentialinfo = GetPaint(nodes, lines);
    ui->textBrowser->append(QString::fromStdString(potentialinfo));
    ui->widget->update();
}

void MainWindow::on_Insert_button_clicked()
{
    GetParameter();
    try {
        Insert(heapid, para1);
        ui->textBrowser->setPlainText("Insert OK.");
        UpdateWidget();
    } catch(const string& message) {
        ui->textBrowser->setPlainText(QString::fromStdString(message));
    }
}

void MainWindow::on_DeleteMin_button_clicked()
{
    GetParameter();
    try {
        DeleteMin(heapid);
        ui->textBrowser->setPlainText("DeleteMin OK.");
        UpdateWidget();
    } catch(const string& message) {
        ui->textBrowser->setPlainText(QString::fromStdString(message));
    }
}

void MainWindow::on_Decrease_button_clicked()
{
    GetParameter();
    try {
        DecreaseKey(heapid, para1, para2);
        ui->textBrowser->setPlainText("DecreaseKey OK.");
        UpdateWidget();
    } catch(const string& message) {
        ui->textBrowser->setPlainText(QString::fromStdString(message));
    }
}

void MainWindow::on_Delete_button_clicked()
{
    GetParameter();
    try {
        Delete(heapid, para1);
        ui->textBrowser->setPlainText("Delete OK.");
        UpdateWidget();
    } catch(const string& message) {
        ui->textBrowser->setPlainText(QString::fromStdString(message));
    }
}

void MainWindow::on_Union_button_clicked()
{
    GetParameter();
    try {
        Union(para1, para2);
        ui->textBrowser->setPlainText("Union OK.");
        UpdateWidget();
    } catch(const string& message) {
        ui->textBrowser->setPlainText(QString::fromStdString(message));
    }
}

void MainWindow::on_Help_button_clicked()
{
    string message = "Fibonacci heap with GUI.\n\"Insert\" will insert value [para1] into heap No. [HeapID];\n\"DeleteMin\" will delete the minimal value in heap No. [HeapID];\n\"Delete\" will delete one node with value [para1] from heap No. [HeapID];\n\"Decease\" will decrease [para2] from the value of a node with value [para1] in heap No. [HeapID];\n\"Union\" will union two heaps No. [para1] and [para2].\nHeapID and parameters should be valid integers, or will be seen as 0;\nIn the GUI, black nodes are marked, white ones are unmarked. The node with red frame is the MinRoot in a heap.";
    ui->textBrowser->setPlainText(QString::fromStdString(message));
}

void MainWindow::on_About_button_clicked()
{
    string message = "Fibonacci heap with GUI.\nProject by I_Am_Danny for Data Structure(2023 Fall), NJU.\nWith credit of Dev-Cpp, Qt6 and Windows 11.";
    ui->textBrowser->setPlainText(QString::fromStdString(message));
}

void MainWindow::on_Load_button_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Open FibonacciHeap File", "", "DannyFibonacciHeap files (*.dfh)");
    if (filePath.isEmpty())
        return;

    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        try {
            LoadHeap(QTextStream(&file));
            ui->textBrowser->setPlainText("Load OK.");
            UpdateWidget();
        } catch(const string& message) {
            ui->textBrowser->setPlainText(QString::fromStdString(message));
        }
        file.close();
    }
    else
        ui->textBrowser->setPlainText("Failed to open the file.");
}

void MainWindow::on_Save_button_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Open FibonacciHeap File", "", "DannyFibonacciHeap files (*.dfh)");
    if (filePath.isEmpty())
        return;

    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        try {
            SaveHeap(QTextStream(&file));
            ui->textBrowser->setPlainText("Save OK.");
            UpdateWidget();
        } catch(const string& message) {
            ui->textBrowser->setPlainText(QString::fromStdString(message));
        }
        file.close();
    }
    else
        ui->textBrowser->setPlainText("Failed to open the file.");
}

