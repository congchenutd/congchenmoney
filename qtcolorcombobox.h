/****************************************************************************
**
** Copyright (C) 2003-2006 Trolltech ASA. All rights reserved.
**
** This file is part of a Qt Solutions component.
**
** Licensees holding valid Qt Solutions licenses may use this file in
** accordance with the Qt Solutions License Agreement provided with the
** Software.
**
** See http://www.trolltech.com/products/qt/addon/solutions/ 
** or email sales@trolltech.com for information about Qt Solutions
** License Agreements.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/
#ifndef COLORCOMBOBOX_H
#define COLORCOMBOBOX_H
#include <qcombobox.h>
#include <qcolor.h>

#if defined(Q_WS_WIN)
#  if !defined(QT_QTCOLORCOMBOBOX_EXPORT) && !defined(QT_QTCOLORCOMBOBOX_IMPORT)
#    define QT_QTCOLORCOMBOBOX_EXPORT
#  elif defined(QT_QTCOLORCOMBOBOX_IMPORT)
#    if defined(QT_QTCOLORCOMBOBOX_EXPORT)
#      undef QT_QTCOLORCOMBOBOX_EXPORT
#    endif
#    define QT_QTCOLORCOMBOBOX_EXPORT __declspec(dllimport)
#  elif defined(QT_QTCOLORCOMBOBOX_EXPORT)
#    undef QT_QTCOLORCOMBOBOX_EXPORT
#    define QT_QTCOLORCOMBOBOX_EXPORT __declspec(dllexport)
#  endif
#else
#  define QT_QTCOLORCOMBOBOX_EXPORT
#endif

class QT_QTCOLORCOMBOBOX_EXPORT QtColorComboBox : public QComboBox
{
    Q_OBJECT
public:
    QtColorComboBox(QWidget *parent = 0, const char *name = 0);
    void insertColor(int index, const QColor &color, const QString &name);
    inline void addColor(const QColor &color, const QString &name)
                { insertColor(colorCount(), color, name); }

    int colorCount() const;

    void setCurrentColor(const QColor &color);
    QColor currentColor() const;

    QColor color(int index) const;

    void setColorDialogEnabled(bool enabled = true);
    bool isColorDialogEnabled() const;

    void setStandardColors();

    QSize sizeHint() const;

signals:
    void activated(const QColor &color);
    void highlighted(const QColor &color);

private slots:
    void emitActivatedColor(int index);
    void emitHighlightedColor(int index);

private:
    QColor lastActivated;
    int numUserColors;
    bool colorDialogEnabled;
};

#endif

