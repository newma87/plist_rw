/*
 *  plist_rw version 1.0
 *  author by newma
 *  email to newma@live.cn
 */
#include <QtGui>
#include <stdlib.h>

#include "core/frame.h"
#include "core/framecollector.h"
#include "mainwindow.h"
#include "settingdialog.h"

#include "io/plistxmlreader.h"
#include "io/plistxmlwriter.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{    
    m_pFrameCollector = FrameCollector::instance();

    setupActions();
    setupMenus();
    setupWidgets();
    setupDockWidgets();
    statusBar()->show();
    statusBar()->showMessage(tr("Ready"));

    readSettings();
    setCurrentFile("");

    connectFrameCollector();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (checkSaved())
    {
        writeSettings();
        event->accept();
    }
    else
    {
        event->ignore();
    }
}

bool MainWindow::open()
{
    if (checkSaved())
    {
        QString fileName = QFileDialog::getOpenFileName(this, tr("Open plist file"), tr(""), QString("*.plist"));
        if (!fileName.isEmpty())
        {
            return loadFile(fileName);
        }
    }

    return false;
}

bool MainWindow::save()
{
    if (!m_pFrameCollector->isModified())
    {
        return true;
    }

    if (m_curFile.isEmpty())
    {
        return saveAs();
    }
    else
    {
        return saveFile(m_curFile);
    }
}

bool MainWindow::saveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save plist file"));
    if (!fileName.isEmpty())
    {
        return saveFile(fileName);
    }
    return false;
}

void MainWindow::adjustSize()
{
    m_pFrameCollector->adjustSize(true, false);

    statusBar()->showMessage(tr("New Image Size: %1x%2")
                             .arg(m_pFrameCollector->getSize().width())
                             .arg(m_pFrameCollector->getSize().height()));
}

void MainWindow::clearup()
{
    m_pFrameCollector->clear();
    m_pFrameCollector->setModified(true);
    m_pWorkplace->updateContent();
    m_pAttrWidget->refresh();

    onContentWasModify();

    statusBar()->showMessage(tr("Clear done, file is empty."), 2000);
}

void MainWindow::importImages()
{
    QString directName = QFileDialog::getExistingDirectory(this, tr("Import image source"));
    if (!directName.isEmpty())
    {
#ifndef QT_NO_CURSOR
        QApplication::setOverrideCursor(Qt::WaitCursor);
#endif

        importImageFiles(directName);

        m_pImagelist_dock->show();

#ifndef QT_NO_CURSOR
        QApplication::restoreOverrideCursor();
#endif
    }
}

void MainWindow::exportImages()
{
    QString directName = QFileDialog::getExistingDirectory(this, tr("Export images to files"));
    if (!directName.isEmpty())
    {
#ifndef QT_NO_CURSOR
        QApplication::setOverrideCursor(Qt::WaitCursor);
#endif

        FrameCollector::const_iterator it = m_pFrameCollector->constBegin();
        for (; it != m_pFrameCollector->constEnd(); ++it)
        {
            const Frame* pFrame = *it;
            if (pFrame->bValid)
            {
                QString savePath = directName + "/" + pFrame->name;
                pFrame->image.toImage().save(savePath, "PNG");
            }
        }

        QMessageBox::information(this, tr("Export Image"), tr("Export images done!"));

#ifndef QT_NO_CURSOR
        QApplication::restoreOverrideCursor();
#endif
    }
}

void MainWindow::settings()
{
    SettingDialog dlg(this);
    if (dlg.exec() == QDialog::Accepted)
    {
        m_pFrameCollector->setSize(QSize(dlg.getWidth(), dlg.getHeight()));

        statusBar()->showMessage(tr("New Image Size: %1x%2")
                             .arg(m_pFrameCollector->getSize().width())
                             .arg(m_pFrameCollector->getSize().height()));
    }
}

void MainWindow::onContentWasModify()
{
    this->setWindowModified(m_pFrameCollector->isModified());
}

bool MainWindow::loadFile(QString &file)
{
    bool bRet = false;
#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif

    disconnectFrameCollector();

    PlistXmlReader reader(m_pFrameCollector);

    if (reader.read(file))
    {
        setCurrentFile(file);
        bRet = true;
    }

    connectFrameCollector();

    m_pWorkplace->updateContent();
    m_pAttrWidget->refresh();

    m_pActionExport->setDisabled(false);

#ifndef QT_NO_CURSOR
     QApplication::restoreOverrideCursor();
#endif

    statusBar()->showMessage(tr("File loaded..."), 2000);

    return bRet;
}

