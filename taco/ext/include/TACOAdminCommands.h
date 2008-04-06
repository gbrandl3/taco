/*
 * Extensions for the convenient access to TACO
 * Copyright (C) 2006 Jens Krueger <jens.krueger@frm2.tum.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#ifndef TACO_ADMIN_COMMANDS_H
#define TACO_ADMIN_COMMANDS_H

#ifdef __cplusplus
namespace TACO {
	//! Contains all %TACO commands
	namespace Command {
		namespace Admin {
			const DevCommand DEVICE_RESTART = 8000;
			const DevCommand SERVER_RESTART = 8001; 
			const DevCommand CLASS_QUERY	= 8002; 
			const DevCommand DEVICE_QUERY	= 8003; 
			const DevCommand DEVICE_KILL	= 8004;
		}
	}
}
#endif /* __cplusplus */

#endif /* TACO_ADMIN_COMMANDS_H */
