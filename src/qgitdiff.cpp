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


#include "qgitdiff.h"

#include <git2/diff.h>

#include "qgitcommit.h"

#include <iostream>
#include <QDebug>

namespace LibQGit2
{

extern "C" int fileDiffCallBack(const git_diff_delta *delta,float progress,void *payload)
{
    reinterpret_cast<QGitDiff*> (payload)->addFileChanged(delta);
    // we want to continue looping so return 0
    return 0;
}


extern "C" int addPatchHunksCallBack( const git_diff_delta *delta, const git_diff_range *range,
                                        const char *header, size_t header_len, void *payload)
{
    reinterpret_cast<QGitDiff*> (payload)->addPatchHunks(delta, header, header_len);
    // we want to continue looping so return 0
    return 0;
}


extern "C" int addPatchLinesCallBack(const git_diff_delta *delta, const git_diff_range *range,
                                        char usage, const char *line, size_t line_len, void *payload)
{
    reinterpret_cast<QGitDiff*> (payload)->addPatchLines(delta, line, usage, line_len);
    // we want to continue looping so return 0
    return 0;
}

/**
 * @brief QGitDiff::QGitDiff gets the differences between oid1 and oid2
 * @param oid1 - sha of commit to diff from
 * @param oid2 - sha of commit to diff to
 */
QGitDiff::QGitDiff(QGitRepository repo, QGitCommit commitFrom, QGitCommit commitTo)
{
    const git_diff_options opts = GIT_DIFF_OPTIONS_INIT;

    git_diff_tree_to_tree (&diff, repo.data(),commitFrom.tree().data(), commitTo.tree().data(), &opts);


    git_diff_foreach (diff, fileDiffCallBack, addPatchHunksCallBack, addPatchLinesCallBack, this);


}


QGitDiff::~QGitDiff()
{
    git_diff_list_free (diff);
}

void QGitDiff::addPatchLines(const git_diff_delta *delta, const char *line, char usage, int lineLen)
{
    //assumes that this is called after hunk call back so this will
    // just append to the end of the Last map that was created by addHunk
    QString fileName(delta->new_file.path);
    QString currentLine(line);
    QString realLine = currentLine.left(lineLen);

    QString currentFilePatch = deltas[QString(delta->new_file.path)];

    currentFilePatch.append(usage);
    currentFilePatch.append(realLine);

    deltas[fileName] = currentFilePatch;
}


void QGitDiff::addPatchHunks(const git_diff_delta *delta, const char *header, int headerLen)
{
    QString fileName(delta->new_file.path);
    QString currentFilePatch = deltas[QString(delta->new_file.path)];
    QString currentHeader(header);
    QString realHeader = currentHeader.left(headerLen);

    currentFilePatch.append(realHeader);

    deltas[fileName] = currentFilePatch;
}

void QGitDiff::addFileChanged(const git_diff_delta *delta)
{
    fileList.push_back(QString::fromLocal8Bit(delta->new_file.path));
    deltas.insert(QString(delta->new_file.path), QString());
}

QStringList QGitDiff::getFileChangedList()
{
    return fileList;
}

QString QGitDiff::getDeltasForFile(const QString &file)
{
    return deltas[file];
}

}
