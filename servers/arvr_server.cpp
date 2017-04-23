/*************************************************************************/
/*  arvr_server.cpp                                                      */
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
#include "arvr_server.h"
#include "arvr/arvr_interface.h"
#include "arvr/arvr_positional_tracker.h"
#include "global_config.h"

ArVrServer * ArVrServer::singleton = NULL;

ArVrServer * ArVrServer::get_singleton() {
	return singleton;
};

void ArVrServer::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_interface_count"), &ArVrServer::get_interface_count);
	ClassDB::bind_method(D_METHOD("get_interface:ArVrInterface", "idx"), &ArVrServer::get_interface);
	ClassDB::bind_method(D_METHOD("find_interface:ArVrInterface", "name"), &ArVrServer::find_interface);
	ClassDB::bind_method(D_METHOD("get_tracker_count"), &ArVrServer::get_tracker_count);
	ClassDB::bind_method(D_METHOD("get_tracker:ArVrPositionalTracker", "idx"), &ArVrServer::get_tracker);

	BIND_CONSTANT(TRACKER_HMD);
	BIND_CONSTANT(TRACKER_CONTROLLER);
	BIND_CONSTANT(TRACKER_BASESTATION);
	BIND_CONSTANT(TRACKER_UNKNOWN);
	BIND_CONSTANT(TRACKER_HMD_AND_CONTROLLER);
	BIND_CONSTANT(TRACKER_ANY_KNOWN);
	BIND_CONSTANT(TRACKER_ANY);

	ADD_SIGNAL(MethodInfo("interface_added", PropertyInfo(Variant::STRING, "name")));
	ADD_SIGNAL(MethodInfo("interface_removed", PropertyInfo(Variant::STRING, "name")));

	ADD_SIGNAL(MethodInfo("tracker_added", PropertyInfo(Variant::STRING, "name"), PropertyInfo(Variant::INT, "type")));
	ADD_SIGNAL(MethodInfo("tracker_removed", PropertyInfo(Variant::STRING, "name")));
};

void ArVrServer::add_interface(ArVrInterface * p_interface) {
	ERR_FAIL_NULL(p_interface);

	interfaces.push_back(p_interface);
	emit_signal("interface_added", p_interface->get_name());
};

void ArVrServer::remove_interface(ArVrInterface * p_interface) {
	ERR_FAIL_NULL(p_interface);

	int idx = -1;
	for (int i = 0; i < interfaces.size(); i++) {

		if (interfaces[i] == p_interface) {

			idx = i;
			break;
		};
	};

	ERR_FAIL_COND(idx == -1);

	emit_signal("interface_removed", p_interface->get_name());
	interfaces.remove(idx);
};

int ArVrServer::get_interface_count() const {
	return interfaces.size();
};

ArVrInterface * ArVrServer::get_interface(int p_index) const {
	ERR_FAIL_INDEX_V(p_index, interfaces.size(), NULL);

	return interfaces[p_index];	
};

ArVrInterface * ArVrServer::find_interface(const String &p_name) const {
	int idx = -1;
	for (int i = 0; i < interfaces.size(); i++) {

		if (interfaces[i]->get_name() == p_name) {

			idx = i;
			break;
		};
	};

	ERR_FAIL_COND_V(idx == -1, NULL);

	return interfaces[idx];
};

void ArVrServer::add_tracker(ArVrPositionalTracker * p_tracker) {
	ERR_FAIL_NULL(p_tracker);

	trackers.push_back(p_tracker);
	emit_signal("tracker_added", p_tracker->get_name(), p_tracker->get_type());
};

void ArVrServer::remove_tracker(ArVrPositionalTracker * p_tracker) {
	ERR_FAIL_NULL(p_tracker);

	int idx = -1;
	for (int i = 0; i < trackers.size(); i++) {

		if (trackers[i] == p_tracker) {

			idx = i;
			break;
		};
	};

	ERR_FAIL_COND(idx == -1);

	emit_signal("tracker_removed", p_tracker->get_name());
	trackers.remove(idx);
};

int ArVrServer::get_tracker_count() const {
	return trackers.size();
};

ArVrPositionalTracker * ArVrServer::get_tracker(int p_index) const {
	ERR_FAIL_INDEX_V(p_index, trackers.size(), NULL);

	return trackers[p_index];
};

ArVrServer::ArVrServer() {
	singleton = this;
};

ArVrServer::~ArVrServer() {
	singleton = NULL;
};
