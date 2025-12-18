#include <QApplication>
#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>

class Controller : public QObject
{
    Q_OBJECT

public:
    Controller(QLineEdit *inputField, QLabel *label, QObject *parent = nullptr)
        : QObject(parent), m_inputField(inputField), m_label(label)
    {}

public slots:
    void onButtonClicked()
    {
        QString text = m_inputField->text();
        m_label->setText("You entered: " + text);
    }

private:
    QLineEdit *m_inputField;
    QLabel *m_label;
};

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr) : QWidget(parent)
    {
        // Создание виджетов
        m_inputField = new QLineEdit(this);
        m_label = new QLabel("Enter something:", this);
        m_button = new QPushButton("Submit", this);

        // Создание контроллера
        m_controller = new Controller(m_inputField, m_label, this);

        // Сигналы и слоты
        connect(m_button, &QPushButton::clicked, m_controller, &Controller::onButtonClicked);

        // Размещение виджетов на окне
        QVBoxLayout *layout = new QVBoxLayout(this);
        layout->addWidget(m_label);
        layout->addWidget(m_inputField);
        layout->addWidget(m_button);

        setLayout(layout);
        setWindowTitle("Qt Example");
        resize(300, 150);
    }

private:
    QLineEdit *m_inputField;
    QLabel *m_label;
    QPushButton *m_button;
    Controller *m_controller;
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MainWindow window;
    window.show();

    return app.exec();
}

#include "main.moc"
