#include "ui.hpp"
#include "calculator.hpp"

#include <cassert>
#include <cmath>
#include <numeric>
#include <QApplication>

#define assert_runtime_error(f) \
    do { \
        try { \
            f; \
            assert(false); \
        } catch(const std::runtime_error&) {} \
    } while(0)

#define assert_double(f, res) \
    do { \
        assert(std::fabs(f - res) <= 0.001); \
    } while(0)

// Simple tests for testing, if calculator runs correctly
void RunTests() {
    assert_double(CalculateExpression("5"), 5);
    assert_double(CalculateExpression("-5"), -5);
    assert_double(CalculateExpression("+5"), 5);
    assert_double(CalculateExpression("+-+5"), -5);
    assert_double(CalculateExpression("--5"), 5);
    assert_double(CalculateExpression("2*4"), 8);
    assert_double(CalculateExpression("-8.2/3.4"), -2.41176);
    assert_double(CalculateExpression("-5+6"), 1);
    assert_double(CalculateExpression("6--5"), 11);
    assert_double(CalculateExpression("6*4+2"), 26);
    assert_double(CalculateExpression("1.5-3.0*2"), -4.5);
    assert_double(CalculateExpression("1+1.5*4.0-0"), 7);
    assert_double(CalculateExpression("2.0/3.0/3.0/2.0"), 0.11111);
    assert_double(CalculateExpression("1.2+4.5*2.4-0.4/1.5"), 11.73333);
    assert_double(CalculateExpression("1.1-4.0*2.4-6.3/-5.2*474"), 565.76923);
    assert_double(CalculateExpression("1.1-+4.0*2.4-6.3/-5.2*474-49382.4/1.2"), -40586.23076);
    assert_double(CalculateExpression("+-1.0*--1.0 + 2.0"), 1);
    assert_double(CalculateExpression("(1.0)"), 1);
    assert_double(CalculateExpression("-(1.0)"), -1.0);
    assert_double(CalculateExpression("(1+3)*4"), 16);
    assert_double(CalculateExpression("(0.5 + 0.5) * (0.1 * 10.0)"), 1.0);
    assert_double(CalculateExpression("(-5-4-3-2-1) * ---(-2.0)"), -30);
    assert_double(CalculateExpression("(((2.1)) + 3.9) / -6.0"), -1.0);
    assert_double(CalculateExpression("((-5)) * ((((((((1.0)))))))) * (1.0) / (10.0) * ((1.0 + 9.0))"), -5);
    assert_double(CalculateExpression("-(2.3 + 4.5) * (55.3 - 542.3) / 66.6 * (64.1 + 78 * 4) -  (-5-6) / 61.2"), 18701.2722311);

    assert_runtime_error(CalculateExpression(""));
    assert_runtime_error(CalculateExpression("1.0/0.0"));
    assert_runtime_error(CalculateExpression("*1.0"));
    assert_runtime_error(CalculateExpression("5.0 + * 5.3"));
    assert_runtime_error(CalculateExpression("2 ^ 32"));
    assert_runtime_error(CalculateExpression("sin(x)"));
    assert_runtime_error(CalculateExpression("5 + 5 +"));
    assert_runtime_error(CalculateExpression("1.0 / (1.212121 - 1.212121)"));
    assert_runtime_error(CalculateExpression("(5.0))"));
    assert_runtime_error(CalculateExpression("((1.0 + 1.0)"));
}

int main(int argc, char *argv[]) {
    //RunTests();
    QCoreApplication::setApplicationName(QString("Simple QT calculator"));

    QApplication a(argc, argv);
    UI w;
    w.show();
    return a.exec();
}
