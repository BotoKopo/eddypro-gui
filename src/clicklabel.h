/***************************************************************************
  clicklabel.h
  -------------------
  QLabel class with clicked signal
  -------------------
  Copyright (C) 2007-2011, Eco2s team, Antonio Forgione
  Copyright (C) 2011-2014, LI-COR Biosciences
  Author: Antonio Forgione

  This file is part of EddyPro (R).

  EddyPro (R) is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  EddyPro (R) is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with EddyPro (R). If not, see <http://www.gnu.org/licenses/>.
****************************************************************************/

#ifndef CLICKLABEL_H
#define CLICKLABEL_H

#include <QLabel>
#include <QTime>
#include <QMetaType>

class QMouseEvent;

class ClickLabel : public QLabel
{
    Q_OBJECT

    // property system for question mark clickalabel in metadata editor tables
    Q_PROPERTY(HeaderData headerData READ headerData WRITE setHeaderData)
    Q_ENUMS(HeaderData)

public:
    explicit ClickLabel(QWidget *parent = 0, Qt::WindowFlags flags = 0);
    explicit ClickLabel(const QString &text, QWidget *parent = 0, Qt::WindowFlags flags = 0);
    explicit ClickLabel(const ClickLabel& clabel);
    ~ClickLabel();

    enum HeaderData {
        NoHeader = 0,
        AnemNAlign,
        AnemNOffset,
        AnemNSep,
        AnemESep,
        AnemVSep,
        IrgaModel = 10,
        IrgaLPathLength,
        IrgaTPathLength,
        VarIgnoreDesc = 20,
        VarNumericDesc,
        VarDesc,
        VarConv,
        VarMinValue,
        VarMaxValue,
        VarNomTLag,
        VarMinTLag,
        VarMaxTLag
    };
    HeaderData headerData() const { return headerData_; }
    void setHeaderData(HeaderData data);

Q_SIGNALS:
    void clicked();

protected:
    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);

private:
    QTime time;
    HeaderData headerData_;
};

Q_DECLARE_METATYPE(ClickLabel)

#endif // CLICKLABEL_H