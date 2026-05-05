#include "mainwindow.h"
#include "core/combinatorics.h"
#include "core/probability.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QWidget>
#include <QSpinBox>
#include <QComboBox>
#include <QGroupBox>
#include <QStackedWidget>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{

    this->setStyleSheet("QMainWindow { background-color: #2b2b2b; }"
                        "QLabel { color: #ffffff; font-size: 16px; }"
                        "QGroupBox { color: #5dade2; font-size: 18px; font-weight: bold; border: 2px solid #444; margin-top: 15px; padding-top: 20px; }"
                        "QSpinBox { font-size: 16px; padding: 5px; min-height: 35px; }"
                        "QComboBox { font-size: 16px; padding: 5px; min-height: 35px; }"
                        "QPushButton { background-color: #5dade2; color: white; border-radius: 5px; padding: 10px; font-size: 18px; font-weight: bold; min-height: 45px; }"
                        "QPushButton:hover { background-color: #3498db; }");

    QWidget *central = new QWidget(this);
    setCentralWidget(central);
    QVBoxLayout *mainLayout = new QVBoxLayout(central);

    QComboBox *menuSelector = new QComboBox(this);

    menuSelector->addItem("Calculadora Combinatoria");
    menuSelector->addItem("Probabilidad");
    menuSelector->addItem("Variables Aleatorias");
    menuSelector->addItem("Aún falta todo lo demás xd");
    mainLayout->addWidget(new QLabel("Selecciona el cálculo a realizar:"));
    mainLayout->addWidget(menuSelector);
    mainLayout->addSpacing(20);


    QStackedWidget *stack = new QStackedWidget(this);


    stack->addWidget(createCombinatoricsPage());
    stack->addWidget(createProbabilityPage());
    stack->addWidget(new QLabel("Módulo en desarrollo...", this));
    stack->addWidget(createRandomVariablePage());
    mainLayout->addWidget(stack);
    mainLayout->addStretch();

    connect(menuSelector, QOverload<int>::of(&QComboBox::currentIndexChanged),
            stack, &QStackedWidget::setCurrentIndex);
}

//  --- Ventana de combinaciones y variaciones ---
QWidget* MainWindow::createCombinatoricsPage() {
    QWidget *page = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(page);

    QGroupBox *boxParams = new QGroupBox("Parámetros de Cálculo");
    QFormLayout *formLayout = new QFormLayout(boxParams);

    QSpinBox *spinN = new QSpinBox(); spinN->setRange(0, 10000000);
    QSpinBox *spinK = new QSpinBox(); spinK->setRange(0, 10000000);

    QComboBox *comboOp = new QComboBox();
    comboOp->addItems({
        "Variación (nPr)",
        "Combinación (nCr)",
        "Variación con Rep. (n^r)",
        "Combinación con Rep. (C(n+r-1, r))"
    });

    formLayout->addRow("N (Total):", spinN);
    formLayout->addRow("K (Selección):", spinK);
    formLayout->addRow("Operación:", comboOp);
    formLayout->addRow(new QLabel("Congruente con: 10^9 + 7"));

    QPushButton *btn = new QPushButton("Calcular");
    QLabel *lblRes = new QLabel("Resultado: -");
    lblRes->setStyleSheet("font-size: 24px; color: #2ecc71; font-weight: bold; padding: 15px; border: 2px solid #444; background-color: #1e1e1e;");
    lblRes->setAlignment(Qt::AlignCenter);

    layout->addWidget(boxParams);
    layout->addWidget(btn);
    layout->addWidget(lblRes);
    layout->addStretch();

    connect(btn, &QPushButton::clicked, [=]() {
        int n = spinN->value();
        int k = spinK->value();

        if ((comboOp->currentIndex() == 0 || comboOp->currentIndex() == 1) && k > n) {
            lblRes->setText("Error: K > N");
            lblRes->setStyleSheet("font-size: 24px; color: #e74c3c; font-weight: bold; border: 2px solid #e74c3c;");
            return;
        }

        Combinatorics comn;

        comn.init(10000000, 1000000007);

        long long res = 0;
        switch(comboOp->currentIndex()) {
        case 0: res = comn.nPr(n, k); break;
        case 1: res = comn.nCr(n, k); break;
        case 2: res = comn.nPr_rep(n, k); break;
        case 3: res = comn.nCr_rep(n, k); break;
        }

        lblRes->setStyleSheet("font-size: 24px; color: #2ecc71; font-weight: bold; padding: 15px; border: 2px solid #444;");
        lblRes->setText(QString::number(res));
    });

    return page;
}
// --- Ventana de Combinaciones y variaciones ---

