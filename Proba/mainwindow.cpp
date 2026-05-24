#include "mainwindow.h"
#include "core/combinatorics.h"
#include "core/probability.h"
#include "engine/distribution.h"
#include "qcustomplot.h"
#include <QPen>
#include <QBrush>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QWidget>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QGroupBox>
#include <QVector>
#include <QStackedWidget>
#include <QString>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{

    this->setStyleSheet("QMainWindow { background-color: #2b2b2b; }"
                        "QLabel { color: #ffffff; font-size: 16px; }"
                        "QGroupBox { color: #5dade2; font-size: 18px; font-weight: bold; border: 2px solid #444; margin-top: 15px; padding-top: 20px; }"
                        "QSpinBox { font-size: 16px; padding: 5px; min-height: 35px; }"
                        "QDoubleSpinBox { font-size: 16px; padding: 5px; min-height: 35px; }"
                        "QComboBox { font-size: 16px; padding: 5px; min-height: 35px; }"
                        "QPushButton { background-color: #5dade2; color: white; border-radius: 5px; padding: 10px; font-size: 18px; font-weight: bold; min-height: 45px; }"
                        "QPushButton:hover { background-color: #3498db; }");

    QWidget *central = new QWidget(this);
    setCentralWidget(central);
    QVBoxLayout *mainLayout = new QVBoxLayout(central);

    QComboBox *menuSelector = new QComboBox(this);
    menuSelector->addItem("Calculadora Combinatoria");
    menuSelector->addItem("Probabilidad");
    menuSelector->addItem("Distribuciones de Probabilidad");
    menuSelector->addItem("Variables Aleatorias");

    mainLayout->addWidget(new QLabel("Selecciona el cálculo a realizar:"));
    mainLayout->addWidget(menuSelector);
    mainLayout->addSpacing(20);

    QStackedWidget *stack = new QStackedWidget(this);

    stack->addWidget(createCombinatoricsPage());
    stack->addWidget(createProbabilityPage());
    stack->addWidget(createDistributionFunc());
    stack->addWidget(createRandomVar());

    mainLayout->addWidget(stack);
    mainLayout->addStretch();

    connect(menuSelector, QOverload<int>::of(&QComboBox::currentIndexChanged),
            stack, &QStackedWidget::setCurrentIndex);
}

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

        lblRes->setStyleSheet("font-size: 24px; color: #2ecc71; font-weight: bold; padding: 15px; border: 2px solid #444; background-color: #1e1e1e;");
        lblRes->setText(QString::number(res));
    });

    return page;
}

