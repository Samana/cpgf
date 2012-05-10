package com.cpgf.metagen;

import java.lang.reflect.Array;
import java.lang.reflect.Field;
import java.util.Map;

import org.mozilla.javascript.Context;
import org.mozilla.javascript.Function;
import org.mozilla.javascript.NativeArray;
import org.mozilla.javascript.NativeObject;
import org.mozilla.javascript.Scriptable;

import com.cpgf.metagen.metadata.Item;
import com.cpgf.metagen.metawriter.callback.IOutputCallback;
import com.cpgf.metagen.metawriter.callback.ISourceHeaderReplacer;
import com.cpgf.metagen.metawriter.callback.OutputCallbackData;

public class JavascriptConfigLoader implements IOutputCallback, ISourceHeaderReplacer {
	private static Context context;
	private static Scriptable scope;
	private Config config;
	
	private Function jsReplaceSourceHeader;
	private Function jsOutputCallback;
	
	public JavascriptConfigLoader(Config config) {
		this.config = config;
	}

	@Override
	public String replaceSourceHeader(String headerName) {
		if(this.jsReplaceSourceHeader != null) {
			Object[] args = { headerName };
			headerName = (String)(this.jsReplaceSourceHeader.call(context, scope, scope, args));
		}
		return headerName;
	}

	@Override
	public void outputCallback(Item item, OutputCallbackData data) {
		if(this.jsOutputCallback != null) {
			Object[] args = { item, data };
			this.jsOutputCallback.call(context, scope, scope, args);
		}
	}

	public void load(String configFileName) throws Exception {
		String javascriptCode = Util.readTextFromFile(configFileName);
        
		if(context == null) {
			context = Context.enter();
		}
		if(scope == null) {
			scope = context.initStandardObjects();
		}

        context.evaluateString(scope, javascriptCode, configFileName, 1, null);
        
        Object configObject = scope.get("config", scope);
        if(configObject == null || configObject == Scriptable.NOT_FOUND) {
        	this.error("Can't find config object in " + configFileName);
        }
        if(! (configObject instanceof NativeObject)) {
        	this.error("config must be Javascript object in " + configFileName);
        }
        
        this.loadConfig((NativeObject)configObject);
	}
	
	public void free() {
		if(context != null) {
			Context.exit();
			context = null;
		}
	}
	
	private void error(String message) throws Exception {
		Util.error("Config error -- " + message);
	}
	
	private void loadConfig(NativeObject jsObject) throws Exception {
		for(Map.Entry<Object, Object> entry : jsObject.entrySet()) {
			this.loadProperty(entry.getKey(), entry.getValue());
		}
	}
	
	private void loadProperty(Object key, Object value) throws Exception {
		if(! (key instanceof String)) {
			this.error("Unknown config property type " + key);
		}
		
		String propertyName = (String)key;
		Field field = null;
		try {
			field = Config.class.getField(propertyName);
		}
		catch(NoSuchFieldException e) {
			this.error("Can't find config property for " + propertyName);
		}
		
		if(this.setField(field, String.class, propertyName, value)) {
			return;
		}

		if(this.setField(field, boolean.class, propertyName, value)) {
			return;
		}

		if(field.getType().isArray()) {
			if(! (value instanceof NativeArray)) {
				this.error("Property " + propertyName + " must be an array.");
			}
			
			NativeArray na = (NativeArray)(value);
			int count = (int)na.getLength();
			Object[] array = na.toArray();
			Object obj = Array.newInstance(field.getType().getComponentType(), count);
			for(int i = 0; i < count; ++i) {
				Array.set(obj, i, array[i]);
			}
			field.set(this.config, obj);
			return;
		}
		
		if(field.getType().equals(IOutputCallback.class)) {
			if(value != null) {
				if(! (value instanceof Function)) {
					this.error("Property " + propertyName + " must be a function.");
				}
				
				this.jsOutputCallback = (Function)value;
				this.config.metaOutputCallback = this;
			}
			return;
		}

		if(field.getType().equals(ISourceHeaderReplacer.class)) {
			if(value != null) {
				if(! (value instanceof Function)) {
					this.error("Property " + propertyName + " must be a function.");
				}
				
				this.jsReplaceSourceHeader = (Function)value;
				this.config.sourceHeaderReplacer = this;
			}
			return;
		}

		this.error("Unknow value type for property " + propertyName);
	}

	private static Class<?>[][] compatibleTypes = {
		{ boolean.class, Boolean.class }
	};
	
	private static boolean typeIsIn(Class<?> type, Class<?>[] typeList) {
		for(Class<?> t : typeList) {
			if(t.equals(type)) {
				return true;
			}
		}
		
		return false;
	}
	
	private boolean isConvertable(Class<?> javaType, Class<?> jsType) {
		if(javaType.equals(jsType)) {
			return true;
		}
		
		for(Class<?>[] typeList : compatibleTypes) {
			if(typeIsIn(javaType, typeList)) {
				return typeIsIn(jsType, typeList);
			}
		}
		
		return false;
	}
	
	private boolean setField(Field field, Class<?> type, String propertyName, Object value) throws Exception {
		if(field.getType().equals(type)) {
			if(value != null && ! isConvertable(value.getClass(), type)) {
				this.error("Value for " + propertyName + " is wrong.");
			}
			
			field.set(this.config, value);
			return true;
		}
		else {
			return false;
		}
	}

}
