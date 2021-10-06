function Console(stdout, stderr) {
    if (!(this instanceof Console)) {
        return new Console(stdout, stderr);
    }
    this._stdout = stdout || process.Print;
    this._stderr = stderr || process.PrintError;
}

Console.prototype.log = function () {
    const args = Array.from(arguments);
    this._stdout(...args);
}

Console.prototype.error = function () {
    const args = Array.from(arguments);
    this._stderr(...args);
}

this.console = new Console();



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
    const content = process.ReadFile(filename);
    const wrapped = process.Compile(content);
    wrapped.call(this, Module.require, this.exports, this)
}

;(function() {

    if (!process.ARGV[1]) {
        return
    }
    const mainFile = process.ARGV[1];

    const main = new Module(mainFile);
    main.compile(mainFile);
})();
