#include <QIntValidator>
#include "settingdialog.h"
#include "core/framecollector.h"
#include "util/util.h"
#include "ui_settingdialog.h"

SettingDialog::SettingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingDialog)
{
    ui->setupUi(this);

    QIntValidator* validator = new QIntValidator(1, 65535, this);
    ui->txtHeight->setValidator(validator);
    ui->txtWidth->setValidator(validator);

    QSize size = FrameCollector::instance()->getSize();
    m_width = size.width();
    m_height = size.height();

    ui->txtWidth->setText(QString::number(m_width));
    ui->txtHeight->setText(QString::number(m_height));

    setFixedSize(this->size());
}

SettingDialog::~SettingDialog()
{
    delete ui;
}

int SettingDialog::getHeight() const
{
    return m_width;
}

int SettingDialog::getWidth() const
{
    return m_height;
}

void SettingDialog::on_txtHeight_editingFinished()
{
    m_height = ui->txtHeight->text().toInt();
    if (ui->chkPower->isChecked())
    {
        m_height = getPowerOf2Value(m_height);
    }

    if (ui->chkEqual->isChecked())
    {
        m_width = m_height;
        ui->txtWidth->setText(QString::number(m_width));
    }

    ui->txtHeight->setText(QString::number(m_height));
}

void SettingDialog::on_txtWidth_editingFinished()
{
    m_width = ui->txtWidth->text().toInt();
    if (ui->chkPower->isChecked())
    {
        m_width = getPowerOf2Value(m_width);
    }

    if (ui->chkEqual->isChecked())
    {
        m_height = m_width;
        ui->txtHeight->setText(QString::number(m_height));
    }
    ui->txtWidth->setText(QString::number(m_width));
}

void SettingDialog::on_chkPower_toggled(bool checked)
{
    if (checked)
    {
        m_width = getPowerOf2Value(m_width);
        m_height = getPowerOf2Value(m_height);

        ui->txtWidth->setText(QString::number(m_width));
        ui->txtHeight->setText(QString::number(m_height));
    }
}

void SettingDialog::on_chkEqual_toggled(bool checked)
{
    if (checked)
    {
        int val = MAX(m_width, m_height);
        m_width = m_height = val;

        ui->txtWidth->setText(QString::number(m_width));
        ui->txtHeight->setText(QString::number(m_height));
    }
}
