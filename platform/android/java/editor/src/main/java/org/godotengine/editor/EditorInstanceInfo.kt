package org.godotengine.editor

/**
 * Describe the editor instance to launch
 */
data class EditorInstanceInfo(
	val instanceClass: Class<*>,
	val instanceId: Int,
	val processNameSuffix: String,
	val launchAdjacent: Boolean = false
)
