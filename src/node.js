function Module(id = "") {
    this.id = id;
    this.exports = {};
    this.loaded = false;
}

Module.require = function(id) {
    const module = new Module(id)
    module.compile(id)
    this.loaded = true;
    return module.exports;
}

Module.prototype.compile = function(filename) {
    const content = NODE.ReadFile(filename);
    const wrapped = NODE.Compile(content);
    wrapped.call(this, Module.require, this.exports, this)
}

;(function() {
    if (!NODE.ARGV[1]) {
        return
    }

    const main = new Module(NODE.ARGV[1]);
    main.compile(NODE.ARGV[1]);
})();
