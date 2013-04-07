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

extern "C" int printCB(  const git_diff_delta *delta, const git_diff_range *range,
                         char usage, const char *line, size_t line_len, void *payload)
{
    reinterpret_cast<QGitDiff*> (payload)->saveFullPatch(line);

    return 0;
}


/**
 * @brief QGitDiff::QGitDiff
 * @param repo the repository which contains the commits to the
 */
QGitDiff::QGitDiff(QGitRepository repo)
    : _repo (repo)
{
}

/**
 * @brief QGitDiff::diffCommits
 * @param commitFrom the commit to diff from
 * @param commitTo the commit to diff to
 */
void QGitDiff::diffCommits(QGitCommit commitFrom, QGitCommit commitTo)
{
    const git_diff_options opts = GIT_DIFF_OPTIONS_INIT;

    git_diff_tree_to_tree (&diff, _repo.data(), commitFrom.tree().data(), commitTo.tree().data(), &opts);


    git_diff_foreach (diff, fileDiffCallBack, addPatchHunksCallBack, addPatchLinesCallBack, this);

}



QGitDiff::~QGitDiff()
{
    git_diff_list_free (diff);
}

/**
 * @brief QGitDiff::diffWorkingDir gets the changes from head that are in the
 * working directory. If there is no changes then false is returned. If there
 * are changes then are obtained through calls to getDeltasForFile for the deltas
 * and getFileChangedList for the file list and true is returned.
 * @return
 */
bool QGitDiff::diffWorkingDir()
{
    const git_diff_options opts = GIT_DIFF_OPTIONS_INIT;

    QGitCommit commit = _repo.lookupCommit(_repo.head().oid());

    // get the diff
    git_diff_tree_to_workdir (&diff, _repo.data(), commit.tree().data() , &opts);

    // get the diff in a useful form
    git_diff_foreach (diff, fileDiffCallBack, addPatchHunksCallBack, addPatchLinesCallBack, this);

    if (fileList.isEmpty())
    {
        return false;
    }

    return true;
}

void QGitDiff::addPatchLines(const git_diff_delta *delta, const char *line, char usage, int lineLen)
{
    //assumes that this is called after hunk call back so this will
    // just append to the end of the Last map that was created by addHunk
    QString fileName(delta->new_file.path);
    QString currentLine(line);
    QString realLine = currentLine.left(lineLen);

    patchInfo info = deltas[QString(delta->new_file.path)];
    QString currentFilePatch = info.patch;

    currentFilePatch.append(usage);
    currentFilePatch.append(realLine);

    if (usage == '+')
    {
        //std::cout << "addition is hit " <<  info.additions << std::endl;
        info.additions = info.additions + 1;
    }

    if (usage == '-')
    {
        //std::cout << "deletions is hit " <<  info.deletions << std::endl;
        info.deletions = info.deletions + 1;
    }

    deltas[fileName].patch = currentFilePatch;
    deltas[fileName].additions = info.additions;
    deltas[fileName].deletions = info.deletions;
}


void QGitDiff::addPatchHunks(const git_diff_delta *delta, const char *header, int headerLen)
{
    QString fileName(delta->new_file.path);
    QString currentFilePatch = deltas[QString(delta->new_file.path)].patch;
    QString currentHeader(header);
    QString realHeader = currentHeader.left(headerLen);

    currentFilePatch.append(realHeader);

    deltas[fileName].patch = currentFilePatch;
}

void QGitDiff::addFileChanged(const git_diff_delta *delta)
{
    fileList.push_back(QString::fromLocal8Bit(delta->new_file.path));
    patchInfo info;
    info.patch = QString();
    info.additions = 0;
    info.deletions = 0;

    deltas.insert(QString(delta->new_file.path), info);
}

QStringList QGitDiff::getFileChangedList()
{
    return fileList;
}

QString QGitDiff::getDeltasForFile(const QString &file)
{
    return deltas[file].patch;
}

void QGitDiff::saveFullPatch(const char *line)
{
    patch += QString(line);
}

QString QGitDiff::print()
{
    git_diff_print_patch(diff, printCB, this);

    return patch;
}

QString QGitDiff::getDiffString(int additions, int deletions)
{
    QString diffStat;

    for (int i = 0; i < additions; i++)
    {
        diffStat = diffStat + "+";
    }

    for (int i = 0; i < deletions; i++)
    {
        diffStat = diffStat + "-";
    }

    return diffStat;
}

QString QGitDiff::diffStats()
{
    QString stats;

    int totalAdditions = 0;
    int totalDeletions = 0;

    // check to make sure diff has been populated
    if (diff == NULL)
    {
        return stats;
    }

    foreach (QString file, fileList)
    {
        patchInfo info = deltas[file];


        totalAdditions += info.additions;
        totalDeletions += info.deletions;

        stats.append(" " + file + "\t|    " + getDiffString(info.additions, info.deletions) + "\n");
    }
    stats.append(" " + QString::number(fileList.count()));
    stats.append(" file changed, " + QString::number(totalAdditions));
    stats.append(" insertions(+), " + QString::number(totalDeletions));
    stats.append(" deletions(-)\n");

    return stats;
}
}
