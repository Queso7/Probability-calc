#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

// mainwindow.h
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    QWidget* createCombinatoricsPage();
    QWidget* createProbabilityPage();
    QWidget* createRandomVar();
    QWidget* createDistributionFunc();
};
#endif