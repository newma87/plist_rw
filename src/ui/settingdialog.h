#ifndef SETTINGDIALOG_H
#define SETTINGDIALOG_H

#include <QDialog>

namespace Ui {
class SettingDialog;
}

class SettingDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit SettingDialog(QWidget *parent = 0);
    ~SettingDialog();
    
    int getHeight() const;
    int getWidth() const;
private slots:

    void on_txtHeight_editingFinished();

    void on_txtWidth_editingFinished();

    void on_chkPower_toggled(bool checked);

    void on_chkEqual_toggled(bool checked);

private:
    Ui::SettingDialog *ui;

    int m_width;
    int m_height;
};

#endif // SETTINGDIALOG_H
