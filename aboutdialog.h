#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>

namespace Ui {
class aboutDialog;
}

class aboutDialog : public QDialog
{
    Q_OBJECT

public:
    explicit aboutDialog(QWidget *parent = 0);
    ~aboutDialog();
protected:
    void mousePressEvent(QMouseEvent *event);
private:
    Ui::aboutDialog *ui;
};

#endif // ABOUTDIALOG_H
