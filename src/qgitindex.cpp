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

#include "qgitindex.h"
#include "qgitindexentry.h"
#include "qgittree.h"
#include "qgitexception.h"

#include "qgitrepository.h"

#include <QtCore/QFile>

#include <git2/errors.h>
#include <git2/index.h>
#include <git2/repository.h>
#include <git2/tree.h>

namespace LibQGit2
{

QGitIndex::QGitIndex(git_index *index)
    : d(index, git_index_free)
{
}

QGitIndex::QGitIndex(const QGitIndex& other)
    : d(other.d)
{
}

QGitIndex::~QGitIndex()
{
}

void QGitIndex::open(const QString& indexPath)
{
    d.clear();
    git_index *index = 0;
    qGitThrow(git_index_open(&index, QFile::encodeName(indexPath)));
    d = ptr_type(index, git_index_free);
}

QGitOId QGitIndex::createTree()
{
    QGitOId oid;
    qGitThrow(git_index_write_tree(oid.data(), data()));
    return oid;
}

void QGitIndex::clear()
{
    return git_index_clear(data());
}

void QGitIndex::read() const
{
    qGitThrow(git_index_read(data()));
}

void QGitIndex::write()
{
    qGitThrow(git_index_write(data()));
}

size_t QGitIndex::find(const QString& path)
{
    size_t pos;

    git_index_find(&pos, data(), QFile::encodeName(path));

    return pos;
}

void QGitIndex::add(const QString& path, int stage)
{
    qGitThrow(git_index_add_bypath(data(), QFile::encodeName(path)));
}

void QGitIndex::remove(const QString& path, int position)
{
    qGitThrow(git_index_remove(data(), QFile::encodeName(path), position));
}

void QGitIndex::insert(const QGitIndexEntry &source_entry)
{
    qGitThrow(git_index_add(data(), source_entry.data()));
}

QGitIndexEntry QGitIndex::get(int n) const
{
    return QGitIndexEntry(git_index_get_byindex(data(), n));
}

unsigned int QGitIndex::entryCount() const
{
    return git_index_entrycount(data());
}

git_index* QGitIndex::data() const
{
    return d.data();
}

const git_index* QGitIndex::constData() const
{
    return d.data();
}

} // namespace LibQGit2
