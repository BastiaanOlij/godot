/**************************************************************************/
/*  openxr_spatial_marker_tracking.cpp                                    */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             GODOT ENGINE                               */
/*                        https://godotengine.org                         */
/**************************************************************************/
/* Copyright (c) 2014-present Godot Engine contributors (see AUTHORS.md). */
/* Copyright (c) 2007-2014 Juan Linietsky, Ariel Manzur.                  */
/*                                                                        */
/* Permission is hereby granted, free of charge, to any person obtaining  */
/* a copy of this software and associated documentation files (the        */
/* "Software"), to deal in the Software without restriction, including    */
/* without limitation the rights to use, copy, modify, merge, publish,    */
/* distribute, sublicense, and/or sell copies of the Software, and to     */
/* permit persons to whom the Software is furnished to do so, subject to  */
/* the following conditions:                                              */
/*                                                                        */
/* The above copyright notice and this permission notice shall be         */
/* included in all copies or substantial portions of the Software.        */
/*                                                                        */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 */
/**************************************************************************/

#include "openxr_spatial_marker_tracking.h"
#include "openxr_spatial_entity_extension.h"

#include "../../openxr_api.h"
#include "core/config/project_settings.h"
#include "servers/xr_server.h"

////////////////////////////////////////////////////////////////////////////
// OpenXRSpatialCapabilityConfigurationQrCode

void OpenXRSpatialCapabilityConfigurationQrCode::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_enabled_components"), &OpenXRSpatialCapabilityConfigurationQrCode::_get_enabled_components);
}

bool OpenXRSpatialCapabilityConfigurationQrCode::has_valid_configuration() const {
	OpenXRSpatialMarkerTrackingCapability *capability = OpenXRSpatialMarkerTrackingCapability::get_singleton();
	ERR_FAIL_NULL_V(capability, false);

	return capability->is_qrcode_supported();
}

XrSpatialCapabilityConfigurationBaseHeaderEXT *OpenXRSpatialCapabilityConfigurationQrCode::get_configuration() {
	OpenXRSpatialMarkerTrackingCapability *capability = OpenXRSpatialMarkerTrackingCapability::get_singleton();
	ERR_FAIL_NULL_V(capability, nullptr);

	if (capability->is_qrcode_supported()) {
		OpenXRSpatialEntityExtension *se_extension = OpenXRSpatialEntityExtension::get_singleton();
		ERR_FAIL_NULL_V(se_extension, nullptr);

		// Guaranteed components:
		enabled_components.push_back(XR_SPATIAL_COMPONENT_TYPE_MARKER_EXT);
		enabled_components.push_back(XR_SPATIAL_COMPONENT_TYPE_BOUNDED_2D_EXT);

		// Setup our enabled components
		marker_config.enabledComponentCount = enabled_components.size();
		marker_config.enabledComponents = enabled_components.ptr();

		// and return this.
		return (XrSpatialCapabilityConfigurationBaseHeaderEXT *)&marker_config;
	}

	return nullptr;
}

PackedInt64Array OpenXRSpatialCapabilityConfigurationQrCode::_get_enabled_components() const {
	PackedInt64Array components;

	for (const XrSpatialComponentTypeEXT &component_type : enabled_components) {
		components.push_back((int64_t)component_type);
	}

	return components;
}

////////////////////////////////////////////////////////////////////////////
// OpenXRSpatialCapabilityConfigurationMicroQrCode

void OpenXRSpatialCapabilityConfigurationMicroQrCode::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_enabled_components"), &OpenXRSpatialCapabilityConfigurationMicroQrCode::_get_enabled_components);
}

bool OpenXRSpatialCapabilityConfigurationMicroQrCode::has_valid_configuration() const {
	OpenXRSpatialMarkerTrackingCapability *capability = OpenXRSpatialMarkerTrackingCapability::get_singleton();
	ERR_FAIL_NULL_V(capability, false);

	return capability->is_micro_qrcode_supported();
}

