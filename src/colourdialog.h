#ifndef COLOURDIALOG_H
#define COLOURDIALOG_H

#include <QDialog>

namespace Ui {
class colourdialog;
}

class colourdialog : public QDialog
{
    Q_OBJECT

public:
    explicit colourdialog(QWidget *parent = nullptr);
    ~colourdialog();

    QColor selectedColour() const;

private:
    Ui::colourdialog *ui;
};

#endif // COLOURDIALOG_H
