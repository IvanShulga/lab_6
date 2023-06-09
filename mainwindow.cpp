#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_button_add_student_clicked()
{
    Student student;
    student.name = ui->lineEdit_name->text();
    student.group_number = ui->lineEdit_group_number->text();
    student.date = ui->lineEdit_birthday->text();

    try {
        if (student.name.length() < 2 || student.name.length() > 40) {
            throw QString("Invalid name length! Name should be between 2 and 40 characters.");
        }

        if (student.group_number.length() < 2 || student.group_number.length() > 10) {
            throw QString("Invalid group number length! Group number should be between 2 and 10 characters.");
        }

        QString dateFormat = "dd.MM.yyyy";
        QDate enteredDate = QDate::fromString(student.date, dateFormat);
        if (!enteredDate.isValid()) {
            throw QString("Invalid date format! Date should be in the format 'dd.mm.yyyy'.");
        }

        QDate currentDate = QDate::currentDate();
        if (enteredDate > currentDate) {
            throw QString("Invalid date! The student's date of birth cannot be later than today's date.");
        }

        QDate minDate(1900, 1, 1);
        if (enteredDate < minDate) {
            throw QString("Invalid date! The student's date of birth cannot be earlier than the year 1900.");
        }

        QFile file("binary.bin");
        if (!file.open(QIODevice::Append)) {
            throw QString("Can't open file");
        }

        QDataStream out(&file);
        out << student.name << student.group_number << student.date;

        file.close();

        QString message = "You entered:\n"
                          "Student's name: " + student.name + "\n"
                                           "Student's group number: " + student.group_number + "\n"
                                                   "Student's date of birth: " + student.date;

        QMessageBox::information(this, "Student Information", message);

        int age = enteredDate.daysTo(currentDate) / 365;
        if (age < 18) {
            QFile underageFile("underage_students.bin");
            if (!underageFile.open(QIODevice::Append)) {
                throw QString("Can't open file");
            }

            QDataStream underageOut(&underageFile);
            underageOut << student.name << student.group_number << student.date;

            underageFile.close();
        }
    } catch (const QString& error) {
        QMessageBox::critical(this, "Error", error);
    }
}

QString readFileContents(const QString& fileName) {
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Can't open file for reading";
        return "";
    }

    QDataStream in(&file);

    QString allStudentsData;

    while (!in.atEnd()) {
        QString name, groupNumber, date;
        in >> name >> groupNumber >> date;

        QString studentData = "Student's name: " + name +
                              "\nStudent's group number: " + groupNumber +
                              "\nStudent's date of birthday: " + date +
                              "\n\n";

        allStudentsData += studentData;
    }

    file.close();

    if (file.error() != QFile::NoError) {
        qDebug() << "Error while entering file";
    }

    return allStudentsData;
}

void MainWindow::on_button_print_file_clicked()
{
    QString file1Contents = readFileContents("binary.bin");
    QString file2Contents = readFileContents("underage_students.bin");

    ui->label_print_file_1->setText(file1Contents);

    ui->label_print_file_2->setText(file2Contents);
}

void MainWindow::on_button_erase_file_content_clicked()
{

    QFile file1("binary.bin");
    if (!file1.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        qDebug() << "Can't open first file for writing";
        return;
    }
    file1.close();

    if (file1.error() != QFile::NoError) {
        qDebug() << "Error while closing first file";
    }

    QFile file2("underage_students.bin");
    if (!file2.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        qDebug() << "Can't open second file";
        return;
    }
    file2.close();

    if (file2.error() != QFile::NoError)
        qDebug() << "Error while closing second file";
    else
        QMessageBox::information(this, "Files Erased", "You've successfully erased content of both files!");
}

void MainWindow::on_pushButton_3_clicked()
{
    ui->label_print_file_1->setText("");
    ui->label_print_file_2->setText("");
}