XrSpatialCapabilityConfigurationBaseHeaderEXT *OpenXRSpatialCapabilityConfigurationMicroQrCode::get_configuration() {
	OpenXRSpatialMarkerTrackingCapability *capability = OpenXRSpatialMarkerTrackingCapability::get_singleton();
	ERR_FAIL_NULL_V(capability, nullptr);

	if (capability->is_micro_qrcode_supported()) {
		OpenXRSpatialEntityExtension *se_extension = OpenXRSpatialEntityExtension::get_singleton();
		ERR_FAIL_NULL_V(se_extension, nullptr);

		// Guaranteed components:
		enabled_components.push_back(XR_SPATIAL_COMPONENT_TYPE_MARKER_EXT);
		enabled_components.push_back(XR_SPATIAL_COMPONENT_TYPE_BOUNDED_2D_EXT);

		// Setup our enabled components
		marker_config.enabledComponentCount = enabled_components.size();
		marker_config.enabledComponents = enabled_components.ptr();

		// and return this.
		return (XrSpatialCapabilityConfigurationBaseHeaderEXT *)&marker_config;
	}

	return nullptr;
}

PackedInt64Array OpenXRSpatialCapabilityConfigurationMicroQrCode::_get_enabled_components() const {
	PackedInt64Array components;

	for (const XrSpatialComponentTypeEXT &component_type : enabled_components) {
		components.push_back((int64_t)component_type);
	}

	return components;
}

////////////////////////////////////////////////////////////////////////////
// OpenXRSpatialCapabilityConfigurationAruco

void OpenXRSpatialCapabilityConfigurationAruco::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_enabled_components"), &OpenXRSpatialCapabilityConfigurationAruco::_get_enabled_components);

	ClassDB::bind_method(D_METHOD("set_aruco_dict", "aruco_dict"), &OpenXRSpatialCapabilityConfigurationAruco::set_aruco_dict);
	ClassDB::bind_method(D_METHOD("get_aruco_dict"), &OpenXRSpatialCapabilityConfigurationAruco::get_aruco_dict);
	ADD_PROPERTY(PropertyInfo(Variant::INT, "aruco_dict"), "set_aruco_dict", "get_aruco_dict");

	XR_BIND_ENUM_CONSTANTS(XrSpatialMarkerArucoDictEXT);
}

bool OpenXRSpatialCapabilityConfigurationAruco::has_valid_configuration() const {
	OpenXRSpatialMarkerTrackingCapability *capability = OpenXRSpatialMarkerTrackingCapability::get_singleton();
	ERR_FAIL_NULL_V(capability, false);

	return capability->is_aruco_supported();
}

XrSpatialCapabilityConfigurationBaseHeaderEXT *OpenXRSpatialCapabilityConfigurationAruco::get_configuration() {
	OpenXRSpatialMarkerTrackingCapability *capability = OpenXRSpatialMarkerTrackingCapability::get_singleton();
	ERR_FAIL_NULL_V(capability, nullptr);

	if (capability->is_aruco_supported()) {
		OpenXRSpatialEntityExtension *se_extension = OpenXRSpatialEntityExtension::get_singleton();
		ERR_FAIL_NULL_V(se_extension, nullptr);

		// Guaranteed components:
		enabled_components.push_back(XR_SPATIAL_COMPONENT_TYPE_MARKER_EXT);
		enabled_components.push_back(XR_SPATIAL_COMPONENT_TYPE_BOUNDED_2D_EXT);

		// Setup our enabled components
		marker_config.enabledComponentCount = enabled_components.size();
		marker_config.enabledComponents = enabled_components.ptr();

		// and return this.
		return (XrSpatialCapabilityConfigurationBaseHeaderEXT *)&marker_config;
	}

	return nullptr;
}

void OpenXRSpatialCapabilityConfigurationAruco::set_aruco_dict(XrSpatialMarkerArucoDictEXT p_dict) {
	marker_config.arUcoDict = p_dict;
}

XrSpatialMarkerArucoDictEXT OpenXRSpatialCapabilityConfigurationAruco::get_aruco_dict() const {
	return marker_config.arUcoDict;
}

