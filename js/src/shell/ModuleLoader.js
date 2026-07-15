/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

// A basic synchronous module loader for testing the shell.

Reflect.Loader = new class {
    constructor() {
        this.registry = new Map();
        this.modulePaths = new Map();
        this.loadPath = getModuleLoadPath();
    }

    resolve(name) {
        if (os.path.isAbsolute(name))
            return name;

        return os.path.join(this.loadPath, name);
    }

    fetch(path) {
        return os.file.readFile(path);
    }

    loadAndParse(name) {
        let path = this.resolve(name);

        if (this.registry.has(path))
            return this.registry.get(path);

        let source = this.fetch(path);
        let module = parseModule(source, path);
        this.registry.set(path, module);
        this.modulePaths.set(module, path);
        return module;
    }

    loadAndExecute(name) {
        let module = this.loadAndParse(name);
        module.declarationInstantiation();
        return module.evaluation();
    }

    ["import"](name, referencingInfo) {
        let module = this.loadAndParse(name);
        module.declarationInstantiation();
        return module.evaluation();
    }

    populateImportMeta(module, metaObject) {
        // For the shell, use the script's filename as the base URL.

        let path;
        if (ReflectApply(MapPrototypeHas, this.modulePaths, [module])) {
            path = ReflectApply(MapPrototypeGet, this.modulePaths, [module]);
        } else {
            path = "(unknown)";
        }
        metaObject.url = path;
    }
};

setModuleResolveHook((referencingInfo, requestName) => {
    let path = Reflect.Loader.resolve(requestName, referencingInfo);
    return Reflect.Loader.loadAndParse(path);
});
 
setModuleMetadataHook((module, metaObject) => {
    Reflect.Loader.populateImportMeta(module, metaObject);
});
 
setModuleDynamicImportHook((referencingInfo, specifier, promise) => {
    try {
        let path = Reflect.Loader.resolve(specifier, referencingInfo);
        Reflect.Loader.loadAndExecute(path);
        finishDynamicModuleImport(referencingInfo, specifier, promise);
    } catch (err) {
        abortDynamicModuleImport(referencingInfo, specifier, promise, err);
    }
});
