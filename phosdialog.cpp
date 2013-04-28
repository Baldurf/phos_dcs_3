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