PackedInt64Array OpenXRSpatialCapabilityConfigurationAruco::_get_enabled_components() const {
	PackedInt64Array components;

	for (const XrSpatialComponentTypeEXT &component_type : enabled_components) {
		components.push_back((int64_t)component_type);
	}

	return components;
}

////////////////////////////////////////////////////////////////////////////
// OpenXRSpatialCapabilityConfigurationAprilTag

void OpenXRSpatialCapabilityConfigurationAprilTag::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_enabled_components"), &OpenXRSpatialCapabilityConfigurationAprilTag::_get_enabled_components);

	ClassDB::bind_method(D_METHOD("set_april_dict", "april_dict"), &OpenXRSpatialCapabilityConfigurationAprilTag::set_april_dict);
	ClassDB::bind_method(D_METHOD("get_april_dict"), &OpenXRSpatialCapabilityConfigurationAprilTag::get_april_dict);
	ADD_PROPERTY(PropertyInfo(Variant::INT, "april_dict"), "set_april_dict", "get_april_dict");

	XR_BIND_ENUM_CONSTANTS(XrSpatialMarkerAprilTagDictEXT);
}

bool OpenXRSpatialCapabilityConfigurationAprilTag::has_valid_configuration() const {
	OpenXRSpatialMarkerTrackingCapability *capability = OpenXRSpatialMarkerTrackingCapability::get_singleton();
	ERR_FAIL_NULL_V(capability, false);

	return capability->is_april_tag_supported();
}

XrSpatialCapabilityConfigurationBaseHeaderEXT *OpenXRSpatialCapabilityConfigurationAprilTag::get_configuration() {
	OpenXRSpatialMarkerTrackingCapability *capability = OpenXRSpatialMarkerTrackingCapability::get_singleton();
	ERR_FAIL_NULL_V(capability, nullptr);

	if (capability->is_april_tag_supported()) {
		OpenXRSpatialEntityExtension *se_extension = OpenXRSpatialEntityExtension::get_singleton();
		ERR_FAIL_NULL_V(se_extension, nullptr);

		// Guaranteed components:
		enabled_components.push_back(XR_SPATIAL_COMPONENT_TYPE_MARKER_EXT);
		enabled_components.push_back(XR_SPATIAL_COMPONENT_TYPE_BOUNDED_2D_EXT);

		// Setup our enabled components
		marker_config.enabledComponentCount = enabled_components.size();
		marker_config.enabledComponents = enabled_components.ptr();

		// and return this.
		return (XrSpatialCapabilityConfigurationBaseHeaderEXT *)&marker_config;
	}

	return nullptr;
}

void OpenXRSpatialCapabilityConfigurationAprilTag::set_april_dict(XrSpatialMarkerAprilTagDictEXT p_dict) {
	marker_config.aprilDict = p_dict;
}

XrSpatialMarkerAprilTagDictEXT OpenXRSpatialCapabilityConfigurationAprilTag::get_april_dict() const {
	return marker_config.aprilDict;
}

PackedInt64Array OpenXRSpatialCapabilityConfigurationAprilTag::_get_enabled_components() const {
	PackedInt64Array components;

	for (const XrSpatialComponentTypeEXT &component_type : enabled_components) {
		components.push_back((int64_t)component_type);
	}

	return components;
}

////////////////////////////////////////////////////////////////////////////
// OpenXRSpatialComponentMarkerList

void OpenXRSpatialComponentMarkerList::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_marker_type", "index"), &OpenXRSpatialComponentMarkerList::get_marker_type);
	ClassDB::bind_method(D_METHOD("get_marker_id", "index"), &OpenXRSpatialComponentMarkerList::get_marker_id);
	ClassDB::bind_method(D_METHOD("get_marker_data", "snapshot", "index"), &OpenXRSpatialComponentMarkerList::get_marker_data);

	BIND_ENUM_CONSTANT(MARKER_TYPE_UNKNOWN);
	BIND_ENUM_CONSTANT(MARKER_TYPE_QRCODE);
	BIND_ENUM_CONSTANT(MARKER_TYPE_MICRO_QRCODE);
	BIND_ENUM_CONSTANT(MARKER_TYPE_ARUCO);
	BIND_ENUM_CONSTANT(MARKER_TYPE_APRIL_TAG);
	BIND_ENUM_CONSTANT(MARKER_TYPE_MAX);
}

