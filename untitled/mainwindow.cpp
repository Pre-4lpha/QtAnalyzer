#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QtWidgets/QMainWindow>
#include <QtCharts/QChartView>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QLegend>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QHorizontalStackedBarSeries>
#include <QtCharts/QLineSeries>
#include <QtCharts/QCategoryAxis>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
#include <QFileDialog>
#include <QString>
#include <QDebug>
#include "math.h"

QtCharts::QLineSeries* graph_xacc = new QtCharts::QLineSeries();//Graphen für die veschiedenen Achsen und Werte
QtCharts::QLineSeries* graph_xvel = new QtCharts::QLineSeries();//Werden hier schon mal deklariert, damit sie in creat_graphs/show_graph
QtCharts::QLineSeries* graph_xdis = new QtCharts::QLineSeries();//genutzt werden können nd global verfügbar sind und von der Funktion
QtCharts::QLineSeries* graph_yacc = new QtCharts::QLineSeries();
QtCharts::QLineSeries* graph_yvel = new QtCharts::QLineSeries();
QtCharts::QLineSeries* graph_ydis = new QtCharts::QLineSeries();
QtCharts::QLineSeries* graph_zacc = new QtCharts::QLineSeries();
QtCharts::QLineSeries* graph_zvel = new QtCharts::QLineSeries();
QtCharts::QLineSeries* graph_zdis = new QtCharts::QLineSeries();
QtCharts::QLineSeries* graph_gacc = new QtCharts::QLineSeries();
QtCharts::QLineSeries* graph_gvel = new QtCharts::QLineSeries();
QtCharts::QLineSeries* graph_gdis = new QtCharts::QLineSeries();
bool datei_auswahl = false; // bool´s zur Überprüfung ob alle vorherigen Schritte ausgeführt wurden
bool auswertung = false;


class Datenpunkt
{
public:
double time = 0;
double delta_time =0;
double xacc=0;
double xvel=0;//numerische Intregation der Beschleunigung zur Berechnung der Zeit
double xdis=0;// s.o. für Weg, werden in werte_bestimmen erechnet
double yacc=0;
double yvel=0;
double ydis=0;
double zacc=0;
double zvel=0;
double zdis=0;
double gacc=0;
double gvel=0;
double gdis=0;
Datenpunkt(double t , double xa, double ya, double za ):time(t), xacc(xa), yacc(ya), zacc(za) {};//Konstruktor zum Erzeugen des ersten Elements
                                                                                                 // in fillin_data
void werte_bestimmen(Datenpunkt prev) //Übergabe des vorherigen Datenpunkts prev zur Berechung von delta_time und sowie Geschwindigkeit und Weg
{   delta_time = time - prev.time;    // über numerische Integration
    xvel = (delta_time * xacc) + prev.xvel;
    yvel = (delta_time * yacc) + prev.yvel;
    zvel = (delta_time * zacc) + prev.zvel;
    gvel = (delta_time * gacc) + prev.gvel;
    xdis = (delta_time * xvel) + prev.xdis;
    ydis = (delta_time * yvel) + prev.ydis;
    zdis = (delta_time * zvel) + prev.zdis;
    gdis = (delta_time * gvel) + prev.gdis;
};
};

class Graphen{ //macht die Handhabung der Auswahl des gewünschten Graphen einfacher/möglich
public:
QtCharts::QChart *acc;
QtCharts::QChart *vel;
QtCharts::QChart *dis;

};

Graphen x_werte;
Graphen y_werte;
Graphen z_werte;
Graphen g_werte;
std::list<Datenpunkt> datenliste;

QString cleanse_string(QString str) // bereinigt die Daten der CSV-Datei von der [Zahl]E-[x]/E+[x] Notation
{   QString cleansed;
    QString exponent;
    for(int i=0; i < str.size(); i++) {
            if (str[i] != "E")
                {
                cleansed += str[i];
                }
     else
        {
        for( i ++; i < str.size(); i++)
                {
                exponent += str[i];
                }
       i = str.size()-1;

       double zahlenwert = cleansed.toDouble();
       zahlenwert = zahlenwert * pow(10.0,(exponent.toDouble()));
       cleansed = QString::number(zahlenwert);
       }

    }
return cleansed;
}

std::list<Datenpunkt> fill_in_data( QStringList list) //füllt die Datenpunkt mit den ausgelesenen Werten und ruft
{   datenliste = *new std::list<Datenpunkt>();                 // die klasseninterne Methode werte_bestimmen() auf.
    Datenpunkt prev(0,0,0,0);// wird benötigt für das erste Element der Liste, da die Rechnugen sonst nicht funktionieren bzw
    Datenpunkt voll = prev;
    datenliste.push_front(prev);
    for (int  l = 5; l < list.size(); l++){
        list[l] = cleanse_string(list[l]);}
    for (int j = 5; j < list.size()-5;j = j + 5){
        list[j] = cleanse_string(list[j]);
         {  voll.time = list[j].toDouble();  }
         {  voll.xacc = list[j+1].toDouble();  }
         {  voll.yacc = list[j+2].toDouble();  }
         {  voll.zacc = list[j+3].toDouble();  }
         {  voll.gacc = list[j+4].toDouble();
            voll.werte_bestimmen(prev);
            prev = voll;
            datenliste.push_back(voll);
         }
}
return datenliste;
};

