#include "aboutdialog.h"
#include "ui_aboutdialog.h"

aboutDialog::aboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::aboutDialog)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("3VCam"));
    setWindowFlags(this->windowFlags() & ~Qt::WindowMaximizeButtonHint);
    setWindowFlags(Qt::FramelessWindowHint);
    setFixedSize(this->width(), this->height());
}

aboutDialog::~aboutDialog()
{
    delete ui;
}


void aboutDialog::mousePressEvent(QMouseEvent *event)
{
    close();
}
