//
// Copyright (c) 2015, Chaos Software Ltd
//
// V-Ray For Houdini
//
// ACCESSIBLE SOURCE CODE WITHOUT DISTRIBUTION OF MODIFICATION LICENSE
//
// Full license text: https://github.com/ChaosGroup/vray-for-houdini/blob/master/LICENSE
//

#include <boost/format.hpp>

#include "vop_MtlMulti.h"


using namespace VRayForHoudini;


static PRM_Name  rpm_name_mtl_count("mtl_count", "Materials Count");
static PRM_Range rpm_range_mtl_count(PRM_RANGE_RESTRICTED, 1, PRM_RANGE_UI, 8);


// NOTE: Sockets order:
//
//   - Auto. sockets from description
//   - material_1
//   - ...
//   - material_<mtl_count>
//

void VOP::MtlMulti::setPluginType()
{
	pluginType = "MATERIAL";
	pluginID   = "MtlMulti";
}


void VOP::MtlMulti::addPrmTemplate(Parm::PRMTmplList &prmTemplate)
{
	prmTemplate.push_back(PRM_Template(PRM_INT, 1, &rpm_name_mtl_count, PRMoneDefaults, /*choicelist*/ nullptr, &rpm_range_mtl_count));
}


const char* VOP::MtlMulti::inputLabel(unsigned idx) const
{
	const int numBaseInputs = VOP::NodeBase::orderedInputs();

	if (idx < numBaseInputs) {
		return VOP::NodeBase::inputLabel(idx);
	}

	const int socketIndex = idx - numBaseInputs + 1;
	const std::string &label =boost::str(boost::format("Material %i") % socketIndex);

	return label.c_str();
}


int VOP::MtlMulti::getInputFromName(const UT_String &in) const
{
	return getInputFromNameSubclass(in);
}


void VOP::MtlMulti::getInputNameSubclass(UT_String &in, int idx) const
{
	const int numBaseInputs = VOP::NodeBase::orderedInputs();

	if (idx < numBaseInputs) {
		VOP::NodeBase::getInputNameSubclass(in, idx);
	}
	else {
		const int socketIndex = idx - numBaseInputs + 1;
		in = boost::str(boost::format("mtl_%i") % socketIndex);
	}
}


int VOP::MtlMulti::getInputFromNameSubclass(const UT_String &in) const
{
	int inIdx = -1;

	if (in.startsWith("mtl_")) {
		const int numBaseInputs = VOP::NodeBase::orderedInputs();

		int idx = -1;
		sscanf(in.buffer(), "mtl_%i", &idx);

		inIdx = numBaseInputs + idx - 1;
	}
	else {
		inIdx = VOP::NodeBase::getInputFromNameSubclass(in);
	}

	return inIdx;
}


void VOP::MtlMulti::getInputTypeInfoSubclass(VOP_TypeInfo &type_info, int idx)
{
	const int numBaseInputs = VOP::NodeBase::orderedInputs();

	if (idx < numBaseInputs) {
		VOP::NodeBase::getInputTypeInfoSubclass(type_info, idx);
	}
	else {
		type_info.setType(VOP_SURFACE_SHADER);
	}
}


void VOP::MtlMulti::getAllowedInputTypeInfosSubclass(unsigned idx, VOP_VopTypeInfoArray &type_infos)
{
	const int numBaseInputs = VOP::NodeBase::orderedInputs();

	if (idx < numBaseInputs) {
		VOP::NodeBase::getAllowedInputTypeInfosSubclass(idx, type_infos);
	}
	else {
		VOP_TypeInfo type_info(VOP_SURFACE_SHADER);
		type_infos.append(type_info);
	}
}


int VOP::MtlMulti::customInputsCount() const
{
	// One socket per texture
	int numCustomInputs = evalInt(rpm_name_mtl_count.getToken(), 0, 0.0);

	return numCustomInputs;
}


unsigned VOP::MtlMulti::getNumVisibleInputs() const
{
	return orderedInputs();
}


unsigned VOP::MtlMulti::orderedInputs() const
{
	int orderedInputs = VOP::NodeBase::orderedInputs();
	orderedInputs += customInputsCount();

	return orderedInputs;
}


void VOP::MtlMulti::getCode(UT_String &codestr, const VOP_CodeGenContext &)
{
}


OP::VRayNode::PluginResult VOP::MtlMulti::asPluginDesc(Attrs::PluginDesc &pluginDesc, VRayExporter &exporter, OP_Node *parent)
{
	const int mtls_count = evalInt(rpm_name_mtl_count.getToken(), 0, 0.0);

	VRay::ValueList mtls_list;
	VRay::IntList   ids_list;

	for (int i = 1; i <= mtls_count; ++i) {
		const std::string &mtlSockName = boost::str(boost::format("mtl_%i") % i);

		OP_Node *mtl_node = VRayExporter::getConnectedNode(this, mtlSockName);
		if (NOT(mtl_node)) {
			PRINT_WARN("Node \"%s\": Material node is not connected to \"%s\", ignoring...",
					   getName().buffer(), mtlSockName.c_str());
		}
		else {
			VRay::Plugin mtl_plugin = exporter.exportVop(mtl_node);
			if (NOT(mtl_plugin)) {
				PRINT_ERROR("Node \"%s\": Failed to export material node connected to \"%s\", ignoring...",
							getName().buffer(), mtlSockName.c_str());
			}
			else {
				mtls_list.push_back(VRay::Value(mtl_plugin));
				ids_list.push_back(i-1);
			}
		}
	}

	if (NOT(mtls_list.size())) {
		return PluginResult::PluginResultError;
	}

	pluginDesc.addAttribute(Attrs::PluginAttr("mtls_list", mtls_list));
	pluginDesc.addAttribute(Attrs::PluginAttr("ids_list", ids_list));

	OP_Node *mtlid_gen       = VRayExporter::getConnectedNode(this, "mtlid_gen");
	OP_Node *mtlid_gen_float = VRayExporter::getConnectedNode(this, "mtlid_gen_float");

	if (mtlid_gen && NOT(mtlid_gen_float)) {
		pluginDesc.addAttribute(Attrs::PluginAttr("mtlid_gen_float", Attrs::PluginAttr::AttrTypeIgnore));
	}

	if (mtlid_gen_float && NOT(mtlid_gen)) {
		pluginDesc.addAttribute(Attrs::PluginAttr("mtlid_gen", Attrs::PluginAttr::AttrTypeIgnore));
	}

	return PluginResult::PluginResultContinue;
}