void OpenXRSpatialComponentMarkerList::set_capacity(uint32_t p_capacity) {
	marker_data.resize(p_capacity);

	marker_list.markerCount = uint32_t(marker_data.size());
	marker_list.markers = marker_data.ptrw();
}

XrSpatialComponentTypeEXT OpenXRSpatialComponentMarkerList::get_component_type() const {
	return XR_SPATIAL_COMPONENT_TYPE_PLANE_ALIGNMENT_EXT;
}

void *OpenXRSpatialComponentMarkerList::get_structure_data(void *p_next) {
	marker_list.next = p_next;
	return &marker_list;
}

OpenXRSpatialComponentMarkerList::MarkerType OpenXRSpatialComponentMarkerList::get_marker_type(int64_t p_index) const {
	ERR_FAIL_INDEX_V(p_index, marker_data.size(), MARKER_TYPE_UNKNOWN);

	// We can't simply cast these.
	// This may give us problems in the future if we get new types through vendor extensions.
	switch (marker_data[p_index].capability) {
		case XR_SPATIAL_CAPABILITY_MARKER_TRACKING_QR_CODE_EXT: {
			return MARKER_TYPE_QRCODE;
		} break;
		case XR_SPATIAL_CAPABILITY_MARKER_TRACKING_MICRO_QR_CODE_EXT: {
			return MARKER_TYPE_MICRO_QRCODE;
		} break;
		case XR_SPATIAL_CAPABILITY_MARKER_TRACKING_ARUCO_MARKER_EXT: {
			return MARKER_TYPE_ARUCO;
		} break;
		case XR_SPATIAL_CAPABILITY_MARKER_TRACKING_APRIL_TAG_EXT: {
			return MARKER_TYPE_APRIL_TAG;
		} break;
		default: {
			return MARKER_TYPE_UNKNOWN;
		} break;
	}
}

uint32_t OpenXRSpatialComponentMarkerList::get_marker_id(int64_t p_index) const {
	ERR_FAIL_INDEX_V(p_index, marker_data.size(), 0);

	return marker_data[p_index].markerId;
}

Variant OpenXRSpatialComponentMarkerList::get_marker_data(RID p_snapshot, int64_t p_index) const {
	ERR_FAIL_INDEX_V(p_index, marker_data.size(), Variant());

	OpenXRSpatialEntityExtension *se_extension = OpenXRSpatialEntityExtension::get_singleton();
	ERR_FAIL_NULL_V(se_extension, Variant());

	const XrSpatialBufferEXT &data = marker_data[p_index].data;
	switch (data.bufferType) {
		case XR_SPATIAL_BUFFER_TYPE_STRING_EXT: {
			return se_extension->get_string(p_snapshot, data.bufferId);
		} break;
		case XR_SPATIAL_BUFFER_TYPE_UINT8_EXT: {
			return se_extension->get_uint8_buffer(p_snapshot, data.bufferId);
		} break;
		default: {
			return Variant();
		} break;
	}
}

////////////////////////////////////////////////////////////////////////////
// OpenXRMarkerTracker

