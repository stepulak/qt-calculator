#include "ui.hpp"
#include "calculator.hpp"

#include <QBoxLayout>
#include <QMessageBox>
#include <QPushButton>
#include <QAction>

UI::UI(QWidget *parent)
    : QWidget(parent) {
    _mainLayout = new QVBoxLayout();
    SetupDisplayLayout();
    SetupKeyboardLayout();
    this->setLayout(_mainLayout);
}

void UI::SetupDisplayLayout() {
    auto font = QFont("Arial", 25, QFont::Bold);
    _calcBox = new QLineEdit();
    _calcBox->setFont(font);
    _calcBox->setFixedWidth(450);
    connect(_calcBox, &QLineEdit::returnPressed, [&]{EqualsPressed();});
    auto layout = new QHBoxLayout();
    layout->setAlignment(Qt::AlignLeft);
    layout->addWidget(_calcBox);
    _mainLayout->addLayout(layout);
}

void UI::SetupKeyboardLayout() {
    auto layout = new QGridLayout();
    auto keyboardChars = std::string("123BC\n456()\n789+-\n0,=*/");
    int x = 0, y = 0;

    // Create keyboard
    for (auto c : keyboardChars) {
        if (c == '\n') {
            x = 0;
            y++;
            continue;
        }
        auto key = new QPushButton(QString(c));
        layout->addWidget(key, y, x);
        if (c == 'B') {
            connect(key, &QPushButton::clicked, [&]{
                auto text = _calcBox->text();
                text.remove(text.size() - 1, 1);
                _calcBox->setText(text);
            });
        } else if (c == 'C') {
            connect(key, &QPushButton::clicked, [&]{ _calcBox->setText(QString("")); });
        } else if (c != '=') {
            connect(key, &QPushButton::clicked, [&, c]{
                auto text = _calcBox->text();
                text.append(c);
                _calcBox->setText(text);
            });
        } else {
            connect(key, &QPushButton::clicked, [&]{ EqualsPressed(); });
        }
        x++;
    }
    _mainLayout->addLayout(layout);
}

void UI::EqualsPressed() {
    try {
        auto res = CalculateExpression(_calcBox->text().toStdString());
        _calcBox->setText(QString::fromUtf8(std::to_string(res).c_str()));
    } catch (const std::exception& ex) {
        auto msg = std::string("Error: ") + ex.what();
        QMessageBox box;
        box.setText(QString::fromUtf8(msg.c_str()));
        box.exec();
    }
}