bool MainWindow::saveFile(QString &file)
{
    bool bRet = false;

#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif

    PlistXmlWriter writer(m_pFrameCollector);
    if (writer.write(file))
    {
        setCurrentFile(file);
        bRet = true;
    }

#ifndef QT_NO_CURSOR
     QApplication::restoreOverrideCursor();
#endif

    statusBar()->showMessage(tr("File saved..."), 2000);
    return bRet;
}

void MainWindow::importImageFiles(QString &directory)
{
    m_pImageslist->initWithDirectory(directory);
    statusBar()->showMessage(tr("Images imported..."), 2000);
}

bool MainWindow::checkSaved()
{
    if (m_pFrameCollector->isModified())
    {
        QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(this, tr("plist_rw"),
                                   tr("The document has been modified.\n"
                                      "Do you want to save your changes?"),
                                   QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

        if (ret == QMessageBox::Cancel)
        {
            return false;
        }

        if (ret == QMessageBox::Save)
        {
            if (!save())
            {
                QMessageBox::critical(this, tr("plist_rw"), tr("Save file failed!\nApplication will not be closed!"));
                return false;
            }
        }
    }
    return true;
}

void MainWindow::readSettings()
{
    QSettings settings("newma", "plist_rw");
    QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
    QSize size = settings.value("size", QSize(400, 400)).toSize();
    resize(size);
    move(pos);
}

void MainWindow::writeSettings()
{
    QSettings settings("newma", "plist_rw");
    settings.setValue("pos", pos());
    settings.setValue("size", size());
}

void MainWindow::setupActions()
{
    m_pActionOpen = new QAction(tr("&Open..."), this);
    m_pActionOpen->setShortcut(QKeySequence::Open);
    m_pActionOpen->setStatusTip(tr("Open a plist file"));
    m_pActionSave = new QAction(tr("&Save..."), this);
    m_pActionSave->setShortcut(QKeySequence::Save);
    m_pActionSave->setStatusTip(tr("Save this image to plist file"));
    m_pActionSaveAs = new QAction(tr("&SaveAs..."), this);
    m_pActionSaveAs->setShortcut(QKeySequence::SaveAs);
    m_pActionSaveAs->setStatusTip(tr("Save as a plist file"));
    m_pActionClose = new QAction(tr("&Exit..."), this);
    m_pActionClose->setShortcut(QKeySequence::Close);
    m_pActionClose->setStatusTip(tr("Exit application"));
    m_pActionImport = new QAction(tr("&Import Images..."), this);
    m_pActionImport->setToolTip(tr("Import image source"));
    m_pActionImport->setStatusTip(tr("Import image source from a directory"));
    m_pActionExport = new QAction(tr("&Export To Images..."), this);
    m_pActionExport->setToolTip(tr("Break current image into pieces and export"));
    m_pActionExport->setDisabled(true);
    m_pActionExport->setStatusTip(tr("Acording to the plist file, break up the group image and saved by each image's name"));
    m_pActionAjustSize = new QAction(tr("&Adjust Size"), this);
    m_pActionAjustSize->setToolTip(tr("will resize current image with power of 2"));
    m_pActionAjustSize->setStatusTip(tr("Resize current group image into small size with power of 2"));
    m_pActionClear = new QAction(tr("&Clear"), this);
    m_pActionClear->setToolTip("Clear current image");
    m_pActionClear->setStatusTip("Clearup all the image in this plist file");
    m_pActionSetting = new QAction(tr("&Settings"), this);
    m_pActionSetting->setStatusTip(tr("Setting size of group image file"));

    connect(m_pActionOpen, SIGNAL(triggered()), this, SLOT(open()));
    connect(m_pActionSave, SIGNAL(triggered()), this, SLOT(save()));
    connect(m_pActionSaveAs, SIGNAL(triggered()), this, SLOT(saveAs()));
    connect(m_pActionClose, SIGNAL(triggered()), qApp, SLOT(quit()));
    connect(m_pActionImport, SIGNAL(triggered()), this, SLOT(importImages()));
    connect(m_pActionExport, SIGNAL(triggered()), this, SLOT(exportImages()));
    connect(m_pActionAjustSize, SIGNAL(triggered()), this, SLOT(adjustSize()));
    connect(m_pActionClear, SIGNAL(triggered()), this, SLOT(clearup()));
    connect(m_pActionSetting, SIGNAL(triggered()), this, SLOT(settings()));
}

void MainWindow::setupMenus()
{
    m_pFileMenu = menuBar()->addMenu(tr("&File"));
    m_pFileMenu->addAction(m_pActionOpen);
    m_pFileMenu->addAction(m_pActionSave);
    m_pFileMenu->addAction(m_pActionSaveAs);
    m_pFileMenu->addAction(m_pActionImport);
    m_pFileMenu->addAction(m_pActionExport);
    m_pFileMenu->addAction(m_pActionClose);

    m_pEditMenu = menuBar()->addMenu(tr("&Edit"));
    m_pEditMenu->addAction(m_pActionClear);
    m_pEditMenu->addAction(m_pActionAjustSize);
    m_pEditMenu->addAction(m_pActionSetting);

    m_pWindowMenu = menuBar()->addMenu(tr("&Window"));
}


void MainWindow::setupWidgets()
{
    m_pWorkplace = new WorkPlaceWidget(m_pFrameCollector, this);

    QScrollArea* pScroll = new QScrollArea(this);
    pScroll->setWidget(m_pWorkplace);
    pScroll->setWidgetResizable(true);

    setCentralWidget(pScroll);
}

void MainWindow::setupDockWidgets()
{
    m_pAttrwidget_dock = new QDockWidget(tr("Attribute"), this);
    m_pAttrWidget = new AttributeWidget(m_pFrameCollector, this);
    m_pAttrwidget_dock->setWidget(m_pAttrWidget);
    m_pAttrwidget_dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    addDockWidget(Qt::LeftDockWidgetArea, m_pAttrwidget_dock);
    m_pWindowMenu->addAction(m_pAttrwidget_dock->toggleViewAction());

    m_pImagelist_dock = new QDockWidget(tr("Images"), this);
    m_pImageslist = new ImageListWidget(this);
    m_pImagelist_dock->setWidget(m_pImageslist);
    m_pImagelist_dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    addDockWidget(Qt::RightDockWidgetArea, m_pImagelist_dock);
    m_pWindowMenu->addAction(m_pImagelist_dock->toggleViewAction());
    m_pImagelist_dock->hide();
}

void MainWindow::connectFrameCollector()
{
    connect(m_pFrameCollector, SIGNAL(contentWasModified()), this, SLOT(onContentWasModify()));

    connect(m_pFrameCollector, SIGNAL(frameAdded(int)), m_pWorkplace, SLOT(updateContent()));
    connect(m_pFrameCollector, SIGNAL(frameRemoved(int,Frame)), m_pWorkplace, SLOT(updateContent()));
    connect(m_pFrameCollector, SIGNAL(frameChanged(int)), m_pWorkplace, SLOT(updateContent()));
    connect(m_pFrameCollector, SIGNAL(sizeChanged(QSize)), m_pWorkplace, SLOT(updateContentSize(QSize)));

    connect(m_pFrameCollector, SIGNAL(frameAdded(int)), m_pAttrWidget, SLOT(refresh()));
    connect(m_pFrameCollector, SIGNAL(frameRemoved(int,Frame)), m_pAttrWidget, SLOT(refresh()));
    connect(m_pFrameCollector, SIGNAL(frameChanged(int)), m_pAttrWidget, SLOT(updateFrameElement(int)));
    connect(m_pFrameCollector, SIGNAL(sizeChanged(QSize)), m_pAttrWidget, SLOT(refresh()));
}

void MainWindow::disconnectFrameCollector()
{
    disconnect(m_pFrameCollector, SIGNAL(contentWasModified()), this, SLOT(onContentWasModify()));

    disconnect(m_pFrameCollector, SIGNAL(frameAdded(int)), m_pWorkplace, SLOT(updateContent()));
    disconnect(m_pFrameCollector, SIGNAL(frameRemoved(int,Frame)), m_pWorkplace, SLOT(updateContent()));
    disconnect(m_pFrameCollector, SIGNAL(frameChanged(int)), m_pWorkplace, SLOT(updateContent()));
    disconnect(m_pFrameCollector, SIGNAL(sizeChanged(QSize)), m_pWorkplace, SLOT(updateContent()));

    disconnect(m_pFrameCollector, SIGNAL(frameAdded(int)), m_pAttrWidget, SLOT(refresh()));
    disconnect(m_pFrameCollector, SIGNAL(frameRemoved(int,Frame)), m_pAttrWidget, SLOT(refresh()));
    disconnect(m_pFrameCollector, SIGNAL(frameChanged(int)), m_pAttrWidget, SLOT(updateFrameElement(int)));
    disconnect(m_pFrameCollector, SIGNAL(sizeChanged(QSize)), m_pAttrWidget, SLOT(refresh()));
}

void MainWindow::setCurrentFile(const QString &fileName)
{
    m_curFile = fileName;
    setWindowModified(false);
    m_pFrameCollector->setModified(false);

    QString shownName = strippedName(m_curFile);
    if (m_curFile.isEmpty())
        shownName = "untitled.txt";
    setWindowFilePath(shownName);
}

QString MainWindow::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}