void OpenXRMarkerTracker::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_bounds_size", "bounds_size"), &OpenXRMarkerTracker::set_bounds_size);
	ClassDB::bind_method(D_METHOD("get_bounds_size"), &OpenXRMarkerTracker::get_bounds_size);
	ADD_PROPERTY(PropertyInfo(Variant::INT, "bounds_size"), "set_bounds_size", "get_bounds_size");

	ClassDB::bind_method(D_METHOD("set_marker_type", "marker_type"), &OpenXRMarkerTracker::set_marker_type);
	ClassDB::bind_method(D_METHOD("get_marker_type"), &OpenXRMarkerTracker::get_marker_type);
	ADD_PROPERTY(PropertyInfo(Variant::INT, "marker_type"), "set_marker_type", "get_marker_type");

	ClassDB::bind_method(D_METHOD("set_marker_id", "marker_id"), &OpenXRMarkerTracker::set_marker_id);
	ClassDB::bind_method(D_METHOD("get_marker_id"), &OpenXRMarkerTracker::get_marker_id);
	ADD_PROPERTY(PropertyInfo(Variant::INT, "marker_id"), "set_marker_id", "get_marker_id");

	// As the type of marker data can vary, we can't make this a property.
	ClassDB::bind_method(D_METHOD("set_marker_data", "marker_data"), &OpenXRMarkerTracker::set_marker_data);
	ClassDB::bind_method(D_METHOD("get_marker_data"), &OpenXRMarkerTracker::get_marker_data);
}

void OpenXRMarkerTracker::set_bounds_size(Vector2 p_bounds_size) {
	if (bounds_size != p_bounds_size) {
		bounds_size = p_bounds_size;
	}
}

Vector2 OpenXRMarkerTracker::get_bounds_size() const {
	return bounds_size;
}

void OpenXRMarkerTracker::set_marker_type(OpenXRSpatialComponentMarkerList::MarkerType p_marker_type) {
	marker_type = p_marker_type;
}

OpenXRSpatialComponentMarkerList::MarkerType OpenXRMarkerTracker::get_marker_type() const {
	return marker_type;
}

void OpenXRMarkerTracker::set_marker_id(uint32_t p_id) {
	marker_id = p_id;
}

uint32_t OpenXRMarkerTracker::get_marker_id() const {
	return marker_id;
}

void OpenXRMarkerTracker::set_marker_data(const Variant &p_data) {
	marker_data = p_data;
}

Variant OpenXRMarkerTracker::get_marker_data() const {
	return marker_data;
}

////////////////////////////////////////////////////////////////////////////
// OpenXRSpatialMarkerTrackingCapability

OpenXRSpatialMarkerTrackingCapability *OpenXRSpatialMarkerTrackingCapability::singleton = nullptr;

OpenXRSpatialMarkerTrackingCapability *OpenXRSpatialMarkerTrackingCapability::get_singleton() {
	return singleton;
}

OpenXRSpatialMarkerTrackingCapability::OpenXRSpatialMarkerTrackingCapability() {
	singleton = this;
}

OpenXRSpatialMarkerTrackingCapability::~OpenXRSpatialMarkerTrackingCapability() {
	singleton = nullptr;
}

void OpenXRSpatialMarkerTrackingCapability::_bind_methods() {
	ClassDB::bind_method(D_METHOD("is_qrcode_supported"), &OpenXRSpatialMarkerTrackingCapability::is_qrcode_supported);
	ClassDB::bind_method(D_METHOD("is_micro_qrcode_supported"), &OpenXRSpatialMarkerTrackingCapability::is_micro_qrcode_supported);
	ClassDB::bind_method(D_METHOD("is_aruco_supported"), &OpenXRSpatialMarkerTrackingCapability::is_aruco_supported);
	ClassDB::bind_method(D_METHOD("is_april_tag_supported"), &OpenXRSpatialMarkerTrackingCapability::is_april_tag_supported);
}

HashMap<String, bool *> OpenXRSpatialMarkerTrackingCapability::get_requested_extensions() {
	HashMap<String, bool *> request_extensions;

	if (GLOBAL_GET_CACHED(bool, "xr/openxr/extensions/spatial_entity/enabled") && GLOBAL_GET_CACHED(bool, "xr/openxr/extensions/spatial_entity/enable_marker_tracking")) {
		request_extensions[XR_EXT_SPATIAL_MARKER_TRACKING_EXTENSION_NAME] = &spatial_marker_tracking_ext;
	}

	return request_extensions;
}

