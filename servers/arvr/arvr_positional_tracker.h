/*************************************************************************/
/*  arvr_positional_tracker.h                                            */
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
#ifndef ARVR_POSITIONAL_TRACKER_H
#define ARVR_POSITIONAL_TRACKER_H

#include "servers/arvr_server.h"

class ArVrPositionalTracker : public Object {
	GDCLASS(ArVrPositionalTracker, Object);

private:
	Vector3 scale_magneto(const Vector3 & p_magnetometer);
	Basis ArVrPositionalTracker::combine_acc_mag(const Vector3 & p_grav,const Vector3 & p_magneto);

	int mag_count;
	bool has_gyro;
	bool sensor_first;
	Vector3 last_accerometer_data;
	Vector3 last_magnetometer_data;
	Vector3 mag_current_min;
	Vector3 mag_current_max;
	Vector3 mag_next_min;
	Vector3 mag_next_max;

	///@TODO a few support functions for trackers, most are math related and should likely be moved elsewhere
	float floor_decimals(float p_value, float p_decimals){
		float power_of_10 = pow(10.0, p_decimals);
		return floor(p_value * power_of_10) / power_of_10;
	};

	Vector3 floor_decimals(const Vector3 &p_vector, float p_decimals) {
		return Vector3(floor_decimals(p_vector.x, p_decimals), floor_decimals(p_vector.y, p_decimals), floor_decimals(p_vector.z, p_decimals));
	};

	Vector3 low_pass(const Vector3 &p_vector, const Vector3 &p_last_vector, float p_factor) {
		return p_vector + (p_factor * (p_last_vector - p_vector));
	};

	Vector3 scrub(const Vector3 &p_vector, const Vector3 &p_last_vector, float p_decimals, float p_factor) {
		return low_pass(floor_decimals(p_vector, p_decimals), p_last_vector, p_factor);
	};

	ArVrServer::TrackerType type;  // type of tracker
	StringName name;  // (unique) name of the tracker
	int joy_id; // if we also have a related joystick entity, the id of the joystick
	bool tracks_orientation; // do we track orientation?
	Basis orientation; // our orientation
	bool tracks_position; // do we track position?
	Vector3 position; // our position

protected:
	static void _bind_methods();

public:
	void set_type(ArVrServer::TrackerType p_type);
	ArVrServer::TrackerType get_type() const;
	void set_name(const String p_name);
	StringName get_name() const;
	void set_joy_id(int p_joy_id);
	int get_joy_id() const;
	bool get_tracks_orientation() const;
	void set_orientation(const Basis & p_orientation);
	Basis get_orientation() const;
	bool get_tracks_position() const;
	void set_position(const Vector3 & p_position);
	Vector3 get_position() const;

	void set_position_from_sensors(float p_delta_time);

	ArVrPositionalTracker();
	~ArVrPositionalTracker();
};

#endif
