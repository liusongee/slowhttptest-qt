#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QProcess>
#include <QTextCodec>
#include <QCloseEvent>
#include <QTimer>
#include "stdlib.h"
#include "stdio.h"
//#include "QDebug"
#include <QTextStream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    flag_startrun = false;
    out_update_timer= new QTimer(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_start_btn_clicked()
{
    if (ui->start_btn->text() == "stop")
    {
        process->write("quit");
        process->kill();
        ui->start_btn->setText("start");
        return;
    }

    process = new QProcess(this);
    QStringList arg;

    connect(process, SIGNAL(started()), this, SLOT(start_process()));
    connect(process, SIGNAL(readyReadStandardOutput()), this, SLOT(start_read_output()));
    connect(process, SIGNAL(readyReadStandardError()), this, SLOT(start_read_err_output()));
    connect(process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(finish_process(int,QProcess::ExitStatus)));

    connect(out_update_timer, SIGNAL(timeout()), this, SLOT(update_processbar()));

    parseArguments(arg);
  //  qDebug()<<"./slowhttptest-bin/slowhttptest" << " " << arg<<endl;
    process->start("slowhttptest",arg);
  //  process->start("./slowhttptest-bin/slowhttptest",arg);

    if (false == process->waitForStarted())
    {
        ui->output_tb->append("this process can not be called.");
    }

    ui->start_btn->setText("stop");
}

void MainWindow::parseArguments(QStringList &arg)
{
    //interval time and test time
    interval_time = ui->interval_time_le->text().toInt(0,10);
    test_time = ui->test_time_le->text().toInt(0,10);
 //   qDebug()<<interval_time<<" "<<test_time<<endl;

    arg.clear();
    //general options
    arg << "-u"<< ui->serveraddr_ledit->text()+":"+ui->port_ledit->text();
    arg << "-c" << ui->num_conn_le->text();
    arg << "-i"  << ui->interval_time_le->text();
    arg << "-l"  << ui->test_time_le->text();
    arg << "-r" << ui->rate_le->text();
    arg << "-s" << ui->content_len_le->text();
    //test modes
    switch (ui->testmode_combox->currentIndex()) {
    case 0:
        arg << "-H";break;
    case 1:
        arg << "-B"; break;
    case 2:
        arg << "-R"; break;
    case 3:
        arg << "-X"; break;
    default:
        break;
    }
    //report options
    if(ui->report_checkbox->isChecked())
        arg << "-g";
    //probe/proxy options
}

void MainWindow::start_process()
{
    ui->progressBar->setRange(0,test_time);
    ui->progressBar->setValue(0);
  //  out_update_timer->start(1000);
    ui->output_tb->append(tr("process start"));
}

void MainWindow::finish_process(int exitcode,QProcess::ExitStatus exitStatus)
{
    out_update_timer->stop();
    if (exitStatus == QProcess::NormalExit)
    {
        ui->output_tb->append(tr("process exit normal"));
    }
    else
    {
        ui->output_tb->append(tr("process exit crash"));
    }

    ui->output_tb->append("process finised");
    ui->start_btn->setText("start");
    ui->progressBar->setValue(test_time);
}

void MainWindow::start_read_output()
{
    ui->progressBar->setValue(10+ui->progressBar->value());
    QByteArray ba = process->readAllStandardOutput();
    ui->output_tb->append(ba);
/*
    QFile outputfile("/home/liusong/tmp.txt");
    if(outputfile.open( QIODevice::WriteOnly)){
        QTextStream out(&outputfile);
        out<<ba<<endl;
        outputfile.close();
    }else{
        qDebug()<<"cannot open /home/liusong/tmp.txt"<<endl;
    }
    */
}

void MainWindow::start_read_err_output()
{
    QByteArray ba = process->readAllStandardError();
    ui->output_tb->append("error: " + ba);
}

void MainWindow::update_processbar()
{
    //ui->progressBar->valueChanged(100/5);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    process->write("quit");
    process->kill();
    if(!process->waitForFinished(100))
        return;
    event->accept();
}
