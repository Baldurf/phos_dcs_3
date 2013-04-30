/*
 * written by user kharvd on stackoverflow.com 19.02.12
 *
 * http://stackoverflow.com/questions/9351407/how-to-include-qlineedit-in-qmessagebox-for-a-search-form/9351654#9351654
 *
 *
 *      How to show a dialog using phosdialog
 *int base_number = 2; //change 16 to 2 for binary
 *bool ok;
 *QLabel dLabel; //see class reference
 *PHOSDialog *pDialog = new PHOSDialog(&dLabel); //declaring pDialog as a PHOSDialog
 *dLabel.show(); // Not needed. Sets the position of the dialog to the center of the QMainWindow
 *uint SendData;
 *if (pDialog->exec()) {
 *uint SendData = pDialog->InputVal().toUInt(&ok, base_number);
 *}
 *if(ok = false) //if fail
 *{
 *    QMessageBox::information(this, "Conversion error!","Could not convert the input to uint (uint for reference)");
 *    return;
 *}
 *
 *      For debigging purposes
 *QString string;
 *string.setNum(SendData,base_number);
 *QMessageBox::information(this,"New AFL in hex", string);
 */


#include "phosdialog.h"


// http://stackoverflow.com/questions/9351407/how-to-include-qlineedit-in-qmessagebox-for-a-search-form

#include <QVBoxLayout>
#include <QPushButton>
#include <QDialogButtonBox>

PHOSDialog::PHOSDialog(QWidget *parent) :
    QDialog(parent)
{
    LEdit = new QLineEdit(this);
    QPushButton *acceptButton = new QPushButton(tr("Accept"));
    acceptButton->setDefault(true);

    QPushButton *cancelButton = new QPushButton(tr("Cancel"));

    QDialogButtonBox *DialogBox = new QDialogButtonBox(Qt::Horizontal);
    DialogBox->addButton(acceptButton, QDialogButtonBox::AcceptRole);
    DialogBox->addButton(cancelButton, QDialogButtonBox::RejectRole);

    connect(DialogBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(DialogBox, SIGNAL(rejected()), this, SLOT(reject()));

    QVBoxLayout *lt = new QVBoxLayout;
    lt->addWidget(LEdit);
    lt->addWidget(DialogBox);

    setLayout(lt);

    }

    QString PHOSDialog::InputVal() const
    {
        return LEdit->text();
    }
