// Auto generated file, don't modify.

#include "Box2D/Box2D.h"
#include "Box2D/Collision/Shapes/b2Shape.h"

#include "cpgf/metadata/box2d/meta_b2Shape.h"

#include "cpgf/gmetapolicy.h"
#include "cpgf/goutmain.h"

extern const char * _mEta_nS_box2d;

using namespace cpgf;

namespace {

G_AUTO_RUN_BEFORE_MAIN()
{
    {
    GDefineMetaNamespace _ns = GDefineMetaNamespace::define(_mEta_nS_box2d);
    GDefineMetaClass<b2MassData> _d = GDefineMetaClass<b2MassData>::declare("b2MassData");
    _ns._class(_d);
    buildMetaClass_B2MassData(0, _d, NULL, GMetaPolicyCopyAllConstReference());

    }
    

    {
    GDefineMetaNamespace _ns = GDefineMetaNamespace::define(_mEta_nS_box2d);
    GDefineMetaClass<b2Shape> _d = GDefineMetaClass<b2Shape>::Policy<MakePolicy<GMetaRuleDefaultConstructorAbsent, GMetaRuleCopyConstructorAbsent> >::declare("b2Shape");
    _ns._class(_d);
    buildMetaClass_B2Shape(0, _d, NULL, GMetaPolicyCopyAllConstReference());

    }
    

}

} // unnamed namespace
