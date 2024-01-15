#include "askme.h"
#include "ui_askme.h"

#include <QListWidget>

Askme::Askme(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Askme)
{
    ui->setupUi(this);
    cargarDatos();

}

Askme::~Askme()
{
    delete ui;
}

void Askme::on_apunteTomado(Apunte *apunte)
{
    for (int i = 0; i < m_asignaturas.size(); ++i) {
        if (i >= 0 && i < m_asignaturas.size()) {
            Asignatura *a = m_asignaturas[i];
            qDebug().noquote() << a->toString();
        } else {
            // Manejo de error: el índice está fuera de los límites
            qDebug() << "Índice fuera de límites en m_asignaturas: " << i;
        }
    }

    guardar();

}

void Askme::cargarSubVentana(QWidget *ventana)
{
    auto sub = ui->mdiArea->addSubWindow(ventana);
    sub->show();
}

//cuando se agregue un nuevo apunte , guardar Todo en un archivo CSV (Separado por tab). Para esto, crear un metodo en askme.cpp denominado guardar.
// Una vez guardado la informacion, Modificar el metodo cargar() para que lea el archivo CSV y carge la informacion en m_asginaturas
// crear el formulario de Lista de apuntes para mostrar los apuntes guardados

void Askme::cargarDatos()
{
    QFile archivo("datos.csv");

    if (archivo.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream in(&archivo);

        Asignatura *asignatura = nullptr;
        Tema *tema = nullptr;

        while (!in.atEnd())
        {
            QString linea = in.readLine();
            QStringList campos = linea.split("\t");  // Cambia el separador según el formato del archivo

            if (campos.size() > 1)
            {
                QString tipo = campos[0];
                QString valor = campos[1];

                if (tipo == "Asignatura")
                {
                    asignatura = new Asignatura(valor);
                    m_asignaturas.append(asignatura);
                }
                else if (tipo == "Tema")
                {
                    tema = new Tema(valor);
                    asignatura->agregarTema(tema);
                }
                else if (tipo == "Apunte")
                {
                    if (tema)
                    {
                        if (campos.size() > 2)
                        {
                            QString termino = campos[1];
                            QString concepto = campos[2];
                            Apunte *apunte = new Apunte(termino, concepto);
                            tema->agregarApunte(apunte);
                        }
                    }
                }
            }
        }

        archivo.close();
    }
    else
    {
        qDebug() << "No se pudo abrir el archivo para leer.";
    }
}


void Askme::on_actionNuevo_triggered()
{
    ApunteForm *w = new ApunteForm(this);
    w->setAsignaturas(m_asignaturas);
    w->cargarAsignaturas();

    connect(w, SIGNAL(apunteTomado(Apunte*)), this, SLOT(on_apunteTomado(Apunte*)));

    cargarSubVentana(w);
}

void Askme::guardar()
{
    QFile archivo("datos.csv");

    if (archivo.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream out(&archivo);

        // Guardar asignaturas, temas y apuntes en el archivo CSV
        foreach (Asignatura *asignatura, m_asignaturas)
        {
            out << "Asignatura\t" << asignatura->nombre()<< "\n";

            foreach (Tema *tema, asignatura->temas())
            {
                out << "Tema\t" << tema->nombre() << "\n";

                foreach (Apunte *apunte, tema->apuntes())
                {
                    out << "Apunte\t" << apunte->termino() << "\t" << apunte->concepto() << "\n";
                }
            }
        }

        archivo.close();
    }
    else
    {
        qDebug() << "No se pudo abrir el archivo para escribir.";
    }


}


void Askme::mostrarListaApuntes()
{
    // Crear un widget de lista
    QListWidget *listaWidget = new QListWidget(this);

    foreach (Asignatura *asignatura, m_asignaturas)
    {
        foreach (Tema *tema, asignatura->temas())
        {
            foreach (Apunte *apunte, tema->apuntes())
            {
                QString texto = QString("%1 - %2: %3")
                                    .arg(asignatura->nombre())
                                    .arg(tema->nombre())
                                    .arg(apunte->toString());

                QListWidgetItem *item = new QListWidgetItem(texto);
                listaWidget->addItem(item);
            }
        }
    }

    // Crear un diálogo para mostrar la lista
    QDialog *dialogo = new QDialog(this);
    QVBoxLayout *layout = new QVBoxLayout(dialogo);
    layout->addWidget(listaWidget);

    dialogo->setLayout(layout);
    dialogo->exec();  // Mostrar el diálogo modal
}

void Askme::on_actionLista_triggered()
{
    mostrarListaApuntes();
}

