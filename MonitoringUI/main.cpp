#include "mainwindow.h"
#include "../common/appconfig.h"

#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QString configFilePath = QFileDialog::getOpenFileName(
        nullptr,
        "Выберите конфигурационный файл",
        "",
        "XML files (*.xml);;All files (*.*)"
        );

    if (configFilePath.isEmpty()) {
        return 0;
    }

    if (!AppConfig::instance().loadFromFile(configFilePath)) {
        QMessageBox::critical(
            nullptr,
            "Ошибка конфигурации",
            "Не удалось загрузить или прочитать конфигурационный файл.\n"
            "Приложение будет закрыто."
            );
        return 1;
    }
    MainWindow w;
    w.show();
    return a.exec();
}