void OpenXRSpatialMarkerTrackingCapability::on_session_created(const XrSession p_session) {
	OpenXRSpatialEntityExtension *se_extension = OpenXRSpatialEntityExtension::get_singleton();
	ERR_FAIL_NULL(se_extension);

	if (!spatial_marker_tracking_ext) {
		return;
	}

	se_extension->connect(SNAME("spatial_discovery_recommended"), callable_mp(this, &OpenXRSpatialMarkerTrackingCapability::_on_spatial_discovery_recommended));

	if (GLOBAL_GET_CACHED(bool, "xr/openxr/extensions/spatial_entity/enable_builtin_marker_tracking")) {
		// Start by creating our spatial context
		_create_spatial_context();
	}
}

void OpenXRSpatialMarkerTrackingCapability::on_session_destroyed() {
	OpenXRSpatialEntityExtension *se_extension = OpenXRSpatialEntityExtension::get_singleton();
	ERR_FAIL_NULL(se_extension);
	XRServer *xr_server = XRServer::get_singleton();
	ERR_FAIL_NULL(xr_server);

	// Free and unregister our anchors
	for (const KeyValue<XrSpatialEntityIdEXT, Ref<OpenXRMarkerTracker>> &marker_tracker : marker_trackers) {
		xr_server->remove_tracker(marker_tracker.value);
	}
	marker_trackers.clear();

	// Free our spatial context
	if (spatial_context.is_valid()) {
		se_extension->free_spatial_context(spatial_context);
		spatial_context = RID();
	}

	se_extension->disconnect(SNAME("spatial_discovery_recommended"), callable_mp(this, &OpenXRSpatialMarkerTrackingCapability::_on_spatial_discovery_recommended));
}

bool OpenXRSpatialMarkerTrackingCapability::is_qrcode_supported() {
	if (!spatial_marker_tracking_ext) {
		return false;
	}

	OpenXRSpatialEntityExtension *se_extension = OpenXRSpatialEntityExtension::get_singleton();
	ERR_FAIL_NULL_V(se_extension, false);

	return se_extension->supports_capability(XR_SPATIAL_CAPABILITY_MARKER_TRACKING_QR_CODE_EXT);
}

bool OpenXRSpatialMarkerTrackingCapability::is_micro_qrcode_supported() {
	OpenXRSpatialEntityExtension *se_extension = OpenXRSpatialEntityExtension::get_singleton();
	ERR_FAIL_NULL_V(se_extension, false);

	return se_extension->supports_capability(XR_SPATIAL_CAPABILITY_MARKER_TRACKING_MICRO_QR_CODE_EXT);
}

bool OpenXRSpatialMarkerTrackingCapability::is_aruco_supported() {
	OpenXRSpatialEntityExtension *se_extension = OpenXRSpatialEntityExtension::get_singleton();
	ERR_FAIL_NULL_V(se_extension, false);

	return se_extension->supports_capability(XR_SPATIAL_CAPABILITY_MARKER_TRACKING_ARUCO_MARKER_EXT);
}

bool OpenXRSpatialMarkerTrackingCapability::is_april_tag_supported() {
	OpenXRSpatialEntityExtension *se_extension = OpenXRSpatialEntityExtension::get_singleton();
	ERR_FAIL_NULL_V(se_extension, false);

	return se_extension->supports_capability(XR_SPATIAL_CAPABILITY_MARKER_TRACKING_APRIL_TAG_EXT);
}

////////////////////////////////////////////////////////////////////////////
// Discovery logic

