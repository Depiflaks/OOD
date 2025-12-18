//
// Created by smmm on 12/18/25.
//

#include <QApplication>
#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>

int main(int argc, char *argv[]) {
	// 1. Создаем объект приложения
	QApplication app(argc, argv);

	// 2. Создаем главное окно (виджет)
	QWidget window;
	window.setWindowTitle("Тестовое окно Qt 6");
	window.resize(400, 300);

	// 3. Создаем контент (надпись)
	QVBoxLayout *layout = new QVBoxLayout(&window);
	QLabel *label = new QLabel("Привет! Qt и CMake работают успешно.", &window);
	label->setAlignment(Qt::AlignCenter);

	// Добавляем стиль для красоты
	label->setStyleSheet("font-size: 18px; color: #2c3e50; font-weight: bold;");

	layout->addWidget(label);

	// 4. Показываем окно
	window.show();

	// 5. Запускаем цикл обработки событий
	return app.exec();
}