void create_graphs(std::list<Datenpunkt> list) //erstellt alle Graphen und übergibt sie der klasse Graphen
{   auto i = list.begin();
    while (i != list.end())
    {
        graph_xacc->append(i->time,i->xacc);
        graph_xvel->append(i->time,i->xvel);
        graph_xdis->append(i->time,i->xdis);
        graph_yvel->append(i->time,i->yvel);
        graph_ydis->append(i->time,i->yvel);
        graph_yacc->append(i->time,i->yacc);
        graph_zacc->append(i->time,i->zacc);
        graph_zvel->append(i->time,i->zvel);
        graph_zdis->append(i->time,i->zdis);
        graph_gacc->append(i->time,i->gacc);
        graph_gvel->append(i->time,i->gvel);
        graph_gdis->append(i->time,i->gdis);
    i++;
    }
    QtCharts::QChart *xacc = new QtCharts::QChart();
    QtCharts::QChart *xvel = new QtCharts::QChart();
    QtCharts::QChart *xdis = new QtCharts::QChart();
    QtCharts::QChart *yacc = new QtCharts::QChart();
    QtCharts::QChart *yvel = new QtCharts::QChart();
    QtCharts::QChart *ydis = new QtCharts::QChart();
    QtCharts::QChart *zacc = new QtCharts::QChart();
    QtCharts::QChart *zvel = new QtCharts::QChart();
    QtCharts::QChart *zdis = new QtCharts::QChart();
    QtCharts::QChart *gacc = new QtCharts::QChart();
    QtCharts::QChart *gvel = new QtCharts::QChart();
    QtCharts::QChart *gdis = new QtCharts::QChart();
    //chart->legend()->hide();
    xacc->addSeries(graph_xacc);
    xvel->addSeries(graph_xvel);
    xdis->addSeries(graph_xdis);
    x_werte.acc = xacc;
    x_werte.vel = xvel;
    x_werte.dis = xdis;
    yacc->addSeries(graph_yacc);
    yvel->addSeries(graph_yvel);
    ydis->addSeries(graph_ydis);
    y_werte.acc = yacc;
    y_werte.vel = yvel;
    y_werte.dis = ydis;
    zacc->addSeries(graph_zacc);
    zvel->addSeries(graph_zvel);
    zdis->addSeries(graph_zdis);
    z_werte.acc = zacc;
    z_werte.vel = zvel;
    z_werte.dis = zdis;
    gacc->addSeries(graph_gacc);
    gvel->addSeries(graph_gvel);
    gdis->addSeries(graph_gdis);
    g_werte.acc = gacc;
    g_werte.vel = gvel;
    g_werte.dis = gdis;
    //chart->createDefaultAxes();
    //return chartView_;
}

QtCharts::QChart *combobox_auswahl(QComboBox* achse, QComboBox* wert) //Wertet die Eingabe der Dropbdown-Menüs aus
{   QtCharts::QChart *chart = new QtCharts::QChart();
    if (achse->currentText() == "X-Achse")
    {
        if(wert->currentText() == "Beschleunigung")
        {
            chart = x_werte.acc;
        };
        if(wert->currentText() == "Geschwindigkeit")
        {
            chart = x_werte.vel;
        };
        if(wert->currentText() == "Distanz")
        {
            chart = x_werte.dis;
        };

    };
    if (achse->currentText() == "Y-Achse")
    {
        if(wert->currentText() == "Beschleunigung")
        {
            chart = y_werte.acc;
        };
        if(wert->currentText() == "Geschwindigkeit")
        {
            chart = y_werte.vel;
        };
        if(wert->currentText() == "Distanz")
        {
            chart = y_werte.dis;
        };
    };
    if (achse->currentText() == "Z-Achse")
    {
        if(wert->currentText() == "Beschleunigung")
        {
            chart = z_werte.acc;
        };
        if(wert->currentText() == "Geschwindigkeit")
        {
            chart = z_werte.vel;
        };
        if(wert->currentText() == "Distanz")
        {
            chart = z_werte.dis;
        };
    };
    if (achse->currentText() == "Gesamt")
    {
        if(wert->currentText() == "Beschleunigung")
        {
            chart = g_werte.acc;
        };
        if(wert->currentText() == "Geschwindigkeit")
        {
            chart = g_werte.vel;
        };
        if(wert->currentText() == "Distanz")
        {
            chart = g_werte.dis;
        };
    };
    return chart;
}

QString max_acc(std::list<Datenpunkt> list ){// gibt die höchste Beschleunigung an
    double max_acc = 0;
    auto i = list.begin();
        while (i != list.end())
        {
            double current_acc = i->gacc ;
            if(current_acc>max_acc)
            {
                max_acc = current_acc;
            }
            i++;
        }
    return  QString::number(max_acc);
};