Ref<OpenXRFutureResult> OpenXRSpatialMarkerTrackingCapability::_create_spatial_context() {
	OpenXRSpatialEntityExtension *se_extension = OpenXRSpatialEntityExtension::get_singleton();
	ERR_FAIL_NULL_V(se_extension, nullptr);

	TypedArray<OpenXRSpatialCapabilityConfigurationBaseHeader> capability_configurations;

	// Create our configuration objects.
	// For now we enable all supported markers, will need to give some more user control over this.
	if (is_qrcode_supported()) {
		qrcode_configuration.instantiate();
		capability_configurations.push_back(qrcode_configuration);
	}

	if (is_micro_qrcode_supported()) {
		micro_qrcode_configuration.instantiate();
		capability_configurations.push_back(micro_qrcode_configuration);
	}

	if (is_aruco_supported()) {
		aruco_configuration.instantiate();

		// TODO add ability to configure arUcoDict (or make this a default setting on the configuration object)
		capability_configurations.push_back(aruco_configuration);
	}

	if (is_april_tag_supported()) {
		april_tag_configuration.instantiate();

		// TODO add ability to configure aprilDict (or make this a default setting on the configuration object)

		capability_configurations.push_back(april_tag_configuration);
	}

	if (capability_configurations.is_empty()) {
		print_verbose("OpenXR: There are no supported marker types. Marker tracking is not enabled.");
		return nullptr;
	}

	return se_extension->create_spatial_context(capability_configurations, nullptr, callable_mp(this, &OpenXRSpatialMarkerTrackingCapability::_on_spatial_context_created));
}

void OpenXRSpatialMarkerTrackingCapability::_on_spatial_context_created(RID p_spatial_context) {
	spatial_context = p_spatial_context;

	_start_entity_discovery();
}

void OpenXRSpatialMarkerTrackingCapability::_on_spatial_discovery_recommended(RID p_spatial_context) {
	if (p_spatial_context == spatial_context) {
		// Trigger new discovery
		_start_entity_discovery();
	}
}

Ref<OpenXRFutureResult> OpenXRSpatialMarkerTrackingCapability::_start_entity_discovery() {
	OpenXRSpatialEntityExtension *se_extension = OpenXRSpatialEntityExtension::get_singleton();
	ERR_FAIL_NULL_V(se_extension, nullptr);

	// Already running, or ran discovery, cancel/cleanup
	if (discovery_query_result.is_valid()) {
		discovery_query_result->cancel_future();
		discovery_query_result.unref();
	}

	// We want both our anchor and persistence component
	Vector<XrSpatialComponentTypeEXT> component_types;
	component_types.push_back(XR_SPATIAL_COMPONENT_TYPE_MARKER_EXT);
	component_types.push_back(XR_SPATIAL_COMPONENT_TYPE_BOUNDED_2D_EXT);

	// Start our new snapshot.
	discovery_query_result = se_extension->discover_spatial_entities(spatial_context, component_types, nullptr, callable_mp(this, &OpenXRSpatialMarkerTrackingCapability::_on_spatial_discovery_completed));

	return discovery_query_result;
}

