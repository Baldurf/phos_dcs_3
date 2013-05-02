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

/*
 *---------------------------------
 *How to use these dialogs
 *
 *      //Give "message" as a question for the dialog. This is what the Dialog asks for.
 *      QString message = "Give your input";
 *
 *      //the dialog
 *      PHOSDialog *pDia = new PHOSDialog;
 *
 *      //The string version of the dialog
 *      QString streeng = pDia->sPHOSDialog(QString message);
 *
 *      --------------------------------------------------------------------------
 *
 *      //For the integer version of the Dialog:
 *
 *      //Give "message" as a question for the dialog. This is what the Dialog asks for.
 *      //Might be a good idea to include information about base_number
 *
 *      QString message = "Give your input in "base_number" format";
 *
 *      //the dialog
 *      PHOSDialog *pDia = new PHOSDialog;
 *
 *      //integer version of the dialog.
 *      //base_number is the expected base of the input.
 *      uint talll = pDia->iPHOSDialog(uint base_number, QStringmessage);
 *
 *
 */



PHOSDialog::PHOSDialog()
{

}

QString PHOSDialog::sPHOSDialog(QString message)
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("PHOS string dialog"),
                                              message, QLineEdit::Normal,
                                              NULL, &ok);
    if (ok && !text.isEmpty()){
        QMessageBox::information(this,"Value from input:", text); //for debugging
        return text;
    }
    else return NULL;

   }

uint PHOSDialog::iPHOSDialog(int base_number, QString message) //integer PHOSDialog: for integer numbers.. returns a 32 bit uint.
{
     // base_number  Base of the string which is expected. Change 16 to 2 for binary.
    bool ok;
    QString text = QInputDialog::getText(this, tr("PHOS integer dialog"),
                                              message, QLineEdit::Normal,
                                              NULL, &ok);
    if (ok && !text.isEmpty())
    {
        QMessageBox::information(this,"New AFL is:", text); //for debugging
        return text.toUInt(&ok, base_number);
    }
    else return NULL;

}
