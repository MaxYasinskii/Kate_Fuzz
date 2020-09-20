/*  This file is part of the Kate project.
 *  Based on the snippet plugin from KDevelop 4.
 *
 *  SPDX-FileCopyrightText: 2007 Robert Gruber <rgruber@users.sourceforge.net>
 *  SPDX-FileCopyrightText: 2012 Christoph Cullmann <cullmann@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "katesnippetglobal.h"

#include "editsnippet.h"
#include "snippet.h"
#include "snippetcompletionitem.h"
#include "snippetcompletionmodel.h"
#include "snippetrepository.h"
#include "snippetstore.h"

#include <KAboutData>
#include <KLocalizedString>
#include <KPluginFactory>
#include <KPluginLoader>
#include <ktexteditor/application.h>
#include <ktexteditor/codecompletioninterface.h>
#include <ktexteditor/document.h>
#include <ktexteditor/editor.h>
#include <ktexteditor/mainwindow.h>
#include <ktexteditor/view.h>

#include <QDialogButtonBox>
#include <QMenu>

KateSnippetGlobal *KateSnippetGlobal::s_self = nullptr;

KateSnippetGlobal::KateSnippetGlobal(QObject *parent, const QVariantList &)
    : QObject(parent)
{
    s_self = this;

    SnippetStore::init(this);
    m_model.reset(new SnippetCompletionModel);
}

KateSnippetGlobal::~KateSnippetGlobal()
{
    delete SnippetStore::self();
    s_self = nullptr;
}

void KateSnippetGlobal::insertSnippet(Snippet *snippet)
{
    // query active view, always prefer that!
    KTextEditor::View *view = KTextEditor::Editor::instance()->application()->activeMainWindow()->activeView();

    // fallback to stuff set for dialog
    if (!view)
        view = m_activeViewForDialog;

    // no view => nothing to do
    if (!view)
        return;

    // try to insert snippet
    SnippetCompletionItem item(snippet, static_cast<SnippetRepository *>(snippet->parent()));
    item.execute(view, KTextEditor::Range(view->cursorPosition(), view->cursorPosition()));

    // set focus to view
    view->setFocus();
}

void KateSnippetGlobal::insertSnippetFromActionData()
{
    QAction *action = dynamic_cast<QAction *>(sender());
    Q_ASSERT(action);
    Snippet *snippet = action->data().value<Snippet *>();
    Q_ASSERT(snippet);
    insertSnippet(snippet);
}

void KateSnippetGlobal::createSnippet(KTextEditor::View *view)
{
    // no active view, bad
    if (!view)
        return;

    // get mode
    QString mode = view->document()->highlightingModeAt(view->selectionRange().isValid() ? view->selectionRange().start() : view->cursorPosition());
    if (mode.isEmpty())
        mode = view->document()->mode();

    // try to look for a fitting repo
    SnippetRepository *match = nullptr;
    for (int i = 0; i < SnippetStore::self()->rowCount(); ++i) {
        SnippetRepository *repo = dynamic_cast<SnippetRepository *>(SnippetStore::self()->item(i));
        if (repo && repo->fileTypes().count() == 1 && repo->fileTypes().first() == mode) {
            match = repo;
            break;
        }
    }
    bool created = !match;
    if (created) {
        match = SnippetRepository::createRepoFromName(i18nc("Autogenerated repository name for a programming language", "%1 snippets", mode));
        match->setFileTypes(QStringList() << mode);
    }

    EditSnippet dlg(match, nullptr, view);
    dlg.setSnippetText(view->selectionText());
    int status = dlg.exec();
    if (created && status != QDialog::Accepted) {
        // cleanup
        match->remove();
    }
}