void OpenXRSpatialMarkerTrackingCapability::_on_spatial_discovery_completed(RID p_snapshot) {
	OpenXRSpatialEntityExtension *se_extension = OpenXRSpatialEntityExtension::get_singleton();
	ERR_FAIL_NULL(se_extension);
	XRServer *xr_server = XRServer::get_singleton();
	ERR_FAIL_NULL(xr_server);
	OpenXRAPI *openxr_api = OpenXRAPI::get_singleton();
	ERR_FAIL_NULL(openxr_api);

	// Make a copy of the markers we have right now, so we know which ones to clean up.
	LocalVector<XrSpatialEntityIdEXT> current_markers;
	current_markers.resize(marker_trackers.size());
	int m = 0;
	for (const KeyValue<XrSpatialEntityIdEXT, Ref<OpenXRMarkerTracker>> &marker : marker_trackers) {
		current_markers[m++] = marker.key;
	}

	// Build our component data
	TypedArray<OpenXRSpatialComponentData> component_data;

	// We always need a query result data object
	Ref<OpenXRSpatialQueryResultData> query_result_data;
	query_result_data.instantiate();
	component_data.push_back(query_result_data);

	// Add bounded2D
	Ref<OpenXRSpatialComponentBounded2DList> bounded2d_list;
	bounded2d_list.instantiate();
	component_data.push_back(bounded2d_list);

	// Plane alignment list
	Ref<OpenXRSpatialComponentMarkerList> marker_list;
	marker_list.instantiate();
	component_data.push_back(marker_list);

	if (se_extension->query_snapshot(p_snapshot, component_data, nullptr)) {
		// Now loop through our data and update our anchors.
		// Q we're assuming entity id size and state size are equal, is there ever a situation where they would not be?
		int64_t size = query_result_data->get_capacity();
		for (int64_t i = 0; i < size; i++) {
			XrSpatialEntityIdEXT entity_id = query_result_data->get_entity_id(i);
			XrSpatialEntityTrackingStateEXT entity_state = query_result_data->get_entity_state(i);

			// Erase it from our current markers (if we have it, else this is ignored).
			current_markers.erase(entity_id);

			if (entity_state == XR_SPATIAL_ENTITY_TRACKING_STATE_STOPPED_EXT) {
				// We should only get this status on updates as a prelude to needing to remove this marker.
				// So we just update the status.
				if (marker_trackers.has(entity_id)) {
					Ref<OpenXRMarkerTracker> marker_tracker = marker_trackers[entity_id];

					marker_tracker->invalidate_pose(SNAME("default"));
					marker_tracker->set_spatial_tracking_state(XR_SPATIAL_ENTITY_TRACKING_STATE_STOPPED_EXT);
				}
			} else {
				// Process our entity
				bool add_to_xr_server = false;
				Ref<OpenXRMarkerTracker> marker_tracker;

				if (marker_trackers.has(entity_id)) {
					// We know about this one already
					marker_tracker = marker_trackers[entity_id];
				} else {
					// Create a new anchor
					marker_tracker.instantiate();
					marker_tracker->set_entity(se_extension->make_spatial_entity(se_extension->get_spatial_snapshot_context(p_snapshot), entity_id));
					marker_trackers[entity_id] = marker_tracker;

					add_to_xr_server = true;
				}

				// Handle component data
				if (entity_state == XR_SPATIAL_ENTITY_TRACKING_STATE_PAUSED_EXT) {
					marker_tracker->invalidate_pose(SNAME("default"));
					marker_tracker->set_spatial_tracking_state(XR_SPATIAL_ENTITY_TRACKING_STATE_PAUSED_EXT);

					// No further component data will be valid in this state, we need to ignore it!
				} else if (entity_state == XR_SPATIAL_ENTITY_TRACKING_STATE_TRACKING_EXT) {
					Transform3D transform = bounded2d_list->get_center_pose(i);
					marker_tracker->set_pose(SNAME("default"), transform, Vector3(), Vector3());
					marker_tracker->set_spatial_tracking_state(XR_SPATIAL_ENTITY_TRACKING_STATE_TRACKING_EXT);

					// Process our component data.

					// Set bounds size
					marker_tracker->set_bounds_size(bounded2d_list->get_size(i));

					// Set marker data
					marker_tracker->set_marker_type(marker_list->get_marker_type(i));
					marker_tracker->set_marker_id(marker_list->get_marker_id(i));
					marker_tracker->set_marker_data(marker_list->get_marker_data(p_snapshot, i));
				}

				if (add_to_xr_server) {
					// Register with XR server
					xr_server->add_tracker(marker_tracker);
				}
			}
		}

		// Remove any markers that are no longer there...
		for (const XrSpatialEntityIdEXT &entity_id : current_markers) {
			if (marker_trackers.has(entity_id)) {
				Ref<OpenXRMarkerTracker> marker_tracker = marker_trackers[entity_id];

				// Just in case there are still references out there to this marker,
				// reset some stuff.
				marker_tracker->invalidate_pose(SNAME("default"));
				marker_tracker->set_spatial_tracking_state(XR_SPATIAL_ENTITY_TRACKING_STATE_STOPPED_EXT);

				// Remove it from our XRServer
				xr_server->remove_tracker(marker_tracker);

				// Remove it from our trackers
				marker_trackers.erase(entity_id);
			}
		}
	}

	// Now that we're done, clean up our snapshot!
	se_extension->free_spatial_snapshot(p_snapshot);

	// And if this was our discovery snapshot, lets reset it
	if (discovery_query_result.is_valid() && discovery_query_result->get_result_value() == p_snapshot) {
		discovery_query_result.unref();
	}
}
