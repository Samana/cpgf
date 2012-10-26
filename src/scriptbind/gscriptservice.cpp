#include "cpgf/scriptbind/gscriptservice.h"
#include "cpgf/scriptbind/gscriptbind.h"
#include "cpgf/scriptbind/gscriptbindutil.h"
#include "cpgf/gmetadefine.h"

#include "cpgf/metadata/util/gmetadata_metabytearray.h"
#include "cpgf/metadata/util/gmetadata_metaobjectarray.h"
#include "cpgf/metautility/gmetabytearray.h"
#include "cpgf/metautility/gmetaobjectarray.h"

#include "cpgf/scriptbind/gscriptlibraryapi.h"

#include "cpgf/metadata/private/gmetadata_header.h"

#include "cpgf/metatraits/gmetasharedptrtraits_gsharedpointer.h"


namespace cpgf {

IScriptLibraryLoader * createBuiltinLibraries();

namespace {

GSharedPointer<GMetaByteArray> createByteArray()
{
	return GSharedPointer<GMetaByteArray>(new GMetaByteArray);
}

GSharedPointer<GMetaByteArray> createByteArrayWithLength(size_t length)
{
	return GSharedPointer<GMetaByteArray>(new GMetaByteArray(length));
}

GSharedPointer<GMetaObjectArray> createObjectArray(IMetaClass * metaClass)
{
	return GSharedPointer<GMetaObjectArray>(new GMetaObjectArray(metaClass));
}

} // unnamed namespace

template <typename D>
void buildMetaClass_GScriptCoreService(D _d)
{
    _d.CPGF_MD_TEMPLATE _method("cloneClass", &D::ClassType::cloneClass);
    _d.CPGF_MD_TEMPLATE _method("loadLibrary", &D::ClassType::loadLibrary);
}

GScriptCoreService * doBindScriptCoreService(GScriptObject * scriptObject, const char * bindName, IScriptLibraryLoader * libraryLoader)
{
	GScopedPointer<GScriptCoreService> coreService(new GScriptCoreService(scriptObject, bindName, libraryLoader));

	GDefineMetaClass<GScriptCoreService> define = GDefineMetaClass<GScriptCoreService>::Policy<GMetaPolicyNoDefaultAndCopyConstructor>::declare("GScriptCoreService");
	buildMetaClass_GScriptCoreService(define);

	injectObjectToScript(scriptObject, define.getMetaClass(), coreService.get(), bindName);

	GScopedInterface<IMetaItem> metaItem(metaItemToInterface(define.takeMetaClass(), true));
	scriptObject->holdObject(metaItem.get());

	return coreService.take();
}


GScriptCoreService::GScriptCoreService(GScriptObject * scriptObject, const char * bindName, IScriptLibraryLoader * libraryLoader)
	: scriptObject(scriptObject), bindName(bindName), libraryLoader(libraryLoader)
{
}

GScriptCoreService::~GScriptCoreService()
{
}

IMetaClass * GScriptCoreService::cloneClass(IMetaClass * metaClass)
{
	return this->scriptObject->cloneMetaClass(metaClass);
}

bool GScriptCoreService::loadLibrary(const char * namespaces, const GMetaVariadicParam * libraryNames)
{
	if(! this->libraryLoader) {
		this->libraryLoader.reset(createBuiltinLibraries());
		this->libraryLoader->releaseReference();
	}

	if(namespaces == NULL) {
		namespaces = this->bindName.c_str();
	}

	GScopedInterface<IScriptObject> owner(scriptObjectToInterface(this->scriptObject, false));
	for(size_t i = 0; i < libraryNames->paramCount; ++i) {
		char * name = fromVariant<char *>(*(libraryNames->params[i]));
		if(! this->libraryLoader->loadScriptLibrary(owner.get(), namespaces, name)) {
			return false;
		}
	}
	
	return true;
}

bool loadByteArray(IScriptObject * owner, const char * namespaces, const char * /*libraryName*/)
{
	GDefineMetaNamespace ns = GDefineMetaNamespace::declare("");
	
	ns._method("createByteArray", &createByteArray);
	ns._method("createByteArray", &createByteArrayWithLength);

	GDefineMetaClass<GMetaByteArray> gbyteArrayDefine = GDefineMetaClass<GMetaByteArray>::Policy<GMetaPolicyNoCopyConstructor>::declare("GMetaByteArray");
	buildMetaData_byteArray(gbyteArrayDefine);
	ns._class(gbyteArrayDefine);
	
	GScopedInterface<IMetaClass> metaClass(static_cast<IMetaClass *>(metaItemToInterface(ns.takeMetaClass(), true)));
	owner->holdObject(metaClass.get());
	injectObjectToScript(owner, metaClass.get(), NULL, namespaces);
	
	return true;
}

bool loadObjectArray(IScriptObject * owner, const char * namespaces, const char * /*libraryName*/)
{
	GDefineMetaNamespace ns = GDefineMetaNamespace::declare("");
	
	ns._method("createObjectArray", &createObjectArray);

	GDefineMetaClass<GMetaObjectArray> gobjectArrayDefine = GDefineMetaClass<GMetaObjectArray>::Policy<GMetaPolicyNoDefaultAndCopyConstructor>::declare("GMetaObjectArray");
	buildMetaData_metaObjectArray(gobjectArrayDefine);
	ns._class(gobjectArrayDefine);
	
	GScopedInterface<IMetaClass> metaClass(static_cast<IMetaClass *>(metaItemToInterface(ns.takeMetaClass(), true)));
	owner->holdObject(metaClass.get());
	injectObjectToScript(owner, metaClass.get(), NULL, namespaces);
	
	return true;
}

void initializeBuiltinLibraries(GScriptLibraryNamedLoaderHandler * libraryHandler)
{
	libraryHandler->addHandler("builtin.arrays.bytearray", GScriptLibraryLoaderCallback(&loadByteArray));
	libraryHandler->addHandler("builtin.arrays.objectarray", GScriptLibraryLoaderCallback(&loadObjectArray));
}

IScriptLibraryLoader * createBuiltinLibraries()
{
	GScriptLibraryNamedLoaderHandler * namedLoader = new GScriptLibraryNamedLoaderHandler();
	GScopedInterface<IScriptLibraryLoaderHandler> namedLoaderInterface(namedLoader);
	initializeBuiltinLibraries(namedLoader);
	GScriptLibraryLoader * loader = new GScriptLibraryLoader();
	loader->addHandler(namedLoaderInterface.get());
	return loader;
}


} // namespace cpgf


#include "cpgf/metadata/private/gmetadata_footer.h"
