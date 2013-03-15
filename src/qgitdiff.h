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

#ifndef QGITDIFF_H
#define QGITDIFF_H

#include <QStringList>
#include <QMultiMap>
#include <git2/diff.h>

#include "qgitobject.h"
#include "qgittree.h"
#include "qgitrepository.h"
#include "qgittree.h"

namespace LibQGit2
{
    class QGitRepository;
    class QGitCommit;

    class LIBQGIT2_EXPORT QGitDiff : public QGitObject
    {
    public:
         QGitDiff(QGitRepository repo, QGitCommit commitFrom, QGitCommit commitTo);

         virtual ~QGitDiff();

         QStringList getFileChangedList();

         QString getDeltasForFile(const QString &file);

         // public so they can be called from the c callback code
         void addFileDiff(const char *delta);
         void addFileChanged(const git_diff_delta *delta);
         void addPatchHunks(const git_diff_delta *delta, const char *header, int headerLen);
         void addPatchLines(const git_diff_delta *delta, const char *line, char usage, int lineLen);
    private:
         QStringList fileList;
         // Maps filename to patch hunk
         QMap<QString, QString> deltas;
         git_diff_list *diff;

    };
}
#endif // QGITDIFF_H
