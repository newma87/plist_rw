/*
 *  mainwindow.h
 *  plist_rw version 1.0
 *  author by newma
 *  email to newma@live.cn
 */
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDockWidget>

#include "imagelistwidget.h"
#include "workplacewidget.h"
#include "attributewidget.h"

class FrameCollector;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);

protected:
    void closeEvent(QCloseEvent *event);

public slots:
    bool open();
    bool save();
    bool saveAs();

    void adjustSize();
    void importImages();

    void contentWasModify();

private:
    bool loadFile(QString& file);
    bool saveFile(QString& file);
    void importImageFiles(QString& directory);
    void clearup();

private:
    void readSettings();
    void writeSettings();
    bool checkSaved();

    //UI
    void setupActions();
    void setupMenus();
    void setupWidgets();
    void setupDockWidgets();

    void setCurrentFile(const QString &fileName);
    static QString strippedName(const QString &fullFileName);

    bool isModified() const;
    void setModified(bool val);

private:
    // menu
    QMenu* m_pFileMenu;
    QMenu* m_pEditMenu;
    QMenu* m_pWindowMenu;

    //menu actions
    QAction* m_pActionOpen;
    QAction* m_pActionSave;
    QAction* m_pActionSaveAs;
    QAction* m_pActionClose;
    QAction* m_pActionImport;
    QAction* m_pActionAjustSize;

    QString m_curFile;

    QDockWidget* m_pImagelist_dock;
    QDockWidget* m_pAttrwidget_dock;

    ImageListWidget* m_pImageslist;
    WorkPlaceWidget* m_pWorkplace;
    AttributeWidget* m_pAttrWidget;

    FrameCollector* m_pFrameCollector;

    bool m_isModified;
};

#endif // MAINWINDOW_H
