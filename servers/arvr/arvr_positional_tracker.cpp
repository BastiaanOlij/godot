/*************************************************************************/
/*  arvr_postional_tracker.cpp                                           */
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
#include "arvr_positional_tracker.h"
#include "core/os/input.h"

void ArVrPositionalTracker::_bind_methods() {
	// this class is read only from GDScript, so we only have access to getters..
	ClassDB::bind_method(D_METHOD("get_type"), &ArVrPositionalTracker::get_type);
	ClassDB::bind_method(D_METHOD("get_name"), &ArVrPositionalTracker::get_name);
	ClassDB::bind_method(D_METHOD("get_joy_id"), &ArVrPositionalTracker::get_joy_id);
	ClassDB::bind_method(D_METHOD("get_tracks_orientation"), &ArVrPositionalTracker::get_tracks_orientation);
	ClassDB::bind_method(D_METHOD("get_orientation"), &ArVrPositionalTracker::get_orientation);
	ClassDB::bind_method(D_METHOD("get_tracks_position"), &ArVrPositionalTracker::get_tracks_position);
	ClassDB::bind_method(D_METHOD("get_position"), &ArVrPositionalTracker::get_position);
};

void ArVrPositionalTracker::set_type(ArVrServer::TrackerType p_type) {
	type = p_type;
};

ArVrServer::TrackerType ArVrPositionalTracker::get_type() const {
	return type;
};

void ArVrPositionalTracker::set_name(const String p_name) {
	name = p_name;
};

StringName ArVrPositionalTracker::get_name() const {
	return name;
};

void ArVrPositionalTracker::set_joy_id(int p_joy_id) {
	joy_id = p_joy_id;
};

int ArVrPositionalTracker::get_joy_id() const {
	return joy_id;
};

bool ArVrPositionalTracker::get_tracks_orientation() const {
	return tracks_orientation;
};

void ArVrPositionalTracker::set_orientation(const Basis & p_orientation) {
	tracks_orientation = true; // obviously we have this
	orientation = p_orientation;
};

Basis ArVrPositionalTracker::get_orientation() const {
	return orientation;
};

bool ArVrPositionalTracker::get_tracks_position() const {
	return tracks_position;
};

void ArVrPositionalTracker::set_position(const Vector3 & p_position) {
	tracks_position = true; // obviously we have this
	position = p_position;
};

Vector3 ArVrPositionalTracker::get_position() const {
	return position;
};

Vector3 ArVrPositionalTracker::scale_magneto(const Vector3 & p_magnetometer) {
	// Our magnetometer doesn't give us nice clean data.
	// Well it may on Mac OS X because we're getting a calibrated value in the current implementation but Android we're getting raw data. 
	// This is a fairly simple adjustment we can do to correct for the magnetometer data being elliptical 

	Vector3 mag_raw = p_magnetometer;
	Vector3 mag_scaled = p_magnetometer;

	// update our variables every x frames
	if (mag_count > 20) {
		mag_current_min = mag_next_min;
		mag_current_max = mag_next_max;
		mag_count = 0;
	} else {
		mag_count++;
	};

	// adjust our min and max
	if (mag_raw.x > mag_next_max.x) mag_next_max.x = mag_raw.x;
	if (mag_raw.y > mag_next_max.y) mag_next_max.y = mag_raw.y;
	if (mag_raw.z > mag_next_max.z) mag_next_max.z = mag_raw.z;
	
	if (mag_raw.x < mag_next_min.x) mag_next_min.x = mag_raw.x;
	if (mag_raw.y < mag_next_min.y) mag_next_min.y = mag_raw.y;
	if (mag_raw.z < mag_next_min.z) mag_next_min.z = mag_raw.z;
	
	// scale our x, y and z	
	if (!(mag_current_max.x - mag_current_min.x)) {
		mag_raw.x -= (mag_current_min.x + mag_current_max.x) / 2.0;
		mag_scaled.x = (mag_raw.x - mag_current_min.x) / ((mag_current_max.x - mag_current_min.x) * 2.0 - 1.0);
	};
	
	if (!(mag_current_max.y - mag_current_min.y)) {
		mag_raw.y -= (mag_current_min.y + mag_current_max.y) / 2.0;
		mag_scaled.y = (mag_raw.y - mag_current_min.y) / ((mag_current_max.y - mag_current_min.y) * 2.0 - 1.0);
	};

	if (!(mag_current_max.z - mag_current_min.z)) {
		mag_raw.z -= (mag_current_min.z + mag_current_max.z) / 2.0;
		mag_scaled.z = (mag_raw.z - mag_current_min.z) / ((mag_current_max.z - mag_current_min.z) * 2.0 - 1.0);
	};

	return mag_scaled;
};

