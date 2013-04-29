/*
 * written by user kharvd on stackoverflow.com 19.02.12
 *
 * http://stackoverflow.com/questions/9351407/how-to-include-qlineedit-in-qmessagebox-for-a-search-form/9351654#9351654
 *
 */

#ifndef PHOSDIALOG_H
#define PHOSDIALOG_H



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
