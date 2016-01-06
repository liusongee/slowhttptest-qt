#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QProcess>
#include <QTextCodec>
#include <QCloseEvent>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QProcess* process;
    QTimer *out_update_timer;
    int interval_time;
    int test_time;

protected:
    void closeEvent(QCloseEvent *);
    void parseArguments(QStringList &arg);

private slots:
    void on_start_btn_clicked();

    void start_process();
    void start_read_output();
    void start_read_err_output();
    void finish_process(int exitcode, QProcess::ExitStatus exitStatus);

    void update_processbar();

private:
    Ui::MainWindow *ui;
    bool flag_startrun;
};

#endif // MAINWINDOW_H
