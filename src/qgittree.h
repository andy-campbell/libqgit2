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

#ifndef LIBQGIT2_TREE_H
#define LIBQGIT2_TREE_H

#include "qgitobject.h"

#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QPair>

struct git_tree;
struct git_tree_entry;

namespace LibQGit2
{
    class QGitRepository;
    class QGitOId;
    class QGitTreeEntry;

    /**
     * @brief Wrapper class for git_tree.
     * Represents a Git tree object.
     *
     * @ingroup LibQGit2
     * @{
     */
    class LIBQGIT2_TREE_EXPORT QGitTree : public QGitObject
    {
        public:

            /**
             * Creates a QGitTree that points to tree. The pointer object becomes managed by
             * this QGitTree, and must not be passed to another QGitTree or closed outside this
             * object.
             */
            explicit QGitTree(git_tree *tree = 0);

            /**
             * Copy constructor; creates a copy of the object, sharing the same underlaying data
             * structure.
             */
            QGitTree(const QGitTree& other);

            /**
             * Destroys the object.
             */
            ~QGitTree();

            /**
             * * Get the id of a tree.
             * * @return object identity for the tree.
             * */
            QGitOId oid();

            /**
             * Get the number of entries listed in a tree
             * @return the number of entries in the tree
             */
            size_t entryCount();

            /**
             * Lookup a tree entry by its filename
             * @param filename the filename of the desired entry
             * @return the tree entry; NULL if not found
             */
            QGitTreeEntry entryByName(const QString& fileName) const;

            /**
             * Lookup a tree entry by its position in the tree
             * @param idx the position in the entry list
             * @return the tree entry; NULL if not found
             */
            QGitTreeEntry entryByIndex(int idx) const;

            /**
             * Creates a list of all of the entries in the tree
             * this include subfolders.
             * @param getAllEntries
             * @return  a list of tree entries; the list is empty if nothing is found
             */
            const QStringList getAllEntries();

            git_tree* data() const;
            const git_tree* constData() const;

            /**
             * This is not an api call this is part of the callback for the
             * git_tree_walk
             * @param root The location from the root directory
             * @param entry An entry in the tree
             */
            void addEntry(const char *root, const git_tree_entry *entry);
    private:
            QStringList entries;
    };

    /**@}*/
}

#endif // LIBQGIT2_TREE_H
