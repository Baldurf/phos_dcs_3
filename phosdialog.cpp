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


PHOSDialog::PHOSDialog()
{

}

QString PHOSDialog::sPHOSDialog(QString message)
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("QInputDialog::getText()"),
                                              message, QLineEdit::Normal,
                                              NULL, &ok);
    QMessageBox::information(this,"Value from input:", text);
    if (ok && !text.isEmpty()){
        QMessageBox::information(this,"Value from input:", text);
        return text;
    }
    else return NULL;

   }

uint PHOSDialog::iPHOSDialog(int base_number) //integer PHOSDialog: for integer numbers.. returns a 32 bit uint.
{
     // base_number  Base of the string which is expected. Change 16 to 2 for binary.
    bool ok;
    QString text = QInputDialog::getText(this, tr("QInputDialog::getText()"),
                                              tr("Active FEC List:"), QLineEdit::Normal,
                                              NULL, &ok);
    QMessageBox::information(this,"New AFL is:", text);
    if (ok && !text.isEmpty())
    {
        QMessageBox::information(this,"New AFL is:", text);
        return text.toUInt(&ok, base_number);
    }
    else return NULL;

}
