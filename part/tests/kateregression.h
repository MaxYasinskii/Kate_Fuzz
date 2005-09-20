/* This file is part of the KDE libraries
   Copyright (C) 2005 Hamish Rodda <rodda@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef KATEREGRESSION_H
#define KATEREGRESSION_H

#include <QObject>
#include <QMap>

#include <ktexteditor/cursor.h>

class KateDocument;

struct CursorSignalExpectation
{
  CursorSignalExpectation(bool a = false, bool b = false, bool c = false, bool d = false, bool e = false, bool f = false);
  void checkExpectationsFulfilled() const;

  bool expectCharacterDeletedBefore;
  bool expectCharacterDeletedAfter;
  bool expectCharacterInsertedBefore;
  bool expectCharacterInsertedAfter;
  bool expectPositionChanged;
  bool expectPositionDeleted;

  bool watcherExpectCharacterDeletedBefore;
  bool watcherExpectCharacterDeletedAfter;
  bool watcherExpectCharacterInsertedBefore;
  bool watcherExpectCharacterInsertedAfter;
  bool watcherExpectPositionChanged;
  bool watcherExpectPositionDeleted;
};

class KateRegression : public QObject, public KTextEditor::SmartCursorWatcher
{
  Q_OBJECT

  public:
    virtual void positionChanged(KTextEditor::SmartCursor* cursor);
    virtual void positionDeleted(KTextEditor::SmartCursor* cursor);
    virtual void characterDeleted(KTextEditor::SmartCursor* cursor, bool deletedBefore);
    virtual void characterInserted(KTextEditor::SmartCursor* cursor, bool insertedBefore);

  public slots:
    void slotCharacterDeleted(KTextEditor::SmartCursor* cursor, bool before);
    void slotCharacterInserted(KTextEditor::SmartCursor* cursor, bool before);
    void slotPositionChanged(KTextEditor::SmartCursor* cursor);
    void slotPositionDeleted(KTextEditor::SmartCursor* cursor);

  private slots:
    void testAll();

  private:
    void checkSmartManager();
    void addCursorExpectation(KTextEditor::Cursor* cursor, const CursorSignalExpectation& expectation);
    void checkSignalExpectations();

    KateDocument* m_doc;
    QMap<KTextEditor::SmartCursor*, CursorSignalExpectation> m_cursorExpectations;
};

#endif
