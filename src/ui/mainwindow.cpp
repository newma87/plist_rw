﻿/*
 *  plist_rw version 1.0
 *  author by newma
 *  email to newma@live.cn
 */
#include <QtGui>
#include <stdlib.h>

#include "framecollector.h"
#include "mainwindow.h"

#include "io/plistxmlreader.h"
#include "io/plistxmlwriter.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), m_isModified(false)
{    
    m_pFrameCollector = new FrameCollector();

    setupActions();
    setupMenus();
    setupWidgets();
    setupDockWidgets();

    readSettings();
    setCurrentFile("");
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
        QString fileName = QFileDialog::getOpenFileName(this);
        if (!fileName.isEmpty())
        {
            return loadFile(fileName);
        }
    }

    return false;
}

bool MainWindow::save()
{
    if (!isModified())
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
    QString fileName = QFileDialog::getSaveFileName(this);
    if (!fileName.isEmpty())
    {
        return saveFile(fileName);
    }
    return false;
}

void MainWindow::adjustSize()
{
    m_pFrameCollector->adjustSize(true, false);
    contentWasModify();
}

void MainWindow::importImages()
{
    QString directName = QFileDialog::getExistingDirectory(this);
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

void MainWindow::contentWasModify()
{
    setModified(true);
    m_pWorkplace->updateContent();
    m_pAttrWidget->refresh();
}

bool MainWindow::loadFile(QString &file)
{
    bool bRet = false;
#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif

    PlistXmlReader reader(m_pFrameCollector);

    if (reader.read(file))
    {
        setModified(false);
        m_pWorkplace->updateContent();
        m_pAttrWidget->refresh();

        setCurrentFile(file);
        bRet = true;
    }

#ifndef QT_NO_CURSOR
     QApplication::restoreOverrideCursor();
#endif

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
        setModified(false);
        setCurrentFile(file);
        bRet = true;
    }

#ifndef QT_NO_CURSOR
     QApplication::restoreOverrideCursor();
#endif

    return bRet;
}

void MainWindow::importImageFiles(QString &directory)
{
    m_pImageslist->initWithDirectory(directory);
}

void MainWindow::clearup()
{
}

bool MainWindow::checkSaved()
{
    if (isModified())
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
    m_pActionSave = new QAction(tr("&Save..."), this);
    m_pActionSave->setShortcut(QKeySequence::Save);
    m_pActionSaveAs = new QAction(tr("&SaveAs..."), this);
    m_pActionSaveAs->setShortcut(QKeySequence::SaveAs);
    m_pActionClose = new QAction(tr("&Exit..."), this);
    m_pActionClose->setShortcut(QKeySequence::Close);
    m_pActionImport = new QAction(tr("&Import Images..."), this);
    m_pActionAjustSize = new QAction(tr("&Adjust Size"), this);

    connect(m_pActionOpen, SIGNAL(triggered()), this, SLOT(open()));
    connect(m_pActionSave, SIGNAL(triggered()), this, SLOT(save()));
    connect(m_pActionSaveAs, SIGNAL(triggered()), this, SLOT(saveAs()));
    connect(m_pActionClose, SIGNAL(triggered()), qApp, SLOT(quit()));
    connect(m_pActionImport, SIGNAL(triggered()), this, SLOT(importImages()));
    connect(m_pActionAjustSize, SIGNAL(triggered()), this, SLOT(adjustSize()));
}

void MainWindow::setupMenus()
{
    m_pFileMenu = menuBar()->addMenu(tr("&File"));
    m_pFileMenu->addAction(m_pActionOpen);
    m_pFileMenu->addAction(m_pActionSave);
    m_pFileMenu->addAction(m_pActionSaveAs);
    m_pFileMenu->addAction(m_pActionImport);
    m_pFileMenu->addAction(m_pActionClose);

    m_pEditMenu = menuBar()->addMenu(tr("&Edit"));
    m_pEditMenu->addAction(m_pActionAjustSize);

    m_pWindowMenu = menuBar()->addMenu(tr("&Window"));
}


void MainWindow::setupWidgets()
{
    m_pWorkplace = new WorkPlaceWidget(m_pFrameCollector, this);

    QScrollArea* pScroll = new QScrollArea(this);
    pScroll->setWidget(m_pWorkplace);
    pScroll->setWidgetResizable(true);

    setCentralWidget(pScroll);

    connect(m_pWorkplace, SIGNAL(contentWasModify()), this, SLOT(contentWasModify()));
}

void MainWindow::setupDockWidgets()
{
    m_pAttrwidget_dock = new QDockWidget(tr("attribute"), this);
    m_pAttrWidget = new AttributeWidget(m_pFrameCollector, this);
    m_pAttrwidget_dock->setWidget(m_pAttrWidget);
    m_pAttrwidget_dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    addDockWidget(Qt::LeftDockWidgetArea, m_pAttrwidget_dock);
    m_pWindowMenu->addAction(m_pAttrwidget_dock->toggleViewAction());
    connect(m_pAttrWidget, SIGNAL(contentWasModified()), this, SLOT(contentWasModify()));

    m_pImagelist_dock = new QDockWidget(tr("images"), this);
    m_pImageslist = new ImageListWidget(this);
    m_pImagelist_dock->setWidget(m_pImageslist);
    m_pImagelist_dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    addDockWidget(Qt::RightDockWidgetArea, m_pImagelist_dock);
    m_pWindowMenu->addAction(m_pImagelist_dock->toggleViewAction());
    m_pImagelist_dock->hide();
}

void MainWindow::setCurrentFile(const QString &fileName)
{
    m_curFile = fileName;
    setWindowModified(false);

    QString shownName = strippedName(m_curFile);
    if (m_curFile.isEmpty())
        shownName = "untitled.txt";
    setWindowFilePath(shownName);
}

QString MainWindow::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

bool MainWindow::isModified() const
{
    return m_isModified;
}

void MainWindow::setModified(bool val)
{
    m_isModified = val;
    setWindowModified(val);
}