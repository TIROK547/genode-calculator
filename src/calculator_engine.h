#pragma once

#include <QString>
#include <limits>

// ─────────────────────────────────────────────────────────────────────────────
// CalculatorEngine — pure computation layer, no Qt widgets.
// Can be unit-tested independently of the GUI.
// ─────────────────────────────────────────────────────────────────────────────

enum class Op { None, Add, Sub, Mul, Div };

class CalculatorEngine {
public:
    CalculatorEngine();

    // Feed a digit character ('0'–'9') or decimal point ('.')
    void inputDigit(char c);

    // Set pending binary operator; evaluates chain if one already pending
    void setOperator(Op op);

    // Compute and store result; returns the display string
    QString evaluate();

    // AC — clear everything
    void clear();

    // CE — clear current entry only
    void clearEntry();

    // Toggle sign of current entry
    void negate();

    // Divide current entry by 100
    void percent();

    // Value currently shown on the display
    QString displayValue() const;

    bool hasError() const { return m_error; }

private:
    double  m_accumulator;
    double  m_current;
    Op      m_pendingOp;
    bool    m_fresh;    // next digit replaces instead of appending
    bool    m_error;
    QString m_display;

    double applyOp(double lhs, Op op, double rhs);
    void   updateDisplay(double value);
};