QWidget* MainWindow::createProbabilityPage() {
    QWidget *page = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(page);

    QGroupBox *boxParams = new QGroupBox("Cálculo de Probabilidad");
    QFormLayout *formLayout = new QFormLayout(boxParams);

    QComboBox *comboType = new QComboBox();
    comboType->addItems({"Probabilidad Clásica (Casos Fav/Total)", "Probabilidad Condicional (P(A|B))"});

    QDoubleSpinBox *spinA = new QDoubleSpinBox();
    spinA->setRange(0, 1000000);

    QDoubleSpinBox *spinB = new QDoubleSpinBox();
    spinB->setRange(0, 1000000);

    formLayout->addRow("Tipo:", comboType);
    formLayout->addRow("Casos A / Intersección:", spinA);
    formLayout->addRow("Total / Evento B:", spinB);

    QPushButton *btn = new QPushButton("Calcular");
    QLabel *lblRes = new QLabel("Resultado: -");
    lblRes->setStyleSheet("font-size: 24px; color: #2ecc71; font-weight: bold; padding: 15px; border: 2px solid #444; background-color: #1e1e1e;");
    lblRes->setAlignment(Qt::AlignCenter);

    layout->addWidget(boxParams);
    layout->addWidget(btn);
    layout->addWidget(lblRes);
    layout->addStretch();

    connect(btn, &QPushButton::clicked, [=]() {
        try {
            long double a = spinA->value();
            long double b = spinB->value();

            if (a > b) {
                lblRes->setText("Error: Fav > Total");
                lblRes->setStyleSheet("font-size: 20px; color: #e74c3c; font-weight: bold; border: 2px solid #e74c3c;");
                return;
            }

            long double res = 0;
            if (comboType->currentIndex() == 0) {
                res = Probability::classic(a, b);
            } else {
                res = Probability::conditional(a, b);
            }

            lblRes->setText(QString::number(static_cast<double>(res), 'f', 4));

            lblRes->setStyleSheet("font-size: 24px; color: #2ecc71; font-weight: bold; border: 2px solid #444;");

        } catch (const std::invalid_argument& e) {
            lblRes->setText("Error: Divisor en 0");
            lblRes->setStyleSheet("font-size: 20px; color: #e74c3c; font-weight: bold; border: 2px solid #e74c3c;");
        }
    });
    return page;
}
// -- Por el momento solo estas funciones
QWidget* MainWindow::createRandomVariablePage() {
    QWidget *page = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(page);

    QGroupBox *boxParams = new QGroupBox("Propiedades de Variable Aleatoria");
    QFormLayout *formLayout = new QFormLayout(boxParams);

    QComboBox *comboFunc = new QComboBox();
    comboFunc->addItems({"f(x) = 2x [0,1]", "f(x) = 3x^2 [0,1]", "f(x) = 0.5 [0,2]"});

    QDoubleSpinBox *spinA = new QDoubleSpinBox(); spinA->setRange(-100, 100); spinA->setValue(0);
    QDoubleSpinBox *spinB = new QDoubleSpinBox(); spinB->setRange(-100, 100); spinB->setValue(1);

    formLayout->addRow("Función:", comboFunc);
    formLayout->addRow("Límite Inferior (a):", spinA);
    formLayout->addRow("Límite Superior (b):", spinB);

    QPushButton *btn = new QPushButton("Analizar");
    QLabel *lblRes = new QLabel("Esperando análisis...");
    lblRes->setWordWrap(true);
    lblRes->setStyleSheet("font-size: 14px; color: #ecf0f1; background-color: #1e1e1e; padding: 10px; border: 1px solid #444;");

    layout->addWidget(boxParams);
    layout->addWidget(btn);
    layout->addWidget(lblRes);
    layout->addStretch();

    connect(btn, &QPushButton::clicked, [=]() {
        ld a = spinA->value();
        ld b = spinB->value();

        std::function<ld(ld)> f;
        if(comboFunc->currentIndex() == 0) f = [](ld x){ return 2*x; };
        else if(comboFunc->currentIndex() == 1) f = [](ld x){ return 3*x*x; };
        else f = [](ld x){ return 0.5; };

        if (Probability::isDenFunc(f, a, b)) {

            ld ex = Probability::expectation(f, a, b);
            ld var = Probability::variance(f, a, b);

            QString result = QString("Es Función de Densidad.\n\n") +
                             "E[X] = " + QString::number(static_cast<double>(ex), 'f', 4) + "\n" +
                             "V[X] = " + QString::number(static_cast<double>(var), 'f', 4);
            lblRes->setText(result);
            lblRes->setStyleSheet("font-size: 14px; color: #2ecc71; background-color: #1e1e1e; padding: 10px; border: 2px solid #2ecc71;");

        } else {
            lblRes->setText("No es una función de densidad");
            lblRes->setStyleSheet("font-size: 14px; color: #e74c3c; background-color: #1e1e1e; padding: 10px; border: 2px solid #e74c3c;");
        }
    });

    return page;
}

MainWindow::~MainWindow() {}