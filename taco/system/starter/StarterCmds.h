/*****************************************************************************
 * Copyright (C) 2003-2013 Jens Krueger
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
 * File:        $RCSfile: StarterCmds.h,v $
 *
 * Project:     Device Servers with sun-rpc
 *
 * Description: Source code for implementing a starter server
 *
 * Author(s):   Jens Krüger
 * 		$Author: jkrueger1 $
 *
 * Original:    January 2003
 *
 * Version:	$Revision: 1.1 $
 *
 * Date:	$Date: 2013-05-17 08:05:49 $
 *
 *****************************************************************************/
#ifndef __STARTER_CMDS_H__
#define __STARTER_CMDS_H__

#define DevStartAll		3000L
#define DevStopAll		3001L
#define DevStart		3002L
#define DevGetRunningServers	3003L
#define DevGetStoppedServers	3004L
#define DevReadLog		3005L
#define UpdateServerInfo	3006L

#endif