QString max_vel(std::list<Datenpunkt> list ){//s.o. für Geschwindigkeit
    double max_vel = 0;
    auto i = list.begin();
        while (i != list.end())
        {
            double current_vel = i->gvel ;
            if(current_vel>max_vel)
            {
                max_vel = current_vel;
            }
            i++;
        }
    return  QString::number(max_vel);
};


QString dauer(std::list<Datenpunkt> list ){//s.o. für Dauer
auto i = list.begin();
auto j = i;
while(i != list.end()){
    j = i;
    i++;
}
    double dauer = j->time;
    return  QString::number(dauer);
};

QString distanz(std::list<Datenpunkt> list ){//s.o. für Distanz
auto i = list.begin();
auto j = i;
while(i != list.end()){
        j = i;
        i++;
    }
    double distanz = j->gdis;
    return  QString::number(distanz);
};


QString file_name = "";
QStringList wordList;
std::list<Datenpunkt> complete;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QString achsen = "X-Achse,Y-Achse,Z-Achse,Gesamt";
    QStringList achsenwahl= achsen.split(",");
    ui->Achsen_Auswahl_Links->addItems(achsenwahl);
    ui->Achsen_Auswahl_Rechts->addItems(achsenwahl);
    QString werte = "Beschleunigung,Geschwindigkeit,Distanz";
    QStringList wertewahl = werte.split(",");
    ui->Wert_Auswahl_Links->addItems(wertewahl);
    ui->Wert_Auswahl_Rechts->addItems(wertewahl);


}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_BtWaehlen_clicked()//Dateiauswahl
{
    file_name = QFileDialog::getOpenFileName(this,"Wähle die Datei","C:/Users/Nils/Desktop");
    QFile f(file_name);
    ui->label->setText(file_name);
    datei_auswahl = true;

}



void MainWindow::on_BtAuswerten_clicked()//liest die Werte aus der Csv-Datei und erstellt die Graphen
{   if (ui->label->text() == ""){
    QMessageBox::information(this,"..","Es wurde keine passende Datei ausgewählt");
    }
    else
    file_name = ui->label->text();
    QFile f(file_name);
       if (f.open(QIODevice::ReadOnly))
       {
           //file opened successfully
           QString data;
           data = f.readAll();
           QRegExp separator("(,|\n)");
           wordList = data.split(separator);// hierrüber wird die csv-Datei ausgewertet und die Daten einzeln
           f.close();                       // in eine Liste(wordlist) geschrieben, die dann über fillin_data
           complete = fill_in_data(wordList);// in die Datenpunkte(Klasse) geschrieben werden
           create_graphs(complete); //create_graphs erstellt aus complete = Liste(Datenpunkte) die Graphen die über combobox_auswahl asugewählt werden können
           auswertung = true;//
           ui->Lbl_Dauer->setText(ui->Lbl_Dauer->text() + dauer(complete) + " s");
           ui->Lbl_Weg->setText(ui->Lbl_Weg->text() + distanz(complete) + " m");
           ui->Lbl_Mac_Vel->setText(ui->Lbl_Mac_Vel->text() + max_vel(complete)+ " m/s");
           ui->Lbl_Max_Acc->setText(ui->Lbl_Max_Acc->text() + max_acc(complete) + " m^2/s");
       }
        else
       {
        QMessageBox::information(this,"..","Datei konnte nicht gelesen werden");
       }


}



void MainWindow::on_BtDarstellen_Links_clicked() //Darstellen links, ruft combobox_auswahl auf
{   QtCharts::QChart *chart_links = new QtCharts::QChart();
    if (!datei_auswahl)
    {
        QMessageBox::information(this,"..","Es wurde keine Datei ausgewählt");
    }
    else if (!auswertung)
    {
        QMessageBox::information(this,"..","Datei wurde nicht ausgewertet");
    }
    else
    {
        chart_links = combobox_auswahl(ui->Achsen_Auswahl_Links,ui->Wert_Auswahl_Links);//Hiernach sollen noch die Eigenschaften des Graphen (Achsen etc.) angepasst werden


        ui->graphicsview_links->setChart(chart_links);//übergibt den Graphen dem GraphicsView Widget
    }

}

void MainWindow::on_pushButton_2_clicked()//Darstellen rechts, ruft combobox_auswahl auf
{
    QtCharts::QChart *chart_rechts = new QtCharts::QChart();
       if (!datei_auswahl)
       {
           QMessageBox::information(this,"..","Es wurde keine Datei ausgewählt");
       }
       else if (!auswertung)
       {
           QMessageBox::information(this,"..","Datei wurde nicht ausgewertet");
       }
       else
       {
           chart_rechts = combobox_auswahl(ui->Achsen_Auswahl_Rechts,ui->Wert_Auswahl_Rechts);

           ui->graphicsiew_rechts->setChart(chart_rechts);
       }


}
