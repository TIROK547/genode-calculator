#pragma once

#include <QMainWindow>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include "calculator_engine.h"

// ─────────────────────────────────────────────────────────────────────────────
// MainWindow — Qt widget layer that owns the GUI and delegates math to the engine.
// ─────────────────────────────────────────────────────────────────────────────

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override = default;

protected:
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void onDigitClicked(char c);
    void onOperatorClicked(Op op);
    void onEqualClicked();
    void onClearClicked();
    void onClearEntryClicked();
    void onNegateClicked();
    void onPercentClicked();

private:
    void buildUi();
    void updateDisplay();
    QPushButton* makeButton(const QString &text, const QString &styleClass = {});

    CalculatorEngine m_engine;

    // Widgets
    QWidget     *m_central    = nullptr;
    QGridLayout *m_grid       = nullptr;
    QLabel      *m_display    = nullptr;
};
