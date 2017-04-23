/*************************************************************************/
/*  arvr_server.h                                                        */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                    http://www.godotengine.org                         */
/*************************************************************************/
/* Copyright (c) 2007-2017 Juan Linietsky, Ariel Manzur.                 */
/* Copyright (c) 2014-2017 Godot Engine contributors (cf. AUTHORS.md)    */
/*                                                                       */
/* Permission is hereby granted, free of charge, to any person obtaining */
/* a copy of this software and associated documentation files (the       */
/* "Software"), to deal in the Software without restriction, including   */
/* without limitation the rights to use, copy, modify, merge, publish,   */
/* distribute, sublicense, and/or sell copies of the Software, and to    */
/* permit persons to whom the Software is furnished to do so, subject to */
/* the following conditions:                                             */
/*                                                                       */
/* The above copyright notice and this permission notice shall be        */
/* included in all copies or substantial portions of the Software.       */
/*                                                                       */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*/
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY  */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,  */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE     */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                */
/*************************************************************************/

#ifndef ARVR_SERVER_H
#define ARVR_SERVER_H

#include "object.h"
#include "rid.h"
#include "variant.h"

class ArVrInterface;
class ArVrPositionalTracker;

/**
	@author Bastiaan Olij <mux213@gmail.com

	The ARVR server is a singleton object that gives access to the various
	objects and SDKs that are available on the system.
	Because there can be multiple SDKs active this is exposed as an array
	and our ARVR server object acts as a pass through
	Also each positioning tracker is accessible from here.
*/
class ArVrServer : public Object {
	GDCLASS(ArVrServer, Object);

private:
	Vector<ArVrInterface *> interfaces;
	Vector<ArVrPositionalTracker *> trackers;

protected:
	static ArVrServer *singleton;

	static void _bind_methods();

public:
	enum TrackerType {
		TRACKER_HMD = 0x01, /* HMD in the broad sense, mobile device position is typed like this too */
		TRACKER_CONTROLLER = 0x02, /* tracks a controller */
		TRACKER_BASESTATION = 0x04, /* tracks location of a base station */
		TRACKER_UNKNOWN = 0x80, /* unknown tracker */

		TRACKER_HMD_AND_CONTROLLER = 0x03, /* common to select both of these */
		TRACKER_ANY_KNOWN = 0x7f, /* all except unknown */
		TRACKER_ANY = 0xff /* used by get_connected_trackers to return all types */
	};

	static ArVrServer *get_singleton();

	void add_interface(ArVrInterface * p_interface);
	void remove_interface(ArVrInterface * p_interface);
	int get_interface_count() const;
	ArVrInterface * get_interface(int p_index) const;
	ArVrInterface * find_interface(const String &p_name) const;

	void add_tracker(ArVrPositionalTracker * p_tracker);
	void remove_tracker(ArVrPositionalTracker * p_tracker);
	int get_tracker_count() const;
	ArVrPositionalTracker * get_tracker(int p_index) const;

	ArVrServer();
	~ArVrServer();
};

#define ARVR ArVrServer

VARIANT_ENUM_CAST(ArVrServer::TrackerType);

#endif
