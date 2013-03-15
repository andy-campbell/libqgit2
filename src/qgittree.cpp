/******************************************************************************
 * This file is part of the Gluon Development Platform
 * Copyright (c) 2011 Laszlo Papp <djszapi@archlinux.us>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "qgittree.h"

#include "qgittreeentry.h"
#include "qgitrepository.h"
#include "qgitoid.h"

#include <QtCore/QFile>

#include <git2/tree.h>
#include <git2/index.h>


namespace LibQGit2
{

QGitTree::QGitTree(git_tree *tree)
    : QGitObject(reinterpret_cast<git_object*>(tree))
{
}

QGitTree::QGitTree( const QGitTree& other )
    : QGitObject(other)
{
}

QGitTree::~QGitTree()
{
}

QGitOId QGitTree::oid()
{
    return QGitOId(git_tree_id(data()));
}

size_t QGitTree::entryCount()
{
    return git_tree_entrycount(data());
}

QGitTreeEntry QGitTree::entryByName(const QString& fileName) const
{
    git_tree_entry *entry;
    git_tree_entry_bypath(&entry, data(), QFile::encodeName(fileName));
    return QGitTreeEntry(entry ,true);
}

QGitTreeEntry QGitTree::entryByIndex(int idx) const
{
    return QGitTreeEntry(git_tree_entry_byindex(data(), idx), false);
}

extern "C" int addEntriesCallBack(const char *root, const git_tree_entry *entry, void *payload)
{

    reinterpret_cast<QGitTree*> (payload)->addEntry(root, entry);
    // we want to continue looping so return 0
    return 0;
}


void QGitTree::addEntry(const char* root, const git_tree_entry *entry)
{
    git_otype type = git_tree_entry_type(entry);
    if (type = GIT_OBJ_BLOB)
    {
        QString path = QString(root) + QString (git_tree_entry_name(entry));
        entries.push_back(path);
    }
}

const QStringList QGitTree::getAllEntries()
{
    git_tree_walk(data(), GIT_TREEWALK_PRE, addEntriesCallBack, this);

    return entries;
}

git_tree* QGitTree::data() const
{
    return reinterpret_cast<git_tree*>(QGitObject::data());
}

const git_tree* QGitTree::constData() const
{
    return reinterpret_cast<git_tree*>(QGitObject::data());
}

} // namespace LibQGit2
