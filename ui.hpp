#pragma once

#include <QWidget>
#include <QLineEdit>
#include <QVBoxLayout>

class UI : public QWidget
{
    Q_OBJECT

public:
    UI(QWidget *parent = 0);
    virtual ~UI() = default;

private:

    QLineEdit* _calcBox;
    QVBoxLayout* _mainLayout;

    void SetupDisplayLayout();
    void SetupKeyboardLayout();
    void EqualsPressed();
};
