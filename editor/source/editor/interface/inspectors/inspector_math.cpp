#include "inspector_math.h"
#include "../gizmos/imguizmo.h"

bool Inspector_Vector2::inspect(rttr::variant& var, bool readOnly, std::function<rttr::variant(const rttr::variant&)> get_metadata)
{
	auto data = var.get_value<math::vec2>();
	const char* names[] = { "X", "Y" };
	if (gui::DragFloatNEx(names, &data[0], 2, 0.05f))
	{
		var = data;
		return true;
	}
	return false;
}

bool Inspector_Vector3::inspect(rttr::variant& var, bool readOnly, std::function<rttr::variant(const rttr::variant&)> get_metadata)
{
	auto data = var.get_value<math::vec3>();
	const char* names[] = { "X", "Y", "Z" };
	if (gui::DragFloatNEx(names, &data[0], 3, 0.05f))
	{
		var = data;
		return true;
	}
	return false;
}

bool Inspector_Vector4::inspect(rttr::variant& var, bool readOnly, std::function<rttr::variant(const rttr::variant&)> get_metadata)
{
	auto data = var.get_value<math::vec4>();
	const char* names[] = { "X", "Y", "Z", "W" };
	if (gui::DragFloatNEx(names, &data[0], 4, 0.05f))
	{
		var = data;
		return true;
	}
	return false;
}

bool Inspector_Color::inspect(rttr::variant& var, bool readOnly, std::function<rttr::variant(const rttr::variant&)> get_metadata)
{
	auto data = var.get_value<math::color>();
	if (gui::ColorEdit4("", &data.value.x))
	{
		var = data;
		return true;
	}

	return false;
}

bool Inspector_Quaternion::inspect(rttr::variant& var, bool readOnly, std::function<rttr::variant(const rttr::variant&)> get_metadata)
{
	auto data = var.get_value<math::quat>();
	const char* names[] = { "X", "Y", "Z" };

	auto euler_angles = math::eulerAngles(data);
	auto degrees = math::degrees(euler_angles);
	if (gui::DragFloatNEx(names, &degrees[0], 3, 0.05f))
	{
		auto delta = math::radians(degrees) - euler_angles;

		math::quat qx = math::angleAxis(delta.x, math::vec3{ 1.0f, 0.0f, 0.0f });
		math::quat qy = math::angleAxis(delta.y, math::vec3{ 0.0f, 1.0f, 0.0f });
		math::quat qz = math::angleAxis(delta.z, math::vec3{ 0.0f, 0.0f, 1.0f });
		data = qz * qy * qx * data;;
		var = data;
		return true;
	}

	return false;
}


bool Inspector_Transform::inspect(rttr::variant& var, bool readOnly, std::function<rttr::variant(const rttr::variant&)> get_metadata)
{
	auto data = var.get_value<math::transform_t>();
	const char* names[] = { "X", "Y", "Z" };
	math::vec3 position = data.get_position();
	math::vec3 scale = data.get_scale();
	math::quat rotation = data.get_rotation();
	math::vec3 local_euler_angles = math::degrees(math::eulerAngles(rotation));

	const bool is_global = true;

	static math::quat old_quat;
	static math::vec3 euler_angles;
	bool changed = false;
	bool equal = math::epsilonEqual(math::abs(math::dot(old_quat, rotation)), 1.0f, math::epsilon<float>());
	if (!equal && !gui::IsMouseDragging() || imguizmo::is_using())
	{
		euler_angles = local_euler_angles;
		old_quat = rotation;
	}
	gui::Columns(1);
	if (gui::Button("P"))
	{
		data.set_position({ 0.0f, 0.0f, 0.0f });
		changed = true;
	}
	gui::SameLine();
	gui::PushID("Position");
	auto prevPos = position;
	if (gui::DragFloatNEx(names, &position[0], 3, 0.05f))
	{
		auto delta = position - prevPos;
		if (is_global)
			data.translate(delta);
		else
			data.translate_local(delta);
		changed = true;
	}
	gui::PopID();

	if (gui::Button("R"))
	{
		data.set_rotation(math::quat());
		euler_angles = { 0.0f, 0.0f, 0.0f };
		changed = true;
	}
	gui::SameLine();
	gui::PushID("Rotation");

	auto degrees = euler_angles;
	if (gui::DragFloatNEx(names, &degrees[0], 3, 0.05f))
	{
		if(is_global)
			data.rotate(math::radians(degrees - euler_angles));
		else
			data.rotate_local(math::radians(degrees - euler_angles));

		euler_angles = degrees;
		changed = true;
	}
	
	gui::PopID();

	if (gui::Button("S"))
	{
		data.set_scale({ 1.0f, 1.0f, 1.0f });
		changed = true;
	}
	gui::SameLine();
	gui::PushID("Scale");
	if (gui::DragFloatNEx(names, &scale[0], 3, 0.05f))
	{
		data.set_scale(scale);
		changed = true;
	}
	gui::PopID();
	var = data;

	return changed;
}