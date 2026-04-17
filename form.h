#ifndef FORM_H
#define FORM_H

#include <QWidget>

namespace Ui {
class Form;
}

class Form : public QWidget
{
    Q_OBJECT

public:
    explicit Form(QWidget *parent = nullptr);
    ~Form();

private slots:
    void on_settingsButton_clicked();

    void on_disconnectpushButton_clicked();

private:
    Ui::Form *ui;
};

#endif // FORM_H