QWidget* MainWindow::createProbabilityPage() {
    QWidget *page = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(page);

    QGroupBox *boxParams = new QGroupBox("Cálculo de Probabilidad");
    QFormLayout *formLayout = new QFormLayout(boxParams);

    QComboBox *comboType = new QComboBox();
    comboType->addItems({"Probabilidad Clásica (Casos Fav/Total)", "Probabilidad Condicional (P(A|B))"});

    QDoubleSpinBox *spinA = new QDoubleSpinBox(); spinA->setRange(0, 1000000);
    QDoubleSpinBox *spinB = new QDoubleSpinBox(); spinB->setRange(0, 1000000);

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
            lblRes->setStyleSheet("font-size: 24px; color: #2ecc71; font-weight: bold; border: 2px solid #444; background-color: #1e1e1e;");

        } catch (const std::invalid_argument&) {
            lblRes->setText("Error: Divisor en 0");
            lblRes->setStyleSheet("font-size: 20px; color: #e74c3c; font-weight: bold; border: 2px solid #e74c3c;");
        }
    });
    return page;
}
QWidget* MainWindow::createRandomVar() {
    QWidget* page = new QWidget();

    QHBoxLayout* mainHorizontalLayout = new QHBoxLayout(page);
    mainHorizontalLayout->setContentsMargins(10, 10, 10, 10);
    mainHorizontalLayout->setSpacing(20);

    QWidget* leftWidget = new QWidget();
    QVBoxLayout* leftLayout = new QVBoxLayout(leftWidget);
    leftLayout->setContentsMargins(0, 0, 0, 0);

    QGroupBox *boxParams = new QGroupBox("Variables Aleatorias Continuas");
    QFormLayout *formLayout = new QFormLayout(boxParams);
    formLayout->setSpacing(12);

    QLineEdit* funcInput = new QLineEdit();
    funcInput->setPlaceholderText("Ej: 6x - 6x^2  (Sin usar '*')");
    funcInput->setStyleSheet("font-size: 16px; padding: 8px; background-color: #1e1e1e; color: white; border: 1px solid #444; border-radius: 4px;");

    QDoubleSpinBox* limitAInput = new QDoubleSpinBox(); limitAInput->setRange(-1000.0, 1000.0); limitAInput->setValue(0.0);
    QDoubleSpinBox* limitBInput = new QDoubleSpinBox(); limitBInput->setRange(-1000.0, 1000.0); limitBInput->setValue(1.0);
    QSpinBox* momentInput = new QSpinBox(); momentInput->setRange(1, 100); momentInput->setValue(2);

    formLayout->addRow("Función f(x):", funcInput);
    formLayout->addRow("Límite inferior (a):", limitAInput);
    formLayout->addRow("Límite superior (b):", limitBInput);
    formLayout->addRow("Grado momento (n):", momentInput);

    QPushButton* evalButton = new QPushButton("Validar y Calcular");
    evalButton->setMinimumHeight(50);

    QLabel* lblRes = new QLabel(
        "<b>¿Es función de densidad?:</b> -<br>"
        "<b>Esperanza E[x]:</b> -<br>"
        "<b>Momento E[x^n]:</b> -<br>"
        "<b>Varianza V[x]:</b> -"
        );
    lblRes->setWordWrap(true);
    lblRes->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    lblRes->setStyleSheet("font-size: 15px; color: #ecf0f1; background-color: #1e1e1e; padding: 15px; border: 2px solid #444; border-radius: 5px; line-height: 1.6;");

    leftLayout->addWidget(boxParams);
    leftLayout->addWidget(evalButton);
    leftLayout->addWidget(lblRes);
    leftLayout->addStretch();

    QCustomPlot *customPlot = new QCustomPlot();
    customPlot->setBackground(QBrush(QColor("#1e1e1e")));
    customPlot->xAxis->setBasePen(QPen(Qt::white));
    customPlot->yAxis->setBasePen(QPen(Qt::white));
    customPlot->xAxis->setTickLabelColor(Qt::white);
    customPlot->yAxis->setTickLabelColor(Qt::white);
    customPlot->xAxis->setLabel("Soporte (X)");
    customPlot->yAxis->setLabel("Densidad / Probabilidad");
    customPlot->xAxis->setLabelColor(Qt::white);
    customPlot->yAxis->setLabelColor(Qt::white);

    customPlot->legend->setVisible(true);
    customPlot->legend->setBrush(QBrush(QColor(30, 30, 30, 200)));
    customPlot->legend->setTextColor(Qt::white);
    customPlot->legend->setBorderPen(QPen(QColor("#444")));

    mainHorizontalLayout->addWidget(leftWidget, 1);
    mainHorizontalLayout->addWidget(customPlot, 2);

    QObject::connect(evalButton, &QPushButton::clicked, [=]() {
        std::string expression = funcInput->text().toStdString();
        ld a = limitAInput->value();
        ld b = limitBInput->value();
        int n = momentInput->value();

        customPlot->clearGraphs();
        customPlot->replot();

        if (expression.empty() || a >= b) {
            lblRes->setText("<font color='#e74c3c'>Error: Verifica la función y los límites (a < b).</font>");
            return;
        }

        Probability probSystem;

        std::vector<Linear> tokens = probSystem.validate_exp(expression);

        if (tokens.empty()) {
            lblRes->setText("<b>Sintaxis Inválida:</b> No uses '*' o caracteres extraños.<br>Usa formato algebraico: 6x - 6x^2");
            lblRes->setStyleSheet("font-size: 15px; color: #ecf0f1; background-color: #1e1e1e; padding: 15px; border: 2px solid #e74c3c;");
            return;
        }

        bool esDensidad = Probability::isDenFunc(tokens, a, b);

        if (esDensidad) {
            ld ex = Probability::expectation(tokens, a, b);
            ld mn = Probability::moment(tokens, a, b, n);
            ld var = Probability::variance(tokens, a, b);

            QString resText = QString("<b>¿Es función de densidad?:</b> <font color='#2ecc71'>SÍ (Válida)</font><br>") +
                              QString("<b>Esperanza E[X]:</b> %1<br>").arg(QString::number((double)ex, 'f', 4)) +
                              QString("<b>Momento E[X^%1]:</b> %2<br>").arg(n).arg(QString::number((double)mn, 'f', 4)) +
                              QString("<b>Varianza V[X]:</b> %3").arg(QString::number((double)var, 'f', 4));

            lblRes->setText(resText);
            lblRes->setStyleSheet("font-size: 15px; color: #ecf0f1; background-color: #1e1e1e; padding: 15px; border: 2px solid #2ecc71;");

            customPlot->addGraph();
            customPlot->addGraph();

            const int puntos = 400;
            QVector<double> vx(puntos), vy_pdf(puntos), vy_cdf(puntos);
            double extra = (b - a) * 0.3;
            double xStart = a - extra;
            double xEnd = b + extra;
            double step = (xEnd - xStart) / (puntos - 1);
            auto eval_poly = [&tokens](double x) -> double {
                double sum = 0.0;
                for (const auto& t : tokens) sum += t.a * std::pow(x, t.n);
                return sum;
            };

            for (int i = 0; i < puntos; ++i) {
                double x = xStart + i * step;
                vx[i] = x;

                if (x >= a && x <= b) {
                    vy_pdf[i] = eval_poly(x);
                    vy_cdf[i] = static_cast<double>(Probability::integrate(tokens, a, x));
                } else {
                    vy_pdf[i] = 0.0;
                    vy_cdf[i] = (x < a) ? 0.0 : 1.0;
                }
            }

            customPlot->graph(0)->setData(vx, vy_pdf);
            customPlot->graph(0)->setName("Densidad f(x)");
            customPlot->graph(0)->setPen(QPen(QColor("#2ecc71"), 3));
            customPlot->graph(0)->setBrush(QBrush(QColor(46, 204, 113, 30)));

            customPlot->graph(1)->setData(vx, vy_cdf);
            customPlot->graph(1)->setName("Acumulada F(x)");
            customPlot->graph(1)->setPen(QPen(QColor("#5dade2"), 3));

            customPlot->rescaleAxes();
            customPlot->yAxis->setRange(0, std::max(1.1, customPlot->yAxis->range().upper));
            customPlot->replot();

        } else {
            lblRes->setText("<b>¿Es función de densidad?:</b> <font color='#e67e22'>NO (Área != 1 o f(x) < 0 en el intervalo)</font>");
            lblRes->setStyleSheet("font-size: 15px; color: #ecf0f1; background-color: #1e1e1e; padding: 15px; border: 2px solid #e67e22;");
        }
    });

    return page;
}
QWidget* MainWindow::createDistributionFunc() {
    QWidget *page = new QWidget();
    QHBoxLayout *mainHorizontalLayout = new QHBoxLayout(page);

    QWidget *leftWidget = new QWidget();
    QVBoxLayout *leftLayout = new QVBoxLayout(leftWidget);

    QGroupBox *boxParams = new QGroupBox("Catálogo Completo de Distribuciones");
    QFormLayout *formLayout = new QFormLayout(boxParams);

    QComboBox *comboFunc = new QComboBox();
    comboFunc->addItems({
        "Bernoulli", "Binomial", "Geométrica", "Binomial Negativa", "Poisson",
        "Uniforme Discreta", "Uniforme Continua", "Normal", "Exponencial", "Gamma", "Xi Cuadrada"
    });

    QDoubleSpinBox *spinParam1 = new QDoubleSpinBox(); spinParam1->setRange(-1000, 1000000); spinParam1->setValue(0.5);
    QDoubleSpinBox *spinParam2 = new QDoubleSpinBox(); spinParam2->setRange(-1000, 1000000); spinParam2->setValue(1);

    QLabel *lblParam1 = new QLabel("Probabilidad (p):");
    QLabel *lblParam2 = new QLabel("No usado:");
    spinParam2->setEnabled(false);

    formLayout->addRow("Distribución:", comboFunc);
    formLayout->addRow(lblParam1, spinParam1);
    formLayout->addRow(lblParam2, spinParam2);

    connect(comboFunc, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index) {
        spinParam1->setEnabled(true); spinParam2->setEnabled(true);
        switch(index) {
        case 0:
            lblParam1->setText("Probabilidad (p):"); spinParam1->setValue(0.5); spinParam1->setRange(0,1);
            lblParam2->setText("No usado:"); spinParam2->setValue(0); spinParam2->setEnabled(false);
            break;
        case 1:
            lblParam1->setText("Ensayos (n):"); spinParam1->setValue(10); spinParam1->setRange(1, 1000);
            lblParam2->setText("Probabilidad (p):"); spinParam2->setValue(0.5); spinParam2->setRange(0,1);
            break;
        case 2:
            lblParam1->setText("Probabilidad (p):"); spinParam1->setValue(0.3); spinParam1->setRange(0.001, 1);
            lblParam2->setText("No usado:"); spinParam2->setValue(0); spinParam2->setEnabled(false);
            break;
        case 3:
            lblParam1->setText("Éxitos (r):"); spinParam1->setValue(3); spinParam1->setRange(1, 1000);
            lblParam2->setText("Probabilidad (p):"); spinParam2->setValue(0.5); spinParam2->setRange(0.001, 1);
            break;
        case 4:
            lblParam1->setText("Tasa (λ):"); spinParam1->setValue(4.0); spinParam1->setRange(0.001, 1000);
            lblParam2->setText("No usado:"); spinParam2->setValue(0); spinParam2->setEnabled(false);
            break;
        case 5:
            lblParam1->setText("Tamaño (n) :"); spinParam1->setValue(1); spinParam1->setRange(0, 10000);
            lblParam2->setText("No usado:"); spinParam2->setValue(0); spinParam2->setEnabled(false);
            break;
        case 6:
            lblParam1->setText("Límite Inf (a):"); spinParam1->setValue(0); spinParam1->setRange(-10000, 10000);
            lblParam2->setText("Límite Sup (b):"); spinParam2->setValue(5); spinParam2->setRange(-10000, 10000);
            break;
        case 7:
            lblParam1->setText("Media (μ):"); spinParam1->setValue(0); spinParam1->setRange(-10000, 10000);
            lblParam2->setText("Desv Est (σ):"); spinParam2->setValue(1); spinParam2->setRange(0.001, 10000);
            break;
        case 8:
            lblParam1->setText("Tasa (λ):"); spinParam1->setValue(1.5); spinParam1->setRange(0.001, 10000);
            lblParam2->setText("No usado:"); spinParam2->setValue(0); spinParam2->setEnabled(false);
            break;
        case 9:
            lblParam1->setText("Forma (α):"); spinParam1->setValue(2.0); spinParam1->setRange(0.001, 10000);
            lblParam2->setText("Tasa (β):"); spinParam2->setValue(1.0); spinParam2->setRange(0.001, 10000);
            break;
        case 10:
            lblParam1->setText("Grados Lib (k):"); spinParam1->setValue(4); spinParam1->setRange(1, 500);
            lblParam2->setText("No usado:"); spinParam2->setValue(0); spinParam2->setEnabled(false);
            break;
        }
    });

    QPushButton *btn = new QPushButton("Analizar y Graficar Conjuntas");
    QLabel *lblRes = new QLabel("Esperando análisis...");
    lblRes->setWordWrap(true);
    lblRes->setStyleSheet("font-size: 14px; color: #ecf0f1; background-color: #1e1e1e; padding: 10px; border: 1px solid #444;");

    leftLayout->addWidget(boxParams);
    leftLayout->addWidget(btn);
    leftLayout->addWidget(lblRes);
    leftLayout->addStretch();

    QCustomPlot *customPlot = new QCustomPlot();
    customPlot->setMinimumWidth(450);
    customPlot->setBackground(QBrush(QColor("#1e1e1e")));
    customPlot->xAxis->setBasePen(QPen(Qt::white));
    customPlot->yAxis->setBasePen(QPen(Qt::white));
    customPlot->xAxis->setTickLabelColor(Qt::white);
    customPlot->yAxis->setTickLabelColor(Qt::white);
    customPlot->xAxis->setLabel("Soporte (X)");
    customPlot->yAxis->setLabel("Magnitud de Probabilidad / Densidad");
    customPlot->xAxis->setLabelColor(Qt::white);
    customPlot->yAxis->setLabelColor(Qt::white);

    customPlot->legend->setVisible(true);
    customPlot->legend->setBrush(QBrush(QColor(30, 30, 30, 200)));
    customPlot->legend->setTextColor(Qt::white);
    customPlot->legend->setBorderPen(QPen(QColor("#444")));

    mainHorizontalLayout->addWidget(leftWidget, 1);
    mainHorizontalLayout->addWidget(customPlot, 2);

    connect(btn, &QPushButton::clicked, [=]() {
        double p1 = spinParam1->value();
        double p2 = spinParam2->value();
        Distribution* dist = nullptr;
        int idx = comboFunc->currentIndex();

        try {
            if (idx == 0)       dist = new BernoulliDistribution(p1);
            else if (idx == 1)  dist = new BinomialDistribution(static_cast<int>(p1), p2);
            else if (idx == 2)  dist = new GeometricDistribution(p1);
            else if (idx == 3)  dist = new NegativeBinomialDistribution(static_cast<int>(p1), p2);
            else if (idx == 4)  dist = new PoissonDistribution(p1);
            else if (idx == 5)  dist = new UniformDiscreteDistribution(1, static_cast<int>(p1));
            else if (idx == 6)  dist = new UniformContinuousDistribution(p1, p2);
            else if (idx == 7)  dist = new NormalDistribution(p1, p2);
            else if (idx == 8)  dist = new ExponentialDistribution(p1);
            else if (idx == 9)  dist = new GammaDistribution(p1, p2);
            else if (idx == 10) dist = new ChiSquaredDistribution(static_cast<int>(p1));

            if (dist != nullptr) {
                double mu = dist->mean();
                double evalX = dist->isDiscrete() ? std::round(mu) : mu;
                double valFunc = dist->pdf(evalX);

                QString tagFunc = dist->isDiscrete() ? "Función de Masa P(X = k)" : "Función de Densidad f(x)";
                QString puntoEval = dist->isDiscrete() ? QString("k = %1 (Media)").arg(evalX) : QString("x = %1 (Media)").arg(QString::number(evalX, 'f', 2));

                QString info = QString("<b>Distribución:</b> %1<br><br>")
                                   .arg(comboFunc->currentText()) +
                               QString("<b>Valor Esperado E[X]:</b> %1<br>")
                                   .arg(QString::number(mu, 'f', 4)) +
                               QString("<b>Varianza V[X]:</b> %1<br><br>")
                                   .arg(QString::number(dist->variance(), 'f', 4)) +
                               QString("<b>Evaluación en Media:</b><br>%1 = %2<br>")
                                   .arg(tagFunc, QString::number(valFunc, 'f', 4));

                lblRes->setText(info);
                lblRes->setStyleSheet("font-size: 14px; color: #2ecc71; background-color: #1e1e1e; padding: 10px; border: 2px solid #2ecc71;");

                customPlot->clearGraphs();
                customPlot->clearPlottables();

                double minX = dist->xMin();
                double maxX = dist->xMax();

                if (dist->isDiscrete()) {

                    QCPBars *barsPMF = new QCPBars(customPlot->xAxis, customPlot->yAxis);
                    barsPMF->setName("Masa P(X = k)");

                    customPlot->addGraph();
                    customPlot->graph(0)->setName("Acumulada F(k)");

                    int startK = std::max(-100000, static_cast<int>(std::floor(minX + 0.5)));
                    int endK = static_cast<int>(std::floor(maxX - 0.5));

                    QVector<double> cdfX, cdfY;

                    cdfX.append(startK - 1);
                    cdfY.append(0.0);

                    for (int k = startK; k <= endK; ++k) {
                        barsPMF->addData(k, dist->pdf(k));

                        cdfX.append(k);
                        cdfY.append(dist->cdf(k));
                    }

                    cdfX.append(endK + 1);
                    cdfY.append(1.0);

                    barsPMF->setPen(QPen(QColor("#5dade2")));
                    barsPMF->setBrush(QBrush(QColor(93, 173, 226, 100)));
                    barsPMF->setWidth(0.4);

                    customPlot->graph(0)->setData(cdfX, cdfY);

                    customPlot->graph(0)->setLineStyle(QCPGraph::lsStepRight);

                    QPen penCDF(QColor("#e74c3c"));
                    penCDF.setWidthF(3.0);
                    customPlot->graph(0)->setPen(penCDF);
                    customPlot->graph(0)->setBrush(Qt::NoBrush);

                } else {
                    customPlot->addGraph();
                    customPlot->addGraph();

                    const int totalPuntos = 400;
                    QVector<double> vecX(totalPuntos), vecY_PDF(totalPuntos), vecY_CDF(totalPuntos);
                    double paso = (maxX - minX) / (totalPuntos - 1);

                    for (int i = 0; i < totalPuntos; ++i) {
                        double x = minX + i * paso;
                        vecX[i] = x;
                        vecY_PDF[i] = dist->pdf(x);
                        vecY_CDF[i] = dist->cdf(x);
                    }

                    customPlot->graph(0)->setData(vecX, vecY_PDF);
                    customPlot->graph(0)->setName("Densidad f(x)");
                    QPen penPDF(QColor("#2ecc71")); penPDF.setWidthF(3.0);
                    customPlot->graph(0)->setPen(penPDF);
                    customPlot->graph(0)->setBrush(QBrush(QColor(46, 204, 113, 20)));

                    customPlot->graph(1)->setData(vecX, vecY_CDF);
                    customPlot->graph(1)->setName("Acumulada F(x)");
                    QPen penCDF(QColor("#e67e22")); penCDF.setWidthF(3.0);
                    customPlot->graph(1)->setPen(penCDF);
                    customPlot->graph(1)->setBrush(QBrush(QColor(230, 126, 34, 15)));
                }

                customPlot->rescaleAxes();
                customPlot->replot();
                delete dist;
            }
        } catch (const std::exception& e) {
            lblRes->setText(QString("Error de parámetros: %1").arg(e.what()));
            lblRes->setStyleSheet("font-size: 14px; color: #e74c3c; background-color: #1e1e1e; padding: 10px; border: 2px solid #e74c3c;");
            if (dist != nullptr) delete dist;
        } catch (...) {
            lblRes->setText("Error crítico inesperado en los parámetros de entrada.");
            lblRes->setStyleSheet("font-size: 14px; color: #e74c3c; background-color: #1e1e1e; padding: 10px; border: 2px solid #e74c3c;");
            if (dist != nullptr) delete dist;
        }
    });

    return page;
}
MainWindow::~MainWindow() {}