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

#ifndef LIBQGIT2_EXCEPTION_H
#define LIBQGIT2_EXCEPTION_H

#include <QByteArray>

namespace LibQGit2
{
    /**
     * @brief Exception class to throw Git exceptions.
     *
     * @ingroup LibQGit2
     * @{
     */
    class QGitException : public std::exception
    {
        public:
            QGitException(int error);

            ~QGitException() throw();

            const char *what() const throw();

            QByteArray message() const throw();

        private:
            QByteArray m;
    };

    int qGitThrow(int ret);

    /**@}*/
}

#endif // LIBQGIT2_EXCEPTION_H
