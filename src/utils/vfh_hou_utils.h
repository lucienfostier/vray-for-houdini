//
// Copyright (c) 2015-2016, Chaos Software Ltd
//
// V-Ray For Houdini
//
// ACCESSIBLE SOURCE CODE WITHOUT DISTRIBUTION OF MODIFICATION LICENSE
//
// Full license text: https://github.com/ChaosGroup/vray-for-houdini/blob/master/LICENSE
//

#ifndef VRAY_FOR_HOUDINI_HOU_UTILS_H
#define VRAY_FOR_HOUDINI_HOU_UTILS_H

namespace VRayForHoudini {
namespace HOU {

/// Check if Houdini is running with Indie license
int isIndie();

/// Check if Houdini is running with Apprentice license
int isApprentice();

/// Check if Houdini is running with UI enabled
int isUIAvailable();

} // namespace HOU
} // namespace VRayForHoudini

#endif // VRAY_FOR_HOUDINI_HOU_UTILS_H
