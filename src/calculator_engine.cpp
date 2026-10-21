#include "calculator_engine.h"
#include <cmath>

CalculatorEngine::CalculatorEngine()
    : m_accumulator(0.0)
    , m_current(0.0)
    , m_pendingOp(Op::None)
    , m_fresh(true)
    , m_error(false)
    , m_display("0")
{}

void CalculatorEngine::inputDigit(char c) {
    if (m_error) return;
    if (c == '.' && m_display.contains('.') && !m_fresh) return;

    if (m_fresh) {
        m_display = (c == '.') ? "0." : QString(c);
        m_fresh   = false;
    } else {
        if (m_display == "0" && c != '.') m_display = QString(c);
        else                              m_display += c;
    }
    m_current = m_display.toDouble();
}

void CalculatorEngine::setOperator(Op op) {
    if (m_error) return;

    if (m_pendingOp != Op::None && !m_fresh) {
        m_accumulator = applyOp(m_accumulator, m_pendingOp, m_current);
        if (std::isinf(m_accumulator) || std::isnan(m_accumulator)) {
            m_error   = true;
            m_display = "Error";
            return;
        }
        updateDisplay(m_accumulator);
    } else {
        m_accumulator = m_current;
    }

    m_pendingOp = op;
    m_fresh     = true;
}

QString CalculatorEngine::evaluate() {
    if (m_error) return "Error";

    double rhs    = m_fresh ? m_accumulator : m_current;
    double result = (m_pendingOp != Op::None)
                        ? applyOp(m_accumulator, m_pendingOp, rhs)
                        : m_current;

    if (std::isinf(result) || std::isnan(result)) {
        m_error   = true;
        m_display = "Error";
        return m_display;
    }

    m_accumulator = result;
    m_pendingOp   = Op::None;
    m_fresh       = true;
    updateDisplay(result);
    return m_display;
}

void CalculatorEngine::clear() {
    m_accumulator = 0.0;
    m_current     = 0.0;
    m_pendingOp   = Op::None;
    m_fresh       = true;
    m_error       = false;
    m_display     = "0";
}

void CalculatorEngine::clearEntry() {
    if (m_error) { clear(); return; }
    m_current = 0.0;
    m_display = "0";
    m_fresh   = true;
}

void CalculatorEngine::negate() {
    if (m_error) return;
    m_current = -m_display.toDouble();
    updateDisplay(m_current);
    m_fresh = false;
}

void CalculatorEngine::percent() {
    if (m_error) return;
    m_current = m_display.toDouble() / 100.0;
    updateDisplay(m_current);
    m_fresh = false;
}

QString CalculatorEngine::displayValue() const { return m_display; }

// ── private ──────────────────────────────────────────────────────────────────

double CalculatorEngine::applyOp(double lhs, Op op, double rhs) {
    switch (op) {
        case Op::Add:  return lhs + rhs;
        case Op::Sub:  return lhs - rhs;
        case Op::Mul:  return lhs * rhs;
        case Op::Div:
            if (rhs == 0.0) return std::numeric_limits<double>::infinity();
            return lhs / rhs;
        default:       return rhs;
    }
}

void CalculatorEngine::updateDisplay(double value) {
    m_display = QString::number(value, 'g', 12);
    m_current = value;
}
