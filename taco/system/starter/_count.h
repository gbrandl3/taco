/*****************************************************************************
 * Toolkit for building distributed control systems or any other distributed system.
 *
 * Copyright (c) 1990-2005 by European Synchrotron Radiation Facility,
 *                            Grenoble, France
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * File:        $File$
 *
 * Project:     Device Servers with sun-rpc
 *
 * Description: to fix broken std::count() on Solaris CC
 *
 * Author(s):   Andy Goetz
 *              $Author: jkrueger1 $
 *
 * Original:    January 2003
 *
 * Version:     $Revision: 1.2 $
 *
 * Date:        $Date: 2006-09-18 21:45:36 $
 *
 *****************************************************************************/
/*
 */
#ifdef _solaris
namespace _sol {
        template <class Iterator, class T>
        //typename std::iterator_traits<Iterator>::difference_type
        int
        count (Iterator first, Iterator last, T const & value)
        {
                //std::iterator_traits<Iterator>::difference_type n = 0;
                int n = 0;
                while (first != last)
                        if (*first++ == value) ++n;
                return n;
        }
}
#endif

