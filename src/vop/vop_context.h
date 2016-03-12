//
// Copyright (c) 2015, Chaos Software Ltd
//
// V-Ray For Houdini
//
// ACCESSIBLE SOURCE CODE WITHOUT DISTRIBUTION OF MODIFICATION LICENSE
//
// Full license text: https://github.com/ChaosGroup/vray-for-houdini/blob/master/LICENSE
//

#ifndef VRAY_FOR_HOUDINI_VOP_CONTEXT_H
#define VRAY_FOR_HOUDINI_VOP_CONTEXT_H

#include <vfh_vray.h>

#include <OP/OP_OperatorTable.h>
#include <OP/OP_Network.h>
#include <SHOP/SHOP_Node.h>
#include <VOP/VOP_CodeGenerator.h>

#include <unordered_set>

namespace VRayForHoudini {
namespace VOP {


class VRayVOPContextOPFilter:
		public OP_OperatorFilter
{
public:
	VRayVOPContextOPFilter();
	virtual bool allowOperatorAsChild(OP_Operator *op) VRAY_OVERRIDE;

private:
	std::unordered_set<std::string> m_allowedVOPs;
};


class VRayMaterialBuilder:
		public SHOP_Node
{
public:
	static OP_Node            *creator(OP_Network *parent, const char *name, OP_Operator *entry) { return new VRayMaterialBuilder(parent, name, entry); }

public:
/// Allow VOPs only as children
	virtual const char        *getChildType() const VRAY_OVERRIDE { return VOP_OPTYPE_NAME; }
	virtual OP_OpTypeId        getChildTypeID() const VRAY_OVERRIDE { return VOP_OPTYPE_ID; }
	virtual OP_OperatorFilter *getOperatorFilter() VRAY_OVERRIDE{ return &m_opFilter; }
	virtual OP_ERROR           cookMe(OP_Context &context) VRAY_OVERRIDE;

	virtual VOP_CodeGenerator *getVopCodeGenerator() VRAY_OVERRIDE { return &m_codeGen; }
	virtual bool               evalVariableValue(UT_String &value, int index, int thread) VRAY_OVERRIDE;
	virtual bool               hasVexShaderParameter(const char *parm_name) VRAY_OVERRIDE;
	virtual void               opChanged(OP_EventType reason, void *data) VRAY_OVERRIDE;

protected:
	VRayMaterialBuilder(OP_Network *parent, const char *name, OP_Operator *entry, SHOP_TYPE shader_type=SHOP_VOP_MATERIAL);
	virtual ~VRayMaterialBuilder() {}

	virtual void               addNode(OP_Node *node, int notify, int explicitly) VRAY_OVERRIDE;
	virtual void               finishedLoadingNetwork(bool is_child_call) VRAY_OVERRIDE;
	virtual void               ensureSpareParmsAreUpdatedSubclass() VRAY_OVERRIDE;

protected:
	VRayVOPContextOPFilter m_opFilter;
	VOP_CodeGenerator      m_codeGen;
};


class MaterialContext:
		public OP_Network
{
public:
	static OP_Node            *creator(OP_Network *parent, const char *name, OP_Operator *entry) { return new MaterialContext(parent, name, entry); }

	virtual const char        *getChildType() const VRAY_OVERRIDE;
	virtual OP_OpTypeId        getChildTypeID() const VRAY_OVERRIDE;

	virtual int                isNetwork() const VRAY_OVERRIDE { return true; }
	virtual OP_OpTypeId        getOpTypeID() const VRAY_OVERRIDE;
	virtual const char        *getOpType() const VRAY_OVERRIDE;
	virtual OP_DataType        getCookedDataType() const VRAY_OVERRIDE;
	virtual void               deleteCookedData() VRAY_OVERRIDE {}
	virtual int                saveCookedData(std::ostream &os, OP_Context &, int binary=0) VRAY_OVERRIDE;
	virtual int                saveCookedData(const char *filename, OP_Context &) VRAY_OVERRIDE;
	virtual OP_ERROR           cookMe(OP_Context &context) VRAY_OVERRIDE;
	virtual OP_ERROR           bypassMe(OP_Context &context, int &copied_input) VRAY_OVERRIDE;
	virtual const char        *getFileExtension(int binary) const VRAY_OVERRIDE;

protected:
	MaterialContext(OP_Network *parent, const char *name, OP_Operator *entry);
	virtual                   ~MaterialContext() {}

public:
	static void                register_operator(OP_OperatorTable *table);
	static void                register_shop_operator(OP_OperatorTable *table);

};

} // namespace VOP
} // namespace VRayForHoudini

#endif // VRAY_FOR_HOUDINI_VOP_CONTEXT_H
