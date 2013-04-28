#ifndef PHOSDIALOG_H
#define PHOSDIALOG_H

//http://stackoverflow.com/questions/9351407/how-to-include-qlineedit-in-qmessagebox-for-a-search-form

#include <QDialog>
#include <QLineEdit>
#include <QString>

class PHOSDialog : public QDialog
{
    Q_OBJECT
public:
    explicit PHOSDialog(QWidget *parent = 0);
    QString InputVal() const;

private:
    QLineEdit *LEdit;
    
signals:
    
public slots:
    
};

#endif // PHOSDIALOG_H
