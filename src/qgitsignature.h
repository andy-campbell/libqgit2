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

#ifndef LIBQGIT2_SIGNATURE_H
#define LIBQGIT2_SIGNATURE_H

#include "libqgit2_export.h"

#include <QtCore/QString>
#include <QtCore/QDateTime>

#include <git2/signature.h>

namespace LibQGit2
{
    class LIBQGIT2_SIGNATURE_EXPORT QGitSignature
    {
        public:
            /**
             * Construct a new action signature. The signature must be freed
             * manually
             *
             * @name name of the person
             * @email email of the person
             * @time time when the action happened
             * @offset timezone offset in minutes for the time
             */
            QGitSignature(const QString& name, const QString& email, QDateTime dateTime);
            explicit QGitSignature(git_signature *signature = 0);

            /**
             * Construct a copy of an existing signature.
             *
             * All internal strings are also duplicated.
             * @other signature to duplicated
             */
            QGitSignature( const QGitSignature& other );

            /**
             * Delete an existing signature
             */
            ~QGitSignature();

            QString name() const;
            QString email() const;
            QDateTime when() const;

            git_signature* data() const;
            const git_signature* constData() const;

        private:
            git_signature *d;
    };

    class LIBQGIT2_SIGNATURE_EXPORT QGitSignatureRef
    {
        public:
            explicit QGitSignatureRef(const git_signature *signature = 0);

            QGitSignatureRef(const QGitSignatureRef& other);
            QGitSignatureRef(const QGitSignature& other);

            ~QGitSignatureRef();

            QString name() const;
            QString email() const;
            QDateTime when() const;

            const git_signature *data() const;

        private:
            const git_signature *d;
    };
}

#endif // LIBQGIT2_SIGNATURE_H