Basis ArVrPositionalTracker::combine_acc_mag(const Vector3& p_grav,const Vector3& p_magneto) {
	// yup, stock standard cross product solution...
	Vector3 up = -p_grav.normalized();

	Vector3 magneto_east = up.cross(p_magneto.normalized()); // or is this west?, but should be horizon aligned now
	magneto_east.normalize();

	Vector3 magneto = up.cross(magneto_east); // and now we have a horizon aligned north
	magneto.normalize();

	// We use our gravity and magnetometer vectors to construct our matrix
	Basis acc_mag_m3;
	acc_mag_m3.elements[0] = -magneto_east;
	acc_mag_m3.elements[1] = up;
	acc_mag_m3.elements[2] = magneto;
	
	return acc_mag_m3;
};

void ArVrPositionalTracker::set_position_from_sensors(float p_delta_time) {
	// this is a helper function that attempts to adjust our transform using our 9dof sensors
	// 9dof is a misleading marketing term coming from 3 accelerometer axis + 3 gyro axis + 3 magnetometer axis = 9 axis
	// but in reality this only offers 3 dof (yaw, pitch, roll) orientation

	///@TODO instead of passing p_delta_time calculate it here, maybe using OS::get_ticks_msec but it doesn't seem implemented on all platforms (yet)
	tracks_orientation = true;

	// few things we need
	Input * input = Input::get_singleton();
	Vector3 down(0.0, -1.0, 0.0); // Down is Y negative
	Vector3 north(0.0, 0.0, 1.0); // North is Z positive

	// make copies of our inputs
	Vector3 acc = input->get_accelerometer();
	Vector3 gyro = input->get_gyroscope();
	Vector3 grav = input->get_gravity();
	Vector3 magneto = scale_magneto(input->get_magnetometer()); // this may be overkill on iOS because we're already getting a calibrated magnetometer reading

	if (sensor_first) {
		sensor_first = false;
	} else {
		acc = scrub(acc, last_accerometer_data, 2, 0.2);
		magneto = scrub(magneto, last_magnetometer_data, 3, 0.3);
	};

	last_accerometer_data = acc;
	last_magnetometer_data = magneto;

	if (grav.length() < 0.1) {
		// not ideal but use our accelerometer, this will contain shakey shakey user behaviour
		// maybe look into some math but I'm guessing that if this isn't available, its because we lack the gyro sensor to actually work out
		// what a stable gravity vector is
		grav = acc;
		if (grav.length() > 0.1) {
			has_gyro = true;
		};
	} else {
		has_gyro = true;
	};

	bool has_magneto = magneto.length() > 0.1;
	bool has_grav = grav.length() > 0.1;

#ifdef ANDROID_ENABLED
	///@TODO needs testing, i don't have a gyro, potentially can be removed depending on what comes out of issue #8101
	// On Android x and z axis seem inverted
	gyro.x = -gyro.x; gyro.z = -gyro.z;
	grav.x = -grav.x; grav.z = -grav.z;
	magneto.x = -magneto.x; magneto.z =-magneto.z;
#endif

	if (has_gyro) {
		// start with applying our gyro (do NOT smooth our gyro!)
		Basis rotate;
		rotate.rotate(orientation.get_axis(0), -gyro.x * p_delta_time);
		rotate.rotate(orientation.get_axis(1), -gyro.y * p_delta_time);
		rotate.rotate(orientation.get_axis(2), -gyro.z * p_delta_time);
		orientation = rotate * orientation;
	};

	///@TODO improve this, the magnetometer is very fidgity sometimes flipping the axis for no apparent reason (probably a bug on my part)
	// if you have a gyro + accelerometer that combo tends to be better then combining all three but without a gyro you need the magnetometer..
	if (has_magneto && has_grav && !has_gyro) {
		// convert to quaternions, easier to smooth those out
		Quat transform_quat(orientation);
		Quat acc_mag_quat(combine_acc_mag(grav, magneto));
		transform_quat = transform_quat.slerp(acc_mag_quat, 0.1);
		orientation = Basis(transform_quat);
	} else if (has_grav) {
		// use gravity vector to make sure down is down...
		// transform gravity into our world space
		// note that our positioning matrix will be inversed to create our view matrix, so the inverse of that is our positioning matrix, duh
		// hence we can do:
		grav.normalize();
		Vector3 grav_adj = orientation.xform(grav);
		float dot = grav_adj.dot(down);
		if ((dot > -1.0) && (dot < 1.0)) {
			// axis around which we have this rotation
			Vector3 axis = grav_adj.cross(down);
			axis.normalize();

			Basis drift_compensation(axis, -acos(dot) * 0.2);
			orientation = drift_compensation * orientation;
		};
	};
};

ArVrPositionalTracker::ArVrPositionalTracker() {
	type = ArVrServer::TRACKER_UNKNOWN;
	name = "Unknown";
	joy_id = -1;
	tracks_orientation = false;
	tracks_position = false;

	mag_count = 0;
	has_gyro = false;
	sensor_first = true;
	mag_next_min = Vector3(10000, 10000, 10000);
	mag_next_max = Vector3 (-10000, -10000, -10000);
	mag_current_min = Vector3(0, 0, 0);
	mag_current_max = Vector3(0, 0, 0);
};

ArVrPositionalTracker::~ArVrPositionalTracker() {

};
