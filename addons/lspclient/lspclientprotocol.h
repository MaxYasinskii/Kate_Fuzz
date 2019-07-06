/***************************************************************************
 *   Copyright (C) 2019 by Mark Nauwelaerts <mark.nauwelaerts@gmail.com>   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA .        *
 ***************************************************************************/

#ifndef LSPCLIENTPROTOCOL_H
#define LSPCLIENTPROTOCOL_H

#include <QString>
#include <QUrl>
#include <QList>
#include <QVector>

enum class LSPErrorCode
{
    // Defined by JSON RPC
    ParseError = -32700,
    InvalidRequest = -32600,
    MethodNotFound = -32601,
    InvalidParams = -32602,
    InternalError = -32603,
    serverErrorStart = -32099,
    serverErrorEnd = -32000,
    ServerNotInitialized = -32002,
    UnknownErrorCode = -32001,

    // Defined by the protocol.
    RequestCancelled = -32800,
    ContentModified = -32801
};

enum class LSPDocumentSyncKind
{
    None = 0,
    Full = 1,
    Incremental = 2
};

struct LSPCompletionOptions
{
    bool provider = false;
    bool resolveProvider = false;
    QVector<QChar> triggerCharacters;
};

struct LSPSignatureHelpOptions
{
    bool provider = false;
    QVector<QChar> triggerCharacters;
};

struct LSPServerCapabilities
{
    LSPDocumentSyncKind textDocumentSync = LSPDocumentSyncKind::None;
    bool hoverProvider = false;
    LSPCompletionOptions completionProvider;
    LSPSignatureHelpOptions signatureHelpProvider;
    bool definitionProvider = false;
    // FIXME ? clangd unofficial extension
    bool declarationProvider = false;
    bool referencesProvider = false;
    bool documentSymbolProvider = false;
};

enum class LSPMarkupKind
{
    None = 0,
    PlainText = 1,
    MarkDown = 2
};

struct LSPMarkupContent
{
    LSPMarkupKind kind = LSPMarkupKind::None;
    QString value;
};

struct LSPPosition
{
    // both are 0-based
    // (negative if optional/non-present)
    int line;
    int column;
};

struct LSPRange
{
    LSPPosition start;
    LSPPosition end;
};

struct LSPLocation
{
    QUrl uri;
    LSPRange range;
};

struct LSPHover
{
    LSPMarkupContent contents;
    LSPRange range;
};

enum class LSPSymbolKind {
    File = 1,
    Module = 2,
    Namespace = 3,
    Package = 4,
    Class = 5,
    Method = 6,
    Property = 7,
    Field = 8,
    Constructor = 9,
    Enum = 10,
    Interface = 11,
    Function = 12,
    Variable = 13,
    Constant = 14,
    String = 15,
    Number = 16,
    Boolean = 17,
    Array = 18,
};

struct LSPSymbolInformation
{
    LSPSymbolInformation(const QString & _name, LSPSymbolKind _kind,
                         LSPRange _range, const QString & _detail = QString())
        : name(_name), detail(_detail), kind(_kind), range(_range)
    {}
    QString name;
    QString detail;
    LSPSymbolKind kind;
    LSPRange range;
    QList<LSPSymbolInformation> children;
};

enum class LSPCompletionItemKind
{
    Text = 1,
    Method = 2,
    Function = 3,
    Constructor = 4,
    Field = 5,
    Variable = 6,
    Class = 7,
    Interface = 8,
    Module = 9,
    Property = 10,
    Unit = 11,
    Value = 12,
    Enum = 13,
    Keyword = 14,
    Snippet = 15,
    Color = 16,
    File = 17,
    Reference = 18,
    Folder = 19,
    EnumMember = 20,
    Constant = 21,
    Struct = 22,
    Event = 23,
    Operator = 24,
    TypeParameter = 25,
};

struct LSPCompletionItem
{
    QString label;
    LSPCompletionItemKind kind;
    QString detail;
    LSPMarkupContent documentation;
    QString sortText;
    QString insertText;
};

struct LSPParameterInformation
{
    // offsets into overall signature label
    // (-1 if invalid)
    int start;
    int end;
};

struct LSPSignatureInformation
{
    QString label;
    LSPMarkupContent documentation;
    QList<LSPParameterInformation> parameters;
};

struct LSPSignatureHelp
{
    QList<LSPSignatureInformation> signatures;
    int activeSignature;
    int activeParameter;
};

#endif
