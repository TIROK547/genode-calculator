#include "main_window.h"
#include <QKeyEvent>
#include <QVBoxLayout>
#include <QFont>
#include <functional>

// ─────────────────────────────────────────────────────────────────────────────
// Stylesheet — minimal dark theme matching calcirok's aesthetic
// ─────────────────────────────────────────────────────────────────────────────
static const char* kStyleSheet = R"CSS(
QMainWindow, QWidget#central {
    background-color: #1a1a1a;
}

QLabel#display {
    background-color: #111;
    color: #f0f0f0;
    font-size: 36px;
    font-family: "JetBrains Mono", "Consolas", monospace;
    font-weight: 300;
    padding: 12px 16px;
    border-radius: 6px;
    qproperty-alignment: AlignRight;
    min-height: 64px;
}

QPushButton {
    background-color: #2e2e2e;
    color: #d0d0d0;
    border: none;
    border-radius: 6px;
    font-size: 20px;
    font-family: "JetBrains Mono", "Consolas", monospace;
    min-width: 72px;
    min-height: 64px;
}
QPushButton:hover  { background-color: #3a3a3a; }
QPushButton:pressed { background-color: #505050; }

QPushButton[class="op"] {
    background-color: #3b3b3b;
    color: #e8a000;
}
QPushButton[class="op"]:hover { background-color: #484848; }

QPushButton[class="eq"] {
    background-color: #e8a000;
    color: #111;
    font-weight: bold;
}
QPushButton[class="eq"]:hover { background-color: #ffb700; }

QPushButton[class="fn"] {
    background-color: #252525;
    color: #888;
    font-size: 16px;
}
QPushButton[class="fn"]:hover { background-color: #2e2e2e; color: #aaa; }
)CSS";

// ─────────────────────────────────────────────────────────────────────────────

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("calcirok");
    setFixedSize(320, 480);
    setStyleSheet(kStyleSheet);
    buildUi();
    updateDisplay();
}

void MainWindow::buildUi() {
    m_central = new QWidget(this);
    m_central->setObjectName("central");
    setCentralWidget(m_central);

    auto *root = new QVBoxLayout(m_central);
    root->setContentsMargins(12, 12, 12, 12);
    root->setSpacing(8);

    // ── Display ──────────────────────────────────────────────────────────────
    m_display = new QLabel("0", m_central);
    m_display->setObjectName("display");
    root->addWidget(m_display);

    // ── Button grid ──────────────────────────────────────────────────────────
    m_grid = new QGridLayout();
    m_grid->setSpacing(8);
    root->addLayout(m_grid);

    // Row 0: AC  CE  %  ÷
    auto *ac  = makeButton("AC",  "fn");
    auto *ce  = makeButton("CE",  "fn");
    auto *pct = makeButton("%",   "fn");
    auto *div = makeButton("÷",   "op");
    m_grid->addWidget(ac,  0, 0); m_grid->addWidget(ce,  0, 1);
    m_grid->addWidget(pct, 0, 2); m_grid->addWidget(div, 0, 3);

    // Row 1: 7 8 9 ×
    auto *b7  = makeButton("7");  auto *b8  = makeButton("8");
    auto *b9  = makeButton("9");  auto *mul = makeButton("×", "op");
    m_grid->addWidget(b7,  1, 0); m_grid->addWidget(b8,  1, 1);
    m_grid->addWidget(b9,  1, 2); m_grid->addWidget(mul, 1, 3);

    // Row 2: 4 5 6 −
    auto *b4  = makeButton("4");  auto *b5  = makeButton("5");
    auto *b6  = makeButton("6");  auto *sub = makeButton("−", "op");
    m_grid->addWidget(b4,  2, 0); m_grid->addWidget(b5,  2, 1);
    m_grid->addWidget(b6,  2, 2); m_grid->addWidget(sub, 2, 3);

    // Row 3: 1 2 3 +
    auto *b1  = makeButton("1");  auto *b2  = makeButton("2");
    auto *b3  = makeButton("3");  auto *add = makeButton("+", "op");
    m_grid->addWidget(b1,  3, 0); m_grid->addWidget(b2,  3, 1);
    m_grid->addWidget(b3,  3, 2); m_grid->addWidget(add, 3, 3);

    // Row 4: +/- 0 . =
    auto *neg = makeButton("+/−", "fn");
    auto *b0  = makeButton("0");
    auto *dot = makeButton(".");
    auto *eq  = makeButton("=",   "eq");
    m_grid->addWidget(neg, 4, 0); m_grid->addWidget(b0,  4, 1);
    m_grid->addWidget(dot, 4, 2); m_grid->addWidget(eq,  4, 3);

    // ── Connections ──────────────────────────────────────────────────────────
    connect(ac,  &QPushButton::clicked, this, &MainWindow::onClearClicked);
    connect(ce,  &QPushButton::clicked, this, &MainWindow::onClearEntryClicked);
    connect(pct, &QPushButton::clicked, this, &MainWindow::onPercentClicked);
    connect(neg, &QPushButton::clicked, this, &MainWindow::onNegateClicked);
    connect(eq,  &QPushButton::clicked, this, &MainWindow::onEqualClicked);

    connect(div, &QPushButton::clicked, [this]{ onOperatorClicked(Op::Div); });
    connect(mul, &QPushButton::clicked, [this]{ onOperatorClicked(Op::Mul); });
    connect(sub, &QPushButton::clicked, [this]{ onOperatorClicked(Op::Sub); });
    connect(add, &QPushButton::clicked, [this]{ onOperatorClicked(Op::Add); });

    auto connectDigit = [&](QPushButton *btn, char c) {
        connect(btn, &QPushButton::clicked, [this, c]{ onDigitClicked(c); });
    };
    connectDigit(b0, '0'); connectDigit(b1, '1'); connectDigit(b2, '2');
    connectDigit(b3, '3'); connectDigit(b4, '4'); connectDigit(b5, '5');
    connectDigit(b6, '6'); connectDigit(b7, '7'); connectDigit(b8, '8');
    connectDigit(b9, '9'); connectDigit(dot, '.');
}

QPushButton* MainWindow::makeButton(const QString &text, const QString &styleClass) {
    auto *btn = new QPushButton(text, m_central);
    if (!styleClass.isEmpty())
        btn->setProperty("class", styleClass);
    return btn;
}

// ── Slots ────────────────────────────────────────────────────────────────────

void MainWindow::onDigitClicked(char c) {
    m_engine.inputDigit(c);
    updateDisplay();
}

void MainWindow::onOperatorClicked(Op op) {
    m_engine.setOperator(op);
    updateDisplay();
}

void MainWindow::onEqualClicked() {
    m_engine.evaluate();
    updateDisplay();
}

void MainWindow::onClearClicked()       { m_engine.clear();      updateDisplay(); }
void MainWindow::onClearEntryClicked()  { m_engine.clearEntry(); updateDisplay(); }
void MainWindow::onNegateClicked()      { m_engine.negate();     updateDisplay(); }
void MainWindow::onPercentClicked()     { m_engine.percent();    updateDisplay(); }

// ── Keyboard support ─────────────────────────────────────────────────────────

void MainWindow::keyPressEvent(QKeyEvent *event) {
    const auto key = event->key();

    if (key >= Qt::Key_0 && key <= Qt::Key_9)
        onDigitClicked(static_cast<char>('0' + key - Qt::Key_0));
    else if (key == Qt::Key_Period || key == Qt::Key_Comma)
        onDigitClicked('.');
    else if (key == Qt::Key_Plus)        onOperatorClicked(Op::Add);
    else if (key == Qt::Key_Minus)       onOperatorClicked(Op::Sub);
    else if (key == Qt::Key_Asterisk)    onOperatorClicked(Op::Mul);
    else if (key == Qt::Key_Slash)       onOperatorClicked(Op::Div);
    else if (key == Qt::Key_Return || key == Qt::Key_Enter) onEqualClicked();
    else if (key == Qt::Key_Escape)      onClearClicked();
    else if (key == Qt::Key_Backspace)   onClearEntryClicked();
    else if (key == Qt::Key_Percent)     onPercentClicked();
    else QMainWindow::keyPressEvent(event);
}

// ── Display refresh ──────────────────────────────────────────────────────────

void MainWindow::updateDisplay() {
    m_display->setText(m_engine.displayValue());
}